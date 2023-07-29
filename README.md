

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

操作方式

- 短按按键1，开关LED
- 长按按键1两秒后松开，重置WIFI并会自动重启
- 短按按键2，亮度增加
- 短按按键3，亮度减少

首次烧录程序会让你配网，默认会打开AP热点，手机搜索热点``VFD-``开头的没有密码直接连接，按理正常会弹出配网的界面。
如果没弹出界面，那么请在浏览器输入``192.168.4.1``进行配网。
配网成功后会自动连接WIFI然后重启，如果配网失败会重启并重新开启AP热点。
如果在配网状态下长时间没有操作，系统将会超时进入死循环，屏幕会显示error信息。这个时候请手动重启。

### 2. 驱动库使用步骤

1. 首先您需要引入库文件

   ```c
   #include <gui.h>
   ```

2. 在Main方法初始化中Setup方法初始化

   ```c
   // 初始化VFD
   ptInitGPIO();
   delay(3000);
   vfd_gui_init();
   ```

3. 显示您要打印的内容

   ```c
   vfd_gui_set_text("1234AB");//不可超过最大VFD位数6位
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

7. 要点亮的ICON图标，宏定义传参

   ```c
   vfd_gui_set_icon(u32); 
   使用或运算显示多个
   vfd_gui_set_icon(ICON_WIFI | ICON_CLOCK);
   ```

7. 设置亮度等级 1~7

   ```c
   vfd_gui_set_blk_level(size_t level);
   ```

7. 设置两位冒号的显示

   ```c
   第一位冒号，1或者0
   void vfd_gui_set_maohao1(u8 open);

   第二位冒号，1或者0
   void vfd_gui_set_maohao2(u8 open);
   ```