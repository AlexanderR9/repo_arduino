
#include <my_lib.h>

#define TIMER_INTERVAL1    15
#define TIMER_INTERVAL2    50
#define TIMER_INTERVAL3    0
#define TIMER_INTERVAL4    0

//VARS
MyTimerEvent m_timers;


////////////SETUP///////////////////
void setup() 
{
  PinWorker::ownLedOff(); //отключение встроенного светодиода
  Serial.begin(9600); //инициализация com порта для выхлопа

  //init timers
  m_timers.setTimerInterval(1, TIMER_INTERVAL1); //для обновления состояния кнопок
  m_timers.setTimerInterval(2, TIMER_INTERVAL2); //шаг набора/падения скорости
}
void loop() 
{
    m_timers.nextLoop(); //обновление таймеров
    if (m_timers.timerSignal(1)) timerEvent1();  
    if (m_timers.timerSignal(2)) timerEvent2();    
}

void timerEvent1() //срабатывание таймера 1
{
  Serial.println("timerEvent1() exec"); 
}
void timerEvent2() //срабатывание таймера 2
{
  Serial.println("timerEvent2() exec"); 
}
