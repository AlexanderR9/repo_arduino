#ifndef MY_BUTTON_STATES_H
#define MY_BUTTON_STATES_H

#include "pin_worker.h"


//класс для работы с простыми кнопками (тублерами, выключателями)
class ButtonStates
{
public:
	enum CurBState {bsPushed = 801, bsOff, bsInvalid = -1};

	ButtonStates() {reset();}
	ButtonStates(int pin) {reset(); m_pin = uint8_t(pin);}
	
	void updateState(); //проверка и обновление текущего состояния
	void setConfirmCount(int); //установка количества одинаковых состояний подряд для подтверждения изменения
	void init(); //инициализация объекта, необходимо вызвать в setup()
	void init(int); // с заданием пина
	String strState() const;
	
	//количество тиков прошедших с момента последнего изменения состояния
	//если период обновления 0.1 сек, то значение функции надо разделить на 10, 
	//чтобы получить количество секунд (столько держится текущее стостояние кнопки)
	inline int lastStateTime() const {return (m_counter-last_state_number);} 
	
	inline bool invalid() const {return (invalidPin() || confirmOff());}
	inline bool invalidPin() const {return (m_pin < 1 || m_pin > 13);}
	inline bool pushed() const {return (m_state == bsPushed);} //признак того, что в текущий момент контакт замкнут (кнопка нажата)
	inline bool eventChanged() const {return (lastStateTime() == 0);} //признак того, что состояние изменилось только что (типа произошло событие)
	inline uint8_t buttonPin() const {return m_pin;}
	
	String toStr() const; //выхлоп инфы

protected:
	uint8_t m_pin; //цифровой вход на который повешена кнопка (1..13)
	uint8_t m_confirm_count; //количество одинаковых состояний подряд для подтверждения изменения, если 0, то не реагировать на нажатие
	uint32_t m_counter; //счетчик выполнения функции updateState за все время выполнения скетча 
	uint32_t m_changedCounter; //счетчик изменения переменной m_state за все время выполнения скетча 
	int m_state; //текущее состояние кнопки
	uint32_t last_state_number; //значение счетчика m_counter на момент последнего изменения состояния

	void reset(); //сброс всех параметров
	inline bool confirmOff() const {return (m_confirm_count == 0);} //признак того, что объект не реагирует на нажатие кнопки
	void setMode(); //установка соответствующего режима для входа m_pin
	int currentPinState() const; //текущее состояние пина
	
private:
	uint8_t confirm_tick; //счетчик набора одинаковых состояний подряд, для подтверждения изменения состояния кнопки
	
};


/////////////////////////////////////////////////////////////
/////////////////////////DISCRIPTION FUNCS /////////////////
/////////////////////////////////////////////////////////////
String ButtonStates::strState() const
{
	String s = "???";
	switch (m_state)
	{
		case bsPushed: {s = "PUSHED"; break;}
		case bsOff: {s = "OFF"; break;}
		case bsInvalid: {s = "INVALID_BUTTON_STATE"; break;}
		default: break;
	}
	return s;
}
String ButtonStates::toStr() const
{
	String s = "Button info:";
	s += ("  pin="+String(m_pin));
	s += ("  state="+strState());
	s += ("  confirm_count="+String(m_confirm_count));
	s += ("  was_update_count="+String(m_counter));
	s += ("  was_changed_count="+String(m_changedCounter));
	s += ("  state_time="+String(lastStateTime()));
	return s;
}
void ButtonStates::updateState()
{
	m_counter++;
	if (invalid()) return;
	
	int pin_value = currentPinState();
	if (pin_value == m_state) //сигнал не изменился
	{
		confirm_tick = 0;
		return;
	}		
	
	confirm_tick++;
	if (confirm_tick >= m_confirm_count)
	{
		m_state = pin_value;
		m_changedCounter++;
		last_state_number = m_counter;
	}
}
void ButtonStates::init()
{
	setMode();
	m_state = currentPinState();
}
void ButtonStates::init(int pin)
{
	m_pin = uint8_t(pin);
	init();
}
int ButtonStates::currentPinState() const
{
	if (invalid()) return bsInvalid;
	if (PinWorker::isPinOff(m_pin)) return bsOff;
	return bsPushed;	
}
void ButtonStates::reset()
{
	m_pin = 0;
	m_confirm_count = 2;
	m_counter = 0;
	m_changedCounter = 0;
	m_state = bsInvalid;
	confirm_tick = 0;
	last_state_number = 0;
}
void ButtonStates::setConfirmCount(int count)
{
	m_confirm_count = uint8_t(count);
	if (m_confirm_count > 20) m_confirm_count = 2;
}
void ButtonStates::setMode()
{
	if (!invalid()) 
		PinWorker::setModeIn(m_pin);		
	
}
	



#endif



