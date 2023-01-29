#ifndef MY_JOYSTICK_H
#define MY_JOYSTICK_H

#include <Arduino.h>
#include "pin_worker.h"

struct PinState
{
	PinState() {reset();}
	PinState(int a, int b) :key(a), pin(b), state(0) {}
	
	int key;
	int pin;
	uint8_t state; //on-off
	
	void reset() {key = -1; pin = -1; state = 0;}
	PinState operator = (const PinState &ps)
	{
		key = ps.key;
		pin = ps.pin;
		state = ps.state;
	}
	
};

//AbstractJoystick
class AbstractJoystick
{
public:
	enum JState {jOff = 880, jUp, jDown, jLeft, jRight, jMidle, jRST, jSet};
	
	AbstractJoystick() {reset();};
	
	virtual void setPin(int enum_type, int pin) = 0; //назначение пина для конкретного направления, можно задать отрицательный пин для отключения типа направления
	virtual void update() = 0; //обновления текущего состояния (постоянно выполнять в timerEvent)
	virtual bool containsState(int) const; //определяет, присутствует ли в контейнере элемент для указанного состояния 
	//virtual bool containsPin(int) const; //определяет, присутствует ли в контейнере элемент для указанного пина 
	virtual int indexOfState(int) const; ////определяет индекс элемента в контейнере для указанного состояния 
	virtual void init() = 0; //назначение входов поумолчанию
	virtual bool isOff() const {return (m_state == jOff);} //никуда не едем
	
	
	static String stateByValue(int);
	String toStr() const;
	String pinsInfo() const;
	
protected:
	int m_state; //текущее состояние джостика
	uint8_t m_count; //количество элементов m_pins

	virtual void reset();

	// must be last member
	PinState *m_pins; //набор информации соответствий для назнначенных пинов
};

//DiscreteJoystick
class DiscreteJoystick : public AbstractJoystick
{
public:
	DiscreteJoystick() :AbstractJoystick() {};
	
	void setPin(int enum_type, int pin);
	void init();
	void update();
	
	//current forward
	inline bool isUp() const {return (m_state == jUp);} 
	inline bool isDown() const {return (m_state == jDown);} 
	inline bool isLeft() const {return (m_state == jLeft);} 
	inline bool isRight() const {return (m_state == jRight);} 
};

//AnalogJoystick
class AnalogJoystick : public AbstractJoystick
{
public:
	AnalogJoystick() :AbstractJoystick() {};
};






/////////////////class AbstractJoystick///////////////////////////
void AbstractJoystick::reset()
{
	m_pins = NULL;
	m_state = jOff;
	m_count = 0;

}
String AbstractJoystick::toStr() const
{
	String s = stateByValue(m_state);
	s += (",  set pins: "+String(m_count));
	return s;
	
}
String AbstractJoystick::pinsInfo() const
{
	String s = ("\n Pins info:  \n");
	for (int i=0; i<m_count; i++)
		s += ("pin "+String(m_pins[i].pin)+"  -->  "+stateByValue(m_pins[i].key)+"\n");
	return s;
}
bool AbstractJoystick::containsState(int j_state) const
{
	return (indexOfState(j_state) >= 0);
}
int AbstractJoystick::indexOfState(int j_state) const
{
	if (m_count == 0) return -1;
	for (int i=0; i<m_count; i++)
		if (m_pins[i].key == j_state) return i;
	return -1;		
}
String AbstractJoystick::stateByValue(int j_state)
{
	String s = "Joystick state: ";
	switch (j_state)
	{
		case jOff: {s += "OFF"; break;}
		case jLeft: {s += "LEFT"; break;}
		case jRight: {s += "RIGHT"; break;}
		case jUp: {s += "UP"; break;}
		case jDown: {s += "DOWN"; break;}
		case jMidle: {s += "MIDLE"; break;}
		case jRST: {s += "RST"; break;}
		case jSet: {s += "SET"; break;}
		default: {s += "???"; break;}
	}
	return s;
}


/////////////////class DiscreteJoystick///////////////////////////
void DiscreteJoystick::setPin(int j_state, int pin)
{
	int index = indexOfState(j_state);
	if (index < 0)
	{
		PinState *arr = new PinState[m_count+1];
		for (int i=0; i<m_count; i++) arr[i] = m_pins[i];
		arr[m_count] = PinState(j_state, pin);
		
		delete[] m_pins;
		m_pins = arr;
		m_count++;
		
		if (pin > 0) pinMode(pin, INPUT_PULLUP);
	}		
	else if (m_pins[index].pin != pin) 
	{
		if (m_pins[index].pin > 0) pinMode(pin, INPUT);
		if (pin > 0) pinMode(pin, INPUT_PULLUP);
		m_pins[index].pin = pin;
	}
}
void DiscreteJoystick::init()
{
	setPin(jUp, 3);		
	setPin(jDown, 4);		
	setPin(jLeft, 5);		
	setPin(jRight, 6);		
}
void DiscreteJoystick::update()
{
	m_state = jOff;
	if (m_count == 0) return;
	
	for (int i=0; i<m_count; i++)
	{
		if (m_pins[i].pin < 0) continue;
		if (PinWorker::isPinOff(m_pins[i].pin))
		{
			m_pins[i].state = 1;
			m_state = m_pins[i].key;
		}
		else m_pins[i].state = 0; 		
	}
}





#endif



