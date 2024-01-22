//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : lib_llcc68.c
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

#include <lib_llcc68.h>
#include <lib_llcc68_hal.h>
#include <stdlib.h>
#include <string.h>

// Private defines / typedefs ---------------------------------------------------

#define    RSWaveFileLoRaTest
/*!
 * \brief Radio registers definition
 */
typedef struct
{
    uint16_t    Addr;                             //!< The address of the register
    uint8_t    Value;                            //!< The value of the register
}RadioRegisters_t;

// Private variables ------------------------------------------------------------

// Public variables -------------------------------------------------------------

// Private functions prototypes -------------------------------------------------

/*!
 * \brief Stores the last frequency error measured on LoRa received packet
 */
volatile uint32_t FrequencyError = 0;

/*!
 * \brief Hold the status of the Image calibration
 */
static bool ImageCalibrated = false;

RadioOperatingModes_t OperatingMode;
RadioPacketTypes_t PacketType;
RadioLoRaBandwidths_t LoRaBandwidth;
bool PollingMode = false;
PacketParams_t SX1262_PacketParams;
ModulationParams_t SX1262_ModulationParams;
PacketStatus_t SX1262_PacketStatus;
bool IrqState;

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

void SX1262_Init(void)
{
    SX1262Hal_Init();
    SX1262Hal_Reset();
    SX1262Hal_Wakeup();

}

RadioOperatingModes_t SX1262_GetOperatingMode(void)
{
    return OperatingMode;
}

void SX1262_CheckDeviceReady( void )
{
    if( ( SX1262_GetOperatingMode( ) == MODE_SLEEP ) || ( SX1262_GetOperatingMode( ) == MODE_RX_DC ) )
    {
        SX1262Hal_Wakeup( );
        // Switch is turned off when device is in sleep mode and turned on is all other modes
    }
}

void SX1262_SetPayload( uint8_t *payload, uint8_t size )
{
    SX1262Hal_WriteBuffer( 0x00, payload, size );
}

uint8_t SX1262_GetPayload( uint8_t *buffer, uint8_t *size,  uint8_t maxSize )
{
    uint8_t offset = 0;

    SX1262_GetRxBufferStatus( size, &offset );
    if( *size > maxSize )
    {
        return 1;
    }
    SX1262Hal_ReadBuffer( offset, buffer, *size );
    return *size;
}

void SX1262_SendPayload( uint8_t *payload, uint8_t size, uint32_t timeout )
{
    SX1262_SetPayload( payload, size );
    SX1262_SetTx( timeout );
}

uint8_t SX1262_SetSyncWord( uint8_t *syncWord )
{
    SX1262Hal_WriteRegister( REG_LR_SYNCWORDBASEADDRESS, syncWord, 8 );
    return 0;
}

void SX1262_SetCrcSeed( uint16_t seed )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( seed >> 8 ) & 0xFF );
    buf[1] = ( uint8_t )( seed & 0xFF );

    switch( SX1262_GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            SX1262Hal_WriteRegister( REG_LR_CRCSEEDBASEADDR, buf, 2 );
            break;

        default:
            break;
    }
}

void SX1262_SetCrcPolynomial( uint16_t polynomial )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( polynomial >> 8 ) & 0xFF );
    buf[1] = ( uint8_t )( polynomial & 0xFF );

    switch( SX1262_GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            SX1262Hal_WriteRegister( REG_LR_CRCPOLYBASEADDR, buf, 2 );
            break;

        default:
            break;
    }
}

void SX1262_SetWhiteningSeed( uint16_t seed )
{
    uint8_t regValue = 0;

    switch( SX1262_GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            regValue = SX1262Hal_ReadReg( REG_LR_WHITSEEDBASEADDR_MSB ) & 0xFE;
            regValue = ( ( seed >> 8 ) & 0x01 ) | regValue;
            SX1262Hal_WriteReg( REG_LR_WHITSEEDBASEADDR_MSB, regValue ); // only 1 bit.
            SX1262Hal_WriteReg( REG_LR_WHITSEEDBASEADDR_LSB, ( uint8_t )seed );
            break;

        default:
            break;
    }
}

