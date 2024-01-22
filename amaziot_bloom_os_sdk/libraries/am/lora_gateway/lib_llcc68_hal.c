//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : lib_llcc68_hal.c
// Auther      : zhaoning
// Version     :
// Date : 2023-7-21
// Description :
//          
//          
// History     :
//     
//    1. Time         :  2023-7-21
//       Modificator  : zhaoning
//       Modification : Created
//    2.
// Others :
//------------------------------------------------------------------------------

// Includes ---------------------------------------------------------------------

#include "lib_llcc68_hal.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static uint8_t bitcnt;

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

// Public functions prototypes --------------------------------------------------

extern void DelayInMilliSecond(unsigned int us);
extern void Timer0_Switch(unsigned char OnOff);

// Functions --------------------------------------------------------------------

/*!
 * \brief Helper macro to create Interrupt objects only if the pin name is
 *        different from NC
 */
/*#define CreateDioPin( pinName, dio )                 \
if( pinName == NC )                      \
{                                        \
    dio = NULL;                          \
}                                        \
else                                     \
{                                        \
    dio = new InterruptIn( pinName );    \
}*/

/*!
 * \brief Helper macro to avoid duplicating code for setting dio pins parameters
 */
/*#define DioAssignCallback( dio, pinMode, callback )                    \
if( dio != NULL )                                          \
{                                                          \
    dio->mode( pinMode );                                  \
    dio->rise( this, static_cast <Trigger>( callback ) );  \
}*/

/*!
 * \brief Used to block execution waiting for low state on radio busy pin.
 */
//#define WaitOnBusy( )             while( BUSY == 1 ){ }

/*!
 * \brief Used to block execution to give enough time to Busy to go up
 *        in order to respect Tsw, see datasheet ยง8.3.1
 */
/*#define WaitOnCounter( )          for( uint8_t counter = 0; counter < 15; counter++ ) \
  {  __NOP( ); }*/

//////////////////  
//// This code handles cases where assert_param is undefined
//#ifndef assert_param
//#define assert_param( ... )
//#endif

