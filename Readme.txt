***************************************************************
说明：
1. 基础版本来自安信可 esp_iot_sdk_v1.5.2_wxcloud_beta；
2. 原始版本可接受微信LED灯标准控制版本进行控制；
3. 修改内容：
	* 接受H5自定义控制面板发来的控制命令 v161019

***************************************************************
外设说明：
    功能按键GPIO4 短按为LED开关，长按进入配置模式；
 GPIO15 控制airkiss配网指示LED;
 GPIO12 控制模拟开关LED, GPIO14 控制模拟报警LED;

程序配置说明：
    修改 airkiss_cloud.h 中  DEVICE_LICENCE 的属性为真实数值
    修改 airkiss_lan.h 中 DEVICE_TYPE 和 DEVICE_ID 属性为真实数值
    
 微信硬件详细使用说明：
    微信硬件云标准接入流程：http://iot.weixin.qq.com/wiki/new/index.html?page=3-3
    微信硬件云直连开发文档：http://iot.weixin.qq.com/wiki/new/index.html?page=4-4-2
    设备直连微信硬件云案例：http://iot.weixin.qq.com/wiki/new/index.html?page=5-5

下载编译选项：
    BOOT?=new
    APP?=1
    SPI_SPEED?=40
    SPI_MODE?=QIO
    SPI_SIZE_MAP?=2
    
烧录地址：
	boot.bin--------------->0x00000
	user1.1024.new.2.bin--->0x01000
	esp_init_data_default-->0xfc000
	blank.bin-------------->0xfe000
	

日志打印：
    编译时添加 GLOBAL_DEBUG 宏定义（Makefile 中已经添加）可以开启打印功能
    默认串口0输出日志，波特率为115200


