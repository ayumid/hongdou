/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file tm_user.c
 * @date 2020/05/14
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "tm_data.h"
#include "tm_api.h"
#include "tm_user.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
/*************************** Property Func List ******************************/
struct tm_prop_tbl_t tm_prop_list[] = {
    TM_PROPERTY_RW(App_Start),
    TM_PROPERTY_RW(Auto_Switch),
    TM_PROPERTY_RW(Flow_Value),
    TM_PROPERTY_RW(Input_Table),
    TM_PROPERTY_RO(KW),
    TM_PROPERTY_RW(Keep_Table),
    TM_PROPERTY_RW(Leakage_Value),
    TM_PROPERTY_RW(Over_Voltage),
    TM_PROPERTY_RO(Power),
    TM_PROPERTY_RW(Read_Loop),
    TM_PROPERTY_RW(Under_Voltage),
    TM_PROPERTY_RO(ht_electricity),
    TM_PROPERTY_RO(ht_leak_current),
    TM_PROPERTY_RW(ht_lock_cat),
    TM_PROPERTY_RW(ht_rest),
    TM_PROPERTY_RO(ht_voltage_a),
    TM_PROPERTY_RO(ht_voltage_b),
    TM_PROPERTY_RO(ht_voltage_c),
    TM_PROPERTY_RW(switch1)
};
uint16_t tm_prop_list_size = ARRAY_SIZE(tm_prop_list);
/****************************** Auto Generated *******************************/

/***************************** Service Func List *******************************/
struct tm_svc_tbl_t tm_svc_list[] = {0};
uint16_t tm_svc_list_size = 0;
/****************************** Auto Generated *******************************/

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
/**************************** Property Func Read *****************************/
int32_t tm_prop_App_Start_rd_cb(void *data)
{
	boolean val = 0;
	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_bool(data, "App_Start", val);

	return 0;
}

int32_t tm_prop_Auto_Switch_rd_cb(void *data)
{
	boolean val = 0;
	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_bool(data, "Auto_Switch", val);

	return 0;
}

int32_t tm_prop_Flow_Value_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "Flow_Value", val);

	return 0;
}

int32_t tm_prop_Input_Table_rd_cb(void *data)
{
	int32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_enum(data, "Input_Table", val);

	return 0;
}

int32_t tm_prop_KW_rd_cb(void *data)
{
	float32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_float(data, "KW", val);

	return 0;
}

int32_t tm_prop_Keep_Table_rd_cb(void *data)
{
	int32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_enum(data, "Keep_Table", val);

	return 0;
}

int32_t tm_prop_Leakage_Value_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "Leakage_Value", val);

	return 0;
}

int32_t tm_prop_Over_Voltage_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "Over_Voltage", val);

	return 0;
}

int32_t tm_prop_Power_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "Power", val);

	return 0;
}

int32_t tm_prop_Read_Loop_rd_cb(void *data)
{
	int32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_enum(data, "Read_Loop", val);

	return 0;
}

int32_t tm_prop_Under_Voltage_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "Under_Voltage", val);

	return 0;
}

int32_t tm_prop_ht_electricity_rd_cb(void *data)
{
	float32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_float(data, "ht_electricity", val);

	return 0;
}

int32_t tm_prop_ht_leak_current_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "ht_leak_current", val);

	return 0;
}

int32_t tm_prop_ht_lock_cat_rd_cb(void *data)
{
	boolean val = 0;
	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_bool(data, "ht_lock_cat", val);

	return 0;
}

int32_t tm_prop_ht_rest_rd_cb(void *data)
{
	int32_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_enum(data, "ht_rest", val);

	return 0;
}

int32_t tm_prop_ht_voltage_a_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "ht_voltage_a", val);

	return 0;
}

int32_t tm_prop_ht_voltage_b_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "ht_voltage_b", val);

	return 0;
}

int32_t tm_prop_ht_voltage_c_rd_cb(void *data)
{
	int64_t val = 0;

	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_int64(data, "ht_voltage_c", val);

	return 0;
}

