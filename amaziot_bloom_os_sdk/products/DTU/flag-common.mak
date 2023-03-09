
FEEDBACKFILE := $(TOP)\out\feedbackLinkOpt.txt
OBJLIBLISTFILE := $(TOP)\prebuilt\Arbel_PMD2NONE_targ_objliblist.txt
SCATTERFILE := $(TOP)\prebuilt\Crane_DS_16M_Ram_16M_Flash_XIP_CIPSRAM_LteOnly.sct


COMMON_CFLAGS :=

COMMON_CFLAGS += --cpu Cortex-R4 --no_unaligned_access -g -O2 --apcs /inter --diag_suppress 2084,1,2,177,550,6319 --diag_error 47 --gnu --thumb --loose_implicit_cast

COMMON_CFLAGS += -DDATA_COLLECTOR_IMPL -DISPT_OVER_SSP -DDIAG_SSP_DOUBLE_BUFFER_USE_DYNAMIC_ALLOCATION -DENV_XSCALE -DUPGRADE_DSDSLTE \
	-DL1_DCXO_ENABLED -DL1_WIFI_LOCATION -DLTE_HIGH_MOBILITY_OPTIMIZATION -DRUN_XIP_MODE -DCRANE_Z2 -DCA_LONG_IPC_MSG -DNEZHA3 \
	-DNEZHA3_1826 -DFRBD_DSDS_L1 -DUPGRADE_PLMS -DUPGRADE_PLMS_SR -DLTE_GSMMULTIBCCH -DLTE_DSDS_GSMMULTIBCCH -DGPLC_LTE_RSSI_SCAN \
	-DL1V_NEW_RSSI -DUPGRADE_PLMS_3G -DUPGRADE_PLMS_L1 -DUPGRADE_FG_PLMS -DFG_PLMS_URR -DUPGRADE_L1A_FG_PLMS -DUPGRADE_PLMS_STAGE_2 \
	-DUPGRADE_MBCCH -DMULTI_BCCH_READY_IND -DURR_MRAT_ICS_SEARCH -DUPGRADE_ICS -DMRAT_NAS -DUPGRADE_PLMS_SEARCH_API -DICS_MBCCH \
	-DICS_MBCCH_2G_RSSI -DDIAG_NEWPP -DPHS_SW_DEMO -DPHS_SW_DEMO_TTC -DPHS_SW_DEMO_TTC_PM -DFULL_SYSTEM -D_DDR_INIT_ -D_TAVOR_HARBELL_ \
	-DUPGRADE_ARBEL_PLATFORM -D_TAVOR_B0_SILICON_ -DTDL1C_SPY_ENABLE -DDLM_TAVOR -DTAVOR -DFLAVOR_DUALCORE -DDEBUG_D2_MOR_REG_RESEREVED_ENABLE \
	-D_DIAG_USE_COMMSTACK_ -D_TAVOR_DIAG_ -DPM_DEBUG_MODE_ENABLED -DPM_D2FULL_MODE -DPM_EXT_DBG_INT_ARR -DFEATURE_WB_AMR_PS -DUPGRADE_DSDS \
	-DMACRO_FOR_LWG -DHL_LWG -DOPTIMIZE_FOR_2G_BCCH -DSUPPORT_GPLC2_RSSI_WITH_L1A1 -DPLAT_TEST -D_FDI_USE_OSA_ -DPLAT_USE_THREADX -DLWIP_IPNETBUF_SUPPORT \
	-DCRANE_MCU_DONGLE -DAT_FOR_GW20180208SN -DEXT_AT_MODEM_SUPPORT -DATNET_SSL -DAT_OVER_UART -DPHS_SW_DEMO_TTC_PM -DUPGRADE_LTE_ONLY -DMP3_DECODE \
	-DLFS_FILE_SYS -DCMUX_ENABLE -DPPP_ENABLE -DPSM_ENABLE -DCHARGE_DM -DNTP -DYMODEM_EEH_DUMP -DBIP_FUNC_SUPPORT -DDM_LTEONLY_16MPSRAM -DYUGE_PCM_FEATURE \
	-DYUGE_TTS_FEATURE -DGPS_FUNC_SUPPORT -DYUGE_AMR_FEATURE -DASR1601_SDK_BUILD -DENABLE_MAC_TX_DATA_LOGGING -DDISABLE_NVRAM_ACCESS -DINTEL_UPGRADE_EE_HANDLER_SUPPORT \
	-DLTE_W_PS -DL1_DUAL_MODE -DUPGRADE_HERMON_DUAL -DINTEL_UPGRADE_DUAL_RAT -DINTEL_UPGRADE_GPRS_CIPHER_FLUSH -DUPGRADE_ENHANCED_QUAD_BAND -DINTEL_2CHIP_PLAT \
	-DI_2CHIP_PLAT -DUPGRDE_TAVOR_COMMUNICATION -DRUN_WIRELESS_MODEM -DFLAVOR_DDR12MB_GB1MB5 -DFEATURE_SHMEM -DACIPC_ENABLE_NEW_CALLBACK_MECHANISM -DRELIABLE_DATA \
	-DMAP_NSS -DTV_FNAME="\"SW_PLATFORM=PMD2NONE PHS_SW_DEMO PHS_SW_DEMO_PM SRCNUCLEUS FULL_SYSTEM NOACRTC PDFLT PLAT_TEST PV2 DIAGOSHMEM NVM WITHL1V\"" \
	-DTV_FDESC="\"SW_DESCRIPTION=\"" -DENABLE_ACIPC -D_DATAOMSL_ENABLED_ -DUSB_CABLE_DETECTION_VIA_PMIC -DMIPS_TEST -DMIPS_TEST_RAM -DFLAVOR_DIET_RAM \
	-DNVM_INCLUDE -DMSL_INCLUDE -DMSL_POOL_MEM -DOSA_QUEUE_NAMES -D_DIAG_DISABLE_USB_ -DOSA_NUCLEUS -DOSA_USED -DPM_D2NONE_MODE -DCRANE_SOC_TEMPERATURE_SENSOR \
	-DL1_SW_UPDATE_FOR_DIGRF -DPHS_L1_SW_UPDATE_R7 -DUPGRADE_LTE -DFRBD_CALIB_NVM -DFRBD_AGC_CALIB -DFRBD_FDT_CALIB -DHSPA_MPR -DCAPT_PARAMS_OPTIMIZE -DL1_WB_R99_ONLY \
	-DL1V_WB_R99_ONLY -DINTERGRATED_RF_SUPPORT -DL1_RX_DIV_SUPPORT -DENABLE_OOS_HANDLING -DTAVOR_D2_WB_L1_SUPPORT -DL1_DDR_HIGH_FREQ -DUPGRADE_DIGRF3G_SUPPORT -DW_PS_PLUS_G_PAGING \
	-D"NO_APLP=0" -DINTEL_UPGRADE_UNIFIED_VOICE_TASK -DINTEL_UPGRADE_R99 -DAPLP_SPY_ENABLE -D__TARGET_FEATURE_DOUBLEWORD -DWHOLE_UMTS_STACK -DUSE_TTPCOM_CSR_BLUETOOTH_AUDIO_GAIN_CONTROL \
	-DL1_UPGRADE_R5 -DUPGRADE_EDGE -DUPGRADE_R4_FS1 -DINTEL_UPGRADE_GSM_CRL_IF -DUPGRADE_EGPRS_M -DINTEL_UPGRADE_EGPRS_M -DINTEL_UPGRADE_RF_PARAMS_IN_CF_TDS \
	-DINTEL_UPGRADE_2SAMPLES_PER_SYMBOL -D"GPRS_MULTISLOT_CLASS=12" -D"EGPRS_MULTISLOT_CLASS=12" -DMARVELL_UPGRADE_BSIC_REDESIGN -DMSL_INCLUDE -DINTEL_HERMON_SAC -DBT_SUPPORT \
	-DBT_TEST_SUPPORT -DCRANE_CUST_BUILD -DL1_SW_UPDATE_FOR_DIGRF -DFLAVOR_COM -DSILICON_PV2 -DSILICON_SEAGULL -DSILICON_TTC_CORE_SEAGULL -DPCAC_INCLUDE





