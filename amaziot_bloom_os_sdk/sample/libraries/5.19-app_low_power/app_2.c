#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ctype.h"
#include "time.h"
#include "osa.h"
#include "pmic.h"
#include "pmic_rtc.h"
#include "teldef.h"
#include "telatci.h"
#include "telutl.h"

#include "sockets.h"
#include "netdb.h"

#include "gpio.h"
#include "cgpio.h"

#include "UART.h"

extern void yugelogConfig(int type);
extern void yugeLogPrintf(const char *fmt, ...);


#define sdk_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)


#define app_log_debug(fmt, args...) do {sdk_uart_printf("App: "fmt, ##args);} while(0)
#define app_log_info(fmt, args...) do {sdk_uart_printf("App: "fmt, ##args);} while(0)
#define app_log_err(fmt, args...) do {sdk_uart_printf("App: "fmt, ##args);} while(0)

#define APP_MESSAGE_Q_SIZE sizeof(app_msg_t)
#define APP_MESSAGE_Q_MAX 10

#define APP_TASK_STACK_SIZE     (1024*5)
#define APP_TASK_PRIORITY        76

#define APP_TASK_NET_STACK_SIZE     (1024*3)
#define APP_TASK_NET_PRIORITY        75


typedef struct app_msg_s {
    uint32_t msgId;
    uint32_t extra;
    void *data;
} app_msg_t;

typedef struct app_data_s {
    OSMsgQRef cmdq;
    OSTaskRef task_ref;
    void *task_stack;

    void *task_net_stack;
    OSTaskRef task_net_ref;
    OSAFlagRef flag_net_ref;

    OSMutexRef mutex_ref;

    int inited;

    UINT32 task_timeout;

    char server[256];
    int port;

    // note: these buf is not NULL terminated; utlAT_MAX_LINE_LENGTH current value is 2048
    char login_buf[1024];
    unsigned login_buf_len;
    char login_resp_buf[200];
    unsigned login_resp_buf_len;
	
    char heart_buf[560];
    unsigned heart_buf_len;
    char heart_resp_buf[200];
    unsigned heart_resp_buf_len;	
	
    char wake_buf[560];
    unsigned wake_buf_len;


    int heart_miss_max_count;
    int heart_miss_count;
	
    int heart_interval;                   // in seconds
    
    int disable_usb;
	int wakeup_host;
    int connect_retry_count;
    int send_retry_count;

    int recv_error_count;
    int fd_sock;

} app_data_t;

static app_data_t *s_ad = NULL;

#define APP_CMD_START                1
#define APP_CMD_STOP                 2
#define APP_CMD_SUSPEND              3
#define APP_CMD_WAKEUP               4

#define APP_CMD_TEST               100


#define APP_WAKEUP_HOST_GPIO        23


int app_gpio_init(void)
{
	GPIOReturnCode status = GPIORC_OK;

	status = GpioSetDirection(APP_WAKEUP_HOST_GPIO, GPIO_OUT_PIN);
	if (status != GPIORC_OK)
	{
		app_log_debug("app_gpio_init status: 0x%lx", status);
		return -1;	
	}
	return 0;
}

int app_gpio_out(int val)
{
	GPIOReturnCode status = GPIORC_OK;

	status = GpioSetLevel(APP_WAKEUP_HOST_GPIO, val);
	if (status != GPIORC_OK)
	{
		app_log_debug("app_gpio_out status: 0x%lx", status);
		return -1;
	}
	return 0;
}


static int app_send_msg(uint32_t msgId, uint32_t extra, void *data)
{
    app_data_t *ad = s_ad;
    app_msg_t msg;

    if (ad && ad->cmdq) {
        memset(&msg, 0, sizeof(msg));
        msg.msgId = msgId;
        msg.extra = extra;
        msg.data = data;
        if (OS_SUCCESS == OSAMsgQSend(ad->cmdq, sizeof(msg), (UINT8 *)&msg, OSA_NO_SUSPEND)) {
            return 0;
        }
    }
    return -1;
}


static void app_set_mcu_wake(app_data_t *ad, int wake)
{
	app_gpio_out(wake ?  0 : 1);
}

