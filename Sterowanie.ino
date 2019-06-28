// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


// dht 11
#define DHT11_PIN 2
#define DHT11_TYPE DHT11   // DHT 11
//#define DHT11_TYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHT11_PIN, DHT11_TYPE);


#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
/*
// iRda
#include <IRremote.h>
#define irPin 11
IRrecv irrecv(irPin);
decode_results results;
*/
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


// przekazniki
int przekaznik1 = 15;
bool zaCieplo = false;
int tohot = 24;
int cool = 20;
int laststate = 0;
int fanOnOFF = 0;
char st = char(223);
char st1 = (char)223;
void setup() {
  // start serial port
  Serial.begin(9600);

  // termometr
  dht.begin(); // DHT11
  sensors.begin(); //DS18B20
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();
  sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();
  
  //LCD
  lcd.clear();
  lcd.begin(16, 2);  // Inicjalizacja LCD 2x16
  
  // przekazniki setup
  pinMode(przekaznik1, OUTPUT);
}

void loop() {
    sensors.requestTemperatures(); // Send the command to get temperatures
    setState(sensors.getTempC(insideThermometer));
    printFanStatus();
    printTemperature(insideThermometer);
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  float wilgotnosc = dht.readHumidity();
  float t = dht.readTemperature();
  char ll = char(223);
  String Celcius = "C";
  String Procent = "%";
  
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit

  printLcd(0,0,tempC,Celcius,true);
  printLcd(10,1,t,Procent,false); 
  printLcd(0,1,t,Celcius,true); 
}

void printLcd(int start, int line,float data, String mySymbol, bool character)
{
  char ll = char(223);
  
  lcd.setCursor(start, line);
  lcd.print(data);
  if (character == 1 ){ 
      lcd.print(ll);
    }

  lcd.print(mySymbol);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// check temperature and set fan and laststate
void setState(float temperature)
{
  if (temperature >= tohot )
  {
    digitalWrite(przekaznik1, HIGH);
    fanOnOFF = 1;
    laststate = 1;
  }
  else if (temperature < tohot && laststate == 1 && temperature > cool )
  {
   Serial.println("Chłodzimy dalej"); 
   digitalWrite(przekaznik1, HIGH);
   laststate = 1;
   fanOnOFF = 1;
  } else 
  {
    digitalWrite(przekaznik1, LOW);
    laststate = 0;
    fanOnOFF = 0;
    }
}

int lastState()
{
  if (laststate > 1)
  {
   return 0; 
  }
  
  if (laststate == 0)
  {
    return 0;
   } else if (laststate == 1 )
   {
    return 1;
   }
}

void printFanStatus()
{
  if (fanOnOFF == 1)
  {
    lcd.setCursor(10, 0);
    lcd.print("FAN:On");
  } else 
  {
    lcd.setCursor(9, 0);
    lcd.print("FAN:Off");
  }
}