int32_t tm_prop_switch1_rd_cb(void *data)
{
	boolean val = 0;
	/** 根据业务逻辑获取功能点值，设置到val */


	tm_data_struct_set_bool(data, "switch1", val);

	return 0;
}


/****************************** Auto Generated *******************************/

/**************************** Property Func Write ****************************/
int32_t tm_prop_App_Start_wr_cb(void *data)
{
    boolean val = 0;
    tm_data_get_bool(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Auto_Switch_wr_cb(void *data)
{
    boolean val = 0;
    tm_data_get_bool(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Flow_Value_wr_cb(void *data)
{
    int64_t val = 0;
    tm_data_get_int64(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Input_Table_wr_cb(void *data)
{
    int32_t val = 0;
    tm_data_get_enum(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Keep_Table_wr_cb(void *data)
{
    int32_t val = 0;
    tm_data_get_enum(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Leakage_Value_wr_cb(void *data)
{
    int64_t val = 0;
    tm_data_get_int64(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Over_Voltage_wr_cb(void *data)
{
    int64_t val = 0;
    tm_data_get_int64(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Read_Loop_wr_cb(void *data)
{
    int32_t val = 0;
    tm_data_get_enum(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_Under_Voltage_wr_cb(void *data)
{
    int64_t val = 0;
    tm_data_get_int64(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_ht_lock_cat_wr_cb(void *data)
{
    boolean val = 0;
    tm_data_get_bool(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_ht_rest_wr_cb(void *data)
{
    int32_t val = 0;
    tm_data_get_enum(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}

int32_t tm_prop_switch1_wr_cb(void *data)
{
    boolean val = 0;
    tm_data_get_bool(data, &val);
    /** 根据变量val的值，填入下发控制逻辑 */

    /***/
    return 0;
}


/****************************** Auto Generated *******************************/

/**************************** Property Func Notify ***************************/
int32_t tm_prop_App_Start_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_bool(resource, "App_Start", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Auto_Switch_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_bool(resource, "Auto_Switch", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Flow_Value_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "Flow_Value", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Input_Table_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_enum(resource, "Input_Table", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_KW_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_float(resource, "KW", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Keep_Table_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_enum(resource, "Keep_Table", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Leakage_Value_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "Leakage_Value", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Over_Voltage_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "Over_Voltage", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Power_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "Power", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Read_Loop_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_enum(resource, "Read_Loop", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_Under_Voltage_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "Under_Voltage", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_electricity_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_float(resource, "ht_electricity", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_leak_current_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "ht_leak_current", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_lock_cat_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_bool(resource, "ht_lock_cat", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_rest_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_enum(resource, "ht_rest", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_voltage_a_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "ht_voltage_a", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_voltage_b_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "ht_voltage_b", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_ht_voltage_c_notify(void *data, int64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int64(resource, "ht_voltage_c", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_switch1_notify(void *data, boolean val, uint64_t timestamp, uint32_t timeout_ms)
{
	void *resource = NULL;
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_bool(resource, "switch1", val, timestamp);

    if(NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}


/****************************** Auto Generated *******************************/

/***************************** Event Func Notify *****************************/
int32_t tm_event_Fault_Alar_notify(void *data, struct event_Fault_Alar_t val, uint64_t timestamp, uint32_t timeout_ms)
{
    void *resource = NULL;
    void *structure = tm_data_struct_create();
    int32_t ret = 0;

    if(NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_struct_set_string(structure, "Voltage_Fault", val.Voltage_Fault);
    tm_data_struct_set_string(structure, "Overload_Fault", val.Overload_Fault);
    tm_data_struct_set_string(structure, "Leakage_Fault", val.Leakage_Fault);
    tm_data_struct_set_string(structure, "switch1", val.switch1);
    tm_data_struct_set_enum(structure, "Lock", val.Lock);

    tm_data_set_struct(resource, "Fault_Alar", structure, timestamp);

    if(NULL == data)
    {
        ret = tm_post_event(resource, timeout_ms);
    }

    return ret;
}
}


/****************************** Auto Generated *******************************/

/**************************** Service Func Invoke ****************************/

/****************************** Auto Generated *******************************/