static int app_wakeup_internal(app_data_t *ad, int wake, int extra)
{
    char resp_str[64];

    if (wake) {
        if (ad->disable_usb)
            OEM_SLEEP_DISABLE_USB();
		
        if (ad->wakeup_host)
        	app_set_mcu_wake(ad, 1);
		
		OEM_SET_ALLOW_ENTER_SLEEP_FALG(0);		
        if (extra) {
            
        }
    } else {
        if (ad->disable_usb)
            OEM_SLEEP_ENABLE_USB();

        if (ad->wakeup_host)
        	app_set_mcu_wake(ad, 0);
		
		OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);
        if (extra) {
            
        }
    }
    return 0;
}

static void app_net_close(app_data_t *ad)
{
    int fd, closing = 0;

    app_log_info("%s: +\n", __func__);
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    if (ad->fd_sock >= 0) {
        fd = ad->fd_sock;
        app_log_info("%s: try close connection\n", __func__);
        ad->fd_sock = -1;
        close(fd);

        closing = 1;
    }
    OSAMutexUnlock(ad->mutex_ref);
    if (closing)
        OSATaskYield();
    app_log_info("%s: -\n", __func__);
}

static int app_net_send(app_data_t *ad, const void *data, size_t sz)
{
    const char *d = (const char *)data;
    size_t cur = 0;
    int ret;
    while (cur < sz) {
        ret = send(ad->fd_sock, d + cur, sz - cur, 0);
        if (ret < 0) {
            return -1;
        } else if (!ret) {
            return -1;
        }
        cur += ret;
    }
    return 0;
}

static int app_net_init(app_data_t *ad)
{
    int ret;
    struct timeval tv;
	fd_set rfds;
    //char buf[2600];
    char buf[560] = {0};

    if (ad->login_buf_len) {
        ret = app_net_send(ad, ad->login_buf, ad->login_buf_len);
    } else {
        ret = app_net_send(ad, ad->heart_buf, ad->heart_buf_len);
    }
    if (ret < 0) {
        ret = -1;
        goto clean;
    }

    FD_ZERO(&rfds);
    FD_SET(ad->fd_sock, &rfds);
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    ret = select(ad->fd_sock + 1, &rfds, NULL, NULL, &tv);
    if (ret != 1 || !FD_ISSET(ad->fd_sock, &rfds)) {
        ret = -1;
        goto clean;
    }
    ret = recv(ad->fd_sock, buf, sizeof(buf)-1, 0);
    if (ret <= 0) {
        ret = -1;
        goto clean;
    }
    buf[ret] = '\0';

    app_log_info("%s: config login_resp_buf: %d %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n", __func__, ad->login_resp_buf_len,
        ad->login_resp_buf[0], ad->login_resp_buf[1], ad->login_resp_buf[2], ad->login_resp_buf[3], ad->login_resp_buf[4], ad->login_resp_buf[5], ad->login_resp_buf[6], ad->login_resp_buf[7],
        ad->login_resp_buf[8], ad->login_resp_buf[9], ad->login_resp_buf[10], ad->login_resp_buf[11], ad->login_resp_buf[12], ad->login_resp_buf[13], ad->login_resp_buf[14], ad->login_resp_buf[15],
        ad->login_resp_buf[0+16], ad->login_resp_buf[1+16], ad->login_resp_buf[2+16], ad->login_resp_buf[3+16], ad->login_resp_buf[4+16], ad->login_resp_buf[5+16], ad->login_resp_buf[6+16], ad->login_resp_buf[7+16],
        ad->login_resp_buf[8+16], ad->login_resp_buf[9+16], ad->login_resp_buf[10+16], ad->login_resp_buf[11+16], ad->login_resp_buf[12+16], ad->login_resp_buf[13+16], ad->login_resp_buf[14+16], ad->login_resp_buf[15+16]);

    app_log_info("%s: recv login_resp_buf: %d %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n", __func__, ret,
        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
        buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15],
        buf[0+16], buf[1+16], buf[2+16], buf[3+16], buf[4+16], buf[5+16], buf[6+16], buf[7+16],
        buf[8+16], buf[9+16], buf[10+16], buf[11+16], buf[12+16], buf[13+16], buf[14+16], buf[15+16]);
	
    if (ad->login_buf_len) {
        if (ret >= ad->login_resp_buf_len && !memcmp(buf, ad->login_resp_buf, ad->login_resp_buf_len)) {
            goto init_ok;
        }
    } else {
        if (ret >= ad->heart_resp_buf_len && !memcmp(buf, ad->heart_resp_buf, ad->heart_resp_buf_len)) {
            goto init_ok;
        }
    }
    ret = -1;
	
    goto clean;
