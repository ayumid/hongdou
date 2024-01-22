//------------------------------------------------------------------------------
// Copyright , 2017-2021 奇迹物联（北京）科技有限公司
// Filename    : main.c
// Auther      : zhaoning
// Version     :
// Date : 2023-5-17
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-5-17
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "osa.h"
#include "UART.h"
#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"
#include "sys.h"
#include "sdk_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_usb_uart_printf(fmt, args...) \
    do                                \
    {                                 \
        sdklogConfig(1);              \
        sdkLogPrintf(fmt, ##args);    \
    } while (0)
// CATStudio usb log
#define sample_usb_catstudio_printf(fmt, args...) \
    do                                 \
    {                                  \
        sdklogConfig(0);               \
        sdkLogPrintf(fmt, ##args);     \
    } while (0)

#define UPCASE(c) (((c) >= 'a' && (c) <= 'z') ? ((c)-0x20) : (c))
#define sample_usb_sleep(x) OSATaskSleep((x)*200) // second

#define SAMPLE_TASK_STACK_SIZE 1024 * 8

typedef struct
{
    int len;
    UINT8 *UArgs;
} usbParam;

// Private variables ------------------------------------------------------------

OSMsgQRef sample_usb_msgq = NULL;

static UINT8 sample_usb_task_stack[SAMPLE_TASK_STACK_SIZE];
static OSTaskRef sample_usb_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_usb_thread(void *param);

// Public functions prototypes --------------------------------------------------

// Device bootup hook before Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_enter(void);
// Device bootup hook after Phase1Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase1Inits_exit(void);
// Device bootup hook before Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_enter(void);
// Device bootup hook after Phase2Inits.
// If you have some work to be init, you may implete it here.
// ex: you may start your task here. or do some initialize here.
extern void Phase2Inits_exit(void);

// Functions --------------------------------------------------------------------

void sample_usb_connect_status_cbk(UINT8 status)
{
    sample_usb_uart_printf("%s[%d]: status %d\n", __FUNCTION__, __LINE__, status);
}

void Phase1Inits_enter(void)
{
    USB_SET_DISABLE_RNDIS_FLAG(1); // disable the usb rndis
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    OSA_STATUS status;

    USB_REGISTER_CONNECT_CB(sample_usb_connect_status_cbk); // 注册USB插拔回调函数

    sample_usb_uart_printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&sample_usb_msgq, "sample_usb_msgq", sizeof(usbParam), 300, OS_FIFO);
    ASSERT(status == OS_SUCCESS);

    status = OSATaskCreate(&sample_usb_task_ref, sample_usb_task_stack, SAMPLE_TASK_STACK_SIZE, 161, "sample_usb_thread", sample_usb_thread, NULL);
    ASSERT(status == OS_SUCCESS);
}

void sample_usb_oem_usb_rcv_cbk(UINT8 *recv_data, UINT32 recv_len)
{
    usbParam usb_data = {0};
    OSA_STATUS osa_status;

    char *tempbuf = (char *)malloc(recv_len + 10);
    memset(tempbuf, 0x0, recv_len + 10);
    memcpy(tempbuf, (char *)recv_data, recv_len);

    usb_data.UArgs = (UINT8 *)tempbuf;
    usb_data.len = recv_len;

    osa_status = OSAMsgQSend(sample_usb_msgq, sizeof(usbParam), (UINT8 *)&usb_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}

#define OEM_USB_MODEM 0
#define OEM_USB_AT 1

static void sample_usb_thread(void *param)
{
    usbParam usb_temp;
    OSA_STATUS status;

    OEM_USB_OPEN(OEM_USB_AT, sample_usb_oem_usb_rcv_cbk); // 0： modem port， 1： AT port，

    sample_usb_uart_printf("%s\n", __FUNCTION__);

    while (1)
    {
        memset(&usb_temp, 0, sizeof(usbParam));

        status = OSAMsgQRecv(sample_usb_msgq, (UINT8 *)&usb_temp, sizeof(usbParam), OSA_SUSPEND); // recv data from usb

        if (status == OS_SUCCESS)
        {
            if (usb_temp.UArgs)
            {
                sample_usb_uart_printf("%s[%d]: usb_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, usb_temp.len, (char *)(usb_temp.UArgs));

                OEM_USB_SEND_DATA(OEM_USB_AT, (UINT8 *)usb_temp.UArgs, usb_temp.len);

                free(usb_temp.UArgs);
            }
        }
    }
}

// End of file : main.c 2023-5-17 9:22:38 by: zhaoning 

