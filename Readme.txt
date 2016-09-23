***************************************************************
    声明：
        本 SDK 只可用作研究测试学习使用，微信官方未正式发布SDK之前，不保证其稳定性，不可用于产品，
        由此给使用者带来的损失，本公司概不负责！

    深圳市安信可科技有限公司
    2016年4月20日

***************************************************************

本demo演示了如何利用微信直连云控制LED的亮灭和暗度调节

配套开发板为之前微信开发板（机智云开发板），也可从下面地址购买 
https://item.taobao.com/item.htm?spm=a1z10.1-c.w4004-6565671699.12.fmsza3&id=523758972998

外设说明：
    功能按键GPIO4 短按为LED开关，长按进入配置模式

程序配置说明：
    修改 airkiss_cloud.h 中  DEVICE_LICENCE 的属性为真实数值
    修改 airkiss_lan.h 中 DEVICE_TYPE 和 DEVICE_ID 属性为真实数值
    
    以下地址有详细使用说明：
    微信硬件云标准接入流程：http://iot.weixin.qq.com/wiki/new/index.html?page=3-3
    微信硬件云直连开发文档：http://iot.weixin.qq.com/wiki/new/index.html?page=4-4-2
    设备直连微信硬件云案例：http://iot.weixin.qq.com/wiki/new/index.html?page=5-5

下载编译选项：
    BOOT?=new
    APP?=1
    SPI_SPEED?=40
    SPI_MODE?=QIO
    SPI_SIZE_MAP?=2

日志打印：
    编译时添加 GLOBAL_DEBUG 宏定义（Makefile 中已经添加）可以开启打印功能
    默认串口0输出日志，波特率为115200