init_ok:
	
    ret = 0;
clean:
    return ret;
}

static int app_net_connect(app_data_t *ad, int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    int ret;
    struct timeval tv;
    int sockflag, optval;
    socklen_t optlen;
    struct linger lin;
	fd_set wfds;

    sockflag = fcntl(fd, F_GETFL, 0);
    if (sockflag == -1) {
        ret = -1;
        goto clean;
    }
    ret = fcntl(fd, F_SETFL, sockflag | O_NONBLOCK);
    if (ret) {
        ret = -1;
        goto clean;
    }

    ret = connect(fd, addr, addrlen);
    if (!ret) {
        goto connect_ok;
    }
    if (lwip_errno != EINPROGRESS) {
        ret = -1;
        goto clean;
    }

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    ret = select(fd+1, NULL, &wfds, NULL, &tv);
    if (ret != 1 || !FD_ISSET(fd, &wfds)) {
        ret = -1;
        goto clean;
    }
    optlen = sizeof(optval);
    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, (socklen_t *)&optlen);
    if (ret || optval) {
        goto clean;
    }

connect_ok:
    sockflag = fcntl(fd, F_GETFL, 0);
    if (sockflag == -1) {
        ret = -1;
        goto clean;
    }
    ret = fcntl(fd, F_SETFL, sockflag & (~O_NONBLOCK));
    if (ret) {
        ret = -1;
        goto clean;
    }

    optval = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    lin.l_onoff = 1;
    lin.l_linger = 0;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));

    return 0;
clean:
    return ret;
}

static int app_net_create(app_data_t *ad)
{
    struct sockaddr_in addr;
    struct sockaddr_in6 addr6;
    struct addrinfo hints, *result = NULL, *rp;
    struct timeval tv;
    int fd = -1, ret = -1;
    char buf[64];

    if (!ad->server[0] || !ad->port) {
        ret = -1;
        goto clean;
    }	

    memset(&addr, 0, sizeof(addr));
    memset(&addr6, 0, sizeof(addr6));
    if (inet_pton(AF_INET, ad->server, &addr.sin_addr) == 1) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            ret = -1;
            goto clean;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ad->port);
        ret = app_net_connect(ad, fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            goto clean;
        }
    } else if (inet_pton(AF_INET6, ad->server, &addr6.sin6_addr) == 1) {
        fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (fd < 0) {
            ret = -1;
            goto clean;
        }
        addr6.sin6_len = sizeof(addr6);
        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(ad->port);
        ret = app_net_connect(ad, fd, (struct sockaddr *)&addr6, sizeof(addr6));
        if (ret < 0) {
            goto clean;
        }
    } else {
        snprintf(buf, sizeof(buf), "%u", ad->port);

        memset(&hints, 0, sizeof(hints));
        hints.ai_flags = 0;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        ret = getaddrinfo(ad->server, buf, &hints, &result);
        if (ret) {
            ret = -1;
            goto clean;
        }
        for (rp = result; rp; rp = rp->ai_next) {
            fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (fd < 0) {
                continue;
            }
            ret = app_net_connect(ad, fd, rp->ai_addr, rp->ai_addrlen);
            if (ret < 0) {
                close(fd);
                fd = -1;
                continue;
            }
            break;
        }
        freeaddrinfo(result);
        result = NULL;

        if (fd < 0) {
            ret = -1;
            goto clean;
        }
    }

    ad->fd_sock = fd;
    ret = 0;	

clean:
    if (result)
        freeaddrinfo(result);
    if (ret < 0 && fd >= 0)
        close(fd);

    return ret;
}

static int app_net_recv(app_data_t *ad)
{
    int fd, ret, closing = 0;
    uint16_t sz;
    struct timeval tv;
    fd_set rfds;
    //char buf[2600];
    char buf[560] = {0};

    app_log_info("%s: +\n", __func__);

    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    if (ad->fd_sock < 0) {
        app_log_info("%s: sock closed\n", __func__);
        ret = -1;
        closing = 1;
        goto net_err;
    }
    fd = ad->fd_sock;
    OSAMutexUnlock(ad->mutex_ref);


    app_log_info("%s: WAIT\n", __func__);
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    ret = select(fd + 1, &rfds, NULL, NULL, NULL);
    if (ret <= 0 || !FD_ISSET(fd, &rfds)) {
        app_log_info("%s: select error: %d\n", __func__, ret);
        OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
        ret = -1;
        if (ad->fd_sock < 0) {
            app_log_info("%s: sock is closing when waiting\n", __func__);
            closing = 1;
        }
        goto net_err;
    }
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    if (ad->fd_sock < 0) {
        app_log_info("%s: sock is closing when waiting\n", __func__);
        ret = -1;
        closing = 1;
        goto net_err;
    }
    ret = ioctlsocket(ad->fd_sock, FIONREAD, &sz);
    if (ret < 0 || !sz) {
        app_log_info("%s: data is empty\n", __func__);
        ret = -1;
        goto net_err;
    }
    ret = recv(ad->fd_sock, buf, sizeof(buf)-1, 0);
    if (ret <= 0) {
        app_log_info("%s: recv error: %d\n", __func__, ret);
        ret = -1;
        goto net_err;
    }
    buf[ret] = '\0';
    app_log_info("%s: recv DATA: %d %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n", __func__, ret,
        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
        buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);

    if ((ad->wake_buf_len == 0 && ret > 0)
		&& !(ad->heart_resp_buf_len && ret >= ad->heart_resp_buf_len && !memcmp(buf, ad->heart_resp_buf, ad->heart_resp_buf_len))) {
        app_log_info("%s: anydata WAKE\n", __func__);
        ret = 0;
        goto net_wake;
    } else if (ad->wake_buf_len && ret >= ad->wake_buf_len && !memcmp(buf, ad->wake_buf, ad->wake_buf_len)) {
        app_log_info("%s: recv WAKE\n", __func__);
        ret = 0;
        goto net_wake;
    } else if (ad->heart_resp_buf_len && ret >= ad->heart_resp_buf_len && !memcmp(buf, ad->heart_resp_buf, ad->heart_resp_buf_len)) {
        app_log_info("%s: recv heart response\n", __func__);
        ret = 0;
		
		ad->heart_miss_count = 0;
        goto clean;
    } else if (ad->recv_error_count >= 3) {
        app_log_info("%s: recv error too much!!\n", __func__);
        ret = -1;
        goto net_stop;
    } else {
        app_log_info("%s: recv error!\n", __func__);
        ret = 0;
        ad->recv_error_count++;
    }

    goto clean;

net_wake:
	
    app_send_msg(APP_CMD_WAKEUP, 0, NULL);
    goto clean;
net_stop:
net_err:
    if (ad->fd_sock >= 0) {
        close(ad->fd_sock);
        ad->fd_sock = -1;
    }
    if (!closing) {
        ad->task_timeout = OSA_SUSPEND;
        app_send_msg(APP_CMD_STOP, 1, NULL);
    }
clean:
    OSAMutexUnlock(ad->mutex_ref);
    app_log_info("%s: -\n", __func__);
    return ret;
}

static void app_timeout(app_data_t *ad)
{
    int ret, i;

    app_log_info("%s: +\n", __func__);
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    if (ad->fd_sock < 0) {
        app_log_info("%s: sock closed\n", __func__);
        ad->task_timeout = OSA_SUSPEND;
        goto clean;
    }
	
	if (ad->heart_miss_max_count != 0 && ad->heart_miss_count >= ad->heart_miss_max_count){
        app_log_info("%s: heart_miss_count >= heart_miss_max_count\n", __func__);
        ad->task_timeout = OSA_SUSPEND;
        app_send_msg(APP_CMD_STOP, 0, NULL);
        goto clean;
	}

    for (i = 0; i < ad->send_retry_count; i++) {
        ret = app_net_send(ad, ad->heart_buf, ad->heart_buf_len);
        if (ret < 0) {
            OSATaskSleep(5*20); // 0.5s
            continue;
        }
        app_log_info("%s: heart OK\n", __func__);
        break;
    }

    if (i >= ad->send_retry_count) {
        app_log_info("%s: heart ERROR\n", __func__);
        ad->task_timeout = OSA_SUSPEND;
        app_send_msg(APP_CMD_STOP, 0, NULL);
        goto clean;
    }
	ad->heart_miss_count ++;
    ad->task_timeout = ad->heart_interval * 200;

clean:
    OSAMutexUnlock(ad->mutex_ref);
    app_log_info("%s: -\n", __func__);
}

