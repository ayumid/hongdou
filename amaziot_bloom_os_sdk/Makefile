CC := armcc.exe
RM := del /F /Q



PROJECTFLAG ?= flag-common.mak
-include $(TOP)\sample\$(PROJECTFLAG)

COMMON_CFLAGS += -Ikernel
COMMON_CFLAGS += -Iinclude
COMMON_CFLAGS += -Idrivers\internal
COMMON_CFLAGS += -Idrivers\external
COMMON_CFLAGS += -Idrivers\external\oled
COMMON_CFLAGS += -Idrivers\external\wmri
COMMON_CFLAGS += -Ilibraries
COMMON_CFLAGS += -Ilibraries\modbus
COMMON_CFLAGS += -Imoudules\include
COMMON_CFLAGS += -Imoudules\mt_tcp
COMMON_CFLAGS += -Imoudules\mt_mqtt
COMMON_CFLAGS += -Iutils
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\yuge\inc

CUST_SRCS := $(wildcard ./kernel/*.c) \
			 $(wildcard ./drivers/internal/*.c) \
			 $(wildcard ./drivers/external/*.c) \
			 $(wildcard ./drivers/external/oled/*.c) \
			 $(wildcard ./drivers/external/wmri/*.c) \
			 $(wildcard ./libraries/*.c) \
			 $(wildcard ./libraries/modbus/*.c) \
			 $(wildcard ./utils/*.c) \
			 $(wildcard ./examples/peripherals/gpio/*.c) \
			 $(wildcard ./examples/peripherals/adc/*.c) \
			 $(wildcard ./examples/peripherals/flash/*.c) \
			 $(wildcard ./examples/peripherals/i2c/*.c) \
			 $(wildcard ./examples/peripherals/spi/*.c) \
			 $(wildcard ./examples/peripherals/timer/*.c) \
			 $(wildcard ./examples/peripherals/uart/*.c) \
			 $(wildcard ./examples/peripherals/sleep/*.c) \
			 $(wildcard ./examples/libraries/http/*.c) \
			 $(wildcard ./examples/libraries/mqtt/*.c) \
			 $(wildcard ./examples/libraries/tcp/*.c) \
			 $(wildcard ./examples/libraries/alimqtt/*.c) \
			 $(wildcard ./examples/libraries/websocket/*.c) \
			 $(wildcard ./examples/libraries/sslsocket/*.c) \
			 $(wildcard ./examples/libraries/onenetmqtt/*.c)\
			 $(wildcard ./examples/libraries/modbus/*.c) \
			 $(wildcard ./moudules/src/*.c) \
			 $(wildcard ./moudules/mt_tcp/*.c) \
			 $(wildcard ./moudules/mt_mqtt/*.c)
CUST_OBJS := $(patsubst %.c,%.o,$(CUST_SRCS))

CUST_CLRO:=$(subst /,\, $(CUST_OBJS))

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
