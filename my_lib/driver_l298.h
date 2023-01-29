#ifndef MY_DRIVER_L298_H
#define MY_DRIVER_L298_H

#include <Arduino.h>
#include "pin_worker.h"


//motor state struct
struct L298_MotorState
{
	L298_MotorState() {reset();}
	
	int pin1; //цифровой пин ардуины, -1 значит не подключен
	int pin2;
	bool state1; //on-off
	bool state2; //on-off
	
	void reset() {pin1 = pin2 = -1; state1 = state2 = false;}	
	bool isOff() const {return (!state1 && !state2);}
	void setPins(int p1, int p2)
	{
		pin1 = (PinWorker::invalidDigitalPin(p1)) ? -1 : p1;
		pin2 = (PinWorker::invalidDigitalPin(p2)) ? -1 : p2;
		if (pin1 > 0) PinWorker::setModeOut(pin1);
		if (pin2 > 0) PinWorker::setModeOut(pin2);
	}
	void setOff()
	{
		if (pin1 > 0){PinWorker::pinOff(pin1); state1 = false;}
		if (pin2 > 0){PinWorker::pinOff(pin2); state2 = false;}
	}
	void setOn(int input)
	{
		if (input == 1)
		{
			if (pin2 > 0){PinWorker::pinOff(pin2); state2 = false;}
			if (pin1 > 0){PinWorker::pinOn(pin1); state1 = true;}
		}
		if (input == 2)
		{
			if (pin1 > 0){PinWorker::pinOff(pin1); state1 = false;}
			if (pin2 > 0){PinWorker::pinOn(pin2); state2 = true;}
		}
	}
	String pinsInfo() const {return ("pin1="+String(pin1)+"  pin2="+String(pin2));}
	
};

//DRIVER_L298
class DRIVER_L298
{
public:
	DRIVER_L298() {reset();};
	
	void setMotorInputs(int motor_number, int pin1, int pin2); //назначение пинов(2-13) для указанного мотора(1 или 2)
	void init(); //назначение входов поумолчанию: 9-12 пины
	
	void motorOff(int motor_number); //выключить мотор
	void motorOn(int motor_number, int input_number); //включить мотор, input_number 1..2
	
	//проверка состояния мотора
	//cостояние: 0-выключен, 1-включен 1-й вход, 2-включен 2-й вход
	//  -1: неверно указан номер мотор
	int motorState(int) const;
	
	bool isOff() const;
	bool isOff(int) const;	
	String currentState() const;
	String pinsInfo() const;
	
protected:
	L298_MotorState m_motor1;
	L298_MotorState m_motor2;

	void reset() {}
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int DRIVER_L298::motorState(int motor_number) const
{
	if (motor_number == 1) 
	{
		if (m_motor1.state1) return 1;
		if (m_motor1.state2) return 2;
		return 0;
	}
	if (motor_number == 2) 
	{
		if (m_motor2.state1) return 1;
		if (m_motor2.state2) return 2;
		return 0;
	}
	return -1;
}
void DRIVER_L298::init()
{
	setMotorInputs(1, 9, 10);
	setMotorInputs(2, 11, 12);
}
void DRIVER_L298::setMotorInputs(int motor_number, int pin1, int pin2)
{
	if (motor_number == 1) m_motor1.setPins(pin1, pin2);
	if (motor_number == 2) m_motor2.setPins(pin1, pin2);
}
bool DRIVER_L298::isOff() const
{
	return (m_motor1.isOff() && m_motor1.isOff());
}
bool DRIVER_L298::isOff(int motor_number) const
{
	if (motor_number == 1) return m_motor1.isOff();
	if (motor_number == 2) return m_motor2.isOff();
	return true;
}
void DRIVER_L298::motorOff(int motor_number)
{
	if (motor_number == 1) return m_motor1.setOff();
	if (motor_number == 2) return m_motor2.setOff();
}
void DRIVER_L298::motorOn(int motor_number, int input)
{
	if (motor_number == 1) return m_motor1.setOn(input);
	if (motor_number == 2) return m_motor2.setOn(input);
}
String DRIVER_L298::currentState() const
{
	if (isOff()) return "ALL OFF";
	
	String s = "MOTOR_1:";
	if (m_motor1.state1) s += "  input1 ON";
	else if (m_motor1.state2) s += "  input2 ON";
	else s += " OFF";	
	s += ",    MOTOR_2:";
	if (m_motor2.state1) s += "  input1 ON";
	else if (m_motor2.state2) s += "  input2 ON";
	else s += " OFF";

	return s;
}
String DRIVER_L298::pinsInfo() const
{
	String s = "Inputs connection: MOTOR_1: "+m_motor1.pinsInfo();
	s += (",    MOTOR_2: "+m_motor2.pinsInfo());
	return s;	
}



#endif