static void app_start(app_data_t *ad)
{
    int ret, i, fd;

    app_log_info("%s: +\n", __func__);

    app_log_info("%s: server %s:%d\n", __func__, ad->server, ad->port);
    app_log_info("%s: heart_interval %d\n", __func__, ad->heart_interval);
    app_log_info("%s: heart_buf_len %u wake_buf_len %u login_buf_len %u login_resp_buf_len %u disable_usb %d\n", __func__, ad->heart_buf_len, ad->wake_buf_len, ad->login_buf_len, ad->login_resp_buf_len, ad->disable_usb);

	if (ad->wakeup_host)
	    app_set_mcu_wake(ad, 0);

    app_net_close(ad);

    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);

    for (i = 0; i < ad->connect_retry_count; i++) {
        ret = app_net_create(ad);
        if (ret < 0) {
            OSATaskSleep(5*20); // 0.5s
            app_log_info("%s: continue\n", __func__);
            continue;
        }
        break;
    }
    if (i >= ad->connect_retry_count) {
        goto init_err;
    }
    app_log_info("%s: connect OK, try init\n", __func__);

    ret = app_net_init(ad);
    if (ret < 0) {
        goto init_err;
    }
    app_log_info("%s: init OK\n", __func__);

    ad->recv_error_count = 0;

    OSAFlagSet(ad->flag_net_ref, 1, OSA_FLAG_OR);

    if (ad->login_buf_len)
        ad->task_timeout = 1 * 200; // send heart immediately, to make server immediately receive heart after connected.
    else
        ad->task_timeout = ad->heart_interval * 200;
	
    app_send_msg(APP_CMD_SUSPEND, 1, NULL);

    goto clean;

init_err:
    app_send_msg(APP_CMD_STOP, 1, NULL);
    goto clean;

clean:
    OSAMutexUnlock(ad->mutex_ref);
    app_log_info("%s: -\n", __func__);
}

static void app_stop(app_data_t *ad, int extra)
{
    int fd;
    app_log_info("%s: +\n", __func__);
    app_net_close(ad);
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    ad->task_timeout = OSA_SUSPEND;
    OSAMutexUnlock(ad->mutex_ref);
    app_send_msg(APP_CMD_WAKEUP, extra, NULL);
    app_log_info("%s: -\n", __func__);
}

static void app_suspend(app_data_t *ad, int extra)
{
    app_log_info("%s: +\n", __func__);
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    app_wakeup_internal(ad, 0, extra);
    OSAMutexUnlock(ad->mutex_ref);
    app_log_info("%s: -\n", __func__);
}

static void app_wakeup(app_data_t *ad, int extra)
{
    app_log_info("%s: +\n", __func__);
    OSAMutexLock(ad->mutex_ref, OSA_SUSPEND);
    app_wakeup_internal(ad, 1, extra);
    OSAMutexUnlock(ad->mutex_ref);
    app_log_info("%s: -\n", __func__);
}

static void app_test(app_data_t *ad)
{

}

static void app_net_task(void *param)
{
    app_data_t *ad = param;
    UINT32 flags;
    int ret;

    while (1) {
        ret = OSAFlagWait(ad->flag_net_ref, 0xffffffff, OSA_FLAG_OR_CLEAR, &flags, OSA_SUSPEND);
        ASSERT(ret == OS_SUCCESS);
        do {
            ret = app_net_recv(ad);
        } while (!ret);
    }
}

