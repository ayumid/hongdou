CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-common.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================CSDK_CODE start========================#
BUILD_VARIANT := 
BUILD_VARIANT += MBEDTLS

CSDK_SOURCE_FILES :=
CSDK_OBJ_FILES :=
CSDK_OBJ_PATH := 
CSDK_LIB_FILE := 

TUYA_CONNECT_KIT_PATH := $(TOP)/amaziot_bloom_os_sdk/third_party/tuya/tuya-connect-kit

ifneq (${BUILD_VARIANT}, )
CSDK_LIB_FILE += build\CSDK_CODE.lib
-include $(TOP)\CSDK_CODE\CSDK_CODE.mak
endif

COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\utils
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\examples\switch_demo_non_qrcode
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\include
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\utils
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\interface
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\middleware
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\libraries\coreJSON\source\include
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\libraries\coreMQTT\source\include
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\libraries\coreHTTP\source\include
COMMON_CFLAGS += -I$(TUYA_CONNECT_KIT_PATH)\libraries\coreHTTP\source\dependency\3rdparty\http_parser

$(shell mkdir $(CSDK_OBJ_PATH))
#=====================CSDK_CODE end========================#

OBJ_PATH := build\obj

SOURCE_FILES += \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreHTTP/source/core_http_client.c \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreHTTP/source/dependency/3rdparty/http_parser/http_parser.c \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreJSON/source/core_json.c \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreMQTT/source/core_mqtt.c \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreMQTT/source/core_mqtt_serializer.c \
				$(TUYA_CONNECT_KIT_PATH)/libraries/coreMQTT/source/core_mqtt_state.c
				
SOURCE_FILES += \
				$(TUYA_CONNECT_KIT_PATH)/utils/cJSON.c \
				$(TUYA_CONNECT_KIT_PATH)/utils/aes_inf.c \
				$(TUYA_CONNECT_KIT_PATH)/utils/backoff_algorithm.c \
				$(TUYA_CONNECT_KIT_PATH)/utils/crc32.c \
				$(TUYA_CONNECT_KIT_PATH)/utils/uni_md5.c \
				$(TUYA_CONNECT_KIT_PATH)/utils/MultiTimer.c
				
SOURCE_FILES += \
				$(TUYA_CONNECT_KIT_PATH)/platform/linux/network_wrapper.c \
				$(TUYA_CONNECT_KIT_PATH)/platform/linux/storage_wrapper.c \
				$(TUYA_CONNECT_KIT_PATH)/platform/linux/system_wrapper.c
				
SOURCE_FILES += \
				$(TUYA_CONNECT_KIT_PATH)/middleware/mqtt_client_wrapper.c \
				$(TUYA_CONNECT_KIT_PATH)/middleware/http_client_wrapper.c

SOURCE_FILES += \
				$(TUYA_CONNECT_KIT_PATH)/src/atop_base.c \
				$(TUYA_CONNECT_KIT_PATH)/src/atop_service.c \
				$(TUYA_CONNECT_KIT_PATH)/src/file_download.c \
				$(TUYA_CONNECT_KIT_PATH)/src/iotdns.c \
				$(TUYA_CONNECT_KIT_PATH)/src/matop_service.c \
				$(TUYA_CONNECT_KIT_PATH)/src/mqtt_bind.c \
				$(TUYA_CONNECT_KIT_PATH)/src/mqtt_service.c \
				$(TUYA_CONNECT_KIT_PATH)/src/tuya_endpoint.c \
				$(TUYA_CONNECT_KIT_PATH)/src/tuya_iot.c \
				$(TUYA_CONNECT_KIT_PATH)/src/tuya_ota.c

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
