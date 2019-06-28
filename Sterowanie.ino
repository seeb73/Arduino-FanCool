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

// iRda
#include <IRremote.h>
#define irPin 11
IRrecv irrecv(irPin);
decode_results results;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


// przekazniki
int przekaznik1 = 15;
bool zaCieplo = false;
int TOHOT = 24;
int cool = 20;
int laststate = 0;

/*
  Setup function. Here we do the basics
*/
void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // irda
  irrecv.enableIRIn();

  dht.begin();
  // termometr
  sensors.begin();
  
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();

  //LCD
  lcd.begin(16, 2);  // Inicjalizacja LCD 2x16
  lcd.clear();
  // przekazniki setup
  pinMode(przekaznik1, OUTPUT);
}

/*
   Main function. It will request the tempC from the sensors and display on Serial.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");

  // It responds almost immediately. Let's print out the data
  printTemperature(insideThermometer); // Use a simple function to print out the data

  // check server temperature acction
  Serial.println (lastState());
 

  if (zaCieplo == 1 )
  {
    //Serial.println(zaCieplo);
    digitalWrite(przekaznik1, HIGH);
    //Serial.println(" HIGH");
  }
  else if (zaCieplo == 1 && lastState() != 1) {
    //Serial.println(zaCieplo);
    //Serial.println(" LOW");
    digitalWrite(przekaznik1, LOW);
  }
  else if (lastState() == 0){
    lcd.setCursor(11,0);
    lcd.print (lastState());
    Serial.print("Chłodzimy dalej...");
    digitalWrite(przekaznik1, LOW);
  }

  if (irrecv.decode(&results)) {
    Serial.print("0x");
    Serial.println(results.value, HEX);
    //delay(250);
    irrecv.resume();
  }
  //lcd.print(digitalRead(przekaznik1));

}


int lastState()
{
  if (laststate == 0) {
//    digitalWrite(przekaznik1, LOW);
//    lcd.setCursor(11,0);
//    lcd.print ("LOW");
    return 0;
  } else if (laststate == 1 && cool) {
//    lcd.setCursor(11,0);
//    lcd.print ("HIGH");
//    digitalWrite(przekaznik1, HIGH);
    // else if :)
    return 1;
  }
  else {
//    lcd.setCursor(11,0);
//    lcd.print ("HIGH");
//    digitalWrite(przekaznik1, HIGH);
    return 1;
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  float wilgotnosc = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
  //Serial.print("%RH | ");
  
  lcd.setCursor(0, 0);
  lcd.print(tempC);
  lcd.print(" stC");
  
  lcd.setCursor(0, 1);
  lcd.print(wilgotnosc);
  lcd.print("% ");
  lcd.print(t);
  lcd.print(" stC");
  delay (2000);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void irdaRemote(results)
{
  switch (results.value) {
    case 0xE4B320DF:
      Serial.println("w lewo");
      break;

    case 0xE4B340BF:
      Serial.println("w gore");
      break;

    case 0xE4B3A05F:
      Serial.println("w prawo");
      break;

    case 0xE4B3C03F:
      Serial.println("w dol");
      break;

    case 0xE4B3807F:
      Serial.println("srodek");
      break;
  }
}
