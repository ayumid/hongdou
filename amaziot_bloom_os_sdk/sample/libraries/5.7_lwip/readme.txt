netif_example_SendPacket  这个函数是tx里面要增加spi的tx接口

netif_example_GetPacket  要加入到spi  rx接口里面去。

len = p->tot_len + RNDIS_HEADER_SIZE; //RNDIS_HEADER_SIZE just set length for rndis header size, need modify by customer

注意这里这个加的包头是rndis的包头，要改成你们的spi的包头长度


现在的enc28j60.c中的代码是未经验证的，，主要参照上面的说明，客户自己调试验证。