CC := armcc.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-1606l.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================CSDK_CODE start========================#
BUILD_VARIANT := 
#BUILD_VARIANT += MBEDTLS
#BUILD_VARIANT += PAHOMQTT
#BUILD_VARIANT += CJSON

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


COMMON_CFLAGS += -I./
COMMON_CFLAGS += -ILinkSDK\core
COMMON_CFLAGS += -ILinkSDK\core\utils
COMMON_CFLAGS += -ILinkSDK\core\sysdep
COMMON_CFLAGS += -ILinkSDK\components\dynreg
COMMON_CFLAGS += -ILinkSDK\external\mbedtls\include
COMMON_CFLAGS += -ILinkSDK\external\mbedtls\include\mbedtls


OBJ_PATH := build\obj

SOURCE_FILES := $(CURDIR)/main.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c
				
SOURCE_FILES += $(CURDIR)/LinkSDK/demos/mqtt_basic_demo.c
#SOURCE_FILES += $(CURDIR)/LinkSDK/demos/dynreg_basic_demo.c

SOURCE_FILES += $(CURDIR)/LinkSDK/core/aiot_mqtt_api.c \
				$(CURDIR)/LinkSDK/core/aiot_state_api.c \
				$(CURDIR)/LinkSDK/core/sysdep/core_sysdep.c \
				$(CURDIR)/LinkSDK/core/sysdep/core_adapter.c \
				$(CURDIR)/LinkSDK/core/utils/core_auth.c \
				$(CURDIR)/LinkSDK/core/utils/core_diag.c \
				$(CURDIR)/LinkSDK/core/utils/core_global.c \
				$(CURDIR)/LinkSDK/core/utils/core_http.c \
				$(CURDIR)/LinkSDK/core/utils/core_log.c \
				$(CURDIR)/LinkSDK/core/utils/core_sha256.c \
				$(CURDIR)/LinkSDK/core/utils/core_string.c \
				$(CURDIR)/LinkSDK/portfiles/aiot_port/freertos_port.c

SOURCE_FILES += $(CURDIR)/LinkSDK/components/dynreg/aiot_dynreg_api.c
				
SOURCE_FILES += $(CURDIR)/LinkSDK/external/ali_ca_cert.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/aes.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/asn1parse.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/base64.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/bignum.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/cipher.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/cipher_wrap.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/ctr_drbg.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/debug.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/entropy.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/error.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/md.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/md_wrap.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/oid.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/pem.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/pk.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/pk_wrap.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/pkparse.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/platform.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/platform_util.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/rsa.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/rsa_internal.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/sha1.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/sha256.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/ssl_ciphersuites.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/ssl_cli.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/ssl_cookie.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/ssl_tls.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/x509.c \
				$(CURDIR)/LinkSDK/external/mbedtls/library/x509_crt.c
				
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
