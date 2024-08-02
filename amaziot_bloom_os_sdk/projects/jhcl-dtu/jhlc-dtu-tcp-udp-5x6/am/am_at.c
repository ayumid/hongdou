//------------------------------------------------------------------------------
// Copyright , 2017-2023 奇迹物联（北京）科技有限公司
// Filename    : am_at.c
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
#include "teldef.h"
#include "download.h"

#include "am_at.h"
#include "am_gpio.h"

// Private defines / typedefs ---------------------------------------------------

// Private variables ------------------------------------------------------------

static DTU_AT_CMD_PARA_T dtu_atcmd_param;
static unsigned int dtu_at_tmp_count = 0;
static char dtu_at_cmd_tmp[DTU_CMD_LINE_MAX_LINE_SIZE];
static DTU_DSAT_SIO_INFO_TYPE_E g_sio_info;

// Public variables -------------------------------------------------------------

DTU_UART_PARAM_T st_uart;

// Private functions prototypes -------------------------------------------------

static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setip(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheartflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinkflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinktype(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setyzcsq(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_help(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_version(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reload(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reset(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto0(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcmdpw(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setati(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_ota(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmi(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmm(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setip(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);//
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hbflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hb(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linkflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linktype(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_link(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_query(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reset(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reload(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_version(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
//static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_yzcsq(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_o(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_cmdpw(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setati(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_ota(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmi(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmm(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setgps(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setgps(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsim(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsimlock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsim(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsimlock(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp);
#endif

static DTU_AT_CMD_TABLE_T dtu_atcmd_table[] = {
    { "+TRANSIP",dtu_atcmd_setip},
    { "+TRANSHEARTFLAG",dtu_atcmd_setheartflag},
    { "+TRANSHEART",dtu_atcmd_setheart},
    { "+TRANSLINKFLAG",dtu_atcmd_setlinkflag},
    { "+TRANSLINKTYPE",dtu_atcmd_setlinktype},
    { "+TRANSLINK",dtu_atcmd_setlink},
//    { "+TRANSYZCSQ",dtu_atcmd_setyzcsq},
    { "+HELP",dtu_atcmd_help},
    { "+VERSION",dtu_atcmd_version},
    { "+RELOAD",dtu_atcmd_reload},
    { "+RESET",dtu_atcmd_reset},
    { "O",dtu_atcmd_seto},
    { "O0",dtu_atcmd_seto0},
    { "+CMDPW",dtu_atcmd_setcmdpw},
#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
    { "+SIM",dtu_atcmd_setsim},
    { "+SIMLOCK",dtu_atcmd_setsimlock},
#endif
#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
    { "+SETTCPGPS",dtu_atcmd_setgps},
#endif
    { "I",dtu_atcmd_setati},
    { "+OTA",dtu_atcmd_ota},
    { "+CGMI",dtu_atcmd_setcgmi},
    { "+CGMM",dtu_atcmd_setcgmm},
    { "+CGMR",dtu_atcmd_setcgmr},

};

// Public functions prototypes --------------------------------------------------

// Functions --------------------------------------------------------------------

/**
  * Function    : dtu_get_uart_ctx
  * Description : 获取串口结构体
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
DTU_UART_PARAM_T* dtu_get_uart_ctx(void)
{
    return &st_uart;
}

/**
  * Function    : dtu_uart_data_recv_cbk
  * Description : 串口数据接收回调函数，通过UART_OPEN注册给底层
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart_data_recv_cbk(UINT8 *data, UINT32 len)
{
    DTU_MSG_UART_DATA_PARAM_T uart_temp = {0};
    OSA_STATUS status = 0;
    char* data_rcv = NULL;
    
    data_rcv = malloc(len + 1);
    memset(data_rcv, 0, len + 1);
    memcpy(data_rcv, (char *)data, len);
    
    uart_temp.UArgs = (UINT8 *)data_rcv;
    uart_temp.len = len;
    
    status = OSAMsgQSend(st_uart.dtu_msgq_uart, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&uart_temp, OSA_NO_SUSPEND);
    ASSERT(status == OS_SUCCESS);
        
}

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_uart4_data_recv_cbk
  * Description : 串口4接收数据回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart4_data_recv_cbk(UINT8 *data, UINT32 len)
{    
    DTU_MSG_UART_DATA_PARAM_T uart4_data = {0};
    OSA_STATUS osa_status = 0;
    char* data_rcv = NULL;

    data_rcv = malloc(len + 1);
    memset(data_rcv, 0x0, len + 1);
    memcpy(data_rcv, (char *)data, len);
    
//    printf("%s[%d]: recv_len:%d, recv_data:%s\n", __FUNCTION__, __LINE__, len, (char *)(data)); 
    uart4_data.UArgs = (UINT8 *)data_rcv;
    uart4_data.id = DTU_UART4_MSG_ID_RECV;
    uart4_data.len = len;

    osa_status = OSAMsgQSend(st_uart.dtu_msgq_uart4, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&uart4_data, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);
}
#endif

/**
  * Function    : dtu_send_to_uart
  * Description : 发送数据到at串口
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_send_to_uart(char *toUart, int length)
{
    printf("%s[%d] length:%d\n", __FUNCTION__, __LINE__, length);    

    UART_SEND_DATA((UINT8 *)toUart, length);

}

/**
  * Function    : is_begin_with
  * Description : 判断字符串str1是否是以str2开头
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int is_begin_with(const char * str1,char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
        if(*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

/**
  * Function    : is_end_with
  * Description : 判断字符串str1是否是以str2结尾
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
    return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) || (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

/**
  * Function    : at_ParamStrParse
  * Description : 解析at指令字符参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static const uint8_t *at_ParamStrParse(DTU_AT_COMMAND_PARAM_T *param)
{
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;

    // param->value must be started and ended with double-quote
    uint8_t *s = param->value + 1;
    uint8_t *d = param->value;
    uint16_t length = param->length - 2;
    param->length = 0;
    
    while (length > 0)
    {
        uint8_t c = *s++;
        length--;
        // It is more permissive than SPEC.
        if (c == '\\' && length >= 2 && isalnum(s[0]) && isalnum(s[1]))
        {
            *d++ = (HEX2NUM(s[0]) << 4) | HEX2NUM(s[1]);
            s += 2;
            length -= 2;
        }
        else
        {
            *d++ = c;
        }
        param->length++;
    }
    *d = '\0';
    param->type = AT_CMDPARAM_STRING_PARSED;
    
    return param->value;
}

/**
  * Function    : at_ParamStr
  * Description : 判断字符参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static uint8_t *at_ParamStr(DTU_AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;
    if (param->type == AT_CMDPARAM_STRING_PARSED)
        return param->value;
    if (param->type == AT_CMDPARAM_STRING && param->length >= 2)
        return (uint8_t *)at_ParamStrParse(param);

failed:
    *paramok = false;
    return "";
}

/**
  * Function    : at_ParamUint
  * Description : 解析at指令整形参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static UINT32 at_ParamUint(DTU_AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;

    if (param->type == AT_CMDPARAM_NUMBER || param->type == AT_CMDPARAM_RAWTEXT)
    {
        char *endptr = NULL;
        unsigned long value = strtoul((const char *)param->value, &endptr, 10);
        if (*endptr != 0)
        {
            printf("endptr:%s ", endptr);//test
            goto failed;

        }

        return value;
    }

failed:
    *paramok = false;
    return 0;
}

/**
  * Function    : at_ParamUintInRange
  * Description : 判断整形参数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static UINT32 at_ParamUintInRange(DTU_AT_COMMAND_PARAM_T *param, UINT32 minval,
                             uint32_t maxval, bool *paramok)
{
    uint32_t res = at_ParamUint(param, paramok);
    if (*paramok && res >= minval && res <= maxval)
        return res;

    *paramok = false;
    return minval;
}

/**
  * Function    : dtu_package_at_cmd
  * Description : at指令解析
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_package_at_cmd(char *atName, char *atLine,DTU_AT_CMD_PARA_T *dtu_atcmd_param)
{
    char strParam[DTU_CMD_LINE_MAX_LINE_SIZE] = {0};
    UINT8 index = 0;
    int step = 0;
    char cc = 0;
    char *save_buf = NULL;
    char *buf_ptr = NULL;
    
    int data_len = strlen(atLine) - strlen(atName) - 1;
    buf_ptr = atLine + strlen(atName) + 1;
    save_buf = buf_ptr;
    //指令中的参数个数设置为0
    dtu_atcmd_param->paramCount = 0;
    
    //指令后面没有问号，等于号，等于号加问号
    if(strlen(atLine) == strlen(atName))
    {
        printf("dtu_atcmd_param->iType = AT_CMD_EXE");
        dtu_atcmd_param->iType = AT_CMD_EXE;
        return 0;
    }
    //指令后是问号的情况
    if(*(atLine + strlen(atName)) == '?')
    {
        printf("dtu_atcmd_param->iType = AT_CMD_READ");
        if(strlen(atLine)- strlen(atName) == 1)
            dtu_atcmd_param->iType = AT_CMD_READ;
        return 0;
    }
    //指令后是等于号加问号的情况
    if( (*(atLine + strlen(atName)) == '=')&&( *(atLine + strlen(atName) + 1) == '?'))
    {
        printf("dtu_atcmd_param->iType = AT_CMD_TEST");
        if(strlen(atLine)- strlen(atName) == 2)
            dtu_atcmd_param->iType = AT_CMD_TEST;
        return 0;
    }
    //指令为设置指令的情况
    dtu_atcmd_param->iType = AT_CMD_SET;
    //开始解析
    while (data_len > 0)
    {
//        printf("str b:%s",buf_ptr);
        cc = *buf_ptr++;
        data_len--;
        step++;
//        printf("cc:%c",cc);
        if(cc == ',')
        {
//            printf("test00:%s","begin0");
            //0.get the strParam and judge
            if(step < 1)
            {
//                printf("step < 1");
                return -1;
            }
            //连续两个逗号中间无内容，例如订阅的topic里面只设置1 - 2个topic
            if(1 == step)
            {
                dtu_atcmd_param->paramCount++;
                dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + step * sizeof(uint8_t));
                if(dtu_atcmd_param->params[index] == NULL)
                    return -1;
                dtu_atcmd_param->params[index++]->type = AT_CMDPARAM_EMPTY;
//                printf("empty:%d",index);
//                printf("buf_ptr:%s",buf_ptr);
                save_buf = buf_ptr;
                step = 0;
                continue;
            }
//            printf("param:%d",index);
            memset(strParam, 0, DTU_CMD_LINE_MAX_LINE_SIZE);
            memcpy(strParam, save_buf, step - 1);
            //printf("strParam:%s",strParam); test

            //wait for check 
            if(step > 3)
            {
                if(!(strParam[0] == '\"' && strParam[step - 2] == '\"'))
                {
                    //if begin "
                    if(strParam[0] == '\"')
                    {
                        continue;
                    }
                    //if not all num
                    if(strspn(strParam, "0123456789") != strlen(strParam))
                        return -1;
                }
            }
            else
            {
                //if not all num
                if(strspn(strParam, "0123456789") != strlen(strParam))
                        return -1;
            }
//            printf("test:step:%d",step);
//            printf("test:buf_ptr:%s",buf_ptr);
//            printf("test:save_buf:%s",save_buf);
//            1.set dtu_atcmd_param->paramCount
            dtu_atcmd_param->paramCount++;
            //2.malloc dtu_atcmd_param->param
            dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + step*sizeof(uint8_t));
            if(dtu_atcmd_param->params[index] == NULL)
                return -1;
            
            //3.set param->length
            dtu_atcmd_param->params[index]->length = step - 1;
            //4.set param-type
            if(dtu_atcmd_param->params[index]->length > 2)
            {
                if(*save_buf == '\"' && *(save_buf+step-2) == '\"')
                    dtu_atcmd_param->params[index]->type = AT_CMDPARAM_STRING;
                else
                    dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
            }
            else
                dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
            //5.set params->value
            memcpy(dtu_atcmd_param->params[index++]->value, strParam, step);
            save_buf = buf_ptr;   
            step = 0;
        }
    }
//    printf("test00:%s","begin1");
    //1.set param
    dtu_atcmd_param->paramCount++;
    //2.malloc dtu_atcmd_param->param
    dtu_atcmd_param->params[index] = (DTU_AT_COMMAND_PARAM_T*)malloc(sizeof(DTU_AT_COMMAND_PARAM_T) + (step + 1)*sizeof(uint8_t));
    if(dtu_atcmd_param->params[index] == NULL)
        return -1;
    //3.get the strParam and judge
    if(step < 1)
        return -1;
    memset(strParam, 0, DTU_CMD_LINE_MAX_LINE_SIZE);
    memcpy(strParam, save_buf, step);

    //wait for check 
    if(step > 3)
    {
        if(!(strParam[0] == '\"' && strParam[step - 1] == '\"')){
            //if not all num
            if(strspn(strParam, "0123456789") != strlen(strParam))
                return -1;
        }
    }
    else
    {
        //if not all num
        if(strspn(strParam, "0123456789") != strlen(strParam))
                return -1;
    }

    //4.set param->length
    dtu_atcmd_param->params[index]->length = step;
    //5.set param-type
    if(dtu_atcmd_param->params[index]->length > 2)
    {
        if(*save_buf == '\"' && *(save_buf + step - 1)=='\"')
            dtu_atcmd_param->params[index]->type = AT_CMDPARAM_STRING;
        else
            dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
    }
    else
        dtu_atcmd_param->params[index]->type = AT_CMDPARAM_NUMBER;
    //6.set params->value
    memcpy(dtu_atcmd_param->params[index++]->value, strParam, step + 1);
//    printf("test00:%s","end");
    
    return 0;
}

/**
  * Function    : dtu_free_param
  * Description : 释放申请的内存
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_free_param(DTU_AT_CMD_PARA_T *dtu_atcmd_param)
{
    UINT8 i = 0;

    //循环释放所有申请的neicun资源
    for(i = 0; i < dtu_atcmd_param->paramCount; i++)
    {
        if(dtu_atcmd_param->params[i] != NULL)
        {
            free(dtu_atcmd_param->params[i]);
            dtu_atcmd_param->params[i] = NULL;
        }
    }
}

/**
  * Function    : dtu_atcmd_send_responnse
  * Description : 发送at指令回复
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_atcmd_send_responnse(DTU_AT_CMD_RES_T *response)
{
    static const char *MSG_OK="\r\nOK\r\n";
    static const char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[DTU_CMD_LINE_RES_MAX_LINE_SIZE]={0};
    const char *msg = NULL;
    int sz = 0;
    DTU_SOCKET_PARAM_T* dtu_socket_ctx = NULL;
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_socket_ctx = dtu_get_socket_ctx();
    dtu_uart_ctx = dtu_get_uart_ctx();
    
    //判断是否是透传模式，发送数据到服务器
    if(dtu_uart_ctx->uart_mode == DTU_DATA_MODE){
        if(dtu_socket_ctx->fd){
            if (response->result == DSAT_OK) {
                if (response->response && response->response[0]) {
                    if(response->istranscmd == DSAT_OK)
                        sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                    else
                        sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
                }
                msg = MSG_OK;
            } else {
                msg = MSG_ERROR;
            }
            sprintf(resp_buffer + strlen(resp_buffer), "%s", msg);
            //judge is ATO cmd
            if(strcmp(response->response,"ATO")==0 || strcmp(response->response,"ATO0")==0)
                dtu_send_to_uart(resp_buffer, strlen(resp_buffer));
            else
                dtu_socket_write(dtu_socket_ctx->fd,resp_buffer, strlen(resp_buffer));
        }
    }
    //本地AT指令发送回复到串口
    else
    {
        if (response->result == DSAT_OK)
        {
            if (response->response && response->response[0])
            {
                if(response->istranscmd == DSAT_OK)
                    sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
                else
                    sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
                dtu_send_to_uart((char *)resp_buffer, sz);
            }
            msg = MSG_OK;
        }
        else
        {
            msg = MSG_ERROR;
        }
        dtu_send_to_uart((char*)msg, strlen((const char*)msg));
    }

}

/**
  * Function    : dtu_process_at_cmd_line
  * Description : 查找AT指令相应函数，执行相关指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static int dtu_process_at_cmd_line(char *cmdName, char *cmdLine)
{
    DTU_AT_CMD_RES_T *response = NULL;
    int i = 0;

    //申请回复信息内存
    response = malloc(sizeof(DTU_AT_CMD_RES_T));
    if ( response != NULL )
    {
        //清空申请的存储区
        memset(response, 0x0, sizeof(DTU_AT_CMD_RES_T));
        //申请内存
        response->response = malloc(DTU_CMD_LINE_RES_MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;
        response->istranscmd = DSAT_ERROR;
        
        DTU_AT_CMD_TABLE_T *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        //在列表里查找对应的函数执行
        for (i = 0; i < nCommands; i++, atcmd_ptr++) {
            if (strcasecmp((char *)atcmd_ptr->at_cmd_name, cmdName)==0)
            {
                response->result = DSAT_OK;
                response->istranscmd = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL) {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }

        printf("%s[%d] cmd:%d", __FUNCTION__, __LINE__, response->istranscmd);
        //透传模式打开后，at指令无法使用，只能在这里调用SendATCMDWaitResp来实现TEL_AT_CMD_ATP_10里面的at，包含大部分at
        if(response->istranscmd == DSAT_ERROR)
        {
            char at_str[128] = {'\0'};
            sprintf(at_str, "AT%s\r",cmdLine);
            char resp_str[128] = {'\0'};
            response->result = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, NULL, 1, NULL, resp_str, sizeof(resp_str));
            printf("%s[%d] resp_str:%s,%d", __FUNCTION__, __LINE__,resp_str,strlen(resp_str));
            snprintf(response->response, strlen(resp_str) + 1, "%s", resp_str);
        }
        //发送回复
        dtu_atcmd_send_responnse(response);
        //释放内存，并置空
        if(NULL != response->response)
        {
            free(response->response);
            response->response = NULL;
        }
        if(NULL != response)
        {
            free(response);
            response = NULL;
        }
    }
    
    return 0;
}

/**
  * Function    : dtu_process_at_cmd_mode
  * Description : 解析AT指令
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_process_at_cmd_mode(const char *atCmdData, int data_len)
{
    DTU_DSAT_SIO_INFO_TYPE_E *sio_info_ptr = &g_sio_info;
    char *buf_ptr = NULL;
    char cc, atName[DTU_CMD_LINE_RES_MAX_LINE_SIZE] = {0};
    int step = 0;
    int find061 = 0;

    buf_ptr = (char*)atCmdData;
    printf("%s[%d]: ATCMD: %s\n", __FUNCTION__, __LINE__, buf_ptr);
    //memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));//for 32bit error
    while (data_len > 0) {
        cc = *buf_ptr++;
        data_len--;
        
        switch (sio_info_ptr->at_cmd_prep_state) {
        case DSAT_CMD_PREP_STATE_HUNT:
            if ( UPCASE( cc ) == 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_A;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_A:
            if ( UPCASE( cc ) == 'T' ) {
                sio_info_ptr->build_cmd_ptr = sio_info_ptr->cmd_line_buffer;
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_AT;
            } else if ( UPCASE( cc ) != 'A' ) {
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;

        case DSAT_CMD_PREP_STATE_FOUND_AT:
            step++;
            if (cc != '\r') {
                if (cc == '=' || cc == '?') {
                    if(0 == find061)
                    {
                        printf("sio_info_ptr->cmd_line_buffer:%s\n", sio_info_ptr->cmd_line_buffer);
                        strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                        atName[step-1] = '\0';
                        printf("atName is %s\n", atName);
                        find061 = 1;
                    }
                }
                *sio_info_ptr->build_cmd_ptr++ = cc;
            } else {
                /*  EOL found, terminate and parse */
                *sio_info_ptr->build_cmd_ptr = '\0';

                if (!strlen(atName)) {
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step-1);
                    atName[step-1] = '\0';
                    printf("[%d]: atName is %s\n", __LINE__, atName);
                }

                printf("%s[%d]: cmd_line_buffer=%s\n", __FUNCTION__, __LINE__, sio_info_ptr->cmd_line_buffer);

                // T0D0: here should add AT command parse

                dtu_process_at_cmd_line(atName, sio_info_ptr->cmd_line_buffer);
                
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;
            
        default:
            sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            break;
        }
    }
    memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));//for 32bit error
}

/**
  * Function    : check_serial_mode
  * Description : 检查是否是"+++"，是的话退出透传
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void check_serial_mode(DTU_MSG_UART_DATA_PARAM_T *msgUartData)
{
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    //获取串口上下文结构体指针
    dtu_uart_ctx = dtu_get_uart_ctx();

    //如果当前在透传模式
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        //比对字符如果是"+++"，退出透传，同时发送"\r\nOK\r\n"到串口
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0)
        {
            dtu_uart_ctx->uart_mode = DTU_AT_MODE;
            dtu_send_to_uart("\r\nOK\r\n", 6);
        }
    } 
}

/**
  * Function    : dtu_handle_serial_data
  * Description : 区别当前是在at指令模式还是数据透传模式，分别做相应处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_handle_serial_data(DTU_MSG_UART_DATA_PARAM_T *uartData)
{
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    
    //检查是否收到"+++"，退出透传，进入AT模式
    check_serial_mode(uartData);

    //如果是透传模式，就把数据发送到服务器
    if (dtu_uart_ctx->uart_mode == DTU_DATA_MODE)
    {
        printf("It time, dtu_serial_mode is DTU_DATA_MODE!\n");
        dtu_send_serial_data_to_server(uartData);
    }
    //如果是at模式，
    else if (dtu_uart_ctx->uart_mode == DTU_AT_MODE)
    {
        printf("It time, dtu_serial_mode is DTU_AT_MODE!\n");
        //如果在at模式下，又收到了“+++”，不响应
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
        {
            memset(dtu_at_cmd_tmp, 0, sizeof(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //如果在at模式下，收到了"\r\n"，解析at
        else if(strstr((const char*)uartData->UArgs,"\r\n"))
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            printf("%s[%d] send AT command uargs:%s,len:%d\n", __FUNCTION__, __LINE__,dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp) );
            dtu_process_at_cmd_mode(dtu_at_cmd_tmp, strlen(dtu_at_cmd_tmp));
            dtu_at_tmp_count = 0;
        }
        //防止at指令过长，拼接，一般不会到这里
        else
        {
            sprintf(&(dtu_at_cmd_tmp[dtu_at_tmp_count]),"%s",(char *)uartData->UArgs);
            dtu_at_tmp_count += uartData->len;
            if(dtu_at_tmp_count > DTU_CMD_LINE_MAX_LINE_SIZE - 1)
                dtu_at_tmp_count = 0;
            
        }
    }
}

/**
  * Function    : dtu_atcmd_setip
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setip(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setip(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
        
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setheartflag
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheartflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_hbflag(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setheart
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setheart(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_hb(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setlinkflag
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinkflag(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_linkflag(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlinktype(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T pParam = {0};
    
    if(dtu_package_at_cmd(atName, atLine, &pParam)==0)
    {
        result = dtu_at_trans_cmd_func_set_linktype(&pParam, atLine, resp);
        dtu_free_param(&pParam);
    }
    else
    {
        dtu_free_param(&pParam);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setlink
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setlink(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_set_link(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_help
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_help(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_query(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

//static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setyzcsq(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
//{
//    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
//    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};
//    
//    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
//    {
//        result = dtu_at_trans_cmd_func_set_yzcsq(&dtu_atcmd_param, atLine, resp);
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    else
//    {
//        dtu_free_param(&dtu_atcmd_param);
//    }
//    
//    return result;
//}

/**
  * Function    : dtu_atcmd_version
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_version(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_version(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_reload
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reload(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_reload(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_reset
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_reset(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_reset(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_seto
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_o(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_seto0
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_seto0(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_o(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcmdpw
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcmdpw(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd( atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_cmdpw(&dtu_atcmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setati
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setati(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setati(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_ota
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_ota(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_ota(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmi
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmi(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmi(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmm
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmm(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmm(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_atcmd_setcgmr
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setcgmr(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_atcmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName, atLine, &dtu_atcmd_param) == 0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setcgmr(&dtu_atcmd_param, atLine, resp);
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_atcmd_param);
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setip
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setip(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;//类型 1 TCP 0 UDP
    UINT8 *mode = NULL;//模式
    UINT8 *ip = NULL;//IP地址或者域名
    UINT32 port = 0;//端口号
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();

    //解析设置指令
    if (dtu_atcmd_param->iType == AT_CMD_SET)
    {
        //判断是否是三个参数
        if (dtu_atcmd_param->paramCount == 3)
        {
            //第一个参数，调用字符串解析函数解析
             mode = at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
             if (!paramRet)
                 return result;
             if ((strcmp((const char*)mode, "TCP") == 0) || (strcmp((const char*)mode, "tcp") == 0))
             {
                 //set trans type
                 printf("TCP ");
                 type = 0;
             }
             else if ((strcmp((const char*)mode, "UDP") == 0) || (strcmp((const char*)mode, "udp") == 0))
             {
                 //set trans type
                 printf("UDP ");
                 type = 1;
             }
             else
             {
                 return result;
             }
             //第二个参数，调用字符串解析函数解析
             ip = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
             if ((!paramRet)||(strlen((const char*)ip) > DTU_TRANS_IP_LEN))
             {
                 return result;
             }
             //第三个参数，调用整形解析函数解析
             port = at_ParamUintInRange(dtu_atcmd_param->params[2], 0, 65535, &paramRet);
             if (!paramRet)
                 return result;
             //set type
             dtu_file_ctx->socket.type = type;
             //set trans ip
             sprintf(dtu_file_ctx->socket.ip,(const char*)ip);
             //set trans port
             dtu_file_ctx->socket.port = port;
             //return ok
             if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
        else
        {
            return result;
        }
    }
    //组装读指令回复内容
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        //根据类型回复
        if(dtu_file_ctx->socket.type == 0)
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s,\"%s\",%d","+TRANSIP:\"TCP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        else 
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s,\"%s\",%d","+TRANSIP:\"UDP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_hbflag
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hbflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 flag = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            flag = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            //set heartflag
            dtu_file_ctx->hb.heartflag = flag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->hb.heartflag);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_hb
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_hb(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 time = 0;
    UINT8 *heart = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 2)
        {
            time = at_ParamUintInRange(dtu_atcmd_param->params[0], 10, 3600, &paramRet);
            if (!paramRet)
                return result;
            dtu_file_ctx->hb.hearttime = time;
            heart = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[1], &paramRet);
            if ((!paramRet)||(strlen((const char*)heart) > DTU_TRANS_HEART_LEN))
            {
                return result;
            }
            //set heart
            sprintf(dtu_file_ctx->hb.heart,(const char*)heart);
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d,\"%s\"", atLine, dtu_file_ctx->hb.hearttime, dtu_file_ctx->hb.heart);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_linkflag
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linkflag(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 flag = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            flag = at_ParamUintInRange(dtu_atcmd_param->params[0], 0, 3, &paramRet);
            if (!paramRet)
                return result;
            //set linkfalg
            dtu_file_ctx->reg.linkflag = flag;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->reg.linkflag);
        result = DSAT_OK;
    }
    
    return result;
}

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_linktype(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 type = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
            type = at_ParamUintInRange(pParam->params[0], 0, 3, &paramRet);
            if (!paramRet)
                return result;
            //set linktype
            dtu_file_ctx->reg.linktype = type;
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->reg.linktype);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_set_link
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_set_link(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 *link = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {
            link = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            if ((!paramRet)||(strlen((const char*)link) > DTU_TRANS_LINK_LEN))
            {
             return result;
            }
            //set link
            sprintf(dtu_file_ctx->reg.link,(const char*)link);
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%s", atLine, dtu_file_ctx->reg.link);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_query
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_query(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[500] ={0};
        if(dtu_file_ctx->socket.type == 0)
            sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        else
            sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", dtu_file_ctx->socket.ip, dtu_file_ctx->socket.port);
        sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSHEARTFLAG:", dtu_file_ctx->hb.heartflag);
        sprintf(arrStr + strlen(arrStr), "%s%d,\"%s\"","\r\n+TRANSHEART:", dtu_file_ctx->hb.hearttime, dtu_file_ctx->hb.heart);
        sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSLINKFLAG:", dtu_file_ctx->reg.linkflag);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+TRANSLINK:", dtu_file_ctx->reg.link);
        sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+CMDPW:", dtu_file_ctx->net_at.cmdpw);

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIM:", dtu_file_ctx->sim.sim);
        sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIMLOCK:", dtu_file_ctx->sim.simlock);
#endif
#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
        sprintf(arrStr + strlen(arrStr), "%s%d,%d,\"%s\"","\r\n+SETTCPGPS:", dtu_file_ctx->gnss.gpsflag, dtu_file_ctx->gnss.gpstime, dtu_file_ctx->gnss.gpsnmea);
#endif

        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_reload
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reload(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        //init write tans_file
        dtu_init_trans_conf();
        if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
        {
            snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
            result = DSAT_OK;
        }
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_reset
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_reset(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        PM812_SW_RESET();
        result = DSAT_OK;
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_version
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_version(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
#ifdef DTU_UART_9600_BAUD
        char arrStr[] = FWVERSION;
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
        char arrStr[] = FWVERSION;
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
        char arrStr[] = FWVERSION;
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */

        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_o
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_o(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;

    dtu_uart_ctx = dtu_get_uart_ctx();
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
        dtu_uart_ctx->uart_mode = DTU_DATA_MODE;
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_cmdpw
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_cmdpw(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8* cmdpw = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {                      
             cmdpw = (UINT8 *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
             if ((!paramRet)||(strlen((const char*)cmdpw) > DTU_TRANS_CMDPW_LEN))
             {
                 return result;
             }
             //set cmdpw
             sprintf(dtu_file_ctx->net_at.cmdpw,(const char*)cmdpw);
             if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
             
        }
    }
    else if(dtu_atcmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%s", atLine, dtu_file_ctx->net_at.cmdpw);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setati
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setati(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
#ifdef DTU_UART_9600_BAUD
        char arrStr[] ="Manufacturer: Beijing Amaziot Co.,Ltd.\r\nModel: Amaziot AM430E\r\nRevision: AM430E M-L\r\nCorp: JHCL\r\nBaud: 9600";
#endif /* ifdef DTU_UART_9600_BAUD.2022-9-27 14:39:08 */
#ifdef DTU_UART_38400_BAUD
        char arrStr[] ="Manufacturer: Beijing Amaziot Co.,Ltd.\r\nModel: Amaziot AM430E\r\nRevision: AM430E M-L\r\nCorp: JHCL\r\nBaud: 38400";
#endif /* ifdef DTU_UART_38400_BAUD.2022-9-27 14:39:38 */
#ifdef DTU_UART_115200_BAUD
        char arrStr[] ="Manufacturer: Beijing Amaziot Co.,Ltd.\r\nModel: Amaziot AM430E\r\nRevision: AM430E M-L\r\nCorp: JHCL\r\nBaud: 115200";
#endif /* ifdef DTU_UART_115200_BAUD.2022-9-27 14:39:38 */

        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_fota_result_cbk
  * Description : dtu升级回调函数
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static void dtu_fota_result_cbk(UINT32 result)
{
//    printf("%s: result = %ld",__FUNCTION__, result);
    if(result == FOTA_SUCCESS)
    {
        printf("Down Bin Ok! Moudle Rebooting For Update...");
        sleep(1);
        PM812_SW_RESET();// restart will update
    }
    else
    {
        printf("Fota Error,Please try again!");
    }
}

/*****************************************************************
* Function: SDK_MYFOTA_UPDATE
*
* Description:
*     该函数用于请求下载并更新本地固件
* 
* Parameters:
*     url                  [In]     目标固件的URL。
*     username          [In]     http请求服务器用户名。
*     password          [In]     http请求服务器密码。
*     cb                  [In]     fotaRunCallback参数result描述。
*                             0: 文件下载校验成功
*                             1: 文件下载校验失败
*     resetTime         [In]     文件下载成功后，重启进入Fota升级的时间，单位秒；
*                             0: 文件下载成功后，不主动重启进入fota升级，任意时间点调用重启接口或者重新上电都会进入fota升级；
*                             >0: 文件下载成功，resetTime秒后重启进入fota升级。 
* Return:
*     NONE
*
*****************************************************************/
extern void SDK_MYFOTA_UPDATE(char *url, char *username, char *password, fotaRunCallback cb, UINT32 resetTime);

static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_ota(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    char* url = NULL;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (AT_CMD_SET == dtu_atcmd_param->iType)
    {
        if (dtu_atcmd_param->paramCount == 1)
        {                      
            url = (char *)at_ParamStr(dtu_atcmd_param->params[0], &paramRet);
            printf("url:%s",url);
            SDK_MYFOTA_UPDATE(url, "", "", dtu_fota_result_cbk, 20);
            
            result = DSAT_OK;
        }
    }
    
    return result;

}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmi
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmi(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[] ="Beijing Amaziot Co.,Ltd.";
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmm
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmm(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[] ="Amaziot AM430EV5";
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s", arrStr);
        result = DSAT_OK;
    }
    
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setcgmr
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setcgmr(DTU_AT_CMD_PARA_T *dtu_atcmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    
    if (dtu_atcmd_param->iType == AT_CMD_EXE)
    {
        char arrStr[] ="AM430EV5 V";
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s%s", arrStr, DTU_VERSION);
        result = DSAT_OK;
    }
    
    return result;
}

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_atcmd_setgps
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setgps(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T pParam;

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&pParam)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setgps(&pParam,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&pParam);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&pParam);
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setgps
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setgps(DTU_AT_CMD_PARA_T *pParam,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 gpsflag;
    UINT16 gpstime;
    char* gpsnmea;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
        
    dtu_file_ctx = dtu_get_file_ctx();

    if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
            gpsflag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            //set gpsflag
            dtu_file_ctx->gnss.gpsflag = gpsflag;
            if(0 == gpsflag)
            {
                //上报定时器停止
                dtu_gnss_timer_stop();
                dtu_gnss_pw_init();
            }
            if(dtu_trans_conf_file_write(dtu_file_ctx) == 0)
            {
                snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                result = DSAT_OK;
            }
        }
        else if(pParam->paramCount == 3)
        {
            gpsflag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
            if (!paramRet)
                return result;
            gpstime = at_ParamUint(pParam->params[1], &paramRet);
            if (!paramRet)
                return result;
            gpsnmea = (char*)at_ParamStr(pParam->params[2], &paramRet);
            if ((!paramRet)||(strlen(gpsnmea) > 3))
                return result;
            if(1 == gpsflag && gpstime >= 5)
            {
                //set gpsflag
                dtu_file_ctx->gnss.gpsflag = gpsflag;
                dtu_file_ctx->gnss.gpstime = gpstime;
                sprintf(dtu_file_ctx->gnss.gpsnmea, gpsnmea);
                //gnss模块上电
                dtu_gnss_pw_init();
                //上报定时器启动
                dtu_gnss_timer_start();
                if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
                {
                    snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                    result = DSAT_OK;
                }
            }
        }
        dtu_gnss_pw_init();
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d,%d,%s", atLine, dtu_file_ctx->gnss.gpsflag, dtu_file_ctx->gnss.gpstime, dtu_file_ctx->gnss.gpsnmea);
        result = DSAT_OK;
    }
    return result;
}
#endif

#if defined (DTU_TYPE_3IN1) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_atcmd_setsim
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsim(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&dtu_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setsim(&dtu_at_cmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    return result;
}

/**
  * Function    : dtu_atcmd_setsimlock
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_atcmd_setsimlock(char *atName, char *atLine, DTU_AT_CMD_RES_T *resp)
{
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_AT_CMD_PARA_T dtu_at_cmd_param = {0};

    //判断AT指令是否正确
    if(dtu_package_at_cmd(atName,atLine,&dtu_at_cmd_param)==0)
    {
        //调用对应的AT指令处理函数
        result = dtu_at_trans_cmd_func_setsimlock(&dtu_at_cmd_param,atLine,resp);
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    else
    {
        //释放AT指令资源
        dtu_free_param(&dtu_at_cmd_param);
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setsim
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsim(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 sim = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_at_cmd_param->iType)
    {
        if (dtu_at_cmd_param->paramCount == 1)
        {
             sim = at_ParamUintInRange(dtu_at_cmd_param->params[0], 1, 3, &paramRet);
             if (!paramRet)
                return result;
             //set sim
             dtu_file_ctx->sim.sim = sim;
             if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
    }
    else if(dtu_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->sim.sim);
        result = DSAT_OK;
    }
    return result;
}

/**
  * Function    : dtu_at_trans_cmd_func_setsimlock
  * Description : 设置指令解析指令参数，并回复；执行指令，执行相应操作；读指令，回复内容
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
static DTU_DSAT_RESULT_TYPE_E dtu_at_trans_cmd_func_setsimlock(DTU_AT_CMD_PARA_T *dtu_at_cmd_param,char *atLine,DTU_AT_CMD_RES_T *resp)
{
    UINT8 simlock = 0;
    bool paramRet = TRUE;
    //init ruturn error
    DTU_DSAT_RESULT_TYPE_E result = DSAT_ERROR;
    DTU_FILE_PARAM_T* dtu_file_ctx = NULL;
    
    dtu_file_ctx = dtu_get_file_ctx();
    
    if (AT_CMD_SET == dtu_at_cmd_param->iType)
    {
        if (dtu_at_cmd_param->paramCount == 1)
        {
             simlock = at_ParamUintInRange(dtu_at_cmd_param->params[0], 0, 1, &paramRet);
             if (!paramRet)
                return result;
             //set linkfalg
             dtu_file_ctx->sim.simlock = simlock;
             if(dtu_trans_conf_file_write(dtu_file_ctx)==0)
             {
                 snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "AT%s", atLine);
                 result = DSAT_OK;
             }
        }
    }
    else if(dtu_at_cmd_param->iType == AT_CMD_READ)
    {
        snprintf(resp->response, DTU_CMD_LINE_RES_MAX_LINE_SIZE, "%s:%d", atLine, dtu_file_ctx->sim.simlock);
        result = DSAT_OK;
    }
    return result;
}
#endif

/**
  * Function    : dtu_atcmd_dicfg
  * Description : 解析对应指令，判断是否需要去处理
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart_data_recv_thread(void)
{
    DTU_MSG_UART_DATA_PARAM_T uart_temp = {0};
    OSA_STATUS status = {0};
    DTU_UART_PARAM_T* dtu_uart_ctx = NULL;
    
    dtu_uart_ctx = dtu_get_uart_ctx();

    //默认是透传模式
    dtu_uart_ctx->uart_mode = DTU_DATA_MODE;
    
    while (1)
    {
        //清空接收数据缓冲区
        memset(&uart_temp, 0, sizeof(DTU_MSG_UART_DATA_PARAM_T));
        //等待来自底层的串口数据，MV9一次最多发送1024字节
        status = OSAMsgQRecv(st_uart.dtu_msgq_uart, (UINT8 *)&uart_temp, sizeof(DTU_MSG_UART_DATA_PARAM_T), OSA_SUSPEND);
        
        if (status == OS_SUCCESS)
        {
            if (NULL != uart_temp.UArgs)
            {
//                printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                printf("%s[%d]: uart_temp len:%d\n", __FUNCTION__, __LINE__, uart_temp.len);
                //处理串口数据
                dtu_handle_serial_data(&uart_temp);
                //释放数据内存
                free(uart_temp.UArgs);
                //置空
                uart_temp.UArgs = NULL;
            }
        }
    }
}

#if defined (DTU_TYPE_EXSIM_GNSS) || defined (DTU_TYPE_5X6_GNSS) || defined (DTU_TYPE_3IN1_GNSS)
/**
  * Function    : dtu_uart4_data_send_thread
  * Description : gnss数据上报任务
  * Input       : 
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
void dtu_uart4_data_send_thread(void *param)
{
    OSA_STATUS status = 0;
    DTU_MSG_UART_DATA_PARAM_T uart4_rdata = {0};
    DTU_MSG_UART_DATA_PARAM_T uart4_sdata = {0};
    OSA_STATUS osa_status = 0;

    uart4_sdata.id = DTU_UART4_MSG_ID_INIT;

    osa_status = OSAMsgQSend(st_uart.dtu_msgq_uart4, sizeof(DTU_MSG_UART_DATA_PARAM_T), (UINT8*)&uart4_sdata, OSA_NO_SUSPEND);
    ASSERT(osa_status == OS_SUCCESS);

    while (1)
    {
        //清空接收数据缓冲区
        memset(&uart4_rdata, 0x00, sizeof(DTU_MSG_UART_DATA_PARAM_T));
        //等待来自底层的串口数据，MV9一次最多发送1024字节
        status = OSAMsgQRecv(st_uart.dtu_msgq_uart4, (UINT8 *)&uart4_rdata, sizeof(DTU_MSG_UART_DATA_PARAM_T), OSA_SUSPEND);    //recv data from uart
        if (status == OS_SUCCESS)
        {
//            printf("%s: id:%d\n", __FUNCTION__, uart4_data.id);
            //为了安全，等当前贤臣刚开始运行后，在给gnss芯片上电，接收串口4数据
            if(DTU_UART4_MSG_ID_INIT == uart4_rdata.id)
            {
                dtu_gnss_timer_init();
                dtu_gnss_pw_init();
                dtu_gnss_timer_start();
            }
            else if(DTU_UART4_MSG_ID_SEND == uart4_rdata.id)
            {
                dtu_send_gnss_data_to_server();
            }
            else if(DTU_UART4_MSG_ID_RECV == uart4_rdata.id)
            {
                if(NULL != uart4_rdata.UArgs)
                {
//                    printf("%s: len:%d, data:%s\n", __FUNCTION__, uart4_data.len, (char *)(uart4_data.UArgs));
                    dtu_gnss_data_prase(uart4_rdata.UArgs, uart4_rdata.len);
                    free(uart4_rdata.UArgs);
                    uart4_rdata.UArgs = NULL;
                }
            }
        }
    }
}
#endif

// End of file : am_at.c 2023-8-28 10:35:41 by: zhaoning 

