#ifndef MY_TIMER_EVENT_H
#define MY_TIMER_EVENT_H



struct MyTimer
{
	MyTimer() :timer_interval(-1), user_counter(0), is_on(false){}
	
	int16_t timer_interval;
	uint16_t user_counter;	
	bool is_on;

	void setInterval(int16_t value)
	{
		timer_interval = value;
		user_counter = 0;
	}
	void update(uint16_t counter)
	{
		if (timer_interval <= 0) {is_on = false; return;}
		is_on = ((counter % timer_interval) == 0);	
	}
	void updateUserCounter(uint16_t counter)
	{
		if (timer_interval <= 0) return;
		if ((counter % timer_interval) > 0) return;
		user_counter++;
		if (user_counter > 65000) user_counter = 0;
	}
		
};

//класс для задания временных интервалов интервалов срабатывания каких либо функций
//подсчет ведется в десятых долях секунды, т.е. 10 единиц это 1 секунда
class MyTimerEvent
{
public:
  MyTimerEvent() {reset();}

  void reset() {space_loops = 1250; n_loops = 0; m_counter = 1; was_counter = false;}
  
  //задание интевала для указаного таймера, отрицательное значение означает выключение таймера
  //t_number = 1..4, если неверно указан t_number, то ничего не изменится
  //можно задать максимум 65000 единиц, т.е. 6500 сек. (чуть менше 2-х часов)
  void setTimerInterval(uint8_t t_number, int16_t value = -1);
  
  //возвращает состояние сработал/не сработал заданный таймер, может вернуть true только при n_loops == 0
  //необходимо вызывать каждый раз в основной программе в функции loop()
  bool timerSignal(uint8_t t_number) const;
  
  //необходимо вызывать каждый раз в основной программе в функции loop() для пересчета текущего состояния
  void nextLoop();
  
  //возвращает true каждый раз при очередной единицы времени
  bool counterNext() const;
  
  //возвращает количетсво срабатываний указанного таймера
  uint16_t userCounter(uint8_t t_number) const;
  
  
  String strState() const;
  
protected:
  MyTimer m_timer1;
  MyTimer m_timer2;
  MyTimer m_timer3;
  MyTimer m_timer4;
  
  
  uint16_t space_loops; //сколько циклов loop() пропустить после очередного тика, ориентировочно loop() выполняется раз в 4 мк.сек.
  uint16_t n_loops; //счетчик loops после выполнения тика
  uint16_t m_counter; //счетчик единиц времени
  bool was_counter; //взводтится на мгновение при очередном увеличение m_counter
  
	//обновить состояние таймеров при очередном тике
	void recalcTimerStates();
	void updateUserCounter();

};

//////////////////DESCRIPTION FUNCTIONS////////////////////////
String MyTimerEvent::strState() const
{
	String s = "counter = " + String(m_counter);
	return s;
}
void MyTimerEvent::setTimerInterval(uint8_t t_number, int16_t value)
{
	switch (t_number)
	{
		case 1: {m_timer1.setInterval(value); break;}	
		case 2: {m_timer2.setInterval(value); break;}	
		case 3: {m_timer3.setInterval(value); break;}	
		case 4: {m_timer4.setInterval(value); break;}	
		default: break;
	}
	m_counter = 0;	
}
uint16_t MyTimerEvent::userCounter(uint8_t t_number) const
{
	uint16_t z = 0;	 
	switch (t_number)
	{
		case 1: {z = m_timer1.user_counter; break;}	
		case 2: {z = m_timer2.user_counter; break;}	
		case 3: {z = m_timer3.user_counter; break;}	
		case 4: {z = m_timer4.user_counter; break;}	
		default: break;
	}	
	return z;
}
bool MyTimerEvent::timerSignal(uint8_t t_number) const
{
	if (n_loops > 0) return false;
	switch (t_number)
	{
		case 1: return m_timer1.is_on;
		case 2: return m_timer2.is_on;
		case 3: return m_timer3.is_on;
		case 4: return m_timer4.is_on;
		default: break;
	}
	return false;
}
void MyTimerEvent::recalcTimerStates()
{
	m_timer1.update(m_counter);
	m_timer2.update(m_counter);
	m_timer3.update(m_counter);
	m_timer4.update(m_counter);	
}
void MyTimerEvent::updateUserCounter()
{
	m_timer1.updateUserCounter(m_counter);
	m_timer2.updateUserCounter(m_counter);
	m_timer3.updateUserCounter(m_counter);
	m_timer4.updateUserCounter(m_counter);	
}
bool MyTimerEvent::counterNext() const
{
	if (n_loops > 0) return false;
	return was_counter;
}
void MyTimerEvent::nextLoop()
{
  if (n_loops < space_loops) {n_loops++; return;}
  //иначе тик ~ 5 ms
  
  if (millis()%100 == 0) // проверка что прошло 0.1 секунды с момента последнего пересчета
  {
      n_loops = 0;
      m_counter++;     
      if (m_counter > 65000) m_counter = 1;
	  was_counter = true;
	  updateUserCounter();
  }
  else was_counter = false;
	
  recalcTimerStates();
}

#endif