uint32_t SX1262_GetRandom( void )
{
    uint8_t buf[] = { 0, 0, 0, 0 };

    // Set radio in continuous reception
    SX1262_SetRx( 0 );

    OSATaskSleep(1);

    SX1262Hal_ReadRegister( RANDOM_NUMBER_GENERATORBASEADDR, buf, 4 );

    SX1262_SetStandby( STDBY_RC );

    return ( (uint32_t)buf[0] << 24 ) | ( (uint32_t)buf[1] << 16 ) | ( (uint32_t)buf[2] << 8 ) | (uint32_t)buf[3];
}

void SX1262_SetSleep( void )
{
#ifdef ADV_DEBUG
    printf("SetSleep ");
#endif
  uint8_t buf[] = { 0x04, 0, 0, 0 };

    SX1262Hal_WriteCommand( RADIO_SET_SLEEP,buf, 1 );
    OperatingMode = MODE_SLEEP;
}

void SX1262_SetStandby( RadioStandbyModes_t standbyConfig )
{
#ifdef ADV_DEBUG
    printf("SetStandby ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_STANDBY, ( uint8_t* )&standbyConfig, 1 );
    if( standbyConfig == STDBY_RC )
    {
        OperatingMode = MODE_STDBY_RC;
    }
    else
    {
        OperatingMode = MODE_STDBY_XOSC;
    }
}

void SX1262_SetFs( void )
{
#ifdef ADV_DEBUG
    printf("SetFs ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_FS, 0, 0 );
    OperatingMode = MODE_FS;
}

void SX1262_SetTx( uint32_t timeout )
{
    uint8_t buf[3];

    OperatingMode = MODE_TX;
 
#ifdef ADV_DEBUG
    printf("SetTx ");
#endif
    timeout <<= 6;
    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_TX, buf, 3 );
}

void SX1262_SetRxBoosted( uint32_t timeout )
{
    uint8_t buf[3];

    OperatingMode = MODE_RX;

#ifdef ADV_DEBUG
    printf("SetRxBoosted ");
#endif

    SX1262Hal_WriteReg( REG_RX_GAIN, 0x96 ); // max LNA gain, increase current by ~2mA for around ~3dB in sensivity
    timeout <<= 6;
    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_RX, buf, 3 );
}

void SX1262_SetRx( uint32_t timeout )
{
    uint8_t buf[3];

    OperatingMode = MODE_RX;

#ifdef ADV_DEBUG
    printf("SetRx ");
#endif
    timeout <<= 6;
    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_RX, buf, 3 );
}

