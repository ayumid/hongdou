20230508
1. 增加Bloom os文件目录树
2. 修改build.bat，如下，增加后自动拷贝cp_1606L.bin到about中
gnumake -C %MODULE%

@echo off  
echo "copy NEZHAC_CP_CNR_MIFI_TX.bin to ./ "  
echo.  

move .\out\bin\cp_1606L .\download\aboot-tools-2023.04.03-win-x64_1191\images

:END_BAT_FILE

3. 修改setenv.bat

4. 增加工程到example中，复制.mak文件，修改makefile
路径修改为：
-include $(TOP)\amaziot_bloom_os_sdk\examples\libraries\$(PROJECTFLAG)
修改.mak文件为PROJECTFLAG ?= flag-1606l.mak
修改.bat文件为$(TOP)\tools\buildimage_1606l.bat

5. 修改prebuild1606x\Arbel_PMD2NONE_targ_objliblist.txt文件
相对路径../..修改为../../../../

6. 修改flag-1606x-xxx文件，加入COMMON_CFLAGS += -I$(TOP)\amaziot_bloom_os_sdk\utils

参考：
https://rckrv97mzx.feishu.cn/docx/J2W3ds8SMoqwF4xRjFackbsjnFc