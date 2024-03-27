CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-1606l.mak
-include $(TOP)\amaziot_bloom_os_sdk\products\am-xtu\$(PROJECTFLAG)

#=====================dtu version start========================#
DTU_TCP := yes#TCP版本，定义这个宏定义
#DTU_MQTT := yes#MQTT版本，定义这个宏定义
#DTU_GNSS := yes#包含GNSS功能，定义这个宏定义
DTU_JSON := yes#包含JSON协议，定义这个宏定义
DTU_CJSON := yes#包含CJSON，定义这个宏定义
DTU_DODIAI := yes#包含DI，DO，AI功能，定义这个宏定义
DTU_MODBUS := yes#包含Modbus功能，定义这个宏定义
#DTU_HTTP := yes#包含HTTP功能，定义这个宏定义，HTTP作为TCP或MQTT附加功能出现
#DTU_FORMAT := yes#包含格式化透传数据，定义这个宏定义
#DTU_AGNSS := yes#包含AGNSS功能，定义这个宏定义，基于中科微agnss，其它厂家可能不适用

#DTU_EXSIM := yes#使用外置卡功能，定义这个宏定义
#DTU_5X6 := yes#使用5x6卡功能，定义这个宏定义
DTU_3IN1 := yes#使用3和1卡功能，定义这个宏定义

DTU_115200 := yes#使用波特率115200，定义这个宏定义
#DTU_38400 := yes#使用波特率38400，定义这个宏定义
#DTU_9600 := yes#使用波特率9600，定义这个宏定义

ifeq ($(DTU_TCP), yes)
COMMON_CFLAGS +=  -DDTU_BASED_ON_TCP
endif
ifeq ($(DTU_MQTT), yes)
COMMON_CFLAGS +=  -DDTU_BASED_ON_MQTT
endif
ifeq ($(DTU_GNSS), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_GNSS_INCLUDE
endif
ifeq ($(DTU_JSON), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_JSON_INCLUDE
endif
ifeq ($(DTU_CJSON), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_CJSON_INCLUDE
endif
ifeq ($(DTU_DODIAI), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_DODIAI_INCLUDE
COMMON_CFLAGS +=  -DDTU_TYPE_JSON_INCLUDE
endif
ifeq ($(DTU_MODBUS), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_MODBUS_INCLUDE
endif
ifeq ($(DTU_HTTP), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_HTTP_INCLUDE
endif

ifeq ($(DTU_FORMAT), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_FORMAT_INCLUDE
endif
ifeq ($(DTU_AGNSS), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_AGNSS_INCLUDE
endif

ifeq ($(DTU_EXSIM), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_EXSIM
endif
ifeq ($(DTU_5X6), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_5X6
endif
ifeq ($(DTU_3IN1), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_3IN1
endif

ifeq ($(DTU_115200), yes)
COMMON_CFLAGS +=  -DDTU_UART_115200_BAUD
endif
ifeq ($(DTU_38400), yes)
COMMON_CFLAGS +=  -DDTU_UART_38400_BAUD
endif
ifeq ($(DTU_9600), yes)
COMMON_CFLAGS +=  -DDTU_UART_9600_BAUD
endif
#=====================dtu version end========================#
#=====================CSDK_CODE start========================#
BUILD_VARIANT := 
ifeq ($(DTU_CJSON), yes)
BUILD_VARIANT += CJSON
endif
ifeq ($(DTU_MQTT), yes)
BUILD_VARIANT += PAHOMQTT
endif

CSDK_SOURCE_FILES :=
CSDK_OBJ_FILES :=
CSDK_OBJ_PATH := 
CSDK_LIB_FILE := 

ifneq (${BUILD_VARIANT}, )
CSDK_LIB_FILE += build\CSDK_CODE.lib
-include $(TOP)\CSDK_CODE\CSDK_CODE.mak
endif

$(shell mkdir $(CSDK_OBJ_PATH))
#=====================CSDK_CODE end========================#

COMMON_CFLAGS += -I$(CURDIR)\atcmds\inc
COMMON_CFLAGS += -I$(CURDIR)
COMMON_CFLAGS += -I$(TOP)/amaziot_bloom_os_sdk/libraries\am\xtu

ifdef DTU_MODBUS
COMMON_CFLAGS += -I$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3\inc
COMMON_CFLAGS += -I$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3\util
endif

OBJ_PATH := build\obj

SOURCE_FILES := $(CURDIR)/main.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_at.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_file.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_gpio.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_hb.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_trans.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_net.c \
				$(TOP)/amaziot_bloom_os_sdk/utils/utils_string.c

ifeq ($(DTU_TCP), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_socket.c
endif
ifeq ($(DTU_MQTT), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_mqtt.c \
                $(TOP)/amaziot_bloom_os_sdk/utils/utils_sha1.c
endif
ifeq ($(DTU_GNSS), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_gnss.c
endif
ifeq ($(DTU_JSON), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_json.c
endif
ifeq ($(DTU_DODIAI), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_ai.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_di.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_do.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_clock.c
endif
ifeq ($(DTU_MODBUS), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_modbus.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3/util/agile_modbus_slave_util.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3/src/agile_modbus.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3/src/agile_modbus_rtu.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/third_party/agile_modbus-1.1.3/src/agile_modbus_tcp.c
endif
ifeq ($(DTU_HTTP), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_http.c
endif

ifeq ($(DTU_FORMAT), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_format.c
endif
ifeq ($(DTU_AGNSS), yes)
SOURCE_FILES += $(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_agnss.c
endif

OBJ_FILES := $(patsubst %.c,$(OBJ_PATH)/%.o, $(notdir $(SOURCE_FILES)))

$(shell mkdir $(OBJ_PATH))
$(shell $(RM) $(OBJ_PATH)\ext_at_cmds.o)

all: $(OBJ_FILES) $(CSDK_LIB_FILE)
	armlink.exe $^ \
		-o $(TOP)\out\bin\Arbel_PMD2NONE_40M.axf \
		--via $(OBJLIBLISTFILE) \
		--elf \
		--scatter $(SCATTERFILE) \
		--predefine="-DLTEONLY_THIN_SINGLE_SIM" --map --symbols --info sizes,totals \
		--list $(TOP)\out\bin\Arbel_PMD2NONE_40M.map \
		--keep init.o(Header) --keep init.o(Vectors) --diag_suppress 6312,6314,6319,6329 \
		--feedback $(FEEDBACKFILE)

	$(TOP)\tools\buildimage_1606l.bat

	$(warning $(BUILD_DATE))
	$(warning $(BUILD_TIME))

clean:
	$(RM) $(OBJ_PATH) $(CSDK_OBJ_PATH) $(CSDK_LIB_FILE)
	
$(OBJ_FILES): $(SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)

$(CSDK_LIB_FILE): $(CSDK_OBJ_FILES)
	$(AR) -v --create -r $@ $^

$(CSDK_OBJ_FILES): $(CSDK_SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)
	
.PHONY: all clean

