
/** 
 * @file        ENC28J60.c
 * @brief                
 * @details    Project    : BHS-Ethernet 
 * 
 * 
 * @author    Litang
 * @date        1/12/2019
 * @copyright    MICC Tech All rights reserved
 * 
 ************************************************************** 
 * @par
 * 
 * 
 ************************************************************** 
 */
 
 
//===============================INCLUDE==========================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "ql_spi_api.h"
#include "SPI_interface.h"
#include "enc28j60.h"
#include "netif.h"
#include "err.h"
#include "cgpio.h"
#include "UART.h"
#include "lwip_example.h"

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
//#define enc28j60_debug(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define enc28j60_debug(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)
    
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef void (*mutexlock)(void);


//================================================================
#define SPI_OPLEN    1

#define SPI_TRANSFER_BUF_LEN    (4 + MAX_FRAMELEN)

/* Max TX retries in case of collision as suggested by errata datasheet */
#define MAX_TX_RETRYCOUNT    16

enum {
    RXFILTER_NORMAL,
    RXFILTER_MULTI,
    RXFILTER_PROMISC
};

struct enc28j60_net {
    //struct sk_buff *tx_skb;
    //struct work_struct tx_work;
    //struct work_struct irq_work;
    //struct work_struct setrx_work;
    //struct work_struct restart_work;
    
    u8 bank;        /* current register bank selected */
    u16 next_pk_ptr;    /* next packet pointer within FIFO */
    u16 max_pk_counter;    /* statistics: max packet counter */
    u16 tx_retry_count;
    bool hw_enable;
    bool full_duplex;
    int rxfilter;
    u32 msg_enable;
    u8 spi_transfer_buf[SPI_TRANSFER_BUF_LEN];

    struct netif *netif;

    u32 rx_frame_errors;

    u32 rx_packets;
    u64 rx_bytes;
    
    u32 tx_errors;
    u32 tx_packets;
    u64 tx_last_bytes;
    u64 tx_bytes;
};

//=========================CONST DEFINITIONS======================
/* Driver local data */


struct enc28j60_net enc28j60_info;

#define DEBUG_ONFF (1)

#define CONFIG_ENC28J60_WRITEVERIFY
//================================================================


//=========================VARIABLE DECLARATIONS==================


//================================================================

#define ENC28J60_INT_TRIGGER_MSG    0x01
#define ENC28J60_SEND_DATA_MSG        0x02

typedef struct _enc28j60_msg_t
{
    u8        msgid;
    u8*        data;
    u16        length;
    struct enc28j60_net *priv;
}enc28j60_msg, *pEnc28j60_msg;


static void enc28j60_task(void *ptr);
int enc28j60_task_send(enc28j60_msg *msg);

extern err_t netif_example_GetPacket(u8_t *data, u32_t len);
extern void netifapi_example_netif_set_up(void);
extern void netifapi_example_netif_set_down(void);

//=========================ROUTINE PROTOTYPES=====================


static OSMutexRef enc28j60MuteRef = NULL;

void enc28j60MutexInit(void)
{
    OSA_STATUS status;
    status = OSAMutexCreate(&enc28j60MuteRef, OS_PRIORITY);
    ASSERT(status == OS_SUCCESS);
}

void enc28j60MutexLock(void)
{
    OSA_STATUS status;
    status = OSAMutexLock(enc28j60MuteRef, OSA_SUSPEND);
    ASSERT(status == OS_SUCCESS);
}

void enc28j60MutexUnlock(void)
{
    OSA_STATUS status;
    status = OSAMutexUnlock(enc28j60MuteRef);
    ASSERT(status == OS_SUCCESS);
}

QL_SPI_PORT_E cur_spi_port = QL_SPI_PORT0;

void enc28j60_spi_init(void)
{
    cur_spi_port = QL_SPI_PORT0;
    ql_spi_init(cur_spi_port, QL_SPI_MODE0, QL_SPI_CLK_3_25MHZ);
}

/*
 * SPI read buffer
 * wait for the SPI transfer and copy received data to destination
 */
static int spi_read_buf(struct enc28j60_net *priv, int len, u8 *data)
{
    u8 *buffer = malloc(len + 1);
    memset(buffer, 0x00, sizeof(len + 1));
    buffer[0] = ENC28J60_READ_BUF_MEM;
    
    ql_spi_write_read(cur_spi_port, buffer, buffer, len + 1);
    memcpy(data, buffer+1, len);
    free(buffer);
    
    return 0;
}

/*
 * SPI write buffer
 */
static int spi_write_buf(struct enc28j60_net *priv, int len, const u8 *data)
{
    u8 *buffer = malloc(len + 1);
    memset(buffer, 0x00, sizeof(len + 1));
    buffer[0] = ENC28J60_WRITE_BUF_MEM;
    
    memcpy(&buffer[1], data, len);
    ql_spi_write(cur_spi_port, buffer, len+1);
    free(buffer);
    
    return 0;
}

/*
 * basic SPI read operation
 */
static u8 spi_read_op(struct enc28j60_net *priv, u8 op, u8 addr)
{
    u8 tx_buf[2] = {0};
    u8 rx_buf[4] = {0};
    u8 val = 0;
    int slen = SPI_OPLEN;

    /* do dummy read if needed */
    if (addr & SPRD_MASK)
        slen++;

    tx_buf[0] = op | (addr & ADDR_MASK);

    ql_spi_write_read(cur_spi_port, rx_buf, tx_buf, slen + 1);

    val = rx_buf[slen + 1 - 1];

    return val;
}

/*
 * basic SPI write operation
 */
static int spi_write_op(struct enc28j60_net *priv, u8 op,
            u8 addr, u8 val)
{
    int ret;

    u8 buffer[2];
    buffer[0] = op | (addr & ADDR_MASK);
    buffer[1] = val;

    ql_spi_write(cur_spi_port, buffer, 2);    
    return 0;
}

