1、串口默认可用来执行模块通用AT指令，UART_OPEN 后会切换成普通uart通道，不再执行AT指令
2、该demo是在普通uart通道模式下，可以添加一些简单的AT或者客户自定义格式指令