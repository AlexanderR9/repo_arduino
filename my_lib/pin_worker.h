#ifndef MY_PIN_WORKER_H
#define MY_PIN_WORKER_H


class PinWorker
{
public:
	//установка состояния цифрового входа
  static void pinOff(int i) {digitalWrite(i, LOW);}	
  static void pinOn(int i) {digitalWrite(i, HIGH);}
  static void pinInvert(int i);
  
	//чтение состояния цифрового входа
  static bool isPinOff(int i) {uint8_t x = digitalRead(i); return (x == 0);}
  static bool isPinOn(int i) {return (!isPinOff(i));}
  static String pinState(int i) {return (isPinOff(i) ? "OFF" : "ON");}
  static String digitalPinsState();
  static String signalLevel(int i) {return (isPinOff(i) ? "LOW" : "HIGH");}


	//установка режима работы цифрового входа
  static void setModeOut(int i) {pinMode(i, OUTPUT);}
  static void setModeIn(int i) {pinMode(i, INPUT);}
  
  static bool invalidDigitalPin(int i) {return (i < 1 || i > 13);}
  
	//чтение текущего напряжения аналогового входа
  static float analogVolatge(int i, float v_base = 5);

  //отключение встроенного светодиода
  static void ownLedOff();

};

String PinWorker::digitalPinsState()
{
	String s = "\n ----------- DIGITAL PINS STATE ---------------- \n";
	for (int pin=1; pin<=13; pin++)
	{
		s += (("pin ") + String(pin));	
		s += (",   signal "+signalLevel(pin));
		s += "\n";
	}
	return s;
}
float PinWorker::analogVolatge(int i, float v_base)
{
	uint16_t v = analogRead(i);
	return float(v)*v_base/1024;
}
void PinWorker::pinInvert(int i)
{
	if (isPinOff(i)) pinOn(i);
	else pinOff(i);
}
void PinWorker::ownLedOff()
{
  int r_pin = LED_BUILTIN;
  PinWorker::setModeOut(r_pin);
  PinWorker::pinOff(r_pin);
}



#endif

