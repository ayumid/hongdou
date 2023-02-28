# 红豆

#### Bloom OS介绍
Bloom OS 红豆版 基于asr1601/asr1603/asr1606 cat.1芯片开发  

#### 软件架构
amaziot_bloom_os_sdk 包含bloom os平台驱动，示例等  
download 下载工具和编译需要的bin文件，包括固定的bin文件和编译生成的cp.bin  
ota_tool 差分升级包生成批处理和教程  
framework 系统框架  
include 一些头文件  
out 编译生成的cp.bin临时目录，build.bat内已经把生成的文件移动到了download/about.../inage/文件夹  
prebuilt 预编译相关的一些文件，下载后需要替换离线sdk包内对应文件  
tools 编译需要的一些可执行文件等  

#### 安装教程

下载后，放入本地离线SDK中，替换对应文件

#### 使用说明

1.  在amaziot_bloom_os_sdk\examples\路径建立自己的工程文件夹，例如：amaziot_bloom_os_sdk\examples\my_proj
2.  复制peripherals或者libraries中的一个文件到新建的工程文件夹中,例如：amaziot_bloom_os_sdk\examples\my_proj\DTU\
3.  在根目录\hongdou，右键执行在此处打开命令行或者powershell(右键打开xxx教程：https://rckrv97mzx.feishu.cn/wiki/wikcnnpgJ1ZrREMjqVlyVMtguMc)
4.  执行.\build.bat .\amaziot_bloom_os_sdk\examples\peripherals\DTU
5.  打开hongdou\download\aboot-tools-2021.02.20-win-x64\about.exe，参考教程进行编译（教程：https://rckrv97mzx.feishu.cn/wiki/wikcno9WPmvoX3xIFIgvq0Q1ipd）

