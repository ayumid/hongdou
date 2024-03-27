# hongdou

#### 介绍
红豆平台基于AM430EV5模组，硬件接口，软件组件丰富，可以方便开发者使用平台快速开发自己的项目，或者直接使用平台提供的固件生产。

#### 软件架构
红豆版本分为两个部分，一部分为固定的库文件，编译器文件，下载文件等。这一部分体积比较大，一般不会修改。因此将这一部分代码放入wiki上。这一部分文件树介绍如下：
- CSDK_CODE：开源协议栈源代码，在makefile中选择编译。
- CSDK_LIB：库文件，这一部分代码不开源，包括底层的业务逻辑实现。
- download：固件打包，下载的工具所在文件夹，工具名称为about。
- fota_tool：Fota文件生成文件夹。红豆平台只支持最小系统升级
- include：Code和lib中源文件的一些头文件。
- out：编译生成的axf、bin、map文件目录，执行build.bat后，会将bin文件拷贝到download相应目录中。
- prebuilt-xxx：和平台相关的一些文件，Catstudio查看日志需要导入的文件，以及芯片内存分配文件在这里。
- tools：编译所需要一些工具。
- 其它：Excle文件包括IO定义等，其它文本都是一些说明性文件，建议在开发之前查看。
下载地址：https://rckrv97mzx.feishu.cn/drive/folder/UiYAf1xoklW3S7dbY4ScD58CnTs
gitee部分介绍
- libraries：库文件，包括奇迹提供的库文件，以及第三方库文件，开源。
- platform：平台文件，包含涂鸦，阿里云、腾讯云等云平台sdk在这里。
- products：开源产品文件夹，包括源文件，头文件，makefile等。
- sample：Demo文件夹，所有的功能实例。
- utils：工具API所在文件夹。包括字符串操作等。
下载地址：https://gitee.com/ning./hongdou