#include <CytronWiFiShield.h>
#include <CytronWiFiClient.h>
#include <CytronWiFiServer.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>

//initialize data
LiquidCrystal_I2C lcd(0x27,20,4); 
const char *ssid = "Infinity";
const char *pass = "rinvo2021";
ESP8266Server server(80);
ESP8266Client client;
unsigned long weatherStationChannelNumber = 2203309;

//initialize values
char USDebt_buffer[17];
int count = 0;
bool dollar = false;
String temp;
String tempC;
String humidity;
String wind;
String sunset;
String highest;
const char htmlHeader[] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n\r\n"
                        "<!DOCTYPE HTML>\r\n"
                        "<html>\r\n";
                          
void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(9600);

  if(!wifi.begin(4, 5))
  {
    Serial.println(F("Error talking to shield"));
    while(1);
  }
  Serial.println(wifi.firmwareVersion());
  Serial.print(F("Mode: "));Serial.println(wifi.getMode());// 1- station mode, 2- softap mode, 3- both
  Serial.println(F("Start wifi connection"));
  if(!wifi.connectAP(ssid, pass))
  {
    Serial.println(F("Error connecting to WiFi"));
    while(1);
  } 
  Serial.print(F("Connected to "));Serial.println(wifi.SSID());
  server.begin();
  ThingSpeak.begin(client);
  temp = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,1));
  humidity = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,2));
  wind = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,3));
  tempC = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,4));
  sunset = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,5));
  highest = parseString(ThingSpeak.readStringField(weatherStationChannelNumber,6));
  lcd.init();                      // initialize the lcd 
  lcd.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  //serverTest();
  // Print a message to the LCD.
  lcd.backlight();

  //first print -hello world
  lcd.setCursor(1,0);
  lcd.print("Good morning");
  lcd.setCursor(2,1);
  lcd.print("San Diego!");
  delay(5000);
  lcd.clear();

  //second print - temperature
  lcd.setCursor(1, 0);
  lcd.print("Temperature:");
  lcd.setCursor(1,1);
  lcd.print(temp);
  lcd.print(F(" *F / "));
  lcd.setCursor(9,1);
  lcd.print(tempC);
  lcd.print(F(" *C"));
  delay(5000);
  lcd.clear();

  //second print - humidity
  lcd.setCursor(2, 0);
  lcd.print("Humidity:");
  lcd.setCursor(5,1);
  lcd.print(humidity);
  lcd.print(F(" % ")); 
  delay(5000);
  lcd.clear();

  //third print - wind
  lcd.setCursor(2, 0);
  lcd.print("Wind Speed:");
  lcd.setCursor(5,1);
  lcd.print(wind);
  lcd.print(F(" MPH")); 
  delay(5000);
  lcd.clear();

  //fourth print - sunset 
  lcd.setCursor(2, 0);
  lcd.print("Sunset At:");
  lcd.setCursor(5,1);
  lcd.print(sunset);
  lcd.print(F(" pm")); 
  delay(5000);
  lcd.clear();

//sixth print - highest 
  lcd.setCursor(2, 0);
  lcd.print("Highest At:");
  lcd.setCursor(2,1);
  lcd.print(highest);
  lcd.print(F(" degree F")); 
  delay(5000);
  
 //fifth print - us debt
  clientUSDebt(USDebt_buffer);
  lcd.setCursor(0,0);
  lcd.print("Current US Debt:");
  lcd.setCursor(0,1);
  lcd.print(USDebt_buffer);
  delay(5000);
  lcd.clear();

  //last print -have a good day
  lcd.setCursor(0,0);
  lcd.print("Have a great day");
  lcd.setCursor(6,1);
  lcd.print("^_^");
  delay(5000);
  lcd.clear();
}
/*
void serverTest()
{
  ESP8266Client client = server.available();
  if(client.available()>0)
  {
    String req = client.readStringUntil('\r'); 
    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
      Serial.print(F("Invalid request: "));
      Serial.println(req);
      return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print(F("Request: "));
    Serial.println(req);
    client.flush();
    
    if(req.equals("/"))
    {
      IPAddress ip = wifi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      client.print(htmlHeader);
      String htmlBody = "Hello from ESP8266 at ";
      htmlBody += ipStr;
      htmlBody += "</html>\r\n\r\n";
      client.print(htmlBody);
    }
    
    else if(req.equals("/analog"))
    {
      client.print(htmlHeader);
      String htmlBody="";
      for (int a = 0; a < 6; a++)
      {
        htmlBody += "A";
        htmlBody += String(a);
        htmlBody += ": ";
        htmlBody += String(analogRead(a));
        htmlBody += "<br>\r\n";
      }
      htmlBody += "\r\n</html>\r\n";
      client.print(htmlBody);
    }
    
    else if(req.equals("/info"))
    {
      String toSend = wifi.firmwareVersion();
      toSend.replace("\r\n","<br>");
      client.print(htmlHeader);
      client.print(toSend);
      client.print("</html>\r\n");
    }

    else
      client.print("HTTP/1.1 404 Not Found\r\n\r\n");
    client.stop();
  }
}
*/
void clientUSDebt(char USDebt_buffer[17])
{
  const char destServer[] = "164.92.67.221";
  ESP8266Client client;
  if (!client.connect(destServer, 80))
  {
    Serial.println(F("Failed to connect to server."));
    client.stop();
    return;
  }
  
  const char *httpRequest = "GET /api/national_debt HTTP/1.1\r\n"
                           "Host: 164.92.67.221\r\n"
                           "Connection: close\r\n\r\n";
  if(!client.print(httpRequest))
  {
    Serial.println(F("Sending failed"));
    client.stop();
    return;
  }

  // set timeout approximately 5s for server reply
  int i=5000;
  while (client.available()<=0&&i--)
  {
    delay(1);
    if(i==1) {
      Serial.println(F("Timeout"));
      return;
      }
  }
  while (client.available()>0)
  {
    char c = (char)client.read();
    if(c == '$'){
      dollar = true;
      USDebt_buffer[count] = c;
      count++;
    }
    if(dollar == true){
      if(c == '\n'){return;}
      if(isdigit(c)){
        USDebt_buffer[count] = c;
        count++;
      }
    }
  }
  client.stop();
}

//workaround for Cytron WiFi Shield leftover content when client closes
String parseString(String str)
{
  int index = str.indexOf(",CLOSED");
  if(index==-1) return str;
  return;
}