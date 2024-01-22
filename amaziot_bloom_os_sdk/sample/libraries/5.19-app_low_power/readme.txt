模块低功耗使用场景：
一、满足以下条件模块进入低功耗：
	1、休眠使能（默认未使能）：OEM_SET_ALLOW_ENTER_SLEEP_FALG(1);
	2、USB不接或者断开，
	3、UART 30秒内未接收数据
	4、无数据传输后一段时间后RRC Release
		
	可以通过读取UINT32 PlatformWorkLock(void)，判断是否满足休眠条件， 等于0的情况下，模块一段时间内会进入休眠状态

	客户需要处理的就是在适当的场景设置OEM_SET_ALLOW_ENTER_SLEEP_FALG(1); 允许模块进入休眠即可，如果需要退出休眠，也只需再特定的场合设置OEM_SET_ALLOW_ENTER_SLEEP_FALG(0);

二、HOST通过GPIO控制模块休眠唤醒
	详见app_1.c

三、模块连接平台，平台控制休眠唤醒
	详见app_2.c