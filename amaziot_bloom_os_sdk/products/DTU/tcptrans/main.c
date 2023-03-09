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

//add by dmh begin
#include "teldef.h"
#include <stdbool.h>
#include "FDI_TYPE.h"
#include "FDI_FILE.h"
#include "cgpio.h"

#include "utils_string.h"

//extern UART_Port gATUARTIntPortNum;

//å…³æœºï¼šOnKeyPoweroff_Start()ï¼Œé‡å¯ï¼šPM812_SW_RESET();
//add by dmh end



#undef printf
#define printf(fmt, args...) do { RTI_LOG("[sdk]"fmt, ##args); } while(0)//dmh 20201216

#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )
#define sleep(x) OSATaskSleep((x) * 200)//second
#define msleep(x) OSATaskSleep((x) * 20)//100*msecond //add by dmh

//begin for netcheck task
#define _TASK_STACK_SIZE     1024*10
static UINT32 _task_stack[_TASK_STACK_SIZE/sizeof(UINT32)];
#define RECV_MAX_BUFF 2048 //socket revBuf length
static OSTaskRef _task_ref = NULL;
static OSATimerRef _timer_ref = NULL;

static OSATimerRef _cachetimer_ref = NULL;
#define SEND_MAX_BUFF 1024 //socket sendBuf length
unsigned int send_count = 0;
char sendBuf[4096]={0};

static OSFlagRef _flag_ref = NULL;
#define TASK_TIMER_CHANGE_FLAG_BIT    0x01

//end for netcheck task
// begin for sim gpio

#define GPIO_LINK_PIN    84    
#define GPIO_SA_PIN      53  
#define GPIO_SB_PIN      54 

// end for sim gpio



#define DATA_MODE   0
#define AT_MODE     1

#define MAX_LINE_SIZE 512

#define AT_CMD_NAME_MAX_LEN 50 //dmh 16->50

typedef enum {
    DSAT_CMD_PREP_STATE_HUNT,
    DSAT_CMD_PREP_STATE_FOUND_A,
    DSAT_CMD_PREP_STATE_FOUND_AT,
} dsat_cmd_prep_state_enum_type;

typedef struct dsat_sio_info_s {
    char cmd_line_buffer[MAX_LINE_SIZE];
    char *build_cmd_ptr;
    dsat_cmd_prep_state_enum_type  at_cmd_prep_state;
} dsat_sio_info_s_type;


typedef enum {
    DSAT_OK             = 0,
    DSAT_NO_CARRIER     = 3,
    DSAT_ERROR          = 4,
} dsat_result_enum_type;


typedef struct {
	int istranscmd;
    int result;
    char *response;
} AtCmdResponse;

//add by dmh begin
#define TRANS_HEART_LEN 50
#define TRANS_LINK_LEN 50
#define TRANS_IP_LEN 60
#define TRANS_CMDPW_LEN 6

#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : ((c) >= 'A' && (c) <= 'F') ? ((c) - ('A' - 0xa)) : ((c) - ('a' - 0xa)))//test

typedef struct
{
	UINT8 type;
	char ip[65];
	int port;
	UINT8 heartflag;
	int hearttime;
	char heart[52];
	UINT8 linkflag;
	UINT8 linktype;
	char link[52];
	UINT8 yzcsq;
	char cmdpw[10];
	UINT8 sim;
	UINT8 simlock;
}trans_conf;
static trans_conf transconf;//weihu yi ge 
#define AT_COMMAND_PARAM_MAX (16)
typedef enum 
{
    AT_CMDPARAM_EMPTY,
    AT_CMDPARAM_NUMBER,
    AT_CMDPARAM_STRING,
    AT_CMDPARAM_DSTRING,
    AT_CMDPARAM_RAWTEXT,

    AT_CMDPARAM_STRING_PARSED, // string parameter, and parsed
    AT_CMDPARAM_DTMF_PARSED,   // DTMF parameter, and parsed
    AT_CMDPARAM_HEXDATA_PARSED // hexstr parameter, and parsed
}AT_CMDPARAM_TYPE;

typedef enum 
{
    AT_CMD_SET,
    AT_CMD_TEST,
    AT_CMD_READ,
    AT_CMD_EXE
}AT_CMD_TYPE;

typedef struct 
{
    uint8_t type;
    uint16_t length;
    uint8_t value[1];
} AT_COMMAND_PARAM_T;

typedef struct{
	UINT8 iType;//cmd type
	UINT8 paramCount;//param num
	AT_COMMAND_PARAM_T *params[AT_COMMAND_PARAM_MAX];
	
}AT_CMD_PARA;
AT_CMD_PARA pParam;
//begin for 32bit error
unsigned int tmp_count = 0;
char tmp[512] = {0};
//end for 32bit error

//begin for socket
OSMsgQRef    MsgqsockRcv;
typedef struct
{
    int fd;
    char remoteIp[256];
    int remotePort;
    int socketType; //0:TCP  1:UDP
}socketParam;
socketParam sockGroup = {0};

typedef struct
{
    int fd;
    socketParam *sock;
}sockRcvType;

struct sockaddr_in nDestAddr;//for UDP param

//end for socket
//add by dmh end

static int dtu_atcmd_regen(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_regtp(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_regdt(char *atName, char *atLine, AtCmdResponse *resp);

//add by dmh begin
static void _timer_callback(UINT32 tmrId);
int socket_init(void *param);
void sockrcv_thread(void);
static int Msg_tcp_client_recv(int fd,void *sock);
void startCheckNet_thread(void * argv);
void setSockCfn(void);
int socket_write(int sockfd, const void *data, size_t data_sz);
int uarthandle(MsgUartDataParam_sdk * uartData);
void pre_serverhandle(int len , char *rcvdata);
static void _cachetimer_callback(UINT32 tmrId);
void cachercv_thread(void);



static int dtu_atcmd_setip(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setheartflag(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setheart(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setlinkflag(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setlinktype(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setlink(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setyzcsq(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_help(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_version(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_reload(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_reset(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcmdpw(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setsim(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setsimlock(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setati(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmi(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmm(char *atName, char *atLine, AtCmdResponse *resp);
static int dtu_atcmd_setcgmr(char *atName, char *atLine, AtCmdResponse *resp);


static int AT_TRANS_CmdFunc_SETIP(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);//dmh
static int AT_TRANS_CmdFunc_SETHEARTFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETHEART(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETLINKFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETLINK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Query(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Reset(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Reload(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_Version(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETYZCSQ(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_CMDPW(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETSIM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETSIMLOCK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETATI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);
static int AT_TRANS_CmdFunc_SETCGMR(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp);

static void trans_conf_file_init(void);//dmh
static int trans_conf_file_write(trans_conf transconf);//dmh
static trans_conf trans_conf_file_read(int fd);//dmh
static trans_conf init_trans_conf(trans_conf transconf);//dmh

static void trans_sim_init(void);//dmh
static void trans_sim_switch(void);//dmh


static int package_at_cmd(char *atName, char *atLine,AT_CMD_PARA *pParam);//dmh
static void free_pParam(AT_CMD_PARA *pParam);//dmh

static void process_at_cmd_mode(const char *atCmdData, int data_len);


//add by  dmh end
typedef struct {
    unsigned char at_cmd_name[AT_CMD_NAME_MAX_LEN + 1];
    int (*proc_func)(char *, char *, AtCmdResponse *);
} ATCmdTable;

static ATCmdTable dtu_atcmd_table[] = {
    { "+REGEN", dtu_atcmd_regen },
    { "+REGTP", dtu_atcmd_regtp },
    { "+REGDT", dtu_atcmd_regdt },
    // add by dmh begin
    { "+TRANSIP",dtu_atcmd_setip},
	{ "+TRANSHEARTFLAG",dtu_atcmd_setheartflag},
	{ "+TRANSHEART",dtu_atcmd_setheart},
	{ "+TRANSLINKFLAG",dtu_atcmd_setlinkflag},
	{ "+TRANSLINKTYPE",dtu_atcmd_setlinktype},
	{ "+TRANSLINK",dtu_atcmd_setlink},
	{ "+TRANSYZCSQ",dtu_atcmd_setyzcsq},
	{ "+HELP",dtu_atcmd_help},
	{ "+VERSION",dtu_atcmd_version},
	{ "+RELOAD",dtu_atcmd_reload},
	{ "+RESET",dtu_atcmd_reset},
	{ "O",dtu_atcmd_seto},
	{ "O0",dtu_atcmd_seto0},
	{ "+CMDPW",dtu_atcmd_setcmdpw},
	{ "+SIM",dtu_atcmd_setsim},
	{ "+SIMLOCK",dtu_atcmd_setsimlock},
	{ "I",dtu_atcmd_setati},
	{ "+CGMI",dtu_atcmd_setcgmi},
	{ "+CGMM",dtu_atcmd_setcgmm},
	{ "+CGMR",dtu_atcmd_setcgmr},
	// add by dmh end
};


static dsat_sio_info_s_type g_sio_info;


extern OSMsgQRef    MsgUartData_sdk;

extern void set_UartDataSwitch_sdk(BOOL flag);

extern BOOL get_UartDataSwitch_sdk(void);
extern BOOL IsAtCmdSrvReady(void);
extern void PM812_SW_RESET(void);
extern int SendATCMDWaitResp(int sATPInd,char *in_str, int timeout, char *ok_fmt, int ok_flag,
                            char *err_fmt, char *out_str, int resplen);

static void uartdata_thread(void);


uint8_t serial_mode;    // 0:DATA_MODE 1:AT_MDOE

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


void Phase1Inits_enter(void)
{
	UART_IND_DISABLE_SET_FLAG(TRUE);//dmh test
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

	//init transfile
	trans_conf_file_init();

	//init sim
	trans_sim_init();   
    
    printf("%s[%d]: starting...\n", __FUNCTION__, __LINE__);
    status = OSAMsgQCreate(&MsgUartData_sdk, "MsgUartData_sdk", sizeof(MsgUartDataParam_sdk), 500, OS_FIFO);
	DIAG_ASSERT(status == OS_SUCCESS);
	//begin add by dmh
	status = OSAMsgQCreate(&MsgqsockRcv, "MsgqsockRcv", sizeof(sockRcvType), 500, OS_FIFO);
    DIAG_ASSERT(status == OS_SUCCESS);

	status = OSAFlagCreate(&_flag_ref);//flag for send data rev ok
    ASSERT(status == OS_SUCCESS);
	
	status = OSATimerCreate(&_timer_ref);
    ASSERT(status == OS_SUCCESS);

	status = OSATimerCreate(&_cachetimer_ref);
    ASSERT(status == OS_SUCCESS);
	//end add by dmh

	//begin add by dmh
	sys_thread_new("sockrcv_thread", (lwip_thread_fn)sockrcv_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);
	
	sys_thread_new("cachercv_thread", (lwip_thread_fn)cachercv_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);
	//end add by dmh
	
	sys_thread_new("uartdata_thread", (lwip_thread_fn)uartdata_thread, NULL, DEFAULT_THREAD_STACKSIZE*2, 161);

	//begin add by dmh
	status = OSATaskCreate(&_task_ref, _task_stack, _TASK_STACK_SIZE, 150, "test-task", startCheckNet_thread, NULL);
    ASSERT(status == OS_SUCCESS);

    //OSATimerStart(_timer_ref, transconf.hearttime * 200, transconf.hearttime * 200, _timer_callback, 0); // 20 seconds timer
	//end add by dmh 
}
/**
  * Function    : utils_ascii2hex
  * Description : hexè½¬å­—ç¬¦ä¸²
  * Input       : uc_ascii    hexå€¼
  *               
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
int utils_ascii2hex(UINT8 uc_ascii)
{
    if(uc_ascii <= '9')
        return (uc_ascii - '0');
    else if(uc_ascii >= 'A' && uc_ascii <= 'F')
        return (uc_ascii - '7' );
    else if(uc_ascii >= 'a' && uc_ascii <= 'f')
        return (uc_ascii - 'W');
    else
        return -1;
}
#define                 ALIGN_MODE_SUFFIX                       1
#define                 ALIGN_MODE_PREFIX                       0

/**
  * Function    : utils_ascii_str2hex
  * Description : å­—ç¬¦ä¸²è½¬hex
  * Input       : mode    æ¨¡å¼
  *               p_hex   å¾…è½¬æ¢çš„hexé¦–åœ°å€
                  p_str   è½¬æ¢åŽå­˜å‚¨å­—ç¬¦ä¸²çš„é¦–åœ°å€
                  ulLen   é•¿åº¦
  * Output      : 
  * Return      : 
  * Auther      : zhaoning
  * Others      : 
  **/
UINT32 utils_ascii_str2hex(UINT8 mode, char* p_hex, const char* p_str, UINT32 ulLen )
{
    unsigned int i = 0, len;

    len = ulLen;
    
    if((ulLen & 0x01) && (mode == ALIGN_MODE_SUFFIX))
    {
        *p_hex++ = utils_ascii2hex( p_str[0]);
        p_str++;
        len++;
        i++;
    }
    
    len = len >> 1;

    for( ; i < len; i++)
    {
        *p_hex++ = (utils_ascii2hex(p_str[0]) << 4) + utils_ascii2hex( p_str[1]);
        p_str += 2;
    }
    if((ulLen & 0x01) && (mode == ALIGN_MODE_PREFIX))
    {
        *p_hex++ = utils_ascii2hex( p_str[0]);
        i++;
    }

    return (i);
}

//add by dmh begin
/*function for utils*/
/**ÅÐ¶Ïstr1ÊÇ·ñÒÔstr2¿ªÍ·
 * Èç¹ûÊÇ·µ»Ø1
 * ²»ÊÇ·µ»Ø0
 * ³ö´í·µ»Ø-1
 * */
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

/**ÅÐ¶Ïstr1ÊÇ·ñÒÔstr2½áÎ²
 * Èç¹ûÊÇ·µ»Ø1
 * ²»ÊÇ·µ»Ø0
 * ³ö´í·µ»Ø-1
 * */
int is_end_with(const char *str1, char *str2)
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

// =============================================================================
// at_ParamStrParse (param is already checked)
// =============================================================================
static const uint8_t *at_ParamStrParse(AT_COMMAND_PARAM_T *param)
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
// =============================================================================
// at_ParamStr
// =============================================================================
uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok)
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
// =============================================================================
// at_ParamUint
// =============================================================================
uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;

    if (param->type == AT_CMDPARAM_NUMBER || param->type == AT_CMDPARAM_RAWTEXT)
    {
        char *endptr;
        unsigned long value = strtoul((const char *)param->value, &endptr, 10);
		if (*endptr != 0){
			CPUartLogPrintf("dmhtest00:endptr:%s ",endptr);//test
			goto failed;

		}

        return value;
    }

failed:
    *paramok = false;
    return 0;
}
// =============================================================================
// at_ParamUintInRange
// =============================================================================
uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,
                             uint32_t maxval, bool *paramok)
{
    uint32_t res = at_ParamUint(param, paramok);
	if (*paramok && res >= minval && res <= maxval)
        return res;

    *paramok = false;
    return minval;
}
// =============================================================================
// package_at_cmd
// =============================================================================
static int package_at_cmd(char *atName, char *atLine,AT_CMD_PARA *pParam)
{
	
	pParam->paramCount = 0;
	if(strlen(atLine) == strlen(atName)){
		printf("pParam->iType = AT_CMD_EXE");
		pParam->iType = AT_CMD_EXE;
		return 0;
	}
	if(*(atLine+strlen(atName))=='?'){
		printf("pParam->iType = AT_CMD_READ");
		if(strlen(atLine)-strlen(atName) == 1)
			pParam->iType = AT_CMD_READ;
		return 0;
	}
	if( (*(atLine+strlen(atName))=='=')&&( *(atLine+strlen(atName)+1)=='?')){
		printf("pParam->iType = AT_CMD_TEST");
		if(strlen(atLine)-strlen(atName) == 2)
			pParam->iType = AT_CMD_TEST;
		return 0;
	}
	printf("pParam->iType = AT_CMD_SET");
	pParam->iType = AT_CMD_SET;
	char strParam[100]={0};
    UINT8 index=0;
    int step = 0;
	char cc;
	char *save_buf;
	char *buf_ptr;
	int data_len=strlen(atLine)-strlen(atName)-1;
	buf_ptr = atLine+strlen(atName)+1;
    save_buf = buf_ptr;
    while (data_len > 0) {
        cc = *buf_ptr++;
        data_len--;
        step++;
        if(cc == ','){
			printf("dmhtest00:%s","begin0");
			//1.set pParam->paramCount
			pParam->paramCount++;
		    //2.malloc pParam->param
			pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+step*sizeof(uint8_t));
			if(pParam->params[index] == NULL)
				return -1;

			//3.get the strParam and judge
			if(step<=1)
				return -1;
			memset(strParam,0,100);//add by dmh 2020-06-22
			memcpy(strParam, save_buf, step-1);

			//wait for check 
			if(step > 3){
				if(!(strParam[0] == '\"' && strParam[step-2] == '\"')){
					//if not all num
					if(strspn(strParam, "0123456789")!=strlen(strParam))
						return -1;
				}
			}else{
				//if not all num
				if(strspn(strParam, "0123456789")!=strlen(strParam))
						return -1;
			}
			
			//4.set param->length
			pParam->params[index]->length=step-1;
			//5.set param-type
			if(pParam->params[index]->length>2){
				if(*save_buf == '\"' && *(save_buf+step-2)=='\"')
					pParam->params[index]->type=AT_CMDPARAM_STRING;
				else
					pParam->params[index]->type=AT_CMDPARAM_NUMBER;
			}else
				pParam->params[index]->type=AT_CMDPARAM_NUMBER;
			//6.set params->value
			memcpy(pParam->params[index++]->value, strParam, step);
			save_buf = buf_ptr;   
			step = 0;
		}
		           
    }
	printf("dmhtest00:%s","begin1");
	//1.set param
	pParam->paramCount++;
	//2.malloc pParam->param
	pParam->params[index]=malloc(sizeof(AT_COMMAND_PARAM_T)+(step+1)*sizeof(uint8_t));
	if(pParam->params[index] == NULL)
		return -1;
	//3.get the strParam and judge
	if(step<1)
		return -1;
	memset(strParam,0,100);
	memcpy(strParam, save_buf, step);

	//wait for check 
	if(step > 3){
		if(!(strParam[0] == '\"' && strParam[step-1] == '\"')){
			//if not all num
			if(strspn(strParam, "0123456789")!=strlen(strParam))
				return -1;
		}
	}else{
		//if not all num
		if(strspn(strParam, "0123456789")!=strlen(strParam))
				return -1;
	}

	//4.set param->length
	pParam->params[index]->length=step;
	//5.set param-type
	if(pParam->params[index]->length>2){
		if(*save_buf == '\"' && *(save_buf+step-1)=='\"')
			pParam->params[index]->type=AT_CMDPARAM_STRING;
		else
			pParam->params[index]->type=AT_CMDPARAM_NUMBER;
	}else
		pParam->params[index]->type=AT_CMDPARAM_NUMBER;
	//6.set params->value
	memcpy(pParam->params[index++]->value, strParam, step+1);
	printf("dmhtest00:%s","end");
	return 0;
}
// =============================================================================
// free_pParam
// =============================================================================
static void free_pParam(AT_CMD_PARA *pParam)
{
	UINT8 i=0;
	for(i=0;i<pParam->paramCount;i++){
		if(pParam->params[i]!=NULL)
			free(pParam->params[i]);
	}
}

// =============================================================================
// send_to_uart
// =============================================================================
/*
static void send_to_uart(char *toUart, int length)
{
    char *sendData = NULL;
    printf("%s[%d]:toUart=%s, length=%d\n", __FUNCTION__, __LINE__, toUart, length);
    sendData = (char *)malloc(length+1);
    ASSERT(sendData != NULL);
    memset(sendData, 0, length+1);

    memcpy(sendData, toUart, length);
    sendData[length] = '\0';
    
    printf("%s[%d]:send to uart data=%s, length=%d\n", __FUNCTION__, __LINE__, sendData, length);
    send_data_2uart((UINT8 *)sendData, length);
    if (sendData)
        free(sendData);
}
*/
/* This function send data to uart. */
static void send_to_uart(char *toUart, int length)
{
    char *sendData = NULL;
    printf("%s[%d]:toUart=%s, length=%d\n", __FUNCTION__, __LINE__, toUart, length);
    sendData = (char *)malloc(length+1);
    ASSERT(sendData != NULL);
    memset(sendData, 0, length+1);

    memcpy(sendData, toUart, length);
    sendData[length] = '\0';
    
    printf("%s[%d]:send to uart data=%s, length=%d\n", __FUNCTION__, __LINE__, sendData, length);    
	//send_data_2uart(gATUARTIntPortNum, (UINT8 *)sendData, length);   
	UART_SEND_DATA((UINT8 *)sendData, length);
	printf("%s[%d]:sendData==NULL = %d\n", __FUNCTION__, __LINE__, (sendData==NULL));
    if (sendData)
        free(sendData);
}

//add by dmh end

//begin add by dmh
/*function for socket rev */
void sockrcv_thread(void)
{    
    sockRcvType sock_temp;
    int rcv = 0;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
    while (1){
        memset(&sock_temp,0,sizeof(sockRcvType));
        OSAMsgQRecv(MsgqsockRcv, (UINT8 *)&sock_temp, sizeof(sockRcvType), OSA_SUSPEND);
        printf("debug> %s -- %u,  fd=%u",__FUNCTION__,__LINE__,sock_temp.fd);
        rcv = Msg_tcp_client_recv(sock_temp.fd, (void *)&sock_temp);
         
         if(-1 == rcv)
         {
             printf("debug> %s -- %u, restart socket",__FUNCTION__,__LINE__);
             if(sockGroup.fd){
                 close(sockGroup.fd);
                 sockGroup.fd = 0;
             }
         }
         
         printf("debug> %s -- %u,  sock err.",__FUNCTION__,__LINE__);
    }
}
/*function for cache rev */
void cachercv_thread(void)
{    
    OSA_STATUS status;
    UINT32 flag_value;
    UINT16 link_size = 0;
    char* hex_data = NULL;
    while(1) {
        status = OSAFlagWait(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR_CLEAR, &flag_value, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
        if (flag_value & TASK_TIMER_CHANGE_FLAG_BIT) {
           if(sockGroup.fd){
				uint16_t send_size = send_count;
				//add link len
				if(transconf.linkflag==2 || transconf.linkflag==3)
				{
				    if(transconf.linktype == 0)
				    {
				        link_size = strlen(transconf.link);
					}
					else if(transconf.linktype == 1)
					{
                        link_size = strlen(transconf.link) / 2;
                        hex_data = malloc(link_size);
                        utils_ascii_str2hex(0, hex_data, transconf.link, strlen(transconf.link));
					}
					send_size += link_size;
				}
				char *data = malloc(send_size);
		        if (data == NULL)
		        {
		        	printf("dmhtest:malloc(send_size) error ,send_size:%d",send_size);
		            return;
		        }
		        memset(data, 0, send_size);
				//add link data
				if(send_size>send_count)
				{
				    if(transconf.linktype == 0)
				    {
				        memcpy(data, transconf.link, link_size);
					}
					else if(transconf.linktype == 1)
					{
                        memcpy(data, hex_data, link_size);
					}
					
					memcpy(data+link_size, sendBuf, send_count);
				}else
		        	memcpy(data, sendBuf, send_count);
				socket_write(sockGroup.fd,(void *)data,send_size);	
				free(data);
			}
			memset(sendBuf, 0, sizeof(sendBuf));
			send_count = 0;
        }
    }	
}
//int lwip_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
//int lwip_recv    (int s, void *mem, size_t len, int flags);
static int Msg_tcp_client_recv(int fd,void *sock)
{
	char buf[RECV_MAX_BUFF]={0};
	int bytes;
	fd_set master, read_fds;


    struct timeval timeout;
    timeout.tv_sec = 10;
	timeout.tv_usec = 0;
    int status;
    
    sockRcvType *sockparam = (sockRcvType *)sock;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);

 	while(1) {
		
		FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
		status = select(fd + 1, &read_fds, NULL, NULL, &timeout);
		if(status < 0){
			printf("socket select fail");
			sleep(1);
			continue;
		}else if(status == 0){
			printf("socket select timeout");
			sleep(1);
			continue;
		}
        
        if(sockGroup.fd == 0)
            goto PRO_FAIL;
        
		if(FD_ISSET(fd, &read_fds)  >= 0){
            memset(buf,0,RECV_MAX_BUFF);
			//add by dmh judge type
			if(sockGroup.socketType == 0)
				bytes = recv(fd, buf, RECV_MAX_BUFF, 0);
			else
				bytes = recvfrom(fd, buf, RECV_MAX_BUFF, 0,(struct sockaddr *)&nDestAddr, (socklen_t *)&nDestAddr.sin_len);
			if(bytes <= 0) 
			{
				sleep(1);
				goto PRO_FAIL;
			}
			printf("%s[], transport mode,recv data length %u,%s",__FUNCTION__, bytes,buf);
            char *sendData = (char *)malloc(bytes+1);
            ASSERT(sendData != NULL);
            memcpy(sendData, buf, bytes);
            sendData[bytes] = '\0';
            pre_serverhandle(bytes,sendData);
		}
	}  

    
PRO_FAIL:
    //close(fd);
    printf(" %s -- %u socket close", __FUNCTION__,__LINE__);
    //sendIndData(TEL_AT_CMD_ATP_7, "\r\nSOCKET CLOSE\r\n", 16);
    //set_UartDataSwitch_sdk(0); //dmh 2020-07-07
    return -1;
}
void pre_serverhandle(int len , char *rcvdata){

    printf("%s---%d, len=%d",__FUNCTION__,__LINE__,len);
	//judge is net atcmd 
	if(is_begin_with(rcvdata,transconf.cmdpw) == 1){
		process_at_cmd_mode(rcvdata,len);
	}
	else
    	send_to_uart(rcvdata, len);

}

//end add by dmh

//begin add by dmh
static void wait_dev_reg_net(void)
{
    char mmRspBuf[100] = {0};
    int  err;
	int errorCnt=0;//dmh
	while(!IsAtCmdSrvReady())
	{
		OSATaskSleep(100);
	}
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT^SYSINFO\r\n", 3, "^SYSINFO", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==> %s", mmRspBuf);

    if(strstr(mmRspBuf, "^SYSINFO: 2,3") != NULL || strstr(mmRspBuf, "^SYSINFO: 2,2") != NULL){
		errorCnt = 0;
        return;
    }else{
        sleep(3);
		errorCnt++;
		if(errorCnt>20){
			//reset
			send_to_uart("\r\nCheck Net Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_reg;
    } 
}
static void wait_dev_check_csq(void)
{
    char mmRspBuf[100] = {0};
    int  err;
	char *p;
	int csqValue;
	int errorCnt=0;//dmh
    wait_reg:
    memset(mmRspBuf, 0, sizeof(mmRspBuf));
    err = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, "AT+CSQ\r\n", 3, "+CSQ", 1, NULL, mmRspBuf, sizeof(mmRspBuf));
    printf("mmRspBuf==> %s", mmRspBuf);

	if(strlen(mmRspBuf) >=10 ){
		p = strchr(mmRspBuf,':');
		csqValue = atoi(p+1);
	    printf("dmhtest:csq=%d",csqValue);
        if((csqValue>=transconf.yzcsq) && (csqValue != 99))
		{
		  errorCnt = 0;
		  return;
		}
    }
    else{
        sleep(2);
		errorCnt++;
		if(errorCnt>20){
			//reset
			send_to_uart("\r\nCheck Csq Error! Moudle Rebooting...\r\n", 40);
			//sim switch
			trans_sim_switch();	
			sleep(3);//delay for write file
			PM812_SW_RESET();
		}	
        goto wait_reg;
    } 
}


/*function for checknet and init socket*/
void startCheckNet_thread(void * argv)
{

	wait_dev_reg_net();
   	wait_dev_check_csq();

PRO_START:
    
    setSockCfn();
	//Open LinkA
	GpioSetLevel(GPIO_LINK_PIN, 1);
	printf("dmhtest:udp00");
	//heart timer
	if(transconf.heartflag)
		OSATimerStart(_timer_ref, transconf.hearttime * 200, transconf.hearttime * 200 , _timer_callback, 0);
	//link data
	if(transconf.linkflag == 1 || transconf.linkflag == 3)
	{
	    UINT16 link_size = 0;
	    char* hex_data = NULL;
	    if(transconf.linktype == 0)
        {
            link_size = strlen(transconf.link);
        }
        else if(transconf.linktype == 1)
        {
            link_size = strlen(transconf.link) / 2;
            hex_data = malloc(link_size);
            utils_ascii_str2hex(0, hex_data, transconf.link, strlen(transconf.link));
        }
	    char *data = malloc(link_size);
	    if(transconf.linktype == 0)
        {
            memcpy(data, transconf.link, link_size);
        }
        else if(transconf.linktype == 1)
        {
            memcpy(data, hex_data, link_size);
        }
		if(0 != socket_write(sockGroup.fd,(void *)data,link_size))
			goto PRO_START;
	}
    
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
    
        
	while (1) {

		if(sockGroup.fd){  //dmh 2020-04-07
	        sleep(1);
		}else
	    {
			printf(" %s -- %u, restart socket", __FUNCTION__,__LINE__); 
			//Close LinkA
			GpioSetLevel(GPIO_LINK_PIN, 0);
			send_to_uart("\r\nSOCKET RESTART\r\n", 18);
            //serial_mode = AT_MODE;dmh20201023
            break;
		}
    }
    goto PRO_START;
}
void setSockCfn(void)
{
	int errorCnt=0;
    printf("debug> %s -- %u",__FUNCTION__,__LINE__);
	sockGroup.socketType = transconf.type;
    sockGroup.remotePort = transconf.port;
	memcpy(sockGroup.remoteIp,transconf.ip,60);
	printf("%s --- %d, ip=%s, port=%u", __FUNCTION__,__LINE__,sockGroup.remoteIp,sockGroup.remotePort);
    while(socket_init(&sockGroup))
	{
		sleep(3);
		errorCnt++;
		sockGroup.socketType = transconf.type;
		sockGroup.remotePort = transconf.port;
		memcpy(sockGroup.remoteIp,transconf.ip,60);
		if(errorCnt>10){
			//reset
			send_to_uart("\r\nConnect Server Error! Moudle Rebooting...\r\n", 45);
			msleep(1);
			PM812_SW_RESET();
		}
	}
    
    printf("%s --- %d",__FUNCTION__,__LINE__);
}
int socket_init(void *param)
{
    struct hostent* host_entry;
    int fd;
    struct sockaddr_in server;
    
    int res=-1;
    
    OSA_STATUS osaStatus;
    sockRcvType rcvType = {0};
    int result = 0;
    socketParam *sock = (socketParam *)param;
    printf("debug> %s -- %u, socketType = %u",__FUNCTION__,__LINE__,sock->socketType);

    if(sock->fd >0)
    {
        printf(" %s, create_req,  previous socket not closed!\n",__FUNCTION__);
        
    }
    
    host_entry = gethostbyname(sock->remoteIp);
    if (host_entry == NULL) {
        printf("%s,  socket error",__FUNCTION__);
        return -1;
    }
    printf("%s[],DNS gethostbyname,Get %s ip %u.%u.%u.%u\n", __FUNCTION__,sock->remoteIp, host_entry->h_addr_list[0][0] & 0xff,
    host_entry->h_addr_list[0][1] & 0xff, host_entry->h_addr_list[0][2] & 0xff, host_entry->h_addr_list[0][3] & 0xff);

	//add by dmh //add UDP
	if(sock->socketType == 0)
		fd = socket(AF_INET, SOCK_STREAM, sock->socketType);
	else
		fd = socket(AF_INET, SOCK_DGRAM, sock->socketType);
    if(fd <=0 ){
        printf("%s,  socket error",__FUNCTION__);
        return -1;
    }

	if(sock->socketType == 0){//add by dmh judge is TCP
	   server.sin_family = AF_INET;
	   server.sin_port = htons(sock->remotePort);
	   server.sin_addr.s_addr= * (UINT32 *) host_entry->h_addr_list[0];

        //sock-connect
        res = connect(fd, (struct sockaddr *)&server, sizeof(server));
        if(res <0)
        {
            printf("%s[%u],  connect error",__FUNCTION__,__LINE__);
			close(fd);//dmh20201231
            return -1;
        }
	}else{
	    nDestAddr.sin_family = AF_INET;
	    nDestAddr.sin_port = htons(sock->remotePort);
		nDestAddr.sin_len = sizeof(nDestAddr);
	    nDestAddr.sin_addr.s_addr= * (UINT32 *) host_entry->h_addr_list[0];
	}

    printf("debug> %s -- %u, fd = %u, sockGroup.fd = %u",__FUNCTION__,__LINE__,fd, sock->fd);
    sock->fd = fd;
    rcvType.fd = sock->fd;
    rcvType.sock = sock;
    printf("debug> %s -- %u, fd = %u, sockGroup.fd = %u",__FUNCTION__,__LINE__,fd, sock->fd);
    //serial_mode = DATA_MODE;dmh20201023
    send_to_uart("\r\nCONNECT OK\r\n", 14);
    osaStatus = OSAMsgQSend(MsgqsockRcv, sizeof(sockRcvType), (UINT8*)&rcvType, OSA_NO_SUSPEND);
 	if(osaStatus == OS_SUCCESS){
        result = 0;
 	}else{
 	    printf("%s: error!send msg failed!osaStatus - %u\n", __func__, osaStatus);
        result = -1;
 	}
   return result;         
}
//int lwip_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
//int lwip_write (int s, const void *dataptr, size_t size);
int socket_write(int sockfd, const void *data, size_t data_sz)
{
    int err;
    size_t cur = 0;
	while (cur < data_sz) {
        do {
			//add by dmh judge type
			if(sockGroup.socketType == 0){
				err = write(sockfd, (const char *)data + cur, data_sz - cur);
			}
			else{
				err = sendto(sockfd, (const char *)data + cur, data_sz - cur,0,(struct sockaddr *)&nDestAddr, nDestAddr.sin_len);
			}
        } while (err < 0 && errno == EINTR);
        if (err <= 0)
            return -1;
        cur += err;
    }
    return 0;
}
int uarthandle(MsgUartDataParam_sdk * uartData)
{
    int result = 0;
    UINT16 link_size = 0;
    char* hex_data = NULL;
    printf("uartData->UArgs = %s, uartData->len=%d",uartData->UArgs,uartData->len);

	if(send_count >= SEND_MAX_BUFF)
	{
		if(sockGroup.fd){

			///////////////////////////////////////////////////////////////////
			uint16_t send_size = SEND_MAX_BUFF;
			//add link len
			if(transconf.linkflag==2 || transconf.linkflag==3)
			{
				if(transconf.linktype == 0)
                {
                    link_size = strlen(transconf.link);
                }
                else if(transconf.linktype == 1)
                {
                    link_size = strlen(transconf.link) / 2;
                    hex_data = malloc(link_size);
                    utils_ascii_str2hex(0, hex_data, transconf.link, strlen(transconf.link));
                }
                send_size += link_size;
		    }
			char *data = malloc(send_size);
	        if (data == NULL)
	        {
	        	printf("dmhtest:malloc(send_size) error ,send_size:%d",send_size);
	            return 0;
	        }
	        memset(data, 0, send_size);
			//add link data
			if(send_size>SEND_MAX_BUFF)
			{
			    if(transconf.linktype == 0)
                {
                    memcpy(data, transconf.link, link_size);
                }
                else if(transconf.linktype == 1)
                {
                    memcpy(data, hex_data, link_size);
                }

                memcpy(data+link_size, sendBuf, SEND_MAX_BUFF);
			}else
	        	memcpy(data, sendBuf, SEND_MAX_BUFF);
			send_count-=SEND_MAX_BUFF;
			result = socket_write(sockGroup.fd,(void *)data,send_size);	
			free(data);
			//////////////////////////////////////////////////////////////////
			//if(transconf.linkflag==2 || transconf.linkflag==3)//dmh test
			//	socket_write(sockGroup.fd,(void *)transconf.link,strlen(transconf.link));
			//result = socket_write(sockGroup.fd,(void *)sendBuf,SEND_MAX_BUFF);
			//send_count-=SEND_MAX_BUFF;
			
   		}
		else
        	result = -1;
	}
	else{
		//open huancun timer function
		OSATimerStart(_cachetimer_ref, 40, 40, _cachetimer_callback, 0); // 200 ms timer
	}
	printf("%s --- %u, result=%d,sockfd=%u",__FUNCTION__,__LINE__,result,sockGroup.fd);
    
    printf("%s --- %d",__FUNCTION__,__LINE__);
    if(uartData->UArgs)
        free(uartData->UArgs);
    printf("%s --- %d",__FUNCTION__,__LINE__);
    return result;
}
/*function for timer heart*/
static void _timer_callback(UINT32 tmrId)
{
	printf("asr_test  _timer_callback");
    if(serial_mode == DATA_MODE)
    {
    	if(sockGroup.fd && transconf.heartflag == 1){
	        printf("%s --- %d",__FUNCTION__,__LINE__);
	        
	        MsgUartDataParam_sdk heart = {0};
		    char *heartbuf = (char *)malloc(100);
			memset(heartbuf, 0x0, 100);
			sprintf(heartbuf, "%s",transconf.heart);
	        heart.len = strlen(heartbuf);
			heart.UArgs = (UINT8 *)heartbuf;
			OSAMsgQSend(MsgUartData_sdk, sizeof(MsgUartDataParam_sdk), (UINT8*)&heart, OSA_NO_SUSPEND);
    	}
    }
    
}
/*function for cache timer*/
static void _cachetimer_callback(UINT32 tmrId)
{
	printf("asr_test  _cachetimer_callback");
	OSAFlagSet(_flag_ref, TASK_TIMER_CHANGE_FLAG_BIT, OSA_FLAG_OR);
	OSATimerStop(_cachetimer_ref);
}


//end add by dmh


/* process transparent data function */
static void send_serial_data(MsgUartDataParam_sdk * uartData)
{
	printf("%s[%d]: DATA_MODE recvdata: %s\n", __FUNCTION__, __LINE__, uartData->UArgs);
	
	if(sockGroup.fd){
		///////////////////////dmh test/////////////////////////////////
		//close huancun timer
		OSATimerStop(_cachetimer_ref);
		//huan cun
		//sprintf(&(sendBuf[send_count]),"%s",(char *)uartData->UArgs);
		memcpy(&(sendBuf[send_count]),(char *)uartData->UArgs,uartData->len);//dmh 20200709
		send_count += uartData->len;
		if (0 != uarthandle(uartData))
		{
			if(sockGroup.fd){
				close(sockGroup.fd);
				sockGroup.fd = 0;
			}
		  
		}
		
	}
}
static void check_serial_mode(MsgUartDataParam_sdk *msgUartData)
{
    if (serial_mode == DATA_MODE) {
        if(msgUartData->len == 3 && memcmp(msgUartData->UArgs, "+++", 3) == 0) {
            //serial_mode = 0xFF;
            //send_to_uart("a", 1);
            serial_mode = AT_MODE;
			send_to_uart("\r\nOK\r\n", 6);

            // T0D0: Exit 0xff mode after 3 seconds, change to DATA_MODE
        }
    } 
}

static void sendResponse(AtCmdResponse *response)
{
    static const char *MSG_OK="\r\nOK\r\n";
    static const char *MSG_ERROR="\r\nERROR\r\n";
    char resp_buffer[MAX_LINE_SIZE]={0};
    const char *msg = NULL;
    int sz;
	if(serial_mode == DATA_MODE){
		if(sockGroup.fd){
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
				send_to_uart(resp_buffer, strlen(resp_buffer));
			else
				socket_write(sockGroup.fd,resp_buffer, strlen(resp_buffer));
		}
	}else{
	    if (response->result == DSAT_OK) {
	        if (response->response && response->response[0]) {
				if(response->istranscmd == DSAT_OK)
	            	sz = sprintf((char *)resp_buffer, "\r\n%s\r\n", response->response);
				else
					sz = sprintf((char *)resp_buffer, "\r\n%s", response->response);
	            send_to_uart((char *)resp_buffer, sz);
	        }
	        msg = MSG_OK;
	    } else {
	        msg = MSG_ERROR;
	    }
		send_to_uart((char*)msg, strlen((const char*)msg));
	}

}

static int process_at_cmd_line(char *cmdName, char *cmdLine)
{
    AtCmdResponse *response;
    int i;

    response = malloc(sizeof(AtCmdResponse));
    if ( response != NULL ) {
        memset(response, 0x0, sizeof(AtCmdResponse));
        response->response = malloc(MAX_LINE_SIZE);
        response->response[0] = 0;
        response->result = DSAT_ERROR;
        response->istranscmd = DSAT_ERROR;
        
        ATCmdTable *atcmd_ptr = dtu_atcmd_table;
        int nCommands = sizeof(dtu_atcmd_table) / sizeof(dtu_atcmd_table[0]);
        for (i = 0; i < nCommands; i++, atcmd_ptr++) {
            if (strcasecmp((char *)atcmd_ptr->at_cmd_name, cmdName)==0) {
                response->result = DSAT_OK;
                response->istranscmd = DSAT_OK;
                if (atcmd_ptr->proc_func != NULL) {
                    response->result = atcmd_ptr->proc_func(cmdName, cmdLine, response);
                }
                break;
            }
        }

        //begin test by zn
        printf("cmd: %d", response->istranscmd);
        if(response->istranscmd == DSAT_ERROR){
            char at_str[32]={'\0'};
            sprintf(at_str, "AT%s\r",cmdLine);
            char resp_str[64]={'\0'};
            response->result = SendATCMDWaitResp(TEL_AT_CMD_ATP_10, at_str, 3, NULL,1,NULL, resp_str, sizeof(resp_str));
            printf("dmhtest:resp_str:%s,%d",resp_str,strlen(resp_str));
            snprintf(response->response, strlen(resp_str)+1, "%s", resp_str);
        }
        //end test by zn

        sendResponse(response);

        if (response->response)
            free(response->response);
        free(response);
    }
    return 0;
}


static void process_at_cmd_mode(const char *atCmdData, int data_len)
{
    dsat_sio_info_s_type *sio_info_ptr = &g_sio_info;
    char *buf_ptr = NULL;
    char cc, atName[MAX_LINE_SIZE] = {0};
    int step = 0;

    buf_ptr = (char*)atCmdData;
    printf("%s[%d]: AT_MODE recv ATCMD: %s\n", __FUNCTION__, __LINE__, buf_ptr);
    //memset(tmp, 0, sizeof(tmp));//for 32bit error
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
                    printf("sio_info_ptr->cmd_line_buffer:%s\n", sio_info_ptr->cmd_line_buffer);
                    strncpy(atName, sio_info_ptr->cmd_line_buffer, step);
                    atName[step-1] = '\0';
                    printf("atName is %s\n", atName);
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

                process_at_cmd_line(atName, sio_info_ptr->cmd_line_buffer);
                
                sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            }
            break;			 

        default:
            sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
            break;
        }
    }
	memset(tmp, 0, sizeof(tmp));//for 32bit error
}

static void handle_serial_data(MsgUartDataParam_sdk *uartData)
{
    check_serial_mode(uartData);

    if (serial_mode == DATA_MODE) {
        printf("It time, serial_mode is DATA_MODE!\n");
		send_serial_data(uartData);
    } else if (serial_mode == AT_MODE) {
        printf("It time, serial_mode is AT_MODE!\n");
        if(uartData->len == 3 && memcmp(uartData->UArgs, "+++", 3) == 0)
		{
			memset(tmp, 0, sizeof(tmp));
			tmp_count = 0;
		}
		else if(strstr((const char*)uartData->UArgs,"\r\n"))
		{
			sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
			CPUartLogPrintf("[atcmd_test]: send AT command uargs:%s,len:%d\n",tmp, strlen(tmp) );
			process_at_cmd_mode(tmp, strlen(tmp));
			tmp_count = 0;
		}
		else
		{
			sprintf(&(tmp[tmp_count]),"%s",(char *)uartData->UArgs);
			tmp_count += uartData->len;//modify by dmh 2020-06-22
			if(tmp_count>450)
				tmp_count =0;
			
		}
    }
}

static void uartdata_thread(void)
{
    MsgUartDataParam_sdk uart_temp;
    OSA_STATUS status;

    serial_mode = DATA_MODE;
    
    set_UartDataSwitch_sdk(1);  // open pass-through
    //set_UartDataSwitch_sdk(0);  // close pass-through
    
    while (1) {
        memset(&uart_temp, 0, sizeof(MsgUartDataParam_sdk));
        
        status = OSAMsgQRecv(MsgUartData_sdk, (UINT8 *)&uart_temp, sizeof(MsgUartDataParam_sdk), OSA_SUSPEND);
        
        if (status == OS_SUCCESS) {
            if (uart_temp.UArgs) {
                printf("%s[%d]: uart_temp len:%d, data:%s\n", __FUNCTION__, __LINE__, uart_temp.len, (char *)(uart_temp.UArgs));
                handle_serial_data(&uart_temp);
            }
        }
    }
}

static int dtu_atcmd_regen(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;
	
    if (atName) {
        CPUartLogPrintf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }
	
    return result;
}
static int dtu_atcmd_regtp(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    if (atName) {
       // CPUartLogPrintf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        //snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }

    return result;
}
static int dtu_atcmd_regdt(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

    if (atName) {
        CPUartLogPrintf("[atcmd_test]: recv AT command then exec:AT%s\n", atLine);
        snprintf(resp->response, strlen(atLine)+1, "%s", atLine);
        result = DSAT_OK;
    }

    return result;
}
//begin add by dmh
/*function for ATCMD*/
static int dtu_atcmd_setip(char *atName, char *atLine, AtCmdResponse *resp)
{
    dsat_result_enum_type result = DSAT_ERROR;

	//begin dmh test
	AT_CMD_PARA pParam;
	//UINT8 i=0;
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		/*
		*bianli AT_CMD_PARA
		printf("dmhtest-paramiType:%d",pParam.iType);
		printf("dmhtest-paramCount:%d",pParam.paramCount);
		for(i=0;i<pParam.paramCount;i++){
			printf("dmhtest-paramType:%d",pParam.params[i]->type);
			send_to_uart(pParam.params[i]->value,pParam.params[i]->length);
			send_to_uart("\r\n",2);
		}
		*/
		result = AT_TRANS_CmdFunc_SETIP(&pParam,atLine,resp);
		free_pParam(&pParam);
		
	}
	else{
		free_pParam(&pParam);
	}
	//end dmh test
	
    return result;
}
static int dtu_atcmd_setheartflag(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETHEARTFLAG(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setheart(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETHEART(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setlinkflag(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETLINKFLAG(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setlinktype(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETLINKTYPE(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}

static int dtu_atcmd_setlink(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETLINK(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_help(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Query(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setyzcsq(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETYZCSQ(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_version(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Version(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_reload(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Reload(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_reset(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_Reset(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_seto(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_seto0(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_O(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcmdpw(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_CMDPW(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setsim(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETSIM(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setsimlock(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETSIMLOCK(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setati(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETATI(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmi(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMI(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmm(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMM(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}
static int dtu_atcmd_setcgmr(char *atName, char *atLine, AtCmdResponse *resp)
{
	dsat_result_enum_type result = DSAT_ERROR;
	AT_CMD_PARA pParam;
	
	if(package_at_cmd(atName,atLine,&pParam)==0)
	{
		result = AT_TRANS_CmdFunc_SETCGMR(&pParam,atLine,resp);
		free_pParam(&pParam);
	}
	else{
		free_pParam(&pParam);
	}
	return result;
}



//end add by dmh
//begin add by dmh
/*function for ATCMD */
static int AT_TRANS_CmdFunc_SETIP(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	UINT8 type;
	UINT8 *mode;
    UINT8 *ip;
	UINT32 port;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_SET)
    {
    	CPUartLogPrintf("dmhtest00:begin 0021 ");
        if (pParam->paramCount == 3)
        {
			 CPUartLogPrintf("dmhtest00:begin 002 ");
			 mode = at_ParamStr(pParam->params[0], &paramRet);
			 if (!paramRet)
			 	return result;
             if ((strcmp((const char*)mode, "TCP") == 0) || (strcmp((const char*)mode, "tcp") == 0))
             {
             	//set trans type
                CPUartLogPrintf("dmhtest00:TCP ");
				type = 0;
             }
             else if ((strcmp((const char*)mode, "UDP") == 0) || (strcmp((const char*)mode, "udp") == 0))
             {
             	//set trans type
                CPUartLogPrintf("dmhtest00:UDP ");
				type = 1;
             }
             else
             {
             	return result;
             }
			 ip = (UINT8 *)at_ParamStr(pParam->params[1], &paramRet);
			 if ((!paramRet)||(strlen((const char*)ip) > TRANS_IP_LEN))
	              	 return result;
			 
			 port = at_ParamUintInRange(pParam->params[2], 0, 65535, &paramRet);
             if (!paramRet)
			 	return result;
			 //set type
			 transconf.type = type;
			 //set trans ip
			 sprintf(transconf.ip,(const char*)ip);
			 //set trans port
			 transconf.port = port;
			 //return ok
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
		else
		{
			return result;
		}
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		//snprintf(resp->response, strlen(transconf->ip)+1, "%s", transconf->ip);
		CPUartLogPrintf("dmhtest00:read ip ");
		CPUartLogPrintf("dmhtest00:ip:%s",transconf.ip);
		CPUartLogPrintf("dmhtest00:port:%d",transconf.port);
		char arrStr[100]={0};
		if(transconf.type == 0)
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", transconf.ip, transconf.port);
		else 
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", transconf.ip, transconf.port);
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETHEARTFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	
    UINT8 flag;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 flag = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
			 if (!paramRet)
                return result;
			 //set heartflag
			 transconf.heartflag = flag;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
		
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSHEARTFLAG:";
        sprintf(arrStr + strlen(arrStr), "%d", transconf.heartflag);
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETHEART(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 time;
	UINT8 *heart;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 2)
        {
			 time = at_ParamUintInRange(pParam->params[0], 10, 3600, &paramRet);
			 if (!paramRet)
                return result;
			 transconf.hearttime = time;
			 heart = (UINT8 *)at_ParamStr(pParam->params[1], &paramRet);
			 if ((!paramRet)||(strlen((const char*)heart) > TRANS_HEART_LEN))
	         {
	         	return result;
	         }
			 //set heart
			 sprintf(transconf.heart,(const char*)heart);
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSHEART:";
        sprintf(arrStr + strlen(arrStr), "%d,\"%s\"", transconf.hearttime,transconf.heart);//dmh 2019-11-05
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETLINKFLAG(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 flag;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 flag = at_ParamUintInRange(pParam->params[0], 0, 3, &paramRet);
			 if (!paramRet)
                return result;
			 //set linkfalg
			 transconf.linkflag = flag;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSLINKFLAG:";
        sprintf(arrStr + strlen((const char*)arrStr), "%d", transconf.linkflag);//dmh 2019-11-05
        snprintf(resp->response, strlen((const char*)arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETLINKTYPE(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 type = 0;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 type = at_ParamUintInRange(pParam->params[0], 0, 3, &paramRet);
			 if (!paramRet)
                return result;
			 //set linkfalg
			 transconf.linktype = type;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSLINKFLAG:";
        sprintf(arrStr + strlen((const char*)arrStr), "%d", transconf.linkflag);//dmh 2019-11-05
        snprintf(resp->response, strlen((const char*)arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}

static int AT_TRANS_CmdFunc_SETLINK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 *link;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 link = (UINT8 *)at_ParamStr(pParam->params[0], &paramRet);
			 if ((!paramRet)||(strlen((const char*)link) > TRANS_LINK_LEN))
	         {
	         	return result;
	         }
			 //set link
			 sprintf(transconf.link,(const char*)link);
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+TRANSLINK:";
        sprintf(arrStr + strlen(arrStr), "%s", transconf.link);
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}

static int AT_TRANS_CmdFunc_Query(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
	    		
		char arrStr[500] ={0};
		if(transconf.type == 0)
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"TCP\"", transconf.ip, transconf.port);
		else
			sprintf(arrStr + strlen(arrStr), "%s,\"%s\",%d","+TRANSIP:\"UDP\"", transconf.ip, transconf.port);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSHEARTFLAG:", transconf.heartflag);
		sprintf(arrStr + strlen(arrStr), "%s%d,\"%s\"","\r\n+TRANSHEART:", transconf.hearttime,transconf.heart);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSLINKFLAG:", transconf.linkflag);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSLINKTYPE:", transconf.linktype);
		sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+TRANSLINK:", transconf.link);
		sprintf(arrStr + strlen(arrStr), "%s%d","\r\n+TRANSYZCSQ:", transconf.yzcsq);
		sprintf(arrStr + strlen(arrStr), "%s\"%s\"","\r\n+CMDPW:", transconf.cmdpw);
		sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIM:", transconf.sim);
		sprintf(arrStr + strlen(arrStr), "%s\"%d\"","\r\n+SIMLOCK:", transconf.simlock);
		
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
    }
	return result;
	
	
}
static int AT_TRANS_CmdFunc_Reload(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	//init write tans_file
	transconf = init_trans_conf(transconf);	
	if (pParam->iType == AT_CMD_EXE){
		if(trans_conf_file_write(transconf)==0){
		 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
			result = DSAT_OK;
		}
	}
	return result;
}
static int AT_TRANS_CmdFunc_Reset(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		PM812_SW_RESET();
		result = DSAT_OK;
	}
	return result;
}
//AP4000MT_430E_RV3V2.0_V1.2-202209271130                              å¢žåŠ hexå‘é€
static int AT_TRANS_CmdFunc_Version(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="AP4000MT_430E_RV3V2.0_V1.2-202209271130";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETYZCSQ(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 yzcsq;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {			 		 
			 yzcsq = at_ParamUintInRange(pParam->params[0], 0, 255, &paramRet);
			 if (!paramRet)
                return result;
			 //set yzcsq
			 transconf.yzcsq = yzcsq;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
			 
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		char arrStr[] = "+TRANSYZCSQ:";
        sprintf(arrStr + strlen(arrStr), "%d", transconf.yzcsq);//dmh 2019-11-05
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_O(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[100]="AT";
		sprintf(arrStr + strlen(arrStr), "%s",atLine);
	 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		serial_mode = DATA_MODE;
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_CMDPW(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8* cmdpw;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {			 		 
			 cmdpw = (UINT8 *)at_ParamStr(pParam->params[0], &paramRet);
			 if ((!paramRet)||(strlen((const char*)cmdpw) > TRANS_CMDPW_LEN))
	         {
	         	return result;
	         }
			 //set cmdpw
			 sprintf(transconf.cmdpw,(const char*)cmdpw);
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
			 
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		char arrStr[] = "+CMDPW:";
        sprintf(arrStr + strlen(arrStr), "%s", transconf.cmdpw);//dmh 2019-11-05
        snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETSIM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 sim;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 sim = at_ParamUintInRange(pParam->params[0], 1, 3, &paramRet);
			 if (!paramRet)
                return result;
			 //set sim
			 transconf.sim = sim;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+SIM:";
        sprintf(arrStr + strlen((const char*)arrStr), "%d", transconf.sim);//dmh 2019-11-05
        snprintf(resp->response, strlen((const char*)arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETSIMLOCK(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    UINT8 simlock;
	bool paramRet = TRUE;
	//init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	
	if (AT_CMD_SET == pParam->iType)
    {
        if (pParam->paramCount == 1)
        {
			 		 
			 simlock = at_ParamUintInRange(pParam->params[0], 0, 1, &paramRet);
			 if (!paramRet)
                return result;
			 //set linkfalg
			 transconf.simlock = simlock;
			 if(trans_conf_file_write(transconf)==0){
			 	char arrStr[100]="AT";
				sprintf(arrStr + strlen(arrStr), "%s",atLine);
			 	snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
				result = DSAT_OK;
			 }
        }
    }
	else if(pParam->iType == AT_CMD_READ)
	{
		
		char arrStr[] = "+SIMLOCK:";
        sprintf(arrStr + strlen((const char*)arrStr), "%d", transconf.simlock);//dmh 2019-11-05
        snprintf(resp->response, strlen((const char*)arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETATI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    //init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Manufacturer: Beijing Amaziot Co.,Ltd.\r\nModel: Amaziot AM430E\r\nRevision: AM430E RV3V2.0";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMI(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    //init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Beijing Amaziot Co.,Ltd.";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMM(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    //init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="Amaziot AM430E";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}
static int AT_TRANS_CmdFunc_SETCGMR(AT_CMD_PARA *pParam,char *atLine,AtCmdResponse *resp)
{
    //init ruturn error
	dsat_result_enum_type result = DSAT_ERROR;
	if (pParam->iType == AT_CMD_EXE){
		char arrStr[] ="AM430E V1.0";
		snprintf(resp->response, strlen(arrStr)+1, "%s", arrStr);
		result = DSAT_OK;
	}
	return result;
}



//end add by dmh
//begin add by dmh
/*function for file*/
static void trans_conf_file_init(void)
{
	int fd;
	fd = FDI_fopen("trans_file", "rb");
	if (fd) {
	  transconf = trans_conf_file_read(fd);
	} else {
	  //init write tans_file
	  transconf = init_trans_conf(transconf);
	  trans_conf_file_write(transconf);
	}
	char arrStr[500] ={0};
	sprintf(arrStr + strlen(arrStr), "AMAZIOT AP4000MT_430E\r\n");
	send_string_2uart((UINT8 *)arrStr);
  
}
static trans_conf trans_conf_file_read(int fd)
{
	trans_conf transconf;
	
    FDI_fread(&transconf,sizeof(transconf),1,fd);
	FDI_fclose(fd);
    CPUartLogPrintf("debug> ip = %s", transconf.ip);
    
	return transconf;
}
static int trans_conf_file_write(trans_conf transconf)
{
   int fd = 0;
   fd = FDI_fopen("trans_file", "wb");
   if (fd != 0){
        CPUartLogPrintf("debug> open file for write OK");
		FDI_fwrite(&transconf,sizeof(transconf), 1, fd);
		FDI_fclose(fd);
   } else {
        CPUartLogPrintf("debug> open file for write error");
		return -1;
   }
   return 0;
}
static trans_conf init_trans_conf(trans_conf transconf)
{
	transconf.type = 0;
	sprintf(transconf.ip,"183.230.40.40");
	transconf.port=1811;
	transconf.heartflag=1;
	sprintf(transconf.heart,"hearttest");
	transconf.hearttime=30;
	transconf.linkflag=1;
	transconf.linktype = 0;
	sprintf(transconf.link,"*275619#amaziot4000mt#AP4000MT*");
	transconf.yzcsq = 10;
	sprintf(transconf.cmdpw,"am.iot");
	transconf.sim=1;
	transconf.simlock=0;
	return transconf;
}
//end add by dmh
//begin add by dmh
/*function for init sim*/
static void trans_sim_init(void)
{
	int ret;
    
	//init gpio
	ret = GpioSetDirection(GPIO_LINK_PIN, GPIO_OUT_PIN);
    ASSERT(ret == GPIORC_OK);
    
   	ret = GpioSetDirection(GPIO_SA_PIN, GPIO_OUT_PIN);
	ASSERT(ret == GPIORC_OK);
		
	ret = GpioSetDirection(GPIO_SB_PIN, GPIO_OUT_PIN);
	ASSERT(ret == GPIORC_OK);

	//LinkA init
	ret = GpioSetLevel(GPIO_LINK_PIN,0);
	ASSERT(ret == GPIORC_OK);

	switch(transconf.sim){
		case 3:
			ret = GpioSetLevel(GPIO_SA_PIN,0);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,0);
			ASSERT(ret == GPIORC_OK);
			CPUartLogPrintf("dmhtest:GPIO_PIN_53-0&&GPIO_PIN_54-0");
			break;
		case 2:
		    ret = GpioSetLevel(GPIO_SA_PIN,1);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,0);
			ASSERT(ret == GPIORC_OK);
			CPUartLogPrintf("dmhtest:GPIO_PIN_53-1&&GPIO_PIN_54-0");
			break;
		case 1:
		    ret = GpioSetLevel(GPIO_SA_PIN,1);
			ASSERT(ret == GPIORC_OK);
			ret = GpioSetLevel(GPIO_SB_PIN,1);
			ASSERT(ret == GPIORC_OK);
			CPUartLogPrintf("dmhtest:GPIO_PIN_53-1&&GPIO_PIN_54-1");
			break;
		default:
			break;
	}

	//char arrStr[50] ={0};
	//sprintf(arrStr + strlen(arrStr), "\r\nAT+SIM=%d\r\n",transconf.sim);
	//send_string_2uart(arrStr);
	
	
}


static void trans_sim_switch(void)
{
	if(transconf.simlock == 0){
		transconf.sim++;
		if(transconf.sim >3)
			transconf.sim=1;
		CPUartLogPrintf("dmhtest:transconf.sim:%d",transconf.sim);
		trans_conf_file_write(transconf);	
	}
	
}


//end add by dmh 



