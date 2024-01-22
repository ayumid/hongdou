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

#include "UART.h"
#include "osa.h"
#include "teldef.h"
#include "pmic_rtc.h"
#include "osa_mem.h"
#include "ql_i2c_api.h"

// Private macros / types / typedef ---------------------------------------------

/*Log太多的时候不建议使用UART log，会出现很多异常，建议使用CATStudio 查看log*/
// debug uart log
#define sample_i2c_uart_printf(fmt, args...) do { sdklogConfig(1); sdkLogPrintf(fmt, ##args); } while(0)
// CATStudio usb log
#define sample_i2c_catstudio_printf(fmt, args...) do { sdklogConfig(0); sdkLogPrintf(fmt, ##args); } while(0)

#define sample_i2c_sleep(x) OSATaskSleep((x) * 200)//second

#define SAMPLE_I2C_STACK_SIZE            0x2000

#define SAMPLE_I2C_SLAVE_ADDR      0x13  

// Private variables ------------------------------------------------------------

static void* sample_i2c_task_stack = NULL;

static OSTaskRef sample_i2c_task_ref = NULL;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

static void sample_i2c_task(void *ptr);

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

void Phase1Inits_enter(void)
{
}

void Phase1Inits_exit(void)
{
}

void Phase2Inits_enter(void)
{
}

void Phase2Inits_exit(void)
{
    int ret;
    
    sample_i2c_task_stack = malloc(SAMPLE_I2C_STACK_SIZE);
    
    ret = OSATaskCreate(&sample_i2c_task_ref, sample_i2c_task_stack, SAMPLE_I2C_STACK_SIZE, 100, "sample_i2c_task", sample_i2c_task, NULL);
    ASSERT(ret == OS_SUCCESS);
}

static void sample_i2c_task(void *ptr)
{
    unsigned char RegAddr, RegWriteValue, RegReadValue = 0;
    int ret;

    //int i2c_no = 1; // i2c2
    //int i2c_no = 3; // i2c4
    int i2c_no = 0; // i2c1
    int i2c_mode = STANDARD_MODE;
    
    ret = ql_i2c_init(i2c_no, i2c_mode);
    sample_i2c_catstudio_printf("ql_i2c_init ret %d", ret);
    
    while(1){
        
        //---------------------
        //CHIP ID
        //---------------------
        RegAddr       = 0x00;
        //ret = ql_i2c_read(i2c_no, SAMPLE_I2C_SLAVE_ADDR, RegAddr, &RegReadValue, 1);
        ret = ql_i2c_read_ext(i2c_no, SAMPLE_I2C_SLAVE_ADDR, &RegAddr, 1, &RegReadValue, 1);
        sample_i2c_catstudio_printf("read i2c value=0x%x, ret=%d\n", RegReadValue, ret);

        //---------------------
        //Reset
        //---------------------
        RegAddr       = 0x36;
        RegWriteValue = 0xB6;
        //ret = ql_i2c_write(i2c_no, SAMPLE_I2C_SLAVE_ADDR, RegAddr, &RegWriteValue, 1);
        ret = ql_i2c_write_ext(i2c_no, SAMPLE_I2C_SLAVE_ADDR, &RegAddr, 1, &RegWriteValue, 1);
        sample_i2c_catstudio_printf("write i2c value=0x%x, ret=%d\n", RegWriteValue, ret);
    
        OSATaskSleep(5*200);
    }
}

// End of file : main.c 2023-5-17 9:22:25 by: zhaoning 

