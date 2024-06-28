CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q

PROJECTFLAG ?= flag-common.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================CSDK_CODE start========================#
BUILD_VARIANT := 

CSDK_SOURCE_FILES :=
CSDK_OBJ_FILES :=
CSDK_OBJ_PATH := 
CSDK_LIB_FILE := 

ONENET_IOT_CLOUD := $(TOP)/amaziot_bloom_os_sdk/third_party/onenet/mqtts_onejson_soc_v1.0.1

ifneq (${BUILD_VARIANT}, )
CSDK_LIB_FILE += build\CSDK_CODE.lib
-include $(TOP)\CSDK_CODE\CSDK_CODE.mak
endif

COMMON_CFLAGS += --diag_suppress 111,167,171,191,513,546,1293

COMMON_CFLAGS +=  -DCONFIG_ACCESS_LIFE_TIME=60
COMMON_CFLAGS +=  -DCONFIG_SEND_BUF_LEN_MAX=1024
COMMON_CFLAGS +=  -DCONFIG_RECV_BUF_LEN_MAX=1024
COMMON_CFLAGS +=  -DCONFIG_TM_MQTT
COMMON_CFLAGS +=  -DCONFIG_PLAT_ARCH_64BIT=1
COMMON_CFLAGS +=  -DCONFIG_TM=1
COMMON_CFLAGS +=  -DCONFIG_TM_GATEWAY=1
COMMON_CFLAGS +=  -DCONFIG_TM_VERSION="1.0"
COMMON_CFLAGS +=  -DCONFIG_DEFAULT_REQUEST_TIMEOUT=1000
COMMON_CFLAGS +=  -DCONFIG_PACKET_PAYLOAD_LEN_MAX=2048
COMMON_CFLAGS +=  -DCONFIG_HAVE_STDINT=1
COMMON_CFLAGS +=  -DCONFIG_NETWORK_TLS=1

##COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\utils
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\common
#COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\examples\things_model
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\platforms\include
#COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\platforms\http
#COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\platforms\mqtt
#COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\protocols\http
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\protocols\mqtt
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\security\tls
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\tm
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\onenet\utils
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\tools\cJSON
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\tools\wolfssl\port
COMMON_CFLAGS += -I$(ONENET_IOT_CLOUD)\tools\wolfssl\wolfssl-3.15.3

$(shell mkdir $(CSDK_OBJ_PATH))
#=====================CSDK_CODE end========================#

OBJ_PATH := build\obj

SOURCE_FILES += \
				$(ONENET_IOT_CLOUD)/common/log.c \
				$(ONENET_IOT_CLOUD)/common/slist.c \
				$(ONENET_IOT_CLOUD)/common/utils.c
				
SOURCE_FILES += \
				$(ONENET_IOT_CLOUD)/onenet/security/tls/tlss.c \
				$(ONENET_IOT_CLOUD)/onenet/tm/tm_api.c \
				$(ONENET_IOT_CLOUD)/onenet/tm/tm_data.c \
				$(ONENET_IOT_CLOUD)/onenet/tm/tm_onejson.c \
				$(ONENET_IOT_CLOUD)/onenet/tm/tm_mqtt.c \
				$(ONENET_IOT_CLOUD)/onenet/tm/tm_subdev.c \
				$(ONENET_IOT_CLOUD)/onenet/utils/dev_token.c \
				$(ONENET_IOT_CLOUD)/onenet/platforms/linux/osl_linux.c \
				$(ONENET_IOT_CLOUD)/onenet/platforms/linux/tcp_linux.c \
				$(ONENET_IOT_CLOUD)/onenet/platforms/linux/time_linux.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTConnectClient.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTSubscribeClient.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTConnectServer.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTSubscribeServer.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTDeserializePublish.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTUnsubscribeClient.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTFormat.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTUnsubscribeServer.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTPacket.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/mqtt_client.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/mqtt/paho-mqtt/MQTTSerializePublish.c \
				$(ONENET_IOT_CLOUD)/onenet/protocols/http/http_api.c
				
SOURCE_FILES += \
				$(ONENET_IOT_CLOUD)/tools/cJSON/cJSON.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/src/internal.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/src/keys.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/src/ssl.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/src/tls.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/src/wolfio.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/aes.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/asn.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/coding.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hash.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hmac.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/integer.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/md5.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/misc.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/random.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/rsa.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha256.c \
				$(ONENET_IOT_CLOUD)/tools/wolfssl/wolfssl-3.15.3/wolfcrypt/src/wc_port.c
				
SOURCE_FILES += \
				$(ONENET_IOT_CLOUD)/examples/things_model/onenet.c \
				$(ONENET_IOT_CLOUD)/examples/things_model/tm_user.c
				
SOURCE_FILES += \
				$(CURDIR)/main.c \
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

	
.PHONY: all clean