void SX1262_SetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    uint8_t buf[6];

    buf[0] = ( uint8_t )( ( rxTime >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( rxTime >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( rxTime & 0xFF );
    buf[3] = ( uint8_t )( ( sleepTime >> 16 ) & 0xFF );
    buf[4] = ( uint8_t )( ( sleepTime >> 8 ) & 0xFF );
    buf[5] = ( uint8_t )( sleepTime & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_RXDUTYCYCLE, buf, 6 );
    OperatingMode = MODE_RX_DC;
}

void SX1262_SetCad( void )
{
    SX1262Hal_WriteCommand( RADIO_SET_CAD, 0, 0 );
    OperatingMode = MODE_CAD;
}

void SX1262_SetTxContinuousWave( void )
{
#ifdef ADV_DEBUG
    printf("SetTxContinuousWave ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_TXCONTINUOUSWAVE, 0, 0 );
}

void SX1262_SetTxInfinitePreamble( void )
{
#ifdef ADV_DEBUG
    printf("SetTxContinuousPreamble ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_TXCONTINUOUSPREAMBLE, 0, 0 );
}

void SX1262_SetStopRxTimerOnPreambleDetect( bool enable )
{
    SX1262Hal_WriteCommand( RADIO_SET_STOPRXTIMERONPREAMBLE, ( uint8_t* )&enable, 1 );
}

void SX1262_SetLoRaSymbNumTimeout( uint8_t SymbNum )
{
    SX1262Hal_WriteCommand( RADIO_SET_LORASYMBTIMEOUT, &SymbNum, 1 );
}

void SX1262_SetRegulatorMode( RadioRegulatorMode_t mode )
{
#ifdef ADV_DEBUG
    printf("SetRegulatorMode ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_REGULATORMODE, ( uint8_t* )&mode, 1 );
}

void SX1262_Calibrate( CalibrationParams_t calibParam )
{
    SX1262Hal_WriteCommand( RADIO_CALIBRATE, &calibParam.Value, 1 );
}

void SX1262_CalibrateImage( uint32_t freq )
{
    uint8_t calFreq[2];

    if( freq > 900000000 )
    {
        calFreq[0] = 0xE1;
        calFreq[1] = 0xE9;
    }
    else if( freq > 850000000 )
    {
        calFreq[0] = 0xD7;
        calFreq[1] = 0xD8;
    }
    else if( freq > 770000000 )
    {
        calFreq[0] = 0xC1;
        calFreq[1] = 0xC5;
    }
    else if( freq > 460000000 )
    {
        calFreq[0] = 0x75;
        calFreq[1] = 0x81;
    }
    else if( freq > 425000000 )
    {
        calFreq[0] = 0x6B;
        calFreq[1] = 0x6F;
    }
    SX1262Hal_WriteCommand( RADIO_CALIBRATEIMAGE, calFreq, 2 );
}

void SX1262_SetPaConfig( uint8_t paDutyCycle, uint8_t HpMax, uint8_t deviceSel, uint8_t paLUT )
{
    uint8_t buf[4];

#ifdef ADV_DEBUG
    printf("SetPaConfig ");
#endif

    buf[0] = paDutyCycle;
    buf[1] = HpMax;
    buf[2] = deviceSel;
    buf[3] = paLUT;
    SX1262Hal_WriteCommand( RADIO_SET_PACONFIG, buf, 4 );
}

void SX1262_SetRxTxFallbackMode( uint8_t fallbackMode )
{
    SX1262Hal_WriteCommand( RADIO_SET_TXFALLBACKMODE, &fallbackMode, 1 );
}

void SX1262_SetDioIrqParams( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
    uint8_t buf[8];

#ifdef ADV_DEBUG
    printf("SetDioIrqParams ");
#endif

    buf[0] = ( uint8_t )( ( irqMask >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( irqMask & 0x00FF );
    buf[2] = ( uint8_t )( ( dio1Mask >> 8 ) & 0x00FF );
    buf[3] = ( uint8_t )( dio1Mask & 0x00FF );
    buf[4] = ( uint8_t )( ( dio2Mask >> 8 ) & 0x00FF );
    buf[5] = ( uint8_t )( dio2Mask & 0x00FF );
    buf[6] = ( uint8_t )( ( dio3Mask >> 8 ) & 0x00FF );
    buf[7] = ( uint8_t )( dio3Mask & 0x00FF );
    SX1262Hal_WriteCommand( RADIO_CFG_DIOIRQ, buf, 8 );
}

uint16_t SX1262_GetIrqStatus( void )
{
    uint8_t irqStatus[2];

    SX1262Hal_ReadCommand( RADIO_GET_IRQSTATUS, irqStatus, 2 );
    return ( irqStatus[0] << 8 ) | irqStatus[1];
}

void SX1262_SetDio2AsRfSwitchCtrl( uint8_t enable )
{
#ifdef ADV_DEBUG
    printf("SetDio2AsRfSwitchCtrl ");
#endif
    SX1262Hal_WriteCommand( RADIO_SET_RFSWITCHMODE, &enable, 1 );
}

void SX1262_SetDio3AsTcxoCtrl( RadioTcxoCtrlVoltage_t tcxoVoltage, uint32_t timeout )
{
    uint8_t buf[4];

    buf[0] = tcxoVoltage & 0x07;
    buf[1] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( timeout & 0xFF );

    SX1262Hal_WriteCommand( RADIO_SET_TCXOMODE, buf, 4 );
}

void SX1262_SetRfFrequency( uint32_t frequency )
{
    uint8_t buf[4];
    uint32_t freq = 0;

#ifdef ADV_DEBUG
    printf("SetRfFrequency ");
#endif

    if( ImageCalibrated == false )
    {
        SX1262_CalibrateImage( frequency );
        ImageCalibrated = true;
    }

    freq = ( uint32_t )( ( double )frequency / ( double )FREQ_STEP );
    buf[0] = ( uint8_t )( ( freq >> 24 ) & 0xFF );
    buf[1] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( freq & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_RFFREQUENCY, buf, 4 );
}

void SX1262_SetPacketType( RadioPacketTypes_t packetType )
{
#ifdef ADV_DEBUG
    printf("SetPacketType ");
#endif

    // Save packet type internally to avoid questioning the radio
    PacketType = packetType;
    SX1262Hal_WriteCommand( RADIO_SET_PACKETTYPE, ( uint8_t* )&packetType, 1 );
}

RadioPacketTypes_t SX1262_GetPacketType( void )
{
    return PacketType;
}

void SX1262_SetTxParams( int8_t power, RadioRampTimes_t rampTime )
{
    uint8_t buf[2];
    //DigitalIn OPT( A3 );

#ifdef ADV_DEBUG
    printf("SetTxParams ");
#endif
        SX1262_SetPaConfig( 0x04, 0x07, 0x00, 0x01 );
        if( power > 22 )
        {
            power = 22;
        }
        else if( power < -3 )
        {
            power = -3;
        }
        SX1262Hal_WriteReg( REG_OCP, 0x38 ); // current max 160mA for the whole device
    buf[0] = power;
    buf[1] = ( uint8_t )rampTime;
    SX1262Hal_WriteCommand( RADIO_SET_TXPARAMS, buf, 2 );
}

void SX1262_SetModulationParams( ModulationParams_t *modulationParams )
{
    uint8_t n;
    uint32_t tempVal = 0;
    uint8_t buf[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#ifdef ADV_DEBUG
    printf("SetModulationParams ");
#endif

    // Check if required configuration corresponds to the stored packet type
    // If not, silently update radio packet type
    if( PacketType != modulationParams->PacketType )
    {
        SX1262_SetPacketType( modulationParams->PacketType );
    }

    switch( modulationParams->PacketType )
    {
    case PACKET_TYPE_GFSK:
        n = 8;
        tempVal = ( uint32_t )( 32 * ( ( double )XTAL_FREQ / ( double )modulationParams->Params.Gfsk.BitRate ) );
        buf[0] = ( tempVal >> 16 ) & 0xFF;
        buf[1] = ( tempVal >> 8 ) & 0xFF;
        buf[2] = tempVal & 0xFF;
        buf[3] = modulationParams->Params.Gfsk.ModulationShaping;
        buf[4] = modulationParams->Params.Gfsk.Bandwidth;
        tempVal = ( uint32_t )( ( double )modulationParams->Params.Gfsk.Fdev / ( double )FREQ_STEP );
        buf[5] = ( tempVal >> 16 ) & 0xFF;
        buf[6] = ( tempVal >> 8 ) & 0xFF;
        buf[7] = ( tempVal& 0xFF );
        break;
    case PACKET_TYPE_LORA:
        n = 4;
        switch( modulationParams->Params.LoRa.Bandwidth )
        {
            case LORA_BW_500:
                 modulationParams->Params.LoRa.LowDatarateOptimize = 0x00;
                break;
            case LORA_BW_250:
                if( modulationParams->Params.LoRa.SpreadingFactor == 12 )
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x01;
                }
                else
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x00;
                }
                break;
            case LORA_BW_125:
                if( modulationParams->Params.LoRa.SpreadingFactor >= 11 )
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x01;
                }
                else
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x00;
                }
                break;
            case LORA_BW_062:
                if( modulationParams->Params.LoRa.SpreadingFactor >= 10 )
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x01;
                }
                else
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x00;
                }
                break;
            case LORA_BW_041:
                if( modulationParams->Params.LoRa.SpreadingFactor >= 9 )
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x01;
                }
                else
                {
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x00;
                }
                break;
            case LORA_BW_031:
            case LORA_BW_020:
            case LORA_BW_015:
            case LORA_BW_010:
            case LORA_BW_007:
                    modulationParams->Params.LoRa.LowDatarateOptimize = 0x01;
                break;
            default:
                break;
        }
        buf[0] = modulationParams->Params.LoRa.SpreadingFactor;
        buf[1] = modulationParams->Params.LoRa.Bandwidth;
        buf[2] = modulationParams->Params.LoRa.CodingRate;
#ifdef    RSWaveFileLoRaTest
        buf[3] = 0;
#else
        buf[3] = modulationParams->Params.LoRa.LowDatarateOptimize;
#endif
        break;
    default:
    case PACKET_TYPE_NONE:
        return;
    }
    SX1262Hal_WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, n );
}