static void app_task(void *param)
{
    app_data_t *ad = param;
    int ret;
    app_msg_t msg;

    UINT32 flag;

    while (1) {
        memset(&msg, 0, sizeof(msg));
        ret = OSAMsgQRecv(ad->cmdq, (UINT8 *)&msg, APP_MESSAGE_Q_SIZE, ad->task_timeout);
        if (ret == OS_TIMEOUT) {
            app_timeout(ad);
        } else if (ret == OS_SUCCESS) {
            switch (msg.msgId) {
            case APP_CMD_START:
                app_log_info("%s: app_start\n", __func__);
                app_start(ad);
                break;
            case APP_CMD_STOP:
                app_log_info("%s: app_stop\n", __func__);
                app_stop(ad, msg.extra);
                break;
            case APP_CMD_SUSPEND:
                app_log_info("%s: app_suspend\n", __func__);
                app_suspend(ad, msg.extra);
                break;
            case APP_CMD_WAKEUP:
                app_log_info("%s: app_wakeup\n", __func__);
                app_wakeup(ad, msg.extra);
                break;

            case APP_CMD_TEST:
                app_test(ad);
            default:
                break;
            }
        } else {
            ASSERT(0);
        }
    }
}

void app_config(app_data_t *ad)
{
    memset(ad, 0, sizeof(*ad));
	
	memcpy(ad->server, "101.200.35.208", strlen("101.200.35.208"));
	
	ad->port = 8866;

	ad->heart_interval = 60; // uint: s

	memcpy(ad->login_buf, "login", strlen("login"));
	ad->login_buf_len = strlen("login");
	
	memcpy(ad->login_resp_buf, "login_resp", strlen("login_resp"));
	ad->login_resp_buf_len = strlen("login_resp");

	memcpy(ad->heart_buf, "heart", strlen("heart"));
	ad->heart_buf_len = strlen("heart");

	memcpy(ad->heart_resp_buf, "heart_resp", strlen("heart_resp"));
	ad->heart_resp_buf_len = strlen("heart_resp");
	
	memcpy(ad->wake_buf, "wakeup", strlen("wakeup"));
	ad->wake_buf_len = strlen("wakeup");

    ad->connect_retry_count = 3;
    ad->send_retry_count = 3;
    ad->recv_error_count = 0;

    ad->heart_miss_count = 0;	
    ad->heart_miss_max_count = 0;	
}

int app_init_2(void)
{
    app_data_t *ad;
    int ret;

    app_log_info("%s: +\n", __func__);

    ad = s_ad = malloc(sizeof(*s_ad));
    ASSERT(s_ad);
    memset(ad, 0, sizeof(*ad));

	app_config(ad);

    ad->task_timeout = OSA_SUSPEND;
    ad->fd_sock = -1;

	app_gpio_init();

    ret = OSAMutexCreate(&ad->mutex_ref, OS_PRIORITY);
    ASSERT(ret == OS_SUCCESS);

    ret = OSAFlagCreate(&ad->flag_net_ref);
    ASSERT(ret == OS_SUCCESS);
    ad->task_net_stack = malloc(APP_TASK_NET_STACK_SIZE);
    ASSERT(ad->task_net_stack);
    ret = OSATaskCreate(&ad->task_net_ref, ad->task_net_stack, APP_TASK_NET_STACK_SIZE, APP_TASK_NET_PRIORITY, "app_net_task", app_net_task, ad);
    ASSERT(ret == OS_SUCCESS);

    ret = OSAMsgQCreate(&ad->cmdq, "linkmsg", APP_MESSAGE_Q_SIZE, APP_MESSAGE_Q_MAX, OS_FIFO);
    ASSERT(ret == OS_SUCCESS);
    ad->task_stack = malloc(APP_TASK_STACK_SIZE);
    ASSERT(ad->task_stack);
    ret = OSATaskCreate(&ad->task_ref, ad->task_stack, APP_TASK_STACK_SIZE, APP_TASK_PRIORITY, "app_task", app_task, ad);
    ASSERT(ret == OS_SUCCESS);

	if (ad->wakeup_host)
	    app_set_mcu_wake(ad, 1);

    ad->inited = 1;

	do
	{
		int count = 0;

		while(!getCeregReady(isMasterSim0()? IND_REQ_HANDLE : IND_REQ_HANDLE_1)){

			OSATaskSleep(200);
			count++;
			if (count > 150){
				PM812_SW_RESET();
			}
		}
	}while(0);

    app_send_msg(APP_CMD_START, 0, NULL);
clean:
	
    app_log_info("%s: -\n", __func__);
	
    return 0;
}