static void enc28j60_soft_reset(struct enc28j60_net *priv)
{
    spi_write_op(priv, ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    /* Errata workaround #1, CLKRDY check is unreliable,
     * delay at least 1 mS instead */
    OSATaskSleep(1);
}

/*
 * select the current register bank if necessary
 */
static void enc28j60_set_bank(struct enc28j60_net *priv, u8 addr)
{
    u8 b = (addr & BANK_MASK) >> 5;

    /* These registers (EIE, EIR, ESTAT, ECON2, ECON1)
     * are present in all banks, no need to switch bank
     */
    if (addr >= EIE && addr <= ECON1)
        return;

    /* Clear or set each bank selection bit as needed */
    if ((b & ECON1_BSEL0) != (priv->bank & ECON1_BSEL0)) {
        if (b & ECON1_BSEL0)
            spi_write_op(priv, ENC28J60_BIT_FIELD_SET, ECON1,
                    ECON1_BSEL0);
        else
            spi_write_op(priv, ENC28J60_BIT_FIELD_CLR, ECON1,
                    ECON1_BSEL0);
    }
    if ((b & ECON1_BSEL1) != (priv->bank & ECON1_BSEL1)) {
        if (b & ECON1_BSEL1)
            spi_write_op(priv, ENC28J60_BIT_FIELD_SET, ECON1,
                    ECON1_BSEL1);
        else
            spi_write_op(priv, ENC28J60_BIT_FIELD_CLR, ECON1,
                    ECON1_BSEL1);
    }
    priv->bank = b;
}


/*
 * Register access routines through the SPI bus.
 * Every register access comes in two flavours:
 * - nolock_xxx: caller needs to invoke mutex_lock, usually to access
 *   atomically more than one register
 * - locked_xxx: caller doesn't need to invoke mutex_lock, single access
 *
 * Some registers can be accessed through the bit field clear and
 * bit field set to avoid a read modify write cycle.
 */

/*
 * Register bit field Set
 */
static void nolock_reg_bfset(struct enc28j60_net *priv,
                      u8 addr, u8 mask)
{
    enc28j60_set_bank(priv, addr);
    spi_write_op(priv, ENC28J60_BIT_FIELD_SET, addr, mask);
}

static void locked_reg_bfset(struct enc28j60_net *priv,
                      u8 addr, u8 mask)
{
    enc28j60MutexLock();
    nolock_reg_bfset(priv, addr, mask);
    enc28j60MutexUnlock();
}

/*
 * Register bit field Clear
 */
static void nolock_reg_bfclr(struct enc28j60_net *priv,
                      u8 addr, u8 mask)
{
    enc28j60_set_bank(priv, addr);
    spi_write_op(priv, ENC28J60_BIT_FIELD_CLR, addr, mask);
}

static void locked_reg_bfclr(struct enc28j60_net *priv,
                      u8 addr, u8 mask)
{
    enc28j60MutexLock();
    nolock_reg_bfclr(priv, addr, mask);
    enc28j60MutexUnlock();
}

/*
 * Register byte read
 */
static int nolock_regb_read(struct enc28j60_net *priv,
                     u8 address)
{
    enc28j60_set_bank(priv, address);
    return spi_read_op(priv, ENC28J60_READ_CTRL_REG, address);
}

static int locked_regb_read(struct enc28j60_net *priv,
                     u8 address)
{
    int ret;

    enc28j60MutexLock();
    ret = nolock_regb_read(priv, address);
    enc28j60MutexUnlock();

    return ret;
}

/*
 * Register word read
 */
static int nolock_regw_read(struct enc28j60_net *priv,
                     u8 address)
{
    int rl, rh;

    enc28j60_set_bank(priv, address);
    rl = spi_read_op(priv, ENC28J60_READ_CTRL_REG, address);
    rh = spi_read_op(priv, ENC28J60_READ_CTRL_REG, address + 1);

    return (rh << 8) | rl;
}

static int locked_regw_read(struct enc28j60_net *priv,
                     u8 address)
{
    int ret;

    enc28j60MutexLock();
    ret = nolock_regw_read(priv, address);
    enc28j60MutexUnlock();

    return ret;
}

/*
 * Register byte write
 */
static void nolock_regb_write(struct enc28j60_net *priv,
                       u8 address, u8 data)
{
    enc28j60_set_bank(priv, address);
    spi_write_op(priv, ENC28J60_WRITE_CTRL_REG, address, data);
}

static void locked_regb_write(struct enc28j60_net *priv,
                       u8 address, u8 data)
{
    enc28j60MutexLock();
    nolock_regb_write(priv, address, data);
    enc28j60MutexUnlock();
}

/*
 * Register word write
 */
static void nolock_regw_write(struct enc28j60_net *priv,
                       u8 address, u16 data)
{
    enc28j60_set_bank(priv, address);
    spi_write_op(priv, ENC28J60_WRITE_CTRL_REG, address, (u8) data);
    spi_write_op(priv, ENC28J60_WRITE_CTRL_REG, address + 1,
             (u8) (data >> 8));
}

static void locked_regw_write(struct enc28j60_net *priv,
                       u8 address, u16 data)
{
    enc28j60MutexLock();
    nolock_regw_write(priv, address, data);
    enc28j60MutexUnlock();
}


/*
 * Buffer memory read
 * Select the starting address and execute a SPI buffer read
 */
static void enc28j60_mem_read(struct enc28j60_net *priv,
                     u16 addr, int len, u8 *data)
{
    enc28j60MutexLock();
    nolock_regw_write(priv, ERDPTL, addr);
#ifdef CONFIG_ENC28J60_WRITEVERIFY    
    if (DEBUG_ONFF) {
        u16 reg;
        reg = nolock_regw_read(priv, ERDPTL);
        if (reg != addr)
            enc28j60_debug(": %s() error writing ERDPT "
                "(0x%04x - 0x%04x)\n", __func__, reg, addr);
    }
#endif    
    spi_read_buf(priv, len, data);
    enc28j60MutexUnlock();
}

/*
 * Write packet to enc28j60 TX buffer memory
 */
static void
enc28j60_packet_write(struct enc28j60_net *priv, int len, const u8 *data)
{
    enc28j60MutexLock();
    /* Set the write pointer to start of transmit buffer area */
    nolock_regw_write(priv, EWRPTL, TXSTART_INIT);
#ifdef CONFIG_ENC28J60_WRITEVERIFY    
    if (DEBUG_ONFF) {
        u16 reg;
        reg = nolock_regw_read(priv, EWRPTL);
        if (reg != TXSTART_INIT)
            enc28j60_debug(": %s() ERWPT:0x%04x != 0x%04x\n",
                __func__, reg, TXSTART_INIT);
    }
#endif    
    /* Set the TXND pointer to correspond to the packet size given */
    nolock_regw_write(priv, ETXNDL, TXSTART_INIT + len);
    /* write per-packet control byte */
    spi_write_op(priv, ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() after control byte ERWPT:0x%04x\n",
            __func__, nolock_regw_read(priv, EWRPTL));
    /* copy the packet into the transmit buffer */
    spi_write_buf(priv, len, data);
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() after write packet ERWPT:0x%04x, len=%d\n",
             __func__, nolock_regw_read(priv, EWRPTL), len);
    enc28j60MutexUnlock();
}


static int poll_ready(struct enc28j60_net *priv, u8 reg, u8 mask, u8 val)
{
    int count = 0;

    /* 20 msec timeout read */
    while ((nolock_regb_read(priv, reg) & mask) != val) {
        if (count >= 4) {
            if (DEBUG_ONFF)
                enc28j60_debug("reg %02x ready timeout!\n", reg);
            return -1;
        }
        OSATaskSleep(1);
        count++;
    }
    return 0;
}

/*
 * Wait until the PHY operation is complete.
 */
static int wait_phy_ready(struct enc28j60_net *priv)
{
    return poll_ready(priv, MISTAT, MISTAT_BUSY, 0) ? 0 : 1;
}

/*
 * PHY register read
 * PHY registers are not accessed directly, but through the MII
 */
static u16 enc28j60_phy_read(struct enc28j60_net *priv, u8 address)
{
    u16 ret;

    enc28j60MutexLock();
    /* set the PHY register address */
    nolock_regb_write(priv, MIREGADR, address);
    /* start the register read operation */
    nolock_regb_write(priv, MICMD, MICMD_MIIRD);
    /* wait until the PHY read completes */
    wait_phy_ready(priv);
    /* quit reading */
    nolock_regb_write(priv, MICMD, 0x00);
    /* return the data */
    ret  = nolock_regw_read(priv, MIRDL);
    enc28j60MutexUnlock();

    return ret;
}

static int enc28j60_phy_write(struct enc28j60_net *priv, u8 address, u16 data)
{
    int ret;

    enc28j60MutexLock();
    /* set the PHY register address */
    nolock_regb_write(priv, MIREGADR, address);
    /* write the PHY data */
    nolock_regw_write(priv, MIWRL, data);
    /* wait until the PHY write completes and return */
    ret = wait_phy_ready(priv);
    enc28j60MutexUnlock();

    return ret;
}

/*
 * Program the hardware MAC address from dev->dev_addr.
 */
static int enc28j60_set_hw_macaddr(struct enc28j60_net *priv)
{
    int ret;
    struct netif *netif = priv->netif;
    
    enc28j60MutexLock();    
    if (!priv->hw_enable) {
        if (DEBUG_ONFF)
            enc28j60_debug(": %s: Setting MAC address to %pM\n",
                netif->hostname, netif->hwaddr);
        /* NOTE: MAC address in ENC28J60 is byte-backward */
        nolock_regb_write(priv, MAADR5, netif->hwaddr[0]);
        nolock_regb_write(priv, MAADR4, netif->hwaddr[1]);
        nolock_regb_write(priv, MAADR3, netif->hwaddr[2]);
        nolock_regb_write(priv, MAADR2, netif->hwaddr[3]);
        nolock_regb_write(priv, MAADR1, netif->hwaddr[4]);
        nolock_regb_write(priv, MAADR0, netif->hwaddr[5]);
        ret = 0;
    } else {
        if (DEBUG_ONFF)
            enc28j60_debug(": %s() Hardware must be disabled to set "
                "Mac address\n", __func__);
        ret = -1;
    }    
    enc28j60MutexUnlock();
    
    return ret;
}

/*
 * Debug routine to dump useful register contents
 */
static void enc28j60_dump_regs(struct enc28j60_net *priv, const char *msg)
{
    enc28j60MutexLock();
    enc28j60_debug(" %s\n"
        "HwRevID: 0x%02x\n"
        "Cntrl: ECON1 ECON2 ESTAT  EIR  EIE\n"
        "       0x%02x  0x%02x  0x%02x  0x%02x  0x%02x\n"
        "MAC  : MACON1 MACON3 MACON4\n"
        "       0x%02x   0x%02x   0x%02x\n",
        msg, nolock_regb_read(priv, EREVID),
        nolock_regb_read(priv, ECON1), nolock_regb_read(priv, ECON2),
        nolock_regb_read(priv, ESTAT), nolock_regb_read(priv, EIR),
        nolock_regb_read(priv, EIE), nolock_regb_read(priv, MACON1),
        nolock_regb_read(priv, MACON3), nolock_regb_read(priv, MACON4));
    
    enc28j60_debug("Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL\n"
        "       0x%04x 0x%04x 0x%04x  0x%04x  "
        "0x%02x    0x%02x    0x%04x\n",
                nolock_regw_read(priv, ERXSTL), nolock_regw_read(priv, ERXNDL),
        nolock_regw_read(priv, ERXWRPTL),
        nolock_regw_read(priv, ERXRDPTL),
        nolock_regb_read(priv, ERXFCON),
        nolock_regb_read(priv, EPKTCNT),
        nolock_regw_read(priv, MAMXFLL));
    
    enc28j60_debug("Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP\n"
        "       0x%04x 0x%04x 0x%02x     0x%02x     0x%02x\n",
        nolock_regw_read(priv, ETXSTL),
        nolock_regw_read(priv, ETXNDL),
        nolock_regb_read(priv, MACLCON1),
        nolock_regb_read(priv, MACLCON2),
        nolock_regb_read(priv, MAPHSUP));
    enc28j60MutexUnlock();
}

/*
 * ERXRDPT need to be set always at odd addresses, refer to errata datasheet
 */
static u16 erxrdpt_workaround(u16 next_packet_ptr, u16 start, u16 end)
{
    u16 erxrdpt;

    if ((next_packet_ptr - 1 < start) || (next_packet_ptr - 1 > end))
        erxrdpt = end;
    else
        erxrdpt = next_packet_ptr - 1;

    return erxrdpt;
}

/*
 * Calculate wrap around when reading beyond the end of the RX buffer
 */
static u16 rx_packet_start(u16 ptr)
{
    if (ptr + RSV_SIZE > RXEND_INIT)
        return (ptr + RSV_SIZE) - (RXEND_INIT - RXSTART_INIT + 1);
    else
        return ptr + RSV_SIZE;
}

static void nolock_rxfifo_init(struct enc28j60_net *priv, u16 start, u16 end)
{
    u16 erxrdpt;

    if (start > 0x1FFF || end > 0x1FFF || start > end) {
        if (DEBUG_ONFF)
            enc28j60_debug(": %s(%d, %d) RXFIFO "
                "bad parameters!\n", __func__, start, end);
        return;
    }
    /* set receive buffer start + end */
    priv->next_pk_ptr = start;
    nolock_regw_write(priv, ERXSTL, start);
    erxrdpt = erxrdpt_workaround(priv->next_pk_ptr, start, end);
    nolock_regw_write(priv, ERXRDPTL, erxrdpt);
    nolock_regw_write(priv, ERXNDL, end);
}

