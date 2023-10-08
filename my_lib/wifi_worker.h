#ifndef MY_WIFI_WORKER_H
#define MY_WIFI_WORKER_H


#include <ESP8266WiFi.h>
 
/*  ESP8266WiFi enum of connection status

0	WL_IDLE_STATUS	temporary status assigned when WiFi.begin() is called
1	WL_NO_SSID_AVAIL	 when no SSID are available
2	WL_SCAN_COMPLETED	scan networks is completed
3	WL_CONNECTED	when connected to a WiFi network
4	WL_CONNECT_FAILED	when the connection fails for all the attempts
5	WL_CONNECTION_LOST	when the connection is lost
6	WL_DISCONNECTED	when disconnected from a network

*/

/*   ESP8266WiFi enum of sleep mode
typedef enum WiFiSleepType
{
    WIFI_NONE_SLEEP = 0, WIFI_LIGHT_SLEEP = 1, WIFI_MODEM_SLEEP = 2
} WiFiSleepType_t;
*/

//класс для работы модулем ESP8266WiFi
class  MyWiFiWorker
{
public:	
	
	MyWiFiWorker() {reset();}
	MyWiFiWorker(bool b) {reset(); enableSerialDebug(b);}
	virtual ~MyWiFiWorker() {}
  
	inline bool isConnected() const {return (WiFi.status() == WL_CONNECTED);}
	inline bool isDisconnected()  const {return (WiFi.status() == WL_DISCONNECTED);}
	inline bool isIdle()  const {return (WiFi.status() == WL_IDLE_STATUS);}
	inline bool activated() const {return (WiFi.getMode() == WIFI_STA);}
	inline bool disabled() const {return (WiFi.getMode() == WIFI_OFF);}
	inline void enableSerialDebug(bool b) {use_debug = b;}
	inline String connParams() const {return (String("WI-FI network:  "+wifi_name+"    WI-FI password:  "+wifi_passwd));}
	inline bool staticIP_ON() const {return (static_ip[0] > 0);}
	 
	void goSleep(uint32); //secs 
	
	void setConnParams(String name, String pswd) {wifi_name = name; wifi_passwd = pswd;} // установить параметры подключения к  WiFi сети 
	void setStaticIP(uint8_t, uint8_t, uint8_t, uint8_t);
	void tryConnect(); //подключится к WiFi сети, после выполнения метода статус станет WL_DISCONNECTED на некоторое время, далее либо WL_CONNECTED либо ошибка
	void disconnect(); //разорвать соединение, после выполнения метода статус станет WL_IDLE_STATUS если до этого было установлено соединение, далее модуль будет снова пытаться подключится
	void turnOff(); //установить режим модуля ВЫКЛЮЧЕНО (WIFI_OFF), после выполнения метода статус станет WL_DISCONNECTED
	void turnOn(); //установить режим модуля ВКЛЮЧЕНО (WIFI_STA), после выполнения метода статус станет WL_IDLE_STATUS
	String strCommonState() const; //получить общее состояние соединения
	String currentIP() const; 
	String strModeState() const;
	String strConnectionState() const;
	
	static IPAddress googleDNS() {return IPAddress(8, 8, 8, 8);}
	
protected:
	String wifi_name;
	String wifi_passwd;
	bool use_debug; //if serial out
	int16_t static_ip[4];
	
	
	void reset();
	void config();
	void debug(String s, bool err = false) {if (use_debug) {if (err) Serial.print("WARNING: ");  Serial.println(s);}}
	
};

/////////////////////// MAKE METODS ///////////////////////////
void MyWiFiWorker::reset() 
{
	wifi_name = wifi_passwd = ""; 
	use_debug = false;
	for (int i=0; i<4; i++) static_ip[i] = -1;
}
void MyWiFiWorker::goSleep(uint32 secs)
{
	debug("Go sleep ESP module with "+String(secs)+" seconds.");
	//ESP.deepSleep(secs*1000);
	
	//WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
	//WiFi.forceSleepBegin(secs*1000000);
	
	//need to do this func
}
void MyWiFiWorker::setStaticIP(uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	static_ip[0] = x1;
	static_ip[1] = x2;
	static_ip[2] = x3;
	static_ip[3] = x4;
}
void MyWiFiWorker::config()
{
	if (staticIP_ON()) 
	{
		debug("make config WiFi");
		IPAddress ip(static_ip[0], static_ip[1], static_ip[2], static_ip[3]);
		IPAddress dns(MyWiFiWorker::googleDNS());
		IPAddress gateway(ip);   //IP Address of your WiFi Router (Gateway)
		gateway[3] = 1;
		IPAddress mask(255, 255, 255, 0);  //Subnet mask
		
		debug("IP = "+ip.toString());
		debug("GATEWAY = "+gateway.toString());
		debug("MASK = "+mask.toString());
		debug("DNS = "+dns.toString());
		WiFi.config(ip, gateway, mask, dns);
	}	
}
void MyWiFiWorker::disconnect()
{
	debug("Try disconnect from WiFi ....");
	if (disabled()) {debug("ESP module is disabled!", true);  return;}
	if (isDisconnected()) {debug("ESP module already disconnected!", true);  return;}
	WiFi.disconnect();		
	debug("done!");
}
void MyWiFiWorker::tryConnect()
{
	debug("Try connect to WiFi ....");
	if (disabled()) {debug("ESP module is disabled!", true);  return;}
	if (isConnected()) {debug("ESP module already connected!", true);  return;}
	if (wifi_name.length() < 3) {debug("WiFi network name is invalid", true); debug(connParams());  return;}
	if (wifi_passwd.length() < 3) {debug("WiFi network password is invalid", true); debug(connParams());  return;}
	
	config();
	WiFi.begin(wifi_name, wifi_passwd); //connect to WiFi	
	debug("done!");
}
void MyWiFiWorker::turnOff()
{
	debug("Set turn OFF ESP module.");
	if (disabled()) return;
	if (isConnected()) disconnect();
	WiFi.mode(WIFI_OFF);	
}
void MyWiFiWorker::turnOn()
{
	debug("Set turn ON ESP module.");
	if (activated()) return;
	WiFi.mode(WIFI_STA);	
}
String MyWiFiWorker::currentIP() const
{
	if (isConnected()) return WiFi.localIP().toString();
	return "---";	
}
String MyWiFiWorker::strModeState() const
{
	if (disabled()) return "ESP DISABLED";
	if (activated()) return "ESP ACTIVATED";
	return "unknown mode";
}
String MyWiFiWorker::strConnectionState() const
{
	if (isConnected()) return "CONNECTED";
	if (isDisconnected()) return "DISCONNECTED";
	if (isIdle()) return "IDLE";
	
	switch (WiFi.status())
	{
		case WL_NO_SSID_AVAIL: return "NOT FOUND WIFI NETWORK";
		case WL_CONNECT_FAILED: return "CONNECTION_FAILED";
		case WL_CONNECTION_LOST: return "CONNECTION_LOST";
		default: break;
	}
	
	return (String("UNKNOWN STATUS (") + String(WiFi.status()) + String(")"));	
}
String MyWiFiWorker::strCommonState() const
{
	String s = "";
	s += '\n';
	s += " --------- WI-FI OBJ STATE: -------------"; s += '\n';				
	s += "mode: "+strModeState(); s += '\n';
	s += "connection status: "+strConnectionState(); s += '\n';
	s += "IP: "+ currentIP(); s += '\n';	
	
	String ip_t = String(staticIP_ON()?"STATIC":"DHCP");
	s += "IP type: "+ ip_t; s += '\n';	
	return s;
}


#endif



