CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q

PROJECTFLAG ?= flag-common.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================CSDK_CODE start========================#
BUILD_VARIANT := 
BUILD_VARIANT += MBEDTLS
BUILD_VARIANT += CJSON

CSDK_SOURCE_FILES :=
CSDK_OBJ_FILES :=
CSDK_OBJ_PATH := 
CSDK_LIB_FILE := 

TENCENT_IOT_EXPLORER_PATH := $(TOP)/amaziot_bloom_os_sdk/platforms/tencent/qcloud_iot_c_sdk

ifneq (${BUILD_VARIANT}, )
CSDK_LIB_FILE += build\CSDK_CODE.lib
-include $(TOP)\CSDK_CODE\CSDK_CODE.mak
endif

##COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\utils
COMMON_CFLAGS += -I$(TENCENT_IOT_EXPLORER_PATH)\include\exports
COMMON_CFLAGS += -I$(TENCENT_IOT_EXPLORER_PATH)\include
COMMON_CFLAGS += -I$(TENCENT_IOT_EXPLORER_PATH)\sdk_src\internal_inc

$(shell mkdir $(CSDK_OBJ_PATH))
#=====================CSDK_CODE end========================#

OBJ_PATH := build\obj

SOURCE_FILES += \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_action.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_client.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_client_common.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_client_json.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_client_manager.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/data_template_event.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/device_bind.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/dynreg.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/gateway_api.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/gateway_common.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/json_parser.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/json_token.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/kgmusic_client.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_common.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_connect.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_net.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_publish.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_subscribe.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_unsubscribe.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/mqtt_client_yield.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/network_interface.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/network_socket.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/ota_client.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/ota_fetch.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/ota_lib.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/ota_mqtt.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/qcloud_iot_ca.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/qcloud_iot_device.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/qcloud_iot_log.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/resource_client.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/resource_lib.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/resource_mqtt.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/resource_upload.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/service_mqtt.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/string_utils.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_aes.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_base64.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_getopt.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_hmac.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_httpc.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_list.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_md5.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_ringbuff.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_sha1.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_timer.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_url_download.c \
				$(TENCENT_IOT_EXPLORER_PATH)/sdk_src/utils_url_upload.c

SOURCE_FILES += \
				$(TENCENT_IOT_EXPLORER_PATH)/platform/HAL_Device_freertos.c \
				$(TENCENT_IOT_EXPLORER_PATH)/platform/HAL_OS_freertos.c \
				$(TENCENT_IOT_EXPLORER_PATH)/platform/HAL_TCP_lwip.c \
				$(TENCENT_IOT_EXPLORER_PATH)/platform/HAL_Timer_freertos.c \
				$(TENCENT_IOT_EXPLORER_PATH)/platform/HAL_UDP_lwip.c
				
SOURCE_FILES += \
				$(CURDIR)/main.c \
				$(CURDIR)/am_gpio.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c

OBJ_FILES := $(patsubst %.c,$(OBJ_PATH)/%.o, $(notdir $(SOURCE_FILES)))

$(shell mkdir $(OBJ_PATH))
$(shell $(RM) $(OBJ_PATH)\ext_at_cmds.o)

all: $(OBJ_FILES) $(CSDK_LIB_FILE)
	armlink.exe $^ \
		-o $(TOP)\out\bin\Arbel_PMD2NONE_40M.axf \
		--via $(OBJLIBLISTFILE) \
		--elf \
		--scatter $(SCATTERFILE) \
		$(PREDEFINE) \
		--map --symbols --info sizes,totals \
		--list $(TOP)\out\bin\Arbel_PMD2NONE_40M.map \
		--keep init.o(Header) --keep init.o(Vectors) --diag_suppress 6312,6314,6319,6329 \
		--feedback $(FEEDBACKFILE)

	$(BUILDIMAGE)

clean:
	$(RM) $(OBJ_PATH) $(CSDK_OBJ_PATH) $(CSDK_LIB_FILE)
	
$(OBJ_FILES): $(SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)

$(CSDK_LIB_FILE): $(CSDK_OBJ_FILES)
	$(AR) -v --create -r $@ $^

$(CSDK_OBJ_FILES): $(CSDK_SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)
	
.PHONY: all clean