static void nolock_txfifo_init(struct enc28j60_net *priv, u16 start, u16 end)
{
    if (start > 0x1FFF || end > 0x1FFF || start > end) {
        if (DEBUG_ONFF)
            enc28j60_debug(": %s(%d, %d) TXFIFO "
                "bad parameters!\n", __func__, start, end);
        return;
    }
    /* set transmit buffer start + end */
    nolock_regw_write(priv, ETXSTL, start);
    nolock_regw_write(priv, ETXNDL, end);
}


/*
 * Low power mode shrinks power consumption about 100x, so we'd like
 * the chip to be in that mode whenever it's inactive.  (However, we
 * can't stay in lowpower mode during suspend with WOL active.)
 */
static void enc28j60_lowpower(struct enc28j60_net *priv, bool is_low)
{
    enc28j60_debug("%s power...\n",
                is_low ? "low" : "high");

    enc28j60MutexLock();
    if (is_low) {
        nolock_reg_bfclr(priv, ECON1, ECON1_RXEN);
        poll_ready(priv, ESTAT, ESTAT_RXBUSY, 0);
        poll_ready(priv, ECON1, ECON1_TXRTS, 0);
        /* ECON2_VRPS was set during initialization */
        nolock_reg_bfset(priv, ECON2, ECON2_PWRSV);
    } else {
        nolock_reg_bfclr(priv, ECON2, ECON2_PWRSV);
        poll_ready(priv, ESTAT, ESTAT_CLKRDY, ESTAT_CLKRDY);
        /* caller sets ECON1_RXEN */
    }
    enc28j60MutexUnlock();
}

static int enc28j60_hw_init(struct enc28j60_net *priv)
{
    u8 reg;

    if (DEBUG_ONFF)
        enc28j60_debug(": %s() - %s\n", __func__,
            priv->full_duplex ? "FullDuplex" : "HalfDuplex");

    enc28j60MutexLock();
    /* first reset the chip */
    enc28j60_soft_reset(priv);
    /* Clear ECON1 */
    spi_write_op(priv, ENC28J60_WRITE_CTRL_REG, ECON1, 0x00);
    priv->bank = 0;
    priv->hw_enable = false;
    priv->full_duplex = false;
    priv->tx_retry_count = 0;
    priv->max_pk_counter = 0;
    priv->rxfilter = RXFILTER_NORMAL;
    /* enable address auto increment and voltage regulator powersave */
    nolock_regb_write(priv, ECON2, ECON2_AUTOINC | ECON2_VRPS);

    nolock_rxfifo_init(priv, RXSTART_INIT, RXEND_INIT);
    nolock_txfifo_init(priv, TXSTART_INIT, TXEND_INIT);
    enc28j60MutexUnlock();

    /*
     * Check the RevID.
     * If it's 0x00 or 0xFF probably the enc28j60 is not mounted or
     * damaged
     */
    reg = locked_regb_read(priv, EREVID);
    if (DEBUG_ONFF)
        enc28j60_debug(": chip RevID: 0x%02x\n", reg);
    if (reg == 0x00 || reg == 0xff) {
        if (DEBUG_ONFF)
            enc28j60_debug(": %s() Invalid RevId %d\n",
                __func__, reg);
        return 0;
    }

    /* default filter mode: (unicast OR broadcast) AND crc valid */
    locked_regb_write(priv, ERXFCON,
                ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

    /* enable MAC receive */
    locked_regb_write(priv, MACON1,
                MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
    /* enable automatic padding and CRC operations */
    if (priv->full_duplex) {
        locked_regb_write(priv, MACON3,
                    MACON3_PADCFG0 | MACON3_TXCRCEN |
                    MACON3_FRMLNEN | MACON3_FULDPX);
        /* set inter-frame gap (non-back-to-back) */
        locked_regb_write(priv, MAIPGL, 0x12);
        /* set inter-frame gap (back-to-back) */
        locked_regb_write(priv, MABBIPG, 0x15);
    } else {
        locked_regb_write(priv, MACON3,
                    MACON3_PADCFG0 | MACON3_TXCRCEN |
                    MACON3_FRMLNEN);
        locked_regb_write(priv, MACON4, 1 << 6);    /* DEFER bit */
        /* set inter-frame gap (non-back-to-back) */
        locked_regw_write(priv, MAIPGL, 0x0C12);
        /* set inter-frame gap (back-to-back) */
        locked_regb_write(priv, MABBIPG, 0x12);
    }
    /*
     * MACLCON1 (default)
     * MACLCON2 (default)
     * Set the maximum packet size which the controller will accept
     */
    locked_regw_write(priv, MAMXFLL, MAX_FRAMELEN);

    /* Configure LEDs */
    if (!enc28j60_phy_write(priv, PHLCON, ENC28J60_LAMPS_MODE))
        return 0;

    if (priv->full_duplex) {
        if (!enc28j60_phy_write(priv, PHCON1, PHCON1_PDPXMD))
            return 0;
        if (!enc28j60_phy_write(priv, PHCON2, 0x00))
            return 0;
    } else {
        if (!enc28j60_phy_write(priv, PHCON1, 0x00))
            return 0;
        if (!enc28j60_phy_write(priv, PHCON2, PHCON2_HDLDIS))
            return 0;
    }
    if (DEBUG_ONFF)
        enc28j60_dump_regs(priv, "Hw initialized.");

    return 1;
}

static void enc28j60_hw_enable(struct enc28j60_net *priv)
{
    /* enable interrupts */
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() enabling interrupts.\n",
            __func__);

    enc28j60_phy_write(priv, PHIE, PHIE_PGEIE | PHIE_PLNKIE);

    enc28j60MutexLock();
    nolock_reg_bfclr(priv, EIR, EIR_DMAIF | EIR_LINKIF |
             EIR_TXIF | EIR_TXERIF | EIR_RXERIF | EIR_PKTIF);
    nolock_regb_write(priv, EIE, EIE_INTIE | EIE_PKTIE | EIE_LINKIE |
              EIE_TXIE | EIE_TXERIE | EIE_RXERIE);

    /* enable receive logic */
    nolock_reg_bfset(priv, ECON1, ECON1_RXEN);
    priv->hw_enable = true;
    enc28j60MutexUnlock();
}

static void enc28j60_hw_disable(struct enc28j60_net *priv)
{
    enc28j60MutexLock();
    /* disable interrutps and packet reception */
    nolock_regb_write(priv, EIE, 0x00);
    nolock_reg_bfclr(priv, ECON1, ECON1_RXEN);
    priv->hw_enable = false;
    enc28j60MutexUnlock();
}


static int enc28j60_setlink(struct enc28j60_net *priv, u8 autoneg, u16 speed, u8 duplex)
{
    int ret = 0;
#if 0    
    if (!priv->hw_enable) {
        /* link is in low power mode now; duplex setting
         * will take effect on next enc28j60_hw_init().
         */
        if (autoneg == AUTONEG_DISABLE && speed == SPEED_10)
            priv->full_duplex = (duplex == DUPLEX_FULL);
        else {
            if (netif_msg_link(priv))
                dev_warn(&ndev->dev,
                    "unsupported link setting\n");
            ret = -EOPNOTSUPP;
        }
    } else {
        if (netif_msg_link(priv))
            dev_warn(&ndev->dev, "Warning: hw must be disabled "
                "to set link mode\n");
        ret = -EBUSY;
    }
#endif    
    return ret;
}

/*
 * Read the Transmit Status Vector
 */
static void enc28j60_read_tsv(struct enc28j60_net *priv, u8 tsv[TSV_SIZE])
{
    int endptr;

    endptr = locked_regw_read(priv, ETXNDL);
    if (DEBUG_ONFF)
        enc28j60_debug(": reading TSV at addr:0x%04x\n",
             endptr + 1);
    enc28j60_mem_read(priv, endptr + 1, TSV_SIZE, tsv);
}

static void enc28j60_dump_tsv(struct enc28j60_net *priv, const char *msg,
                u8 tsv[TSV_SIZE])
{
    u16 tmp1, tmp2;

    enc28j60_debug(": %s - TSV:\n", msg);
    tmp1 = tsv[1];
    tmp1 <<= 8;
    tmp1 |= tsv[0];

    tmp2 = tsv[5];
    tmp2 <<= 8;
    tmp2 |= tsv[4];

    enc28j60_debug(": ByteCount: %d, CollisionCount: %d,"
        " TotByteOnWire: %d\n", tmp1, tsv[2] & 0x0f, tmp2);
    enc28j60_debug(": TxDone: %d, CRCErr:%d, LenChkErr: %d,"
        " LenOutOfRange: %d\n", TSV_GETBIT(tsv, TSV_TXDONE),
        TSV_GETBIT(tsv, TSV_TXCRCERROR),
        TSV_GETBIT(tsv, TSV_TXLENCHKERROR),
        TSV_GETBIT(tsv, TSV_TXLENOUTOFRANGE));
    enc28j60_debug(": Multicast: %d, Broadcast: %d, "
        "PacketDefer: %d, ExDefer: %d\n",
        TSV_GETBIT(tsv, TSV_TXMULTICAST),
        TSV_GETBIT(tsv, TSV_TXBROADCAST),
        TSV_GETBIT(tsv, TSV_TXPACKETDEFER),
        TSV_GETBIT(tsv, TSV_TXEXDEFER));
    enc28j60_debug(": ExCollision: %d, LateCollision: %d, "
         "Giant: %d, Underrun: %d\n",
         TSV_GETBIT(tsv, TSV_TXEXCOLLISION),
         TSV_GETBIT(tsv, TSV_TXLATECOLLISION),
         TSV_GETBIT(tsv, TSV_TXGIANT), TSV_GETBIT(tsv, TSV_TXUNDERRUN));
    enc28j60_debug(": ControlFrame: %d, PauseFrame: %d, "
         "BackPressApp: %d, VLanTagFrame: %d\n",
         TSV_GETBIT(tsv, TSV_TXCONTROLFRAME),
         TSV_GETBIT(tsv, TSV_TXPAUSEFRAME),
         TSV_GETBIT(tsv, TSV_BACKPRESSUREAPP),
         TSV_GETBIT(tsv, TSV_TXVLANTAGFRAME));
}

/*
 * Receive Status vector
 */
static void enc28j60_dump_rsv(struct enc28j60_net *priv, const char *msg,
                  u16 pk_ptr, int len, u16 sts)
{
    enc28j60_debug(": %s - NextPk: 0x%04x - RSV:\n",
        msg, pk_ptr);
    enc28j60_debug(": ByteCount: %d, DribbleNibble: %d\n", len,
         RSV_GETBIT(sts, RSV_DRIBBLENIBBLE));
    enc28j60_debug(": RxOK: %d, CRCErr:%d, LenChkErr: %d,"
         " LenOutOfRange: %d\n", RSV_GETBIT(sts, RSV_RXOK),
         RSV_GETBIT(sts, RSV_CRCERROR),
         RSV_GETBIT(sts, RSV_LENCHECKERR),
         RSV_GETBIT(sts, RSV_LENOUTOFRANGE));
    enc28j60_debug(": Multicast: %d, Broadcast: %d, "
         "LongDropEvent: %d, CarrierEvent: %d\n",
         RSV_GETBIT(sts, RSV_RXMULTICAST),
         RSV_GETBIT(sts, RSV_RXBROADCAST),
         RSV_GETBIT(sts, RSV_RXLONGEVDROPEV),
         RSV_GETBIT(sts, RSV_CARRIEREV));
    enc28j60_debug(": ControlFrame: %d, PauseFrame: %d,"
         " UnknownOp: %d, VLanTagFrame: %d\n",
         RSV_GETBIT(sts, RSV_RXCONTROLFRAME),
         RSV_GETBIT(sts, RSV_RXPAUSEFRAME),
         RSV_GETBIT(sts, RSV_RXUNKNOWNOPCODE),
         RSV_GETBIT(sts, RSV_RXTYPEVLAN));
}

static void dump_packet(const char *msg, int len, const char *data)
{
    int i;
    char buf[4096] = {0};
    int bufLen = 0;    
    
    enc28j60_debug(": %s - packet len:%d\n", msg, len);

    for(i = 0; i < len; i++){
        bufLen += sprintf(buf+bufLen, "%02X", data[i]);
    }

    enc28j60_debug("dump_packet:%s\n", buf);
}

/*
 * Hardware receive function.
 * Read the buffer memory, update the FIFO pointer to free the buffer,
 * check the status vector and decrement the packet counter.
 */
void enc28j60_hw_rx(struct enc28j60_net *priv)
{
    u8 *readBuf = NULL;
    u16 erxrdpt, next_packet, rxstat;
    u8 rsv[RSV_SIZE];
    int len;

    if (DEBUG_ONFF)
        enc28j60_debug(": RX pk_addr:0x%04x\n",
            priv->next_pk_ptr);

    if (priv->next_pk_ptr > RXEND_INIT) {
        if (DEBUG_ONFF)
            enc28j60_debug("%s() Invalid packet address!! 0x%04x\n",
                __func__, priv->next_pk_ptr);
        /* packet address corrupted: reset RX logic */
        enc28j60MutexLock();
        nolock_reg_bfclr(priv, ECON1, ECON1_RXEN);
        nolock_reg_bfset(priv, ECON1, ECON1_RXRST);
        nolock_reg_bfclr(priv, ECON1, ECON1_RXRST);
        nolock_rxfifo_init(priv, RXSTART_INIT, RXEND_INIT);
        nolock_reg_bfclr(priv, EIR, EIR_RXERIF);
        nolock_reg_bfset(priv, ECON1, ECON1_RXEN);
        enc28j60MutexUnlock();
        priv->rx_frame_errors++;
        return;
    }
    /* Read next packet pointer and rx status vector */
    enc28j60_mem_read(priv, priv->next_pk_ptr, sizeof(rsv), rsv);

    next_packet = rsv[1];
    next_packet <<= 8;
    next_packet |= rsv[0];

    len = rsv[3];
    len <<= 8;
    len |= rsv[2];

    rxstat = rsv[5];
    rxstat <<= 8;
    rxstat |= rsv[4];

    if (DEBUG_ONFF)
        enc28j60_dump_rsv(priv, __func__, next_packet, len, rxstat);

    if (!RSV_GETBIT(rxstat, RSV_RXOK) || len > MAX_FRAMELEN) {
        enc28j60_debug(": %s failed\n",__func__);
        if (DEBUG_ONFF)
            enc28j60_debug("Rx Error (%04x)\n", rxstat);
        priv->rx_frame_errors++;
        if (RSV_GETBIT(rxstat, RSV_CRCERROR))
            priv->rx_frame_errors++;
        if (RSV_GETBIT(rxstat, RSV_LENCHECKERR))
            priv->rx_frame_errors++;
        if (len > MAX_FRAMELEN)
            priv->rx_frame_errors++;
    } else {
        enc28j60_debug(": %s will read\n",__func__);
        
        readBuf = malloc(len);
        if (!readBuf) {
            enc28j60_debug("out of memory for Rx'd frame\n");
        } else {
            /* copy the packet from the receive buffer */
            enc28j60_mem_read(priv,
                rx_packet_start(priv->next_pk_ptr), len, readBuf);
            if (DEBUG_ONFF)
                dump_packet(__func__, len, (char *)readBuf);
            /* update statistics */
            priv->rx_packets++;
            priv->rx_bytes += len;

            netif_example_GetPacket(readBuf, len);
        }
    }
    /*
     * Move the RX read pointer to the start of the next
     * received packet.
     * This frees the memory we just read out
     */
    erxrdpt = erxrdpt_workaround(next_packet, RXSTART_INIT, RXEND_INIT);
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() ERXRDPT:0x%04x\n",
            __func__, erxrdpt);

    enc28j60MutexLock();
    nolock_regw_write(priv, ERXRDPTL, erxrdpt);
#ifdef CONFIG_ENC28J60_WRITEVERIFY
    if (DEBUG_ONFF) {
        u16 reg;
        reg = nolock_regw_read(priv, ERXRDPTL);
        if (reg != erxrdpt)
            enc28j60_debug(": %s() ERXRDPT verify "
                "error (0x%04x - 0x%04x)\n", __func__,
                reg, erxrdpt);
    }
#endif
    priv->next_pk_ptr = next_packet;
    /* we are done with this packet, decrement the packet counter */
    nolock_reg_bfset(priv, ECON2, ECON2_PKTDEC);
    enc28j60MutexUnlock();
}

/*
 * Calculate free space in RxFIFO
 */
static int enc28j60_get_free_rxfifo(struct enc28j60_net *priv)
{
    int epkcnt, erxst, erxnd, erxwr, erxrd;
    int free_space;

    enc28j60MutexLock();
    epkcnt = nolock_regb_read(priv, EPKTCNT);
    if (epkcnt >= 255)
        free_space = -1;
    else {
        erxst = nolock_regw_read(priv, ERXSTL);
        erxnd = nolock_regw_read(priv, ERXNDL);
        erxwr = nolock_regw_read(priv, ERXWRPTL);
        erxrd = nolock_regw_read(priv, ERXRDPTL);

        if (erxwr > erxrd)
            free_space = (erxnd - erxst) - (erxwr - erxrd);
        else if (erxwr == erxrd)
            free_space = (erxnd - erxst);
        else
            free_space = erxrd - erxwr - 1;
    }
    enc28j60MutexUnlock();
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() free_space = %d\n",
            __func__, free_space);
    return free_space;
}


/*
 * Access the PHY to determine link status
 */
void enc28j60_check_link_status(struct enc28j60_net *priv)
{
    u16 reg;
    int duplex;

    reg = enc28j60_phy_read(priv, PHSTAT2);
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() PHSTAT1: %04x, "
            "PHSTAT2: %04x\n", __func__,
            enc28j60_phy_read(priv, PHSTAT1), reg);
    duplex = reg & PHSTAT2_DPXSTAT;

    if (reg & PHSTAT2_LSTAT) {
        enc28j60_debug("link up - %s\n",
            duplex ? "Full duplex" : "Half duplex");
    } else {
        enc28j60_debug("link down\n");
    }
}



void enc28j60_tx_clear(struct enc28j60_net *priv, bool err)
{
    if (err)
        priv->tx_errors++;
    else
        priv->tx_packets++;
    if (priv){
        if (!err)
            priv->tx_bytes += priv->tx_last_bytes;
    }
    locked_reg_bfclr(priv, ECON1, ECON1_TXRTS);
}

int enc28j60_rx_interrupt(struct enc28j60_net *priv)
{
    int pk_counter, ret;

    pk_counter = locked_regb_read(priv, EPKTCNT);
    if (pk_counter)
        enc28j60_debug(": intRX, pk_cnt: %d\n", pk_counter);
    if (pk_counter > priv->max_pk_counter) {
        /* update statistics */
        priv->max_pk_counter = pk_counter;
        if (priv->max_pk_counter > 1)
            enc28j60_debug(": RX max_pk_cnt: %d\n",
                priv->max_pk_counter);
    }
    ret = pk_counter;
    while (pk_counter-- > 0)
        enc28j60_hw_rx(priv);

    return ret;
}


void enc28j60_irq_work_handler(struct enc28j60_net *priv)
{
    int intflags, loop;

    enc28j60_debug(": %s() enter\n", __func__);
    /* disable further interrupts */
    locked_reg_bfclr(priv, EIE, EIE_INTIE);

    do {
        loop = 0;
        intflags = locked_regb_read(priv, EIR);
        enc28j60_debug(": intflags(%X)\n", intflags);
        
        /* DMA interrupt handler (not currently used) */
        if ((intflags & EIR_DMAIF) != 0) {
            loop++;
            enc28j60_debug(": intDMA(%d)\n", loop);
            locked_reg_bfclr(priv, EIR, EIR_DMAIF);
        }
        /* LINK changed handler */
        if ((intflags & EIR_LINKIF) != 0) {
            loop++;
            enc28j60_debug(": intLINK(%d)\n", loop);
            enc28j60_check_link_status(priv);
            /* read PHIR to clear the flag */
            enc28j60_phy_read(priv, PHIR);
        }
        /* TX complete handler */
        if (((intflags & EIR_TXIF) != 0) &&
            ((intflags & EIR_TXERIF) == 0)) {
            bool err = false;
            loop++;
            enc28j60_debug(": intTX(%d)\n", loop);
            priv->tx_retry_count = 0;
            if (locked_regb_read(priv, ESTAT) & ESTAT_TXABRT) {
                enc28j60_debug("Tx Error (aborted)\n");
                err = true;
            }
            if (DEBUG_ONFF) {
                u8 tsv[TSV_SIZE];
                enc28j60_read_tsv(priv, tsv);
                enc28j60_dump_tsv(priv, "Tx Done", tsv);
            }
            enc28j60_tx_clear(priv, err);
            locked_reg_bfclr(priv, EIR, EIR_TXIF);
        }
        /* TX Error handler */
        if ((intflags & EIR_TXERIF) != 0) {
            u8 tsv[TSV_SIZE];

            loop++;
            enc28j60_debug(": intTXErr(%d)\n", loop);
            locked_reg_bfclr(priv, ECON1, ECON1_TXRTS);
            enc28j60_read_tsv(priv, tsv);
            if (DEBUG_ONFF)
                enc28j60_dump_tsv(priv, "Tx Error", tsv);
            /* Reset TX logic */
            enc28j60MutexLock();
            nolock_reg_bfset(priv, ECON1, ECON1_TXRST);
            nolock_reg_bfclr(priv, ECON1, ECON1_TXRST);
            nolock_txfifo_init(priv, TXSTART_INIT, TXEND_INIT);
            enc28j60MutexUnlock();
            /* Transmit Late collision check for retransmit */
            if (TSV_GETBIT(tsv, TSV_TXLATECOLLISION)) {
                enc28j60_debug(": LateCollision TXErr (%d)\n",
                        priv->tx_retry_count);
                if (priv->tx_retry_count++ < MAX_TX_RETRYCOUNT)
                    locked_reg_bfset(priv, ECON1,
                               ECON1_TXRTS);
                else
                    enc28j60_tx_clear(priv, true);
            } else
                enc28j60_tx_clear(priv, true);
            locked_reg_bfclr(priv, EIR, EIR_TXERIF | EIR_TXIF);
        }
        /* RX Error handler */
        if ((intflags & EIR_RXERIF) != 0) {
            loop++;
            enc28j60_debug(": intRXErr(%d)\n", loop);
            /* Check free FIFO space to flag RX overrun */
            if (enc28j60_get_free_rxfifo(priv) <= 0) {
                enc28j60_debug(": RX Overrun\n");
            }
            locked_reg_bfclr(priv, EIR, EIR_RXERIF);
        }
        /* RX handler */
        if (enc28j60_rx_interrupt(priv))
            loop++;
    } while (loop);

    /* re-enable interrupts */
    locked_reg_bfset(priv, EIE, EIE_INTIE);    
    enc28j60_debug(": %s() exit\n", __func__);
}


/*
 * Hardware transmit function.
 * Fill the buffer memory and send the contents of the transmit buffer
 * onto the network
 */
void enc28j60_hw_tx(struct enc28j60_net *priv, u8 *data, u32 len)
{
    if (DEBUG_ONFF)
        enc28j60_debug(": Tx Packet Len:%d\n", len);

    if (DEBUG_ONFF)
        dump_packet(__func__,len, (char *)data);
    
    enc28j60_packet_write(priv, len, data);
    priv->tx_last_bytes = len;
#ifdef CONFIG_ENC28J60_WRITEVERIFY
    /* readback and verify written data */
    if (DEBUG_ONFF) {
        int test_len, k;
        u8 test_buf[64]; /* limit the test to the first 64 bytes */
        int okflag;

        test_len = len;
        if (test_len > sizeof(test_buf))
            test_len = sizeof(test_buf);

        /* + 1 to skip control byte */
        enc28j60_mem_read(priv, TXSTART_INIT + 1, test_len, test_buf);
        okflag = 1;
        for (k = 0; k < test_len; k++) {
            if (data[k] != test_buf[k]) {
                enc28j60_debug(": Error, %d location differ: "
                     "0x%02x-0x%02x\n", k,
                     data[k], test_buf[k]);
                okflag = 0;
            }
        }
        if (!okflag)
            enc28j60_debug(": Tx write buffer, "
                "verify ERROR!\n");
    }
#endif
    /* set TX request flag */
    locked_reg_bfset(priv, ECON1, ECON1_TXRTS);
}

void enc28j60_tx_work_handler(u8_t *data, u32_t len)
{
    enc28j60_msg sendmsg;

    struct enc28j60_net *priv = &enc28j60_info;

    memset(&sendmsg, 0x00, sizeof(enc28j60_msg));

    sendmsg.msgid = ENC28J60_SEND_DATA_MSG;
    sendmsg.data = malloc(len+1);
    memset(sendmsg.data, 0x00, (len+1));
    memcpy(sendmsg.data, data, len);
    sendmsg.length = len;
    sendmsg.priv = priv;
        
    enc28j60_task_send(&sendmsg);
}

static void enc28j60_irq(void)
{

}

static void enc28j60_tx_timeout(void)
{

}


/*
 * Open/initialize the board. This is called (in the current kernel)
 * sometime after booting when the 'ifconfig' program is run.
 *
 * This routine should set everything up anew at each open, even
 * registers that "should" only need to be set once at boot, so that
 * there is non-reboot way to recover if something goes wrong.
 */
static int enc28j60_net_open(struct enc28j60_net *priv)
{
    if (DEBUG_ONFF)
        enc28j60_debug(": %s() enter\n", __func__);

    /* Reset the hardware here (and take it out of low power mode) */
    enc28j60_lowpower(priv, false);
    enc28j60_hw_disable(priv);
    if (!enc28j60_hw_init(priv)) {
        enc28j60_debug("hw_reset() failed\n");
        return -1;
    }
    /* Update the MAC address (in case user has changed it) */
    enc28j60_set_hw_macaddr(priv);
    /* Enable interrupts */
    enc28j60_hw_enable(priv);
    /* check link status */
    enc28j60_check_link_status(priv);

    netifapi_example_netif_set_up();
    return 0;
}

/* The inverse routine to net_open(). */
static int enc28j60_net_close(struct enc28j60_net *priv)
{
    enc28j60_debug(": %s() enter\n", __func__);

    enc28j60_hw_disable(priv);
    enc28j60_lowpower(priv, true);
    
    netifapi_example_netif_set_down();
    return 0;
}

static void enc28j60_restart_work_handler(struct enc28j60_net *priv)
{
    int ret;

    enc28j60MutexLock();
    enc28j60_net_close(priv);
    ret = enc28j60_net_open(priv);
    enc28j60_debug(": %s() enc28j60_net_open ret %d\n", __func__, ret);

    enc28j60MutexUnlock();
}












#define ENC28J60_TASK_MSGQ_MSG_SIZE              (sizeof(enc28j60_msg))
#define ENC28J60_TASK_MSGQ_QUEUE_SIZE            (512)

static OSMsgQRef enc28j60MsgQ = NULL;

#define ENC28J60_STACK_SIZE    (1024*32)
static void* enc28j60_task_stack = NULL;

static OSTaskRef enc28j60_task_ref = NULL;

int enc28j60_task_ready = 0;
// interrupts start

UINT32 int_detect_gpio=22;
UINT32 int_detect_gpio_insert_level=0;    // low trigger


OS_HISR     int_detect_hisr_ref;
OSATaskRef int_detect_task_ref=NULL;

UINT32 get_int_trigger_status(void)
{
    UINT32 value;

    value=GpioGetLevel(int_detect_gpio);

    if(value==int_detect_gpio_insert_level)
        return 1;
    else
        return 0;

}

void int_det_hisr(void)
{
    OS_STATUS os_status;
    UINT32 trigger_status;

    trigger_status = get_int_trigger_status();

    enc28j60_debug("trigger_status = %u", trigger_status);
    
    if (trigger_status && enc28j60_task_ready){
        enc28j60_msg msg;

        memset(&msg, 0x00, sizeof(enc28j60_msg));
        msg.msgid = ENC28J60_INT_TRIGGER_MSG;    
        msg.priv = &enc28j60_info;    
        enc28j60_task_send(&msg);
    }
}

void int_det_lisr (void)
{
    STATUS status;
    
    status = OS_Activate_HISR(&int_detect_hisr_ref);
    ASSERT(status == OS_SUCCESS);
}

void enc28j60_task(void *ptr)
{
    OS_STATUS  status;
    enc28j60_msg msg = {0};
    struct enc28j60_net *priv = &enc28j60_info;

    if (DEBUG_ONFF)
        OSATaskSleep(200*8);
    memset(priv, 0x00, sizeof(struct enc28j60_net));

    enc28j60MutexInit();
    enc28j60_spi_init();

    priv->netif = (struct netif *)ptr;
    
    enc28j60_net_open(priv);

    enc28j60_task_ready = 1;
    while(1) {
        memset(&msg, 0x00, sizeof(enc28j60_msg));
        status = OSAMsgQRecv(enc28j60MsgQ, (void *)&msg, ENC28J60_TASK_MSGQ_MSG_SIZE, OSA_SUSPEND);
        
        if (status == OS_SUCCESS){
            enc28j60_debug("msg.msgid %s", msg.msgid == ENC28J60_INT_TRIGGER_MSG?"Int":"Send");

            if (msg.msgid == ENC28J60_INT_TRIGGER_MSG){
                if (msg.priv)
                    enc28j60_irq_work_handler(msg.priv);
            }else if (msg.msgid == ENC28J60_SEND_DATA_MSG){
                enc28j60_hw_tx(msg.priv, msg.data, msg.length);
            }
        }
        
        if (msg.data)
            free(msg.data);
    }
}


int enc28j60_task_send(enc28j60_msg *msg)
{
    int ret = 0;
    OSA_STATUS status = OSAMsgQSend(enc28j60MsgQ, ENC28J60_TASK_MSGQ_MSG_SIZE, (void*)msg, OSA_NO_SUSPEND);
    if (status != OS_SUCCESS) {
        enc28j60_debug("%s, OSAMsgQSend lose, msg->id=%d, status=%d", __FUNCTION__, msg->msgid, status);
        if (msg->data) {
            free(msg->data);
        }
        ret = -1;
    }    
    
    return ret;
}



void enc28j60_task_init(struct netif *netif)
{
    int ret;
    
    GPIOConfiguration config;
    Os_Create_HISR(&int_detect_hisr_ref, "int_det_hisr", int_det_hisr, 2);

    if(int_detect_gpio != 0)
    {    
        config.pinDir = GPIO_IN_PIN;
        config.pinEd = GPIO_TWO_EDGE;
        config.pinPull = GPIO_PULLUP_ENABLE;
        config.isr = int_det_lisr;
        GpioInitConfiguration(int_detect_gpio, config);
    }    

    /*creat message*/
    ret = OSAMsgQCreate(&enc28j60MsgQ, 
                            "enc28j60MsgQ", 
                            ENC28J60_TASK_MSGQ_MSG_SIZE, 
                            ENC28J60_TASK_MSGQ_QUEUE_SIZE, 
                            OS_FIFO);
    ASSERT(ret == OS_SUCCESS);
    
    enc28j60_task_stack = malloc(ENC28J60_STACK_SIZE);
    
    ret = OSATaskCreate(&enc28j60_task_ref, 
                            enc28j60_task_stack, 
                            ENC28J60_STACK_SIZE, 
                            101, 
                            "enc28j60_task", 
                            enc28j60_task, 
                            netif);
    ASSERT(ret == OS_SUCCESS);
    
}


