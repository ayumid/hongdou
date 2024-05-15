CC := armcc.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-common.mak
-include $(TOP)\amaziot_bloom_os_sdk\$(PROJECTFLAG)


OBJ_PATH := build\obj

SOURCE_FILES := $(CURDIR)/main.c \
				$(CURDIR)/websocket.c \
				$(CURDIR)/atcmds/src/ext_at_cmds.c \
				$(CURDIR)/atcmds/src/ext_at_controller.c

OBJ_FILES := $(patsubst %.c,$(OBJ_PATH)/%.o, $(notdir $(SOURCE_FILES)))

BUILD_VARIANT := 
BUILD_VARIANT += MBEDTLS

-include $(TOP)\CSDK_CODE\CSDK_CODE.mak

$(shell mkdir $(OBJ_PATH))

all: $(OBJ_FILES)
	armlink.exe $^ \
		-o $(TOP)\out\bin\Arbel_PMD2NONE_40M.axf \
		--via $(OBJLIBLISTFILE) \
		--elf \
		--scatter $(SCATTERFILE) \
		--predefine="-DLTEONLY_THIN_SINGLE_SIM" --map --symbols --info sizes,totals \
		--list $(TOP)\out\bin\Arbel_PMD2NONE_40M.map \
		--keep init.o(Header) --keep init.o(Vectors) --diag_suppress 6312,6314,6319,6329 \
		--feedback $(FEEDBACKFILE)

	$(TOP)\tools\buildimage.bat

clean:
	$(RM) $(OBJ_PATH)
	
$(OBJ_FILES): $(SOURCE_FILES)
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $(filter %$(patsubst %.o,/%.c, $(notdir $@)), $^)
	
.PHONY: all clean
