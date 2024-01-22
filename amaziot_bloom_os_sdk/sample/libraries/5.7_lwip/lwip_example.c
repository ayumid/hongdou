#include "duster.h"
#include "stats.h"
#include "snmp.h"
#include "sys_arch.h"
#include "etharp.h"
#include "ethip6.h"
#include "ip4.h"
#include "lwip_stats.h"
#include "dialer_task.h"
#include "lwip_api.h"
#include "bsp.h"
#include "netifapi.h"
#include "ping.h"
#include "tcp.h"
#include "tcp_impl.h"
#include "ftp_client.h"
#include "mfg_tool.h"
#include "lwip_customer.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
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

#define SPI_HEADER_LEN    0

/*external var declare here*/
extern GlobalParameter_Duster2Dialer gUI2DialerPara;
extern GlobalParameter_Dialer2Duster gDialer2UIPara;
extern u8_t current_PP;
extern u32_t g_server_ip;

static struct netif netif_example = {0};
static statisticsData *st_netif_example = &(gDialer2UIPara.USB); // or &(gDialer2UIPara.WLAN);

/*static function define here*/

static inline void netif_set_tx_info(statisticsData *ptr, u32_t len)
{
    if ((ptr == NULL) || (len == 0))
    {
        return;
    }
    ptr->TX_Packets++;
    ptr->TX_Bytes_Calc += len;
    ptr->TX_Bytes += ptr->TX_Bytes_Calc >> 20;         /*turn bytes to Mbytes*/
    ptr->TX_Bytes_Calc = ptr->TX_Bytes_Calc & 0xFFFFF; /*get leave bytes after turn up to Mbytes*/
}

static inline void netif_set_rx_info(statisticsData *ptr, u32_t len)
{
    if ((ptr == NULL) || (len == 0))
    {
        return;
    }
    ptr->RX_Packets++;
    ptr->RX_Bytes_Calc += len;
    ptr->RX_Bytes += ptr->RX_Bytes_Calc >> 20;         /*turn bytes to Mbytes*/
    ptr->RX_Bytes_Calc = ptr->RX_Bytes_Calc & 0xFFFFF; /*get leave bytes after turn up to Mbytes*/
}

static inline void netif_set_tx_error_info(statisticsData *ptr, u32_t len)
{
    if ((ptr == NULL) || (len == 0))
    {
        return;
    }
    ptr->tx_error_Packets++;
    ptr->tx_error_Bytes += len;
    ptr->tx_error_MBytes += ptr->tx_error_Bytes >> 20;   /*turn bytes to Mbytes*/
    ptr->tx_error_Bytes = ptr->tx_error_Bytes & 0xFFFFF; /*get leave bytes after turn up to Mbytes*/
}

static inline void netif_set_rx_error_info(statisticsData *ptr, u32_t len)
{
    if ((ptr == NULL) || (len == 0))
    {
        return;
    }
    ptr->error_Packets++;
    ptr->error_Bytes_Calc += len;
    ptr->error_Bytes += ptr->error_Bytes_Calc >> 20;         /*turn bytes to Mbytes*/
    ptr->error_Bytes_Calc = ptr->error_Bytes_Calc & 0xFFFFF; /*get leave bytes after turn up to Mbytes*/
}

/***************************************/
/* example code for new ethernet netif add*/
/***************************************/
/* Copy data from driver to lwip pbuf */
static struct pbuf *netif_example_copy_data(u8_t *data, u32_t len)
{
    struct pbuf *p = NULL;

    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);

    if (p != NULL)
    {
        MEMCPY(p->payload, data, p->len);
        LINK_STATS_INC(link.recv);
    }
    else
    {
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        catstudio_printf("lwiperr: %s, pbuf_alloc failed!", __FUNCTION__);
    }

    return p;
}

/**
 *function: netif_example_GetPacket
 *description: this function called by linklayer driver
 *input:
 *   u8_t *data: data pointer, with ethernet header first
 *   u32_t len: data len.
 **/
err_t netif_example_GetPacket(u8_t *data, u32_t len)
{
    struct pbuf *p = NULL;
    struct netif *netif = &netif_example;
    err_t ret = ERR_OK;

    if ((data == NULL) || (len == 0))
    {
        catstudio_printf("lwiperr: %s, param check error!", __FUNCTION__);
        return ERR_MEM;
    }

    ret = netif_ready(netif);
    if (ret != ERR_OK)
    {
        catstudio_printf("lwiperr: %s, netif not ready", __FUNCTION__);
        goto _get_packet_in_ret;
    }

    /*do pre check, keep it*/
    ret = tcpip_input_eth_check(data, len, netif);
    if (ERR_OK != ret)
    {
        goto _get_packet_in_ret;
    }

    /*copy data to pbuf, then free data outside */
    p = netif_example_copy_data(data, len);
    if (p)
    {
        ret = netif->input(p, netif);
    }
    else
    {
        ret = ERR_BUF;
    }

_get_packet_in_ret:

    if (ERR_OK == ret)
    {
        netif_set_rx_info(st_netif_example, len);
    }
    else
    {
        netif_set_rx_error_info(st_netif_example, len);
    }

    return ret;
}