void SX1262_SetPacketParams( PacketParams_t *packetParams )
{
    uint8_t n;
    uint8_t crcVal = 0;
    uint8_t buf[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#ifdef ADV_DEBUG
    printf("SetPacketParams ");
#endif

    // Check if required configuration corresponds to the stored packet type
    // If not, silently update radio packet type
    if( PacketType != packetParams->PacketType )
    {
        SX1262_SetPacketType( packetParams->PacketType );
    }

    switch( packetParams->PacketType )
    {
    case PACKET_TYPE_GFSK:
        if( packetParams->Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_IBM )
        {
            SX1262_SetCrcSeed( CRC_IBM_SEED );
            SX1262_SetCrcPolynomial( CRC_POLYNOMIAL_IBM );
            crcVal = RADIO_CRC_2_BYTES;
        }
        else if(  packetParams->Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_CCIT )
        {
            SX1262_SetCrcSeed( CRC_CCITT_SEED );
            SX1262_SetCrcPolynomial( CRC_POLYNOMIAL_CCITT );
            crcVal = RADIO_CRC_2_BYTES_INV;
        }
        else
        {
            crcVal = packetParams->Params.Gfsk.CrcLength;
        }
        n = 9;
        // convert preamble length from byte to bit
        packetParams->Params.Gfsk.PreambleLength = packetParams->Params.Gfsk.PreambleLength << 3;

        buf[0] = ( packetParams->Params.Gfsk.PreambleLength >> 8 ) & 0xFF;
        buf[1] = packetParams->Params.Gfsk.PreambleLength;
        buf[2] = packetParams->Params.Gfsk.PreambleMinDetect;
        buf[3] = ( packetParams->Params.Gfsk.SyncWordLength << 3 ); // convert from byte to bit
        buf[4] = packetParams->Params.Gfsk.AddrComp;
        buf[5] = packetParams->Params.Gfsk.HeaderType;
        buf[6] = packetParams->Params.Gfsk.PayloadLength;
        buf[7] = crcVal;
        buf[8] = packetParams->Params.Gfsk.DcFree;
        break;
    case PACKET_TYPE_LORA:
        n = 6;
        buf[0] = ( packetParams->Params.LoRa.PreambleLength >> 8 ) & 0xFF;
        buf[1] = packetParams->Params.LoRa.PreambleLength;
        buf[2] = packetParams->Params.LoRa.HeaderType;
        buf[3] = packetParams->Params.LoRa.PayloadLength;
        buf[4] = packetParams->Params.LoRa.CrcMode;
        buf[5] = packetParams->Params.LoRa.InvertIQ;
        break;
    default:
    case PACKET_TYPE_NONE:
        return;
    }
    SX1262Hal_WriteCommand( RADIO_SET_PACKETPARAMS, buf, n );
}

void SX1262_SetCadParams( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, RadioCadExitModes_t cadExitMode, uint32_t cadTimeout )
{
    uint8_t buf[7];

    buf[0] = ( uint8_t )cadSymbolNum;
    buf[1] = cadDetPeak;
    buf[2] = cadDetMin;
    buf[3] = ( uint8_t )cadExitMode;
    buf[4] = ( uint8_t )( ( cadTimeout >> 16 ) & 0xFF );
    buf[5] = ( uint8_t )( ( cadTimeout >> 8 ) & 0xFF );
    buf[6] = ( uint8_t )( cadTimeout & 0xFF );
    SX1262Hal_WriteCommand( RADIO_SET_CADPARAMS, buf, 7 );
    OperatingMode = MODE_CAD;
}

void SX1262_SetBufferBaseAddresses( uint8_t txBaseAddress, uint8_t rxBaseAddress )
{
    uint8_t buf[2];

#ifdef ADV_DEBUG
    printf("SetBufferBaseAddresses ");
#endif

    buf[0] = txBaseAddress;
    buf[1] = rxBaseAddress;
    SX1262Hal_WriteCommand( RADIO_SET_BUFFERBASEADDRESS, buf, 2 );
}

RadioStatus_t SX1262_GetStatus( void )
{
    uint8_t stat = 0;
    RadioStatus_t status;

    SX1262Hal_ReadCommand( RADIO_GET_STATUS, ( uint8_t * )&stat, 1 );
    status.Value = stat;
    return status;
}

int8_t SX1262_GetRssiInst( void )
{
    uint8_t rssi;

    SX1262Hal_ReadCommand( RADIO_GET_RSSIINST, ( uint8_t* )&rssi, 1 );
    return( -( rssi / 2 ) );
}

void SX1262_GetRxBufferStatus( uint8_t *payloadLength, uint8_t *rxStartBufferPointer )
{
    uint8_t status[2];

    SX1262Hal_ReadCommand( RADIO_GET_RXBUFFERSTATUS, status, 2 );

    // In case of LORA fixed header, the payloadLength is obtained by reading
    // the register REG_LR_PAYLOADLENGTH
    if( ( SX1262_GetPacketType( ) == PACKET_TYPE_LORA ) && ( SX1262Hal_ReadReg( REG_LR_PACKETPARAMS ) >> 7 == 1 ) )
    {
        *payloadLength = SX1262Hal_ReadReg( REG_LR_PAYLOADLENGTH );
    }
    else
    {
        *payloadLength = status[0];
    }
    *rxStartBufferPointer = status[1];
}

void SX1262_GetPacketStatus( PacketStatus_t *pktStatus )
{
    uint8_t status[3];

    SX1262Hal_ReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

    pktStatus->packetType = SX1262_GetPacketType( );
    switch( pktStatus->packetType )
    {
        case PACKET_TYPE_GFSK:
            pktStatus->Params.Gfsk.RxStatus = status[0];
            pktStatus->Params.Gfsk.RssiSync = -status[1] / 2;
            pktStatus->Params.Gfsk.RssiAvg = -status[2] / 2;
            pktStatus->Params.Gfsk.FreqError = 0;
            break;

        case PACKET_TYPE_LORA:
            pktStatus->Params.LoRa.RssiPkt = -status[0] / 2;
            ( status[1] < 128 ) ? ( pktStatus->Params.LoRa.SnrPkt = status[1] / 4 ) : ( pktStatus->Params.LoRa.SnrPkt = ( ( status[1] - 256 ) /4 ) );
            pktStatus->Params.LoRa.SignalRssiPkt = -status[2] / 2;
            pktStatus->Params.LoRa.FreqError = FrequencyError;
            break;

        default:
        case PACKET_TYPE_NONE:
            // In that specific case, we set everything in the pktStatus to zeros
            // and reset the packet type accordingly
            memset( pktStatus, 0, sizeof( PacketStatus_t ) );
            pktStatus->packetType = PACKET_TYPE_NONE;
            break;
    }
}

RadioError_t SX1262_GetDeviceErrors( void )
{
    RadioError_t error;

    SX1262Hal_ReadCommand( RADIO_GET_ERROR, ( uint8_t * )&error, 2 );
    return error;
}

void SX1262_ClearIrqStatus( uint16_t irq )
{
    uint8_t buf[2];
#ifdef ADV_DEBUG
    printf("ClearIrqStatus ");
#endif
    buf[0] = ( uint8_t )( ( ( uint16_t )irq >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( ( uint16_t )irq & 0x00FF );
    SX1262Hal_WriteCommand( RADIO_CLR_IRQSTATUS, buf, 2 );
}

void SX1262_SetPollingMode( void )
{
    PollingMode = true;
}

void SX1262_SetInterruptMode( void )
{
    PollingMode = false;
}

void SX1262_OnDioIrq( void )
{
    /*
     * When polling mode is activated, it is up to the application to call
     * ProcessIrqs( ). Otherwise, the driver automatically calls ProcessIrqs( )
     * on radio interrupt.
     */
    if( PollingMode == true )
    {
        IrqState = true;
    }
    else
    {
        SX1262_ProcessIrqs( );
    }
}

void SX1262_ProcessIrqs( void )
{
    if( PollingMode == true )
    {
        if( IrqState == true )
        {
            __disable_irq( );
            IrqState = false;
            __enable_irq( );
        }
        else
        {
            return;
        }
    }

    uint16_t irqRegs = SX1262_GetIrqStatus( );
    SX1262_ClearIrqStatus( IRQ_RADIO_ALL );

#ifdef ADV_DEBUG
    printf("0x%04x\n\r", irqRegs );
#endif

    if( ( irqRegs & IRQ_HEADER_VALID ) == IRQ_HEADER_VALID )
    {
        // LoRa Only
        FrequencyError = 0x000000 | ( (uint32_t)( 0x0F & SX1262Hal_ReadReg( REG_FREQUENCY_ERRORBASEADDR ) ) << 16 );
        FrequencyError = FrequencyError | ((uint32_t) SX1262Hal_ReadReg( REG_FREQUENCY_ERRORBASEADDR + 1 ) << 8 );
        FrequencyError = FrequencyError | ( SX1262Hal_ReadReg( REG_FREQUENCY_ERRORBASEADDR + 2 ) );
    }

    if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
    {
        if( txDone != NULL )
        {
            txDone( );
        }
    }

    if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
    {
        if( ( irqRegs & IRQ_CRC_ERROR ) == IRQ_CRC_ERROR )
        {
            if( rxError != NULL )
            {
                rxError( IRQ_CRC_ERROR_CODE );
            }
        }
        else
        {
            if( rxDone != NULL )
            {
                rxDone( );
            }
        }
    }

    if( ( irqRegs & IRQ_CAD_DONE ) == IRQ_CAD_DONE )
    {
        if( cadDone != NULL )
        {
            cadDone( ( irqRegs & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED );
        }
    }

    if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT )
    {
        if( ( txTimeout != NULL ) && ( OperatingMode == MODE_TX ) )
        {
            txTimeout( );
        }
        else if( ( rxTimeout != NULL ) && ( OperatingMode == MODE_RX ) )
        {
            rxTimeout( );
        }
        else
        {
            //assert_param( FAIL );
        }
    }
    
/*
    //IRQ_PREAMBLE_DETECTED                   = 0x0004,
    if( irqRegs & IRQ_PREAMBLE_DETECTED )
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_PBL_DETECT_CODE);
            
        }
    }

    //IRQ_SYNCWORD_VALID                      = 0x0008,
    if( irqRegs & IRQ_SYNCWORD_VALID )
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_SYNCWORD_VALID_CODE  );
        }
    }

    //IRQ_HEADER_VALID                        = 0x0010,
    if ( irqRegs & IRQ_HEADER_VALID ) 
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_HEADER_VALID_CODE );
        }
    } 

    //IRQ_HEADER_ERROR                        = 0x0020,
    if( irqRegs & IRQ_HEADER_ERROR )
    {
        if( rxError != NULL )
        {
            rxError( IRQ_HEADER_ERROR_CODE );
        }
    }  
*/
}

// End of file : lib_llcc68.c 2023-7-21 16:42:34 by: zhaoning 

