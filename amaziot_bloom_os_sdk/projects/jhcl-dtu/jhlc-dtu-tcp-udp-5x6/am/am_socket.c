//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_socket.c
// Auther      : zhaoning
// Version     :
// Date : 2023-8-28
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-8-28
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"

#include "am_socket.h"
#include "am.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_RECV_MAX_BUFF 2048 //socket revBuf length

// Private variables ------------------------------------------------------------

static struct sockaddr_in dtu_n_dest_addr;//for UDP param

// Public variables -------------------------------------------------------------

DTU_SOCKET_PARAM_T dtu_socket_group;

// Private functions prototypes -------------------------------------------------

static int dtu_socket_init(void *param);
static int dtu_socket_dl_data_recv(int fd,void *sock);
static void dtu_socket_dl_data_pre_check(int len , char *rcvdata);

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_socket_ctx
  * Description : 获取socket上下文指针
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_SOCKET_PARAM_T* dtu_get_socket_ctx(void)
{
    return &dtu_socket_group;
}

/**
  * Function    : dtu_set_socket_fn
  * Description : 
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_set_socket_fn(void)
{
    int errorCnt = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
//    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
    dtu_socket_group.socketType = dtu_file_ctx->socket.type;
    dtu_socket_group.remotePort = dtu_file_ctx->socket.port;
    memcpy(dtu_socket_group.remoteIp, dtu_file_ctx->socket.ip,60);
//    printf("%s[%d] ip:%s, port:%d", __FUNCTION__,__LINE__, dtu_socket_group.remoteIp, dtu_socket_group.remotePort);
    
    while(dtu_socket_init(&dtu_socket_group))
    {
        sleep(3);
        errorCnt++;
        dtu_socket_group.socketType = dtu_file_ctx->socket.type;
        dtu_socket_group.remotePort = dtu_file_ctx->socket.port;
        memcpy(dtu_socket_group.remoteIp, dtu_file_ctx->socket.ip, 60);
        if(errorCnt > 10)
        {
            //reset
            dtu_send_to_uart("\r\nConnect Server Error! Moudle Rebooting...\r\n", 45);
            msleep(1);
            PM812_SW_RESET();
        }
    }
    
//    printf("%s[%d] socket ready",__FUNCTION__,__LINE__);
}

/**
  * Function    : dtu_socket_init
  * Description : 初始化socket
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_socket_init(void *param)
{
    struct hostent* host_entry = NULL;
    int fd = 0;
    struct sockaddr_in server = {0};
    int res = -1;
    OSA_STATUS osaStatus = {0};
    DTU_SOCKET_RECV_TYPE_T rcvType = {0};
    int result = 0;
    
    DTU_SOCKET_PARAM_T *sock = (DTU_SOCKET_PARAM_T *)param;
//    printf("%s[%d] socketType = %d", __FUNCTION__, __LINE__, sock->socketType);

    if(sock->fd >0)
    {
        printf("%s[%d] create_req,  previous socket not closed!\n",__FUNCTION__, __LINE__);
    }
    
    host_entry = gethostbyname(sock->remoteIp);
    if (host_entry == NULL)
    {
        printf("%s[%d] socket error",__FUNCTION__, __LINE__);
        return -1;
    }
//    printf("%s[%d] DNS gethostbyname,Get %s ip %u.%u.%u.%u\n", __FUNCTION__, __LINE__, sock->remoteIp, host_entry->h_addr_list[0][0] & 0xff,
//    host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);

    //add UDP
    if(sock->socketType == 0)
        fd = socket(AF_INET, SOCK_STREAM, sock->socketType);
    else
        fd = socket(AF_INET, SOCK_DGRAM, sock->socketType);
    if(fd <= 0 )
    {
        printf("%s[%d] socket error",__FUNCTION__,__LINE__);
        return -1;
    }

    if(sock->socketType == 0)
    {
       server.sin_family = AF_INET;
       server.sin_port = htons(sock->remotePort);
       server.sin_addr.s_addr = *(UINT32 *) host_entry->h_addr_list[0];

        //sock-connect
        res = connect(fd, (struct sockaddr *)&server, sizeof(server));
        if(res < 0)
        {
            printf("%s[%d] connect error",__FUNCTION__, __LINE__);
            close(fd);
            return -1;
        }
    }
    else
    {
        dtu_n_dest_addr.sin_family = AF_INET;
        dtu_n_dest_addr.sin_port = htons(sock->remotePort);
        dtu_n_dest_addr.sin_len = sizeof(dtu_n_dest_addr);
        dtu_n_dest_addr.sin_addr.s_addr = *(UINT32 *) host_entry->h_addr_list[0];
    }

//    printf("%s[%d] fd: %d, dtu_socket_group.fd = %d",__FUNCTION__, __LINE__, fd, sock->fd);
    sock->fd = fd;
    rcvType.fd = sock->fd;
    rcvType.sock = sock;
//    printf("%s[%d] fd: %d, dtu_socket_group.fd = %d",__FUNCTION__, __LINE__, fd, sock->fd);
    //dtu_serial_mode = DTU_DATA_MODE;
    dtu_send_to_uart("\r\nCONNECT OK\r\n", 14);
    osaStatus = OSAMsgQSend(dtu_socket_group.dtu_msgq_socket_recv, sizeof(DTU_SOCKET_RECV_TYPE_T), (UINT8*)&rcvType, OSA_NO_SUSPEND);
    if(osaStatus == OS_SUCCESS)
    {
        result = 0;
    }
    else
    {
        printf("%s[%d] error!send msg failed! osaStatus:%d\n", __FUNCTION__, __LINE__, osaStatus);
        result = -1;
    }
    
    return result;
}

/**
  * Function    : dtu_socket_write
  * Description : socket发送
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_socket_write(int sockfd, const void *data, size_t data_sz)
{
    int err = 0;
    size_t cur = 0;
    
    while (cur < data_sz)
    {
        do{
            //judge type
            //int lwip_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
            //int lwip_write (int s, const void *dataptr, size_t size);
            if(dtu_socket_group.socketType == 0)
            {
                err = write(sockfd, (const char *)data + cur, data_sz - cur);
            }
            else
            {
                err = sendto(sockfd, (const char *)data + cur, data_sz - cur, 0, (struct sockaddr *)&dtu_n_dest_addr, dtu_n_dest_addr.sin_len);
            }
        }while (err < 0 && errno == EINTR);
        if (err <= 0)
            return -1;
        cur += err;
    }
    
    return 0;
}


/**
  * Function    : dtu_send_serial_data_to_server
  * Description : 数据发送到服务器
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_send_serial_data_to_server(DTU_MSG_UART_DATA_PARAM_T * uartData)
{
    UINT16 link_size = 0;
    char* hex_data = NULL;
    uint32_t send_size = 0;
    char *data = NULL;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    DTU_TIME_PARAM_T* dtu_timer_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    dtu_timer_ctx = dtu_get_timer_ctx();
    
//    printf("%s[%d]: DTU_DATA_MODE recv len: %d\n", __FUNCTION__, __LINE__, uartData->len);
    
    if(dtu_socket_group.fd)
    {
        //判断是否设置了上报心跳，设置后，关闭心跳定时器
        if(dtu_file_ctx->hb.heartflag)
            OSATimerStop(dtu_timer_ctx->dtu_timer_ref);

        //发送长度
        send_size = uartData->len;
        //判断linkflag，是否需要拼接注册包
        if(dtu_file_ctx->reg.linkflag == 2 || dtu_file_ctx->reg.linkflag == 3)
        {
            if(dtu_file_ctx->reg.linktype == 0)
            {
                link_size = strlen(dtu_file_ctx->reg.link);
            }
            else if(dtu_file_ctx->reg.linktype == 1)
            {
                link_size = strlen(dtu_file_ctx->reg.link) / 2;
                hex_data = malloc(link_size);
                utils_ascii_str2hex(0, hex_data, dtu_file_ctx->reg.link, strlen(dtu_file_ctx->reg.link));
            }
            send_size += link_size;
        }
        data = malloc(send_size + 1);
        if (data == NULL)
        {
            printf("%s[%d] malloc error, send_size:%d", __FUNCTION__, __LINE__, send_size);
            return;
        }
        memset(data, 0, send_size + 1);
        //add link data
        if(send_size > uartData->len)
        {
            if(dtu_file_ctx->reg.linktype == 0)
            {
                memcpy(data, dtu_file_ctx->reg.link, link_size);
            }
            else if(dtu_file_ctx->reg.linktype == 1)
            {
                memcpy(data, hex_data, link_size);
            }
            
            memcpy(data + link_size, uartData->UArgs, uartData->len);
        }
        else
        {
            memcpy(data, uartData->UArgs, uartData->len);
        }
        
        printf("%s[%d] senddata len: %d\n", __FUNCTION__, __LINE__, send_size);
        //数据发送到服务器
        dtu_socket_write(dtu_socket_group.fd,(void *)data, send_size);

        //释放内存
        if(NULL != hex_data)
        {
            free(hex_data);
            hex_data = NULL;
        }
        if(NULL != data)
        {
            free(data);
            data = NULL;
        }
        //判断标志，是否需要重新打开心跳定时器
        if(dtu_file_ctx->hb.heartflag)
        {
            OSATimerStart(dtu_timer_ctx->dtu_timer_ref, dtu_file_ctx->hb.hearttime * 200, dtu_file_ctx->hb.hearttime * 200 , dtu_hb_timer_callback, 0);
        }
    }
}

/**
  * Function    : dtu_sockrcv_thread
  * Description : socket下行任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_sockrcv_thread(void)
{
    DTU_SOCKET_RECV_TYPE_T sock_temp = {0};
    int rcv = 0;
//    printf("%s[%d]",__FUNCTION__,__LINE__);
    
    while(1)
    {
        //清除套接字信息
        memset(&sock_temp, 0, sizeof(DTU_SOCKET_RECV_TYPE_T));
        //等待底层来的消息
        OSAMsgQRecv(dtu_socket_group.dtu_msgq_socket_recv, (UINT8 *)&sock_temp, sizeof(DTU_SOCKET_RECV_TYPE_T), OSA_SUSPEND);
        printf("%s[%d] fd:%d",__FUNCTION__,__LINE__,sock_temp.fd);
        //读取下行的数据
        rcv = dtu_socket_dl_data_recv(sock_temp.fd, (void *)&sock_temp);
        //错误处理
        if(-1 == rcv)
        {
            printf("%s[%d] restart socket",__FUNCTION__,__LINE__);
            if(dtu_socket_group.fd)
            {
                close(dtu_socket_group.fd);
                dtu_socket_group.fd = 0;
            }
        }
        printf("%s[%d] sock err.",__FUNCTION__,__LINE__);
    }
}

/**
  * Function    : dtu_socket_dl_data_recv
  * Description : 监控socket下行
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_socket_dl_data_recv(int fd,void *sock)
{
    char buf[DTU_RECV_MAX_BUFF] = {0};
    int bytes = 0;
    fd_set master = {0};
    fd_set read_fds = {0};

    struct timeval timeout = {0};
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    int status = 0;
    
    DTU_SOCKET_RECV_TYPE_T *sockparam = (DTU_SOCKET_RECV_TYPE_T *)sock;
    printf("%s[%d]",__FUNCTION__,__LINE__);

    while(1)
    {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        status = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if(status < 0)
        {
            printf("%s[%d] socket select fail",__FUNCTION__,__LINE__);
            sleep(1);
            continue;
        }
        else if(status == 0)
        {
            printf("%s[%d] socket select timeout",__FUNCTION__,__LINE__);
            sleep(1);
            continue;
        }

        if(dtu_socket_group.fd == 0)
            goto PRO_FAIL;

        if(FD_ISSET(fd, &read_fds)  >= 0)
        {
            memset(buf, 0, DTU_RECV_MAX_BUFF);
            //int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
            //int lwip_recv    (int s, void *mem, size_t len, int flags);
            if(dtu_socket_group.socketType == 0)
                bytes = recv(fd, buf, DTU_RECV_MAX_BUFF, 0);
            else
                bytes = recvfrom(fd, buf, DTU_RECV_MAX_BUFF, 0, (struct sockaddr *)&dtu_n_dest_addr, (socklen_t *)&dtu_n_dest_addr.sin_len);
            if(bytes <= 0) 
            {
                sleep(1);
                goto PRO_FAIL;
            }
            printf("%s[%d] transport mode,recv data length %d",__FUNCTION__, __LINE__, bytes);
            char *sendData = (char *)malloc(bytes + 1);
            ASSERT(sendData != NULL);
            memcpy(sendData, buf, bytes);
            sendData[bytes] = '\0';
            //判断是否是网络AT指令
            dtu_socket_dl_data_pre_check(bytes,sendData);
            
            if(NULL != sendData)
            {
                free(sendData);
                sendData = NULL;
            }
        }
    }

PRO_FAIL:
    //close(fd);
    printf("%s[%d] socket close", __FUNCTION__,__LINE__);
    //sendIndData(TEL_AT_CMD_ATP_7, "\r\nSOCKET CLOSE\r\n", 16);
    return -1;
}

/**
  * Function    : dtu_socket_dl_data_pre_check
  * Description : 判断下行数据类型，网络AT指令调用函数处理；透传数据直接发送到串口
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_socket_dl_data_pre_check(int len , char *rcvdata)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    printf("%s[%d] len:%d",__FUNCTION__,__LINE__,len);
    //判断数据最前几个字节是否是网络at指令标志
    if(is_begin_with(rcvdata,dtu_file_ctx->net_at.cmdpw) == 1 && strstr((const char*)rcvdata,"\r\n") != NULL)
    {
        //执行网络at
        dtu_process_at_cmd_mode(rcvdata, len);
    }
    else
    {
        //发送数据到串口
        dtu_send_to_uart(rcvdata, len);
    }
}

// End of file : am_socket.c 2023-8-28 10:59:53 by: zhaoning 

