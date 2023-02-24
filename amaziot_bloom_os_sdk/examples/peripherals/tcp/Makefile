CC := armcc.exe
RM := cmd /c del /F /Q


PROJECTFLAG ?= flag-common.mak
-include $(TOP)\sample\$(PROJECTFLAG)


all: main.o ext_at_cmds.o ext_at_controller.o
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
	$(RM) *.o

%.o: %.c
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $^

ext_at_cmds.o: atcmds\src\ext_at_cmds.c
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $^

ext_at_controller.o: atcmds\src\ext_at_controller.c
	$(CC) -c $(COMMON_CFLAGS) $(CFLAGS) -o $@ $^
	
.PHONY: all clean