void SX1262MOSIOut(void)
{
    int ret = 0;
    
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(MOSI_PIN, config);

    //设置高电平
    ret = GpioSetLevel(MOSI_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void SX1262SCKOut(void)
{
    int ret = 0;
    
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(SCK_PIN, config);

    //设置高电平
    ret = GpioSetLevel(SCK_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void SX1262NssOut(void)
{
    int ret = 0;
    
    //初始化引脚
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(NSS_PIN, config);

    //设置高电平
    ret = GpioSetLevel(NSS_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void SX1262POROut(void)
{
    int ret = 0;
    
    //初始化引脚
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(POR_PIN, config);
    //设置高电平
    ret = GpioSetLevel(POR_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void SX1262BusyOut(void)
{
    int ret = 0;
    
    //初始化引脚
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(BUSY_PIN, config);

    //设置高电平
    ret = GpioSetLevel(BUSY_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void SX1262MISOOut(void)
{
    int ret = 0;
    
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_OUT_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULLUP_ENABLE;

    GpioInitConfiguration(MISO_PIN, config);

    //设置高电平
    ret = GpioSetLevel(MISO_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

//void SX1262DIO1Out(void)
//{
//    int ret = 0;
//    
//    GPIOConfiguration config = {0};

//    config.pinDir = GPIO_OUT_PIN;
//    config.pinEd = GPIO_NO_EDGE;
//    config.pinPull = GPIO_PULLUP_ENABLE;

//    GpioInitConfiguration(DIO1_PIN, config);

//    //设置高电平
//    ret = GpioSetLevel(DIO1_PIN, 1);
//    ASSERT(ret == GPIORC_OK);
//}

void SX1262PORIn(void)
{
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULL_DISABLE;

    GpioInitConfiguration(POR_PIN, config);
}

void SX1262BusyIn(void)
{
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULL_DISABLE;

    GpioInitConfiguration(BUSY_PIN, config);
}

void SX1262MISOIn(void)
{
    GPIOConfiguration config = {0};

    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULL_DISABLE;

    GpioInitConfiguration(MISO_PIN, config);
}

void dev_slave_hex_printf(uint8_t* addr, uint8_t len)
{
    int j = 0;
    for(j = 0; j < len; j++)
    {
        uprintf("%02X", addr[j]);
    }
}

void SX1262Dio1In(void)
{
//    GPIOConfiguration config = {0};

//    config.pinDir = GPIO_IN_PIN;
//    config.pinEd = GPIO_RISE_EDGE;
//    config.pinPull = GPIO_PULL_DISABLE;
//    config.isr = dio1_gpio_irq_handler;
//    GpioInitConfiguration(DIO1_PIN, config);
    GPIOConfiguration config = {0};
    
    config.pinDir = GPIO_IN_PIN;
    config.pinEd = GPIO_NO_EDGE;
    config.pinPull = GPIO_PULL_DISABLE;

    GpioInitConfiguration(DIO1_PIN, config);

}

//void SX1262Dio2In(void)
//{
//    
//}

//void SX1262Dio3In(void)
//{
//    
//}

void SetSX1262SCK(void)
{
    int ret = 0;

    ret = GpioSetLevel(SCK_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void ClrSX1262SCK(void)
{
    int ret = 0;

    ret = GpioSetLevel(SCK_PIN, 0);
    ASSERT(ret == GPIORC_OK);
}

void SetSX1262POR(void)
{
    int ret = 0;

    ret = GpioSetLevel(POR_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void ClrSX1262POR(void)
{
    int ret = 0;

    ret = GpioSetLevel(POR_PIN, 0);
    ASSERT(ret == GPIORC_OK);
}

void SetSX1262MOSI(void)
{
    int ret = 0;

    ret = GpioSetLevel(MOSI_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void ClrSX1262MOSI(void)
{
    int ret = 0;

    ret = GpioSetLevel(MOSI_PIN, 0);
    ASSERT(ret == GPIORC_OK);
}

void SetSX1262Nss(void)
{
    int ret = 0;

    ret = GpioSetLevel(NSS_PIN, 1);
    ASSERT(ret == GPIORC_OK);
}

void ClrSX1262Nss(void)
{
    int ret = 0;

    ret = GpioSetLevel(NSS_PIN, 0);
    ASSERT(ret == GPIORC_OK);
}

//void SetSX1262Busy(void)
//{
//    int ret = 0;

//    ret = GpioSetLevel(BUSY_PIN, 1);
//    ASSERT(ret == GPIORC_OK);
//}

//void ClrSX1262Busy(void)
//{
//    HAL_GPIO_WritePin(BUSY_GPIO_PORT, BUSY_PIN, GPIO_PIN_RESET); 
//}

//void ClrSX1262Dio1(void)
//{
//    int ret = 0;

//    ret = GpioSetLevel(DIO1_PIN, 0);
//    ASSERT(ret == GPIORC_OK);
//}

//void SetSX1262MISO(void)
//{
//    int ret = 0;

//    ret = GpioSetLevel(MISO_PIN, 1);
//    ASSERT(ret == GPIORC_OK);
//}

//void ClrSX1262MISO(void)
//{
//    int ret = 0;

//    ret = GpioSetLevel(MISO_PIN, 0);
//    ASSERT(ret == GPIORC_OK);
//}

uint8_t SX1262MISO(void)
{
    return GpioGetLevel(MISO_PIN);
}

uint8_t SX1262Dio1(void)
{
    return GpioGetLevel(DIO1_PIN);
}

void WaitOnBusy(void)
{
    uint8_t res = 0;
    
    while(1)
    {
        res = GpioGetLevel(BUSY_PIN);
//        uprintf("BUSY_PIN:%d\n", res);
        if(res == 0)
            break;
    }
}

void dtu_delay_us(UINT32 count)
{
    int i = 0;
    int j = 0;

    for(j = 0; j < count; j++)
    {
        for(i = 0; i < 35; i++)
        {
//            i = i;
        }
    }
}

#if 1
void SX1262Hal_SPIWrite( uint8_t Data )
{
//    uint8_t bitcnt = 0;


    ClrSX1262Nss();
//     ClrSX1262SCK();
 
    for(bitcnt = 8; bitcnt != 0; bitcnt--)
    {
        if(Data&0x80)
            SetSX1262MOSI();
        else
            ClrSX1262MOSI();
        Data <<= 1; 
        Timer0_Switch(1);
        DelayInMilliSecond(1);
        Timer0_Switch(0);
//        dtu_delay_us(2);
        
        SetSX1262SCK();
        Timer0_Switch(1);
        DelayInMilliSecond(1);
        Timer0_Switch(0);
//        dtu_delay_us(2);
        ClrSX1262SCK();
    }
//     ClrSX1262SCK();
//     SetSX1262MOSI();
}

uint8_t SX1262Hal_SPIWriteRead( uint8_t dataTX )
{
//    uint8_t    bitcnt;    
    uint8_t    dataRX;

//     SetSX1262MOSI();
//     ClrSX1262SCK();
    ClrSX1262Nss();
     dataRX=0;
     for(bitcnt=8; bitcnt!=0; bitcnt--)
    {
        dataRX <<= 1;
        if(dataTX&0x80)
            SetSX1262MOSI();
        else
            ClrSX1262MOSI();
        dataTX <<= 1;         
        Timer0_Switch(1);
        DelayInMilliSecond(1);
        Timer0_Switch(0);
//        dtu_delay_us(2);
        SetSX1262SCK();
        Timer0_Switch(1);
        DelayInMilliSecond(1);
        Timer0_Switch(0);
//        dtu_delay_us(2);
        if(SX1262MISO())
            dataRX |= 0x01;
        ClrSX1262SCK();
    }
//    ClrSX1262SCK();        
//    SetSX1262MOSI();

    return dataRX;
}
#else
void SX1262Hal_SPIWrite( uint8_t Data )
{
    uint8_t    dataTX = 0;

    dataTX = Data;
    ql_spi_write(QL_SPI_PORT0, (unsigned char*)&dataTX, 1);

}

uint8_t SX1262Hal_SPIWriteRead( uint8_t dataTX )
{
    uint8_t    _dataTX = 0;
    uint8_t    dataRX = 0;

    _dataTX = dataTX;
    ql_spi_write_read(QL_SPI_PORT0, (unsigned char*)&_dataTX, (unsigned char*)&dataRX, 1);

    return dataRX;
}
#endif /* ifdef 0.2024-1-10 10:54:08 by: zhaoning */

void SX1262Hal_Init(void)
{
    SX1262POROut();
    ClrSX1262POR();
    SX1262NssOut();
    SX1262MOSIOut();
    SX1262MISOIn();
    SX1262BusyIn();
    SX1262Dio1In();
//    SX1262Dio2In();
//    SX1262Dio3In();
    SX1262SCKOut();
    SetSX1262Nss();
    SetSX1262MOSI();
    ClrSX1262SCK();
}

void SX1262Hal_Reset( void )
{
//    __disable_irq();
    OSATaskSleep( 4 );
    SX1262POROut();
    ClrSX1262POR();
    OSATaskSleep( 10 );
    SetSX1262POR();
    SX1262PORIn();
    OSATaskSleep( 10 );
//    __enable_irq();
}

void SX1262Hal_GetStatus( void )
{
    ClrSX1262Nss();
    SX1262Hal_SPIWrite( RADIO_GET_STATUS );
    SX1262Hal_SPIWrite( 0 );
    SetSX1262Nss();
}

void SX1262Hal_Wakeup( void )
{
//    __disable_irq();
    SX1262Hal_GetStatus();
    OSATaskSleep( 4 );
    SX1262Hal_GetStatus();
    WaitOnBusy( );
//    __enable_irq();
}


void SX1262Hal_WriteCommand( RadioCommands_t command,uint8_t *buffer,uint16_t size )
{
        uint16_t i;
        WaitOnBusy( );

        ClrSX1262Nss();
        SX1262Hal_SPIWrite( ( uint8_t )command );
        for( i = 0; i < size; i++ )
        {
            SX1262Hal_SPIWrite( buffer[i] );
        }
        SetSX1262Nss();

    //lib_cs32_delay_us(20);
    if( command != RADIO_SET_SLEEP )
    {
        WaitOnBusy( );
    }
}

void SX1262Hal_ReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint16_t i;
    WaitOnBusy( );
        ClrSX1262Nss();
        if( command == RADIO_GET_STATUS )
        {
            SX1262Hal_SPIWrite( ( uint8_t )command );
            buffer[0] = SX1262Hal_SPIWriteRead( 0 );
            //SX1262Hal_SPIWrite( 0 );
        }
        else
        {
            SX1262Hal_SPIWrite( ( uint8_t )command );
            SX1262Hal_SPIWrite( 0 );
            for(  i = 0; i < size; i++ )
            {
                 buffer[i] = SX1262Hal_SPIWriteRead( 0 );
            }
        }
        SetSX1262Nss();
    //lib_cs32_delay_us(20);
    WaitOnBusy( );
}

void SX1262Hal_WriteRegister( uint16_t address, uint8_t *buffer, uint16_t size )
{
    uint16_t i;
    WaitOnBusy( );

        ClrSX1262Nss();
        SX1262Hal_SPIWrite( RADIO_WRITE_REGISTER );
        SX1262Hal_SPIWrite( ( address & 0xFF00 ) >> 8 );
        SX1262Hal_SPIWrite( address & 0x00FF );
        for( i = 0; i < size; i++ )
        {
            SX1262Hal_SPIWrite( buffer[i] );
        }
        SetSX1262Nss();
    //lib_cs32_delay_us(20);
    WaitOnBusy( );
}

void SX1262Hal_WriteReg( uint16_t address, uint8_t value )
{
    SX1262Hal_WriteRegister( address, &value, 1 );
}

void SX1262Hal_ReadRegister( uint16_t address, uint8_t *buffer, uint16_t size )
{
    uint16_t i;
    WaitOnBusy( );

        ClrSX1262Nss();
        SX1262Hal_SPIWrite( RADIO_READ_REGISTER );
        SX1262Hal_SPIWrite( ( address & 0xFF00 ) >> 8 );
        SX1262Hal_SPIWrite( address & 0x00FF );
        SX1262Hal_SPIWrite( 0 );
        for(  i = 0; i < size; i++ )
        {
            buffer[i] = SX1262Hal_SPIWriteRead( 0 );
        }
        SetSX1262Nss();
    //lib_cs32_delay_us(20);
    WaitOnBusy( );
}

uint8_t SX1262Hal_ReadReg( uint16_t address )
{
    uint8_t data;

    SX1262Hal_ReadRegister( address, &data, 1 );
    return data;
}

void SX1262Hal_WriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    uint16_t i;
    WaitOnBusy( );

        ClrSX1262Nss();
        SX1262Hal_SPIWrite( RADIO_WRITE_BUFFER );
        SX1262Hal_SPIWrite( offset );
        for(  i = 0; i < size; i++ )
        {
            SX1262Hal_SPIWrite( buffer[i] );
        }
        SetSX1262Nss();
    //lib_cs32_delay_us(20);
    WaitOnBusy( );
}

void SX1262Hal_ReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    uint16_t i;
    WaitOnBusy( );

        ClrSX1262Nss();
        SX1262Hal_SPIWrite( RADIO_READ_BUFFER );
        SX1262Hal_SPIWrite( offset );
        SX1262Hal_SPIWrite( 0 );
        for(  i = 0; i < size; i++ )
        {
            buffer[i] = SX1262Hal_SPIWriteRead( 0 );
        }
        SetSX1262Nss();
    //lib_cs32_delay_us(20);
    WaitOnBusy( );
}

//uint8_t SX1262Hal_GetDioStatus( void )
//{
//    uint8_t    temp=0;
//    if (SX1262Dio3()) temp|=8;
//    if (SX1262Dio2()) temp|=4;
//    if (SX1262Dio1()) temp|=2;
//    if (SX1262Busy()) temp|=1;
//        
//    return temp;
//}

// End of file : lib_llcc68_hal.c 2023-7-21 16:39:19 by: zhaoning 

