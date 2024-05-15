CC := armcc.exe
AR := armar.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-1606l.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)

#=====================CSDK_CODE start========================#
BUILD_VARIANT := 

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

COMMON_CFLAGS += -I$(CURDIR)\am\inc
COMMON_CFLAGS += -I$(CURDIR)\atcmds\inc
COMMON_CFLAGS += -I$(TOP)/amaziot_bloom_os_sdk/libraries\am\xtu
COMMON_CFLAGS +=  -DDTU_BASED_ON_TCP
COMMON_CFLAGS +=  -DDTU_TYPE_5X6
COMMON_CFLAGS +=  -DDTU_UART_115200_BAUD

OBJ_PATH := build\obj

SOURCE_FILES := $(CURDIR)/main.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_file.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_at.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_trans.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_socket.c \
				$(TOP)/amaziot_bloom_os_sdk/libraries/am/xtu/am_hb.c \
				
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

clean:
	$(RM) $(OBJ_PATH) $(CSDK_OBJ_PATH) $(CSDK_LIB_FILE)
	
$(OBJ_FILES): $(SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)

$(CSDK_LIB_FILE): $(CSDK_OBJ_FILES)
	$(AR) -v --create -r $@ $^

$(CSDK_OBJ_FILES): $(CSDK_SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)
	
.PHONY: all clean

