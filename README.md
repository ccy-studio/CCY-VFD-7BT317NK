
# CCY-VFD-7BT317NK
>  VFD屏幕7-BT-317NK驱动程序，主控ESP8266，驱动IC PT6315


开发环境：Arduino、PlatformIO

VFD驱动PT6315驱动程序，用于驱动自制VFD时钟项目。

主控ESP-12S安信可模块，集成WIFI配网自动校时为一体的桌面时间小制作。

博客： www.saisaiwa.com

VFD屏WIFI时钟硬件开源链接：http://www.saisaiwa.com/archives/vfd-clock-7-BT-317NK

VFD屏PT6315驱动时序：http://www.saisaiwa.com/archives/pt6315-code-jangj

交流群： http://www.saisaiwa.com/about



### 1. 计划实现功能

#### 1.1 release - 1.0

- WIFI配网
- 联网校时
- 按键切换显示Style与内容



### 2. 驱动库使用步骤

1. 首先您需要引入库文件

   ```c
   #include <gui.h>
   ```

2. 在Main方法初始化中Setup方法初始化

   ```c
   	// 初始化VFD
       delay(3000);
       vfd_gui_init();
   ```

3. 显示您要打印的内容

   ```c
   vfd_gui_set_text("1234ABC");//不可超过最大VFD位数7位
   ```

4. 显示单个字符并指定要显示的坐标

   ```c
   vfd_gui_set_one_text(0, 'C');// 坐标从0开始最大到6 共计7位
   ```

5. 背光开关

   ```c
   vfd_gui_set_bck(u8 onOff);// 传入1 开启，0关闭
   ```

6. 清屏

   ```c
   vfd_gui_clear();
   ```

7. 其他待补充...
