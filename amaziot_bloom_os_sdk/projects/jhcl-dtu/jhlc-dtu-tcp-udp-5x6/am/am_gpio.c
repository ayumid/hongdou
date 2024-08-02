//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_gpio.c
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

#include "am_gpio.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_trans_net_led_init
  * Description : 初始化sim卡
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_net_led_init(void)
{
    int ret = 0;
    
    //初始化网络灯控制引脚
    ret = GpioSetDirection(GPIO_LINK_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);

    //设置网络灯位低电平，灭
    ret = GpioSetLevel(GPIO_LINK_PIN,1);
    ASSERT(ret == GPIORC_OK);
}

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_trans_sim_switch
  * Description : 切外置卡
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_sim_switch(void)
{
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if(dtu_file_ctx->sim.simlock == 0)
    {
        dtu_file_ctx->sim.sim++;
        if(dtu_file_ctx->sim.sim > 3)
        {
            dtu_file_ctx->sim.sim = 1;
        }
        printf("st_dtu_trans_conf.sim:%d", dtu_file_ctx->sim.sim);
        dtu_trans_conf_file_write(dtu_file_ctx);
    }
}

/**
  * Function    : dtu_trans_sim_init
  * Description : 初始化sim卡
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_trans_sim_init(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    //初始化三合一卡控制引脚
    ret = GpioSetDirection(GPIO_SA_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    //初始化三合一卡控制引脚
    ret = GpioSetDirection(GPIO_SB_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);

    //根据形参判断三合一卡是哪张卡，没有贴三合一卡，这里无效
    switch(dtu_file_ctx->sim.sim)
    {
        case 3:
            ret = GpioSetLevel(GPIO_SA_PIN,0);
            ASSERT(ret == GPIORC_OK);
            ret = GpioSetLevel(GPIO_SB_PIN,0);
            ASSERT(ret == GPIORC_OK);
            printf("GPIO_PIN_13-0&&GPIO_PIN_54-0");
            break;
        case 2:
            ret = GpioSetLevel(GPIO_SA_PIN,1);
            ASSERT(ret == GPIORC_OK);
            ret = GpioSetLevel(GPIO_SB_PIN,0);
            ASSERT(ret == GPIORC_OK);
            printf("GPIO_PIN_13-1&&GPIO_PIN_54-0");
            break;
        case 1:
            ret = GpioSetLevel(GPIO_SA_PIN,1);
            ASSERT(ret == GPIORC_OK);
            ret = GpioSetLevel(GPIO_SB_PIN,1);
            ASSERT(ret == GPIORC_OK);
            printf("GPIO_PIN_13-1&&GPIO_PIN_54-1");
            break;
        default:
            break;
    }
}
#endif

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_gnss_pw_init
  * Description : 初始化gnss电源控制引脚
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_gnss_pw_init(void)
{
    int ret = 0;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    //初始化gnss控制引脚
    ret = GpioSetDirection(GPIO_GNSS_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
   
    //设置是否打开gnss电源
    if(0 == dtu_file_ctx->gnss.gpsflag)
    {
        ret = GpioSetLevel(GPIO_GNSS_PIN, 0);
        printf("%s[%d] gnsspin low", __FUNCTION__, __LINE__);
        ASSERT(ret == GPIORC_OK);
    }
    else if(dtu_file_ctx->gnss.gpsflag >= 1)
    {
        ret = GpioSetLevel(GPIO_GNSS_PIN, 1);
        printf("%s[%d] gnsspin high", __FUNCTION__, __LINE__);
        ASSERT(ret == GPIORC_OK);
    }
}
#endif

// End of file : am_gpio.c 2023-8-28 12:05:44 by: zhaoning 

