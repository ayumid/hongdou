/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file tm_user.h
 * @date 2020/05/14
 * @brief
 */

#ifndef __TM_USER_H__
#define __TM_USER_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"
#include "tm_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/****************************** Structure type *******************************/
struct event_Fault_Alar_t
{
    int8_t *Voltage_Fault;
    int8_t *Overload_Fault;
    int8_t *Leakage_Fault;
    int8_t *switch1;
    int32_t Lock;
};


/****************************** Auto Generated *******************************/

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
/*************************** Property Func List ******************************/
extern struct tm_prop_tbl_t tm_prop_list[];
extern uint16_t tm_prop_list_size;
/****************************** Auto Generated *******************************/

/**************************** Service Func List ******************************/
extern struct tm_svc_tbl_t tm_svc_list[];
extern uint16_t tm_svc_list_size;
/****************************** Auto Generated *******************************/

/**************************** Property Func Read ****************************/
int32_t tm_prop_App_Start_rd_cb(void *data);
int32_t tm_prop_Auto_Switch_rd_cb(void *data);
int32_t tm_prop_Flow_Value_rd_cb(void *data);
int32_t tm_prop_Input_Table_rd_cb(void *data);
int32_t tm_prop_KW_rd_cb(void *data);
int32_t tm_prop_Keep_Table_rd_cb(void *data);
int32_t tm_prop_Leakage_Value_rd_cb(void *data);
int32_t tm_prop_Over_Voltage_rd_cb(void *data);
int32_t tm_prop_Power_rd_cb(void *data);
int32_t tm_prop_Read_Loop_rd_cb(void *data);
int32_t tm_prop_Under_Voltage_rd_cb(void *data);
int32_t tm_prop_ht_electricity_rd_cb(void *data);
int32_t tm_prop_ht_leak_current_rd_cb(void *data);
int32_t tm_prop_ht_lock_cat_rd_cb(void *data);
int32_t tm_prop_ht_rest_rd_cb(void *data);
int32_t tm_prop_ht_voltage_a_rd_cb(void *data);
int32_t tm_prop_ht_voltage_b_rd_cb(void *data);
int32_t tm_prop_ht_voltage_c_rd_cb(void *data);
int32_t tm_prop_switch1_rd_cb(void *data);

/****************************** Auto Generated *******************************/

/**************************** Service Func Invoke ****************************/

/****************************** Auto Generated *******************************/

/**************************** Property Func Write ****************************/
int32_t tm_prop_App_Start_wr_cb(void *data);
int32_t tm_prop_Auto_Switch_wr_cb(void *data);
int32_t tm_prop_Flow_Value_wr_cb(void *data);
int32_t tm_prop_Input_Table_wr_cb(void *data);
int32_t tm_prop_Keep_Table_wr_cb(void *data);
int32_t tm_prop_Leakage_Value_wr_cb(void *data);
int32_t tm_prop_Over_Voltage_wr_cb(void *data);
int32_t tm_prop_Read_Loop_wr_cb(void *data);
int32_t tm_prop_Under_Voltage_wr_cb(void *data);
int32_t tm_prop_ht_lock_cat_wr_cb(void *data);
int32_t tm_prop_ht_rest_wr_cb(void *data);
int32_t tm_prop_switch1_wr_cb(void *data);

/****************************** Auto Generated *******************************/

/**************************** Property Func Notify ***************************/
int32_t tm_prop_App_Start_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Auto_Switch_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Flow_Value_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Input_Table_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_KW_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Keep_Table_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Leakage_Value_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Over_Voltage_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Power_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Read_Loop_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_Under_Voltage_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_electricity_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_leak_current_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_lock_cat_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_rest_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_voltage_a_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_voltage_b_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_ht_voltage_c_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_switch1_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms);

/****************************** Auto Generated *******************************/

/***************************** Event Func Notify *****************************/
int32_t tm_event_Fault_Alar_notify(void *data, struct event_Fault_Alar_t val, uint64_t timestamp, uint32_t timeout_ms);

/****************************** Auto Generated *******************************/

#ifdef __cplusplus
}
#endif

#endif
