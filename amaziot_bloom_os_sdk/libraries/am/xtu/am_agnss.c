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

#include "am_agnss.h"

#include "sockets.h"
#include "ip_addr.h"
#include "netdb.h"

// Private defines / typedefs ---------------------------------------------------

#define DTU_RECV_MAX_BUFF 2048 //socket revBuf length

// Private variables ------------------------------------------------------------

static struct sockaddr_in dtu_n_dest_addr;//for UDP param
static UINT8 dtu_agnss_task_stack[DTU_AGNSS_TASK_STACK_SIZE];
static OSTaskRef dtu_agnss_task_ref = NULL;
static UINT8 dtu_agnss_dl_task_stack[DTU_AGNSS_TASK_STACK_SIZE];
static OSTaskRef dtu_agnss_dl_task_ref = NULL;

static DTU_SOCKET_PARAM_T dtu_agnss_socket_group;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static int dtu_agnss_socket_init(void *param);
static int dtu_socket_dl_data_recv(int fd,void *sock);

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
//DTU_SOCKET_PARAM_T* dtu_get_socket_ctx(void)
//{
//    return &dtu_agnss_socket_group;
//}

/**
  * Function    : dtu_agnss_socket_init
  * Description : 初始化socket
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_agnss_socket_init(void *param)
{
    struct hostent* host_entry = NULL;
    int fd = 0;
    struct sockaddr_in server = {0};
    int res = -1;
    OSA_STATUS osaStatus = {0};
    DTU_SOCKET_RECV_TYPE_T rcvType = {0};
    int result = 0;
    
    DTU_SOCKET_PARAM_T *sock = (DTU_SOCKET_PARAM_T *)param;
//    uprintf("%s[%d] socketType = %d", __FUNCTION__, __LINE__, sock->socketType);

    if(sock->fd >0)
    {
        uprintf("%s[%d] create_req,  previous socket not closed!\n",__FUNCTION__, __LINE__);
    }
    
    host_entry = gethostbyname(sock->remoteIp);
    if (host_entry == NULL)
    {
        uprintf("%s[%d] socket error",__FUNCTION__, __LINE__);
        return -1;
    }
//    uprintf("%s[%d] DNS gethostbyname,Get %s ip %u.%u.%u.%u\n", __FUNCTION__, __LINE__, sock->remoteIp, host_entry->h_addr_list[0][0] & 0xff,
//    host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);

    //add UDP
    if(sock->socketType == 0)
        fd = socket(AF_INET, SOCK_STREAM, sock->socketType);
    else
        fd = socket(AF_INET, SOCK_DGRAM, sock->socketType);
    if(fd <= 0 )
    {
        uprintf("%s[%d] socket error",__FUNCTION__,__LINE__);
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
            uprintf("%s[%d] connect error",__FUNCTION__, __LINE__);
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

//    uprintf("%s[%d] fd: %d, dtu_agnss_socket_group.fd = %d",__FUNCTION__, __LINE__, fd, sock->fd);
    sock->fd = fd;
    rcvType.fd = sock->fd;
    rcvType.sock = sock;
    uprintf("%s[%d] fd: %d, agnss connect succ fd = %d",__FUNCTION__, __LINE__, fd, sock->fd);
    //dtu_serial_mode = DTU_DATA_MODE;
//    dtu_send_to_uart("\r\nCONNECT OK\r\n", 14);
    osaStatus = OSAMsgQSend(dtu_agnss_socket_group.dtu_msgq_socket_recv, sizeof(DTU_SOCKET_RECV_TYPE_T), (UINT8*)&rcvType, OSA_NO_SUSPEND);
    if(osaStatus == OS_SUCCESS)
    {
        result = 0;
    }
    else
    {
        uprintf("%s[%d] error!send msg failed! osaStatus:%d\n", __FUNCTION__, __LINE__, osaStatus);
        result = -1;
    }
    
    return result;
}

/**
  * Function    : dtu_agnss_socket_write
  * Description : socket发送
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int dtu_agnss_socket_write(const void *data, size_t data_sz)
{
    int err = 0;
    size_t cur = 0;

    if(0 == dtu_agnss_socket_group.fd)
    {
        uprintf("%s[%d] socket fd err", __FUNCTION__, __LINE__);
        return -2;
    }
    
    while (cur < data_sz)
    {
        do{
            //judge type
            //int lwip_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
            //int lwip_write (int s, const void *dataptr, size_t size);
            if(dtu_agnss_socket_group.socketType == 0)
            {
                err = write(dtu_agnss_socket_group.fd, (const char *)data + cur, data_sz - cur);
            }
            else
            {
                err = sendto(dtu_agnss_socket_group.fd, (const char *)data + cur, data_sz - cur, 0, (struct sockaddr *)&dtu_n_dest_addr, dtu_n_dest_addr.sin_len);
            }
        }while (err < 0 && errno == EINTR);
        
        if (err <= 0)
        {
            uprintf("%s[%d] socket send err", __FUNCTION__, __LINE__);
            return -1;
        }
        
        cur += err;
    }
    
    return 0;
}

/**
  * Function    : dtu_agnss_sockrcv_thread
  * Description : socket下行任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_agnss_sockrcv_thread(void)
{
    DTU_SOCKET_RECV_TYPE_T sock_temp = {0};
    int rcv = 0;
//    uprintf("%s[%d]",__FUNCTION__,__LINE__);
    
    while(1)
    {
        //清除套接字信息
        memset(&sock_temp, 0, sizeof(DTU_SOCKET_RECV_TYPE_T));
        //等待底层来的消息
        OSAMsgQRecv(dtu_agnss_socket_group.dtu_msgq_socket_recv, (UINT8 *)&sock_temp, sizeof(DTU_SOCKET_RECV_TYPE_T), OSA_SUSPEND);
        uprintf("%s[%d] fd:%d",__FUNCTION__,__LINE__,sock_temp.fd);
        //读取下行的数据
        rcv = dtu_socket_dl_data_recv(sock_temp.fd, (void *)&sock_temp);
        //错误处理
        if(-1 == rcv)
        {
            uprintf("%s[%d] restart socket",__FUNCTION__,__LINE__);
            if(dtu_agnss_socket_group.fd)
            {
                close(dtu_agnss_socket_group.fd);
                dtu_agnss_socket_group.fd = 0;
            }
        }
        uprintf("%s[%d] sock err.",__FUNCTION__,__LINE__);
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
    uprintf("%s[%d]",__FUNCTION__,__LINE__);

    while(1)
    {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        status = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if(status < 0)
        {
            uprintf("%s[%d] socket select fail",__FUNCTION__,__LINE__);
            dtu_sleep(1);
            continue;
        }
        else if(status == 0)
        {
            uprintf("%s[%d] socket select timeout",__FUNCTION__,__LINE__);
            dtu_sleep(1);
            continue;
        }

        if(dtu_agnss_socket_group.fd == 0)
            goto PRO_FAIL;

        if(FD_ISSET(fd, &read_fds)  >= 0)
        {
            memset(buf, 0, DTU_RECV_MAX_BUFF);
            //int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
            //int lwip_recv    (int s, void *mem, size_t len, int flags);
            if(dtu_agnss_socket_group.socketType == 0)
                bytes = recv(fd, buf, DTU_RECV_MAX_BUFF, 0);
            else
                bytes = recvfrom(fd, buf, DTU_RECV_MAX_BUFF, 0, (struct sockaddr *)&dtu_n_dest_addr, (socklen_t *)&dtu_n_dest_addr.sin_len);
            if(bytes <= 0) 
            {
                dtu_sleep(1);
                goto PRO_FAIL;
            }
            uprintf("%s[%d] transport mode,recv data length %d",__FUNCTION__, __LINE__, bytes);

            //发送数据到芯片
        }
    }

PRO_FAIL:
    //close(fd);
    uprintf("%s[%d] socket close", __FUNCTION__,__LINE__);
    //sendIndData(TEL_AT_CMD_ATP_7, "\r\nSOCKET CLOSE\r\n", 16);
    return -1;
}

/**
  * Function    : dtu_agnss_thread
  * Description : 维护驻网任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_agnss_thread(void * argv)
{
    //获取agps数据，发送报文
    char data_test[128] = {0};
    
    dtu_agnss_socket_group.socketType = 0;
    dtu_agnss_socket_group.remotePort = 2621;
    memcpy(dtu_agnss_socket_group.remoteIp, "121.41.40.95",strlen("121.41.40.95"));
//    uprintf("%s[%d] ip:%s, port:%d", __FUNCTION__,__LINE__, dtu_agnss_socket_group.remoteIp, dtu_agnss_socket_group.remotePort);
    //初始化socket，连接中科微agnss服务器
    dtu_agnss_socket_init(&dtu_agnss_socket_group);

    //拼接发送到服务器的数据，注意经纬度需要所需定位地点的粗略经纬度
    strcat(data_test, "user=fengyihuan@hzzkw.com;pwd=feixiang;cmd=full;lat=");
    strcat(data_test, "38.06362");
    strcat(data_test, ";lon=");
    strcat(data_test, "114.51092");
    strcat(data_test, ";alt=0\r\n");

    uprintf( "[TCP] send data: %s", data_test)
    //数据发送到服务器
    dtu_agnss_socket_write((void *)data_test, strlen(data_test));

    while (1)
    {
        //
    }

}

/**
  * Function    : dtu_agnss_sokcet_task_init
  * Description : socket任务，以及相关资源初始化
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_agnss_sokcet_task_init(void)
{
    OSA_STATUS status = 0;
    
    //创建agnss主任务，包括网络连接，发送数据给服务器
    status = OSATaskCreate(&dtu_agnss_task_ref, dtu_agnss_task_stack, DTU_AGNSS_TASK_STACK_SIZE, 150, "agnss_task", dtu_agnss_thread, NULL);
    ASSERT(status == OS_SUCCESS);

    //下行数据接收线程
    status = OSATaskCreate(&dtu_task_ref, dtu_task_stack, DTU_DEFAULT_THREAD_STACKSIZE * 5, 150, "dtu_agnss_sockrcv_thread", dtu_agnss_sockrcv_thread, NULL);
    ASSERT(status == OS_SUCCESS);
}

// End of file : am_socket.c 2023-8-28 10:59:53 by: zhaoning 

