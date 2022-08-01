#include <OneWire.h> 
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define Buttons A0
#define Relay 3
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS); 

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;

int deviceCount = 0;

// Enable init screen
bool initScreen = true;

// Default values of current and target temperatures
signed int currentTemperature = -10;
signed int targetTemperature = -15;
// Temperature trashold from target. Default: +/-1 degree by Celsius.
int temperatureTrashold = 1;


float resistor1 = 220;
float resistor2 = 900;

int Vbutton = 5;
float Vmax = (5 * resistor1 + 5 * resistor2) / resistor2;

const char *ErrorMessages[4] = {
  "Unknown error   ",
  "TS device error ",
  "TS address error",
  "TS disconnected "
};

void setup() {

  lcd.begin(16, 2);
  
  if (initScreen) printInitScreen();
  
  initPins();

  initTSDevice();

  delay(1000);
}

void loop() {
  lcd.clear();
  sensors.requestTemperatures();
  
  updateTargetTemperature();
  printTargetTemperature();

  getTemperature(insideThermometer);
  printCurrentTemperature();


  delay(200);
}

void initPins() {
  pinMode(Buttons, INPUT);
  pinMode(Relay, OUTPUT);
}

void initTSDevice() {
  sensors.begin(); 
  printDeviceCount();
  printDeviceState();
  getDeviceAddress();
  printDeviceAddress(insideThermometer);
  sensors.setResolution(insideThermometer, 9);
}

void readButtons() {
  Vbutton = (signed int)analogRead(Buttons) * Vmax / 1024;
}

void updateTargetTemperature() {
  readButtons();

  switch (Vbutton) {
    case 3:
      --targetTemperature;
      lcd.setCursor(0, 1);
      lcd.print("-");
      break;
    case 2:
      ++targetTemperature;
      lcd.setCursor(0, 1);
      lcd.print("+");
      break;
  }
}

void getDeviceAddress(){
  // Error 2: Device adress was not found
  if (!sensors.getAddress(insideThermometer, 0)) {printError(2);}

}

void getTemperature(DeviceAddress deviceAddress)
{
  currentTemperature = sensors.getTempC(deviceAddress);
  if (currentTemperature == DEVICE_DISCONNECTED_C)
  {
    printError(3);
    return;
  }
}

void printInitScreen() {
  lcd.setCursor(0, 0);
  lcd.print("     Fridge     ");
  lcd.setCursor(0, 1);
  lcd.print("   Controller   ");
  delay(2000);
  lcd.clear();  // Clears the display

  lcd.setCursor(0, 0);
  lcd.print("Sergey Bezugliy");
  lcd.setCursor(3, 1);
  lcd.print("codenv.top");
  delay(3000);
  lcd.clear();  // Clears the display
};

void printParameter(char *parameterAbbreviation, int startCol, int Row, int *valueRef) {
  lcd.setCursor(startCol, Row);
  lcd.print(parameterAbbreviation);
  lcd.setCursor(strlen(parameterAbbreviation) + startCol, Row);
  lcd.print(*valueRef);
}

void printCurrentTemperature() {
  // Error 1: Device doesn't work.
  if (currentTemperature == -127) printError(1);
  else printParameter("TC:", 1, 0, &currentTemperature);
}

void printTargetTemperature() {
  printParameter("TT:", 1, 1, &targetTemperature);
}

void printDeviceAddress(DeviceAddress deviceAddress)
{
  if (initScreen) {
    for (uint8_t i = 0; i < 8; i++)
    {
      if (deviceAddress[i] < 16) printParameter("DA:", 9, 0, 0);
      lcd.print(deviceAddress[i]);
    }
  }
}

void printDeviceCount() {
  deviceCount = sensors.getDeviceCount();
  if (initScreen) printParameter("SC:", 1, 0, &deviceCount);
}

void printDeviceState() {
  int ppmState = 0;
  if (sensors.isParasitePowerMode()) ppmState = 1;
  // Parasite power mode off
  if (initScreen) printParameter("PPM:", 1, 0, &ppmState);
}

void printError(int code){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error ");
  lcd.print(code);
  lcd.setCursor(0, 1);
  lcd.print(ErrorMessages[code]);
  delay(10000);
}