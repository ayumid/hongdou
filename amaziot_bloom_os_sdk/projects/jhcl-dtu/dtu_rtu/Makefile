CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-1606l.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================dtu version start========================#
DTU_TCP := yes#TCP版本，定义这个宏定义

#DTU_EXSIM := yes#使用外置卡功能，定义这个宏定义
DTU_5X6 := yes#使用5x6卡功能，定义这个宏定义
#DTU_3IN1 := yes#使用3和1卡功能，定义这个宏定义

DTU_JHCL := yes#佳华储良版本，定义这个宏定义
DTU_9600 := yes#使用波特率9600，定义这个宏定义

ifeq ($(DTU_TCP), yes)
COMMON_CFLAGS +=  -DDTU_BASED_ON_TCP
endif
ifeq ($(DTU_GNSS), yes)
COMMON_CFLAGS +=  -DDTU_TYPE_GNSS_INCLUDE
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

ifeq ($(DTU_JHCL), yes)
COMMON_CFLAGS +=  -DDTU_UART_JHCL
endif
ifeq ($(DTU_9600), yes)
COMMON_CFLAGS +=  -DDTU_UART_9600_BAUD
endif
#=====================dtu version end========================#
#=====================CSDK_CODE start========================#
BUILD_VARIANT := 
ifeq ($(DTU_JSON), yes)
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
COMMON_CFLAGS += -I$(CURDIR)\am

OBJ_PATH := build\obj

SOURCE_FILES := $(CURDIR)/main.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c \
				$(CURDIR)/am/am_at.c \
				$(CURDIR)/am/am_file.c \
				$(CURDIR)/am/am_gpio.c \
				$(TOP)/amaziot_bloom_os_sdk/utils/utils_string.c

ifeq ($(DTU_TCP), yes)
SOURCE_FILES += $(CURDIR)/am/am_socket.c
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

