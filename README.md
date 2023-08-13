

# CCY-VFD-7BT317NK
>  VFD屏幕7-BT-317NK驱动程序，主控ESP8266，驱动IC PT6315


开发环境：Arduino、PlatformIO

VFD驱动PT6315驱动程序，用于驱动自制VFD时钟项目。

主控ESP-12S安信可模块，集成WIFI配网自动校时为一体的桌面时间小制作。

博客： www.saisaiwa.com

VFD屏WIFI时钟硬件开源链接：http://www.saisaiwa.com/archives/vfd-clock-7-BT-317NK

VFD屏PT6315驱动时序：http://www.saisaiwa.com/archives/pt6315-code-jangj

交流群： http://www.saisaiwa.com/about

本项目是PlatformIO项目介于有些朋友电脑缺失环境或者网络原因一直卡着无法完成安装的原因，这里提供一个云编译的方法。此方法使用云端Docker容器借助云环境可以实现快速编译出所需要的.bin烧录文件。 
具体操作教程请看：http://www.saisaiwa.com/archives/1691566266606

doc 文件夹下有所需要的一些资料文件。

## 1. 计划实现功能

### V1板
https://oshwhub.com/yc_chen/vfd-317nk-qu-dong-ban

####  release - 1.0

- WIFI配网
- 联网校时
- 按键切换显示Style与内容

操作方式

- 短按按键1，开关LED
- 长按按键1两秒后松开，重置WIFI并会自动重启
- 短按按键2，亮度增加
- 短按按键3，亮度减少

####  release - 1.1 (20230812)

新增功能
- 长文字滚动显示
- 长文字循环轮播
- 添加G1段动画效果
- 重构字库逻辑
- 新增更多ASCII字库支持
- 优化稳定性

---

### V2板
https://oshwhub.com/yc_chen/v2-vfd-7-bt-317nk-wifi-shi-zhong

####  master

新增功能
- 长文字滚动显示
- 长文字循环轮播
- 添加G1段动画效果
- 重构字库逻辑
- 新增更多ASCII字库支持
- 优化稳定性
- V2新版电路专用PWM支持



首次烧录程序会让你配网，默认会打开AP热点并显示(ap-run)，手机搜索热点``VFD-``开头的没有密码直接连接，按理正常会弹出配网的界面。
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

API GUI操作方法

```c

/**
 * 清空VFD屏幕显示,循环刷新如果使用vfd_gui_set_text方法不需要使用它。
 */
void vfd_gui_clear();

/**
 * 在指定位置显示一个char字符,index从1~6
 */
void vfd_gui_set_one_text(size_t index, char oneChar);

/**
 * 显示一串文字，从0位开始。
 * 不足8位补零(清空显示，方便每次不用调用clear防止闪屏出现)
 */
u8 vfd_gui_set_text(const char* string);

/**
 * 要点亮的ICON图标，宏定义传参
 */
void vfd_gui_set_icon(u32 buf,u8 is_save_state = 1);
/**
 * 背光开关
 */
void vfd_gui_set_bck(u8 onOff);

/**
 * 设置亮度等级 1~7
 */
void vfd_gui_set_blk_level(size_t level);

/**
 * 第一位冒号，参数bool类型
 */
void vfd_gui_set_maohao1(u8 open);

/**
 * 第二位冒号，参数bool类型
 */
void vfd_gui_set_maohao2(u8 open);

/**
 * 循环滚动展示所有文字,可显示任意长字符内容
 * @param string 要展示的内容字符串
 * @param delay_ms 循环展示刷新频率单位 Ms
 * @param loop_count循环播放的次数
 **/
void vfd_gui_set_long_text(const char* string, u32 delay_ms, size_t loop_count);
/**
 * 如果调用vfd_gui_set_long_text有存在循环播放未结束的调用此方法将会强制结束
*/
void vfd_gui_cancel_long_text();

/**
 * 获取字库值，内部用
*/
u32 gui_get_font(char c);

/**
 * G1动画特效
 * @param isAuto 是否自动完成触发 1自动循环执行播放,0.外部给时钟触发(方法调用一次执行动画一帧)
*/
void vfd_gui_anno_for_g1(u8 isAuto = 1);
```