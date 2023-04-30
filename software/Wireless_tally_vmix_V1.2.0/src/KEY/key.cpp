#include "key.h"
#include "../main.h"
#include "../config.h"
#include "oneButton.h"

OneButton button(KeyPin, true);
//实例化一个OneButton对象
//里面可以传三个参数:
//pin : 按钮的pin角
//activeLow : true:按下为低电平; false : 按下为高电平;不设置时默认值为：true
//pullupActive : 如果有上拉电阻就激活上拉电阻

//按键事件回调函数
//单击 在未直播/预览时通过蓝色LED闪烁显示ID号 
void attachClick()
{
#ifdef INFODEBUG
  Serial.println("click-单击");
#endif
    Var.keyState = OneClick;
}

//双击 暂定
void attachDoubleClick()
{
#ifdef INFODEBUG
  Serial.println("doubleclick-双击");
#endif
    Var.keyState = DoubleClick;
}

//长铵开始
void attachLongPressStart()
{
#ifdef INFODEBUG
  Serial.println("longPressStart-长按开始");
#endif
}
//长按过程
void attachDuringLongPress()
{
  if (button.isLongPressed())
  {
#ifdef INFODEBUG
  Serial.println("duringLongPress-长按期间");
#endif
  }
}

//长按结束 进入AP配置模式
void attachLongPressStop()
{
#ifdef INFODEBUG
  Serial.println("longPressStop-长按结束");
#endif
    Var.keyState = LongClick;
}

//按下多次
void attachMultiClick()
{
  #ifdef INFODEBUG
  Serial.printf("getNumberClicks-总共按了：%d次。\r\n",button.getNumberClicks());
  switch(button.getNumberClicks()){
    	case 3:{Serial.printf("switch语句判断出打印3次。\r\n");break;}
    	case 4:{Serial.printf("switch语句判断出打印4次。\r\n");break;}
    	case 5:{Serial.printf("switch语句判断出打印5次。\r\n");break;}
    	case 6:{Serial.printf("switch语句判断出打印6次。\r\n");break;}
    	default:{Serial.printf("switch语句判断出打印其它次数:[%d]。\r\n",button.getNumberClicks());break;}
  }
#endif
}

void KeyInit(void) {
    button.reset();  // 清除一下按钮状态机的状态
    button.setDebounceTicks(80);  // 设置消抖时长为80毫秒,默认值为：50毫秒
    button.setClickTicks(500);  // 设置单击时长为500毫秒,默认值为：400毫秒
    button.setPressTicks(1000);  // 设置长按时长为1000毫秒,默认值为：800毫秒

    button.attachClick(attachClick);  // 初始化单击回调函数
    button.attachDoubleClick(attachDoubleClick);  // 初始化双击回调函数
    button.attachLongPressStart(attachLongPressStart);  // 初始化长按开始回调函数
    button.attachDuringLongPress(attachDuringLongPress);  // 初始化长按期间回调函数
    button.attachLongPressStop(attachLongPressStop);  // 初始化长按结束回调函数
    button.attachMultiClick(attachMultiClick);  // 初始化按了多次(3次或以上)回调函数
}

void KeyLoop(void) {
    button.tick();  // 不断检测按钮按下状态
}