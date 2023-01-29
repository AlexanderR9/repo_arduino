#ifndef MY_RTC_STRUCT_H
#define MY_RTC_STRUCT_H


#include <iarduino_RTC.h>


//структура для хранения времени и даты
struct RTC_struct
{
  RTC_struct() {reset();}
  
  uint8_t m_day;
  uint8_t m_month;
  uint8_t m_year;
  uint8_t m_hour;
  uint8_t m_min;
  uint8_t m_sec;
  uint8_t week_day;

  void reset() {m_day = m_month = m_year = m_hour = m_min = m_sec = 0; week_day = 0;}
  String getDate() const
  {
    String s = String(m_day) + "."  + String(m_month) + "." + String(m_year);
    return s;
  }
  String getTime() const
  {
    String s = String(m_hour) + ":"  + String(m_min) + ":" + String(m_sec);
    return s;
  }
  String toStr(bool wd = false) const
  {
    String s1 = "Date: " + getDate();
    String s2 = "Time: " + getTime();
    String s3 = "WeekDay: " + String(week_day);
    String s = s1 + "    " + s2;
    if (wd) s += ("   " + s3); 
    return s;
  }
  void setDT(uint8_t dd, uint8_t dm,uint8_t dy, uint8_t th, uint8_t tm, uint8_t ts, uint8_t wd)
  {
    m_day = dd;
    m_month = dm;
    m_year = dy;
    m_hour = th;
    m_min = tm;
    m_sec = ts;
    week_day = wd;  
  }
};

//класс для работы модулем реального времени DS3231
class MyRTC
{
public:
	MyRTC();
	virtual ~MyRTC() {if (rtc_obj) delete rtc_obj;}
	
	inline void begin() {if (rtc_obj) rtc_obj->begin();}
	inline String toStr(bool with_dw) const {return m_dt.toStr(with_dw);}
	inline const RTC_struct& dt() const {return m_dt;}
	
	void setDate(uint8_t d, uint8_t m, uint8_t y, uint8_t dw = 99);
	void setTime(uint8_t h, uint8_t m, uint8_t s = 0);
	void setDateTime(uint8_t d, uint8_t month, uint8_t y, uint8_t h, uint8_t min, uint8_t s, uint8_t dw = 99);
	void readCurrentDT();
	void printDT();
	
protected:
	iarduino_RTC *rtc_obj;
	RTC_struct m_dt;
	
	
	
};
///////////////////////////////////////////
MyRTC::MyRTC()
	:rtc_obj(NULL)
{
	
	//iarduino_RTC rtc_obj(RTC_DS3231); //инициализация объекта RTC модуля
	rtc_obj = new iarduino_RTC(RTC_DS3231);
	
}
void MyRTC::printDT()
{
	readCurrentDT();
	Serial.println("Curent datetime: " + toStr(true));
}
void MyRTC::readCurrentDT()
{
  if (rtc_obj) rtc_obj->gettime();  
  m_dt.m_day = rtc_obj->day;
  m_dt.m_month = rtc_obj->month;
  m_dt.m_year = rtc_obj->year;
  m_dt.m_hour = rtc_obj->Hours;
  m_dt.m_min = rtc_obj->minutes;
  m_dt.m_sec = rtc_obj->seconds;
  m_dt.week_day = rtc_obj->weekday;
}
void MyRTC::setDate(uint8_t d, uint8_t m, uint8_t y, uint8_t dw)
{
	if (!rtc_obj) return;
	
	readCurrentDT();
	if (dw > 6) dw = m_dt.week_day;
	rtc_obj->settime(m_dt.m_sec, m_dt.m_min, m_dt.m_hour, d, m, y, dw); 
}
void MyRTC::setTime(uint8_t h, uint8_t m, uint8_t s)
{
	if (!rtc_obj) return;
	
	readCurrentDT();
	rtc_obj->settime(s, m, h, m_dt.m_day, m_dt.m_month, m_dt.m_year, m_dt.week_day); 
}
void MyRTC::setDateTime(uint8_t d, uint8_t month, uint8_t y, uint8_t h, uint8_t min, uint8_t s, uint8_t dw)
{
	if (!rtc_obj) return;
	
	if (dw > 6) dw = 0;
	rtc_obj->settime(s, min, h, d, month, y, dw); 
}



#endif