COMMON_CFLAGS += -Iatcmds\inc

COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\yuge\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\UART\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\core\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\PMU\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\GPIO\inc

COMMON_CFLAGS += -I$(TOP)\include\asr1601\diag\diag_logic\src
COMMON_CFLAGS += -I$(TOP)\include\asr1601\csw\SysCfg\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\csw\platform\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\env\win32\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\csw\BSP\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\csw\platform\dev_plat\build
COMMON_CFLAGS += -I$(TOP)\include\asr1601\os\osa\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\os\threadx\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\os\nu_xscale\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\lwipv4v6\src\include\lwip
COMMON_CFLAGS += -I$(TOP)\include\asr1601\diag\diag_logic\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\timer\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\intc\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\csw\PM\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\pm\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\TickManager\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\BSP\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\atcmdsrv\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\atparser\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\sdk\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\httpclient\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\ci\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\lwipv4v6\src\include\arch
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\lwipv4v6\src\include\ipv4
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\lwipv4v6\src\include\ipv6
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\mmi_mat\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\tavor\Arbel\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\tavor\env\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\modem\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\duster\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\fota\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\I2C\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\ACIPC\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\mbedTLS\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\mbedTLS\include\mbedtls
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\mbedTLS\include\psa
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\fatsys\flash
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\FDI\src\INCLUDE
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\MMU\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\FDI\src\FDI_ADD
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\FDI\src\FM_INC
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\fatsys\fs\hdr
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\littlefs\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tts\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\dial\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\paho_mqtt\mqttclient
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\paho_mqtt\mqtt
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\paho_mqtt\common
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\paho_mqtt\platform
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\csw_memory\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\paho_mqtt\network
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\utilities\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\commpm\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\nvm\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\EEhandler\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\EEhandler\src
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\RTC\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\telephony\ci_client\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\BT_device\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\UART\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\mrd\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\softutil\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\mbedTLS\asros
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\SPI\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\websocket\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\atnet_srv\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\softutil\fotacomm\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\aud_sw\Audio\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\aud_sw\ACM_COMM\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\aud_sw\audio_stub\src
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hop\aam\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\aud_sw\AudioHAL\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\hal\dbgshell\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\amr\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\aud_sw\AudioService\inc
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\interface
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\libraries\coreHTTP\source\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\libraries\coreHTTP\source\dependency\3rdparty\http_parser
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\libraries\coreJSON\source\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\libraries\coreMQTT\source\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\middleware
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\tuya\utils
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\mp3\include
COMMON_CFLAGS += -I$(TOP)\include\asr1601\pcac\opencpu\inc