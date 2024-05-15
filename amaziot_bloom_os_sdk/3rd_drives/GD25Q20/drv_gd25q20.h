//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : drv_gd25q20.h
// Auther      : zhaoning
// Version     :
// Date : 2024-5-11
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2024-5-11
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------
// Define to prevent recursive inclusion ----------------------------------------
#ifndef _DRV_GD25Q20_H_
#define _DRV_GD25Q20_H_

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------

#include "sdk_api.h"

//#include "ql_spi_api.h"
#include "cgpio.h"
#include "cgpio_HW.h"
#include "bsp.h"

// Public defines / typedefs ----------------------------------------------------

#define DRV_GD25Q20_SPI_CS                  17
#define DRV_GD25Q20_SPI_CLK                 16
#define DRV_GD25Q20_SPI_MOSI                18
#define DRV_GD25Q20_SPI_MISO                19

#define DRV_GD25Q20_DUMMY_BYTE              0xFF

#define DRV_GD25Q20_WRITE_ENABLE            0x06
#define DRV_GD25Q20_WRITE_DISABLE           0x04

#define DRV_GD25Q20_STATUS_REG1             0x05
#define DRV_GD25Q20_STATUS_REG2             0x35
#define DRV_GD25Q20_WRITE_STATUS_REG        0x01
#define DRV_GD25Q20_READ_DATA               0x03
#define DRV_GD25Q20_FAST_READ               0x0B

#define DRV_GD25Q20_PAGE_PROGRAM            0x02

#define DRV_GD25Q20_SECTOR_ERASE            0x20
#define DRV_GD25Q20_BLOCK32_ERASE           0x52
#define DRV_GD25Q20_BLOCK64_ERASE           0xD8
#define DRV_GD25Q20_CHIP_ERASE              0xC7

#define DRV_GD25Q20_POWER_DOWN              0xB9

#define DRV_GD25Q20_RELEASE_POWER_DOWN      0xAB
#define DRV_GD25Q20_DEVICE_ID               0xAB
#define DRV_GD25Q20_MANUFACTURER_ID         0x90
#define DRV_GD25Q20_JEDEC_ID                0x9F
#define DRV_GD25Q20_UNIQUE_ID_NUM           0x4B

#define BIT_BUSY                            0x01
#define BIT_WEL                             0x02

#define DRV_GD25Q20_PAGE_SIZE               256
#define DRV_GD25Q20_SOCTOR_SIZE             4096

#define RDID                                0xC84012

/* 是否启用模拟 SPI */
#define          USE_SIMULATE_SPI           0

#define          MAX_TIME_OUT               ((uint32_t)0x1000)

#define DRV_GD25Q20_GPIO_HIGH               1
#define DRV_GD25Q20_GPIO_LOW                0

// Public functions prototypes --------------------------------------------------

void drv_gd25q20_init(void);
uint32_t drv_gd25q20_read_identification (void);
uint8_t drv_gd25q20_read_device_id(void);
uint16_t drv_gd25q20_read_manufacturer_id(void);
void drv_gd25q20_sector_erase(uint32_t addr);
void drv_gd25q20_write(uint8_t* pbuf, uint32_t addr, uint16_t len);
void drv_gd25q20_read( uint8_t* pbuf, uint32_t addr, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _DRV_GD25Q20_H_.2024-5-11 15:23:57 by: zhaoning */