/**
 *function: netif_example_SendPacket
 *description: this function called by lwip, provider by linklayer driver
 *input:
 *   u8_t *data: data pointer, with ethernet header first, and with space for linklayer
 *   u32_t len: data len.
 **/
void netif_example_SendPacket(u8_t *data, u32_t len)
{    
    extern void enc28j60_tx_work_handler(u8_t *data, u32_t len);
    
    enc28j60_tx_work_handler(data, len);
}

static err_t netif_example_low_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = NULL;
    u8_t *buf = NULL;
    u8_t *payload = NULL;
    u16_t len = 0;
    u16_t size = 0;
    err_t ret = ERR_OK;

    if (p)
    {
        size = p->tot_len;
    }

    if (NULL == netif || p == NULL)
    {
        catstudio_printf("lwiperr: %s, netif is null", __FUNCTION__);
        ret = ERR_IF;
        goto _low_output_ret;
    }

    if (!netif_is_up(netif))
    {
        catstudio_printf("lwiperr: %s, netif is down after retry", __FUNCTION__);
        ret = ERR_IF;
        goto _low_output_ret;
    }

    catstudio_printf("lwip: netif_%s low output,pbuf=%lx,payload=%lx,pkt_mac=%lx,tot_len=%d,len=%d,type=%d",
                     netif->hostname, p, p->payload, p->pkt_mac, p->tot_len, p->len, p->type);

    /* reshape pbuf for pc output, keep it here. */
    p = ethernet_dl_pbuf_reshape(netif, p);
    if (NULL == p)
    {
        ret = ERR_ARG;
        goto _low_output_ret;
    }

    /*now p->payload pointer to ethernet header pointer, need do copy, call low send api to send data*/
    len = p->tot_len + SPI_HEADER_LEN; // RNDIS_HEADER_SIZE just set length for rndis header size, need modify by customer
    buf = mem_malloc(len);                // mem_free
    if (buf == NULL)
    {
        ret = ERR_MEM;
        catstudio_printf("lwiperr: lwip -> usb buf null");
        goto _low_output_ret;
    }

    payload = buf + SPI_HEADER_LEN;
    for (q = p; q != NULL; q = q->next)
    {
        MEMCPY(payload, q->payload, q->len);
        payload += q->len;
    }

    /*linklayer send packet functon, customer define*/
    netif_example_SendPacket(buf, len);

    lwip_tcpip_unlock();
    LINK_STATS_INC(link.xmit);

_low_output_ret:
    if (ret != ERR_OK)
    {
        netif_set_tx_error_info(st_netif_example, size);
    }
    else
    {
        netif_set_tx_info(st_netif_example, size);
    }
    return ret;
}

static err_t netif_example_init(struct netif *netif)
{
    LWIP_ASSERT_NOW((netif != NULL));

    /* Initialize interface hostname */
    netif->hostname = "example";

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100 * 1024 * 1024);

    netif->name[0] = 'e';
    netif->name[1] = 'x';
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output; // no modiify
    netif->linkoutput = netif_example_low_output;
    netif->output_ip6 = ethip6_output; // no modiify

    /* initialize the hardware */
    u8_t s1 = sys_gen_rand() & 0xFF;
    u8_t s2 = sys_gen_rand() & 0xFF;
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = s1;
    netif->hwaddr[2] = s2;
    netif->hwaddr[3] = s2;
    netif->hwaddr[4] = s2;
    netif->hwaddr[5] = s2;

    netif->mtu = 1500;
    netif->mtu6 = 1500;
    netif->if_in_pkts = 0;
    netif->if_out_pkts = 0;
    netif->if_in_discards = 0;
    netif->if_out_discards = 0;

    netif_set_ip6_linklocal(netif, 0, htonl(0x1234));

    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST |
                   NETIF_FLAG_ETHARP |
                   NETIF_FLAG_LINK_UP |
                   NETIF_FLAG_INTER |
                   NETIF_FLAG_INIT_DONE |
                   NETIF_FLAG_ETHERNET |
                   NETIF_FLAG_IF_USB | // NETIF_FLAG_IF_USB denote wired. NETIF_FLAG_IF_UAP denote wireless.
                   NETIF_FLAG_STATIC_MEM;

    return ERR_OK;
}

void netifapi_example_setup(void)
{
    struct ip_addr ip, mask, gw;

    /*for mifi: IP_192_168_ADDR, for dongle: IP_169_254_ADDR*/
    ip.addr = htonl(g_server_ip);

    IP4_ADDR(&mask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);

    netifapi_netif_add(&netif_example, &ip, &mask, &gw, NULL, netif_example_init, ethernet_netif_input);
    netifapi_netif_set_down(&netif_example);
}

void netifapi_example_netif_set_up(void)
{
    catstudio_printf("enc28j60 netifapi_example_netif_set_up");
    netifapi_netif_set_up(&netif_example);
}

void netifapi_example_netif_set_down(void)
{
    catstudio_printf("enc28j60 netifapi_netif_set_down");
    netifapi_netif_set_down(&netif_example);
}
