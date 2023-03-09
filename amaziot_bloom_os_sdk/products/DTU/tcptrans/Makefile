CC := armcc.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-common.mak
-include $(TOP)\amaziot_bloom_os_sdk\products\DTU\$(PROJECTFLAG)


#COMMON_CFLAGS += -Ikernel
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\include
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\drivers\internal
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\drivers\external
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\drivers\external\oled
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\drivers\external\wmri
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\libraries
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\libraries\modbus
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\moudules\include
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\moudules\mt_tcp
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\moudules\mt_mqtt
COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\utils

CUST_SRCS := $(wildcard ./*.c) \
             $(wildcard ./atcmds/src/*.c) \
			 $(wildcard ../../../drivers/internal/*.c) \
			 $(wildcard ../../../drivers/external/*.c) \
			 $(wildcard ../../../drivers/external/oled/*.c) \
			 $(wildcard ../../../drivers/external/wmri/*.c) \
			 $(wildcard ../../../libraries/*.c) \
			 $(wildcard ../../../libraries/modbus/*.c) \
			 $(wildcard ../../../utils/*.c) \
#			 $(wildcard ../../../kernel/*.c) \

CUST_OBJS := $(patsubst %.c,%.o,$(CUST_SRCS))

CUST_CLRO :=$(subst /,\, $(CUST_OBJS))

all: $(CUST_OBJS)
	armlink.exe $^ \
		-o $(TOP)\out\bin\Arbel_PMD2NONE_40M.axf \
		--via $(OBJLIBLISTFILE) \
		--elf \
		--scatter $(SCATTERFILE) \
		--predefine="-DL1_WIFI_LOCATION" --predefine="-DDM_LTEONLY_16MPSRAM" --map --symbols --info sizes,totals \
		--list $(TOP)\out\bin\Arbel_PMD2NONE_40M.map \
		--keep init.o(Header) --keep init.o(Vectors) --diag_suppress 6312,6314,6319,6329 \
		--feedback $(FEEDBACKFILE)

	$(TOP)\tools\buildimage.bat

clean:
	$(RM) $(CUST_CLRO)

$(CUST_OBJS) : %.o : %.c
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $^



.PHONY: all clean

