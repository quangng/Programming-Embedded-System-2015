/*
  Abo Akademi University 
  Programming Embedded Systems 2015
  Final project:  
  Authors:  Vu Nguyen <quangngmetro@gmail.com>
                  
*/

//Project headers to include user libraries and system libraries
#include <TSL2561.h>
#include <DHT11.h>
#include <Wire.h>
#include <LiquidCrystal.h>


#define DEBUG 0
#define SERIAL_BAUD_RATE 9600
#define DHT_PIN 4
#define DEBOUNCE_TIME 100	//100 milliseconds
#define LIGHT_ON_TIME	60000	//1 minute
#define TEMP_ON_TIME 180000	//3 minutes
#define HUM_ON_TIME 180000	//3 minutes


#define TEMP_LOWER_LIMIT	20
#define TEMP_UPPER_LIMIT	30
#define HUM_LOWER_LIMIT	30
#define HUM_UPPER_LIMIT	40
#define LIGHT_LOWER_LIMIT 445
#define LIGHT_UPPER_LIMIT 455
#define IDEAL_OFFICE_LUX 450

#define LIGHT_ON true
#define LIGHT_OFF false

#define LCD_RS 10
#define LCD_ENABLE 9
#define LCD_D4 8
#define LCD_D5 7
#define LCD_D6 6
#define LCD_D7 5
#define LCD_ROW 2
#define LCD_COLUMN 16

//user defined data types
typedef enum {MODE1, MODE2, MODE3, MODE4, MODE5} Tmode;

//global variables
Tmode currentMode = MODE1;
Tmode nextMode = MODE2;

bool lightState = LIGHT_ON;
uint16_t lightSelection = IDEAL_OFFICE_LUX;                //from 445 to 455 lux
uint8_t humiditySelection = HUM_LOWER_LIMIT; 		//from 30 to 40%
uint8_t temperatureSelection = TEMP_LOWER_LIMIT; 	//from 20 to 30 *C

uint8_t humidity = 0;
int8_t temperature = 0;
uint32_t light = 0;

const uint8_t humidifierPin = 13; 	//green led
const uint8_t lightPin = 12; 		//white led
const uint8_t heaterPin = 11;		//red led
volatile uint32_t lastDebounceTime = 0;
volatile uint32_t lastLightOnTime = 0;
volatile uint32_t lastTempOnTime = 0;
volatile uint32_t lastHumOnTime = 0;

//object declarations
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
TSL2561 tsl(TSL2561_ADDR_LOW);
DHT11 dht11(DHT_PIN);

//function prototypes
uint32_t readLight(void);
void isrMode();
void isrTempHumLight();
void isrLeftButtonHandler();
void isrRightButtonHandler();


static void displayMode1();
static void displayMode2();
static void displayMode3();
static void displayMode4();
static void displayMode5();

//initialization function
void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);

  dht11.begin();

  lcd.begin(LCD_COLUMN, LCD_ROW);

  if (!tsl.begin()) {
    #ifdef DEBUF
    Serial.println("TSL2561 not found!");
    #endif
    while (1);
  }
  tsl.setGain(TSL2561_GAIN_0X);         			  // set no gain (for bright situtations)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)

  attachInterrupt(1, isrRightButtonHandler, RISING);  //attached to Arduino pin 3, right-hand side button
  attachInterrupt(0, isrLeftButtonHandler, RISING);  //attached to Arduino pin 2, left-hand side button

  pinMode(lightPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(humidifierPin, OUTPUT);
}

//forever loop
void loop(void) {
  //read light sensor
  light = readLight();

  //read HT11 sensor
  if(dht11.read()) {
    temperature = dht11.readTemperature();
    humidity = dht11.readHumidity();
  } else {
    while(1);
  }

  //display the user interface on the 16X2 LCD
  if (currentMode == MODE1) {
    displayMode1();
  } else if (currentMode == MODE2) {
    displayMode2();
  } else if (currentMode == MODE3) {
    displayMode3();
  } else if (currentMode == MODE4){
    displayMode4();
  } else {
    displayMode5();
  }

  //controlling the outputs: light, humidifier, heater
  //control the heater represented by the red led
  if (temperature < temperatureSelection) {
    digitalWrite(heaterPin, HIGH);
    lastTempOnTime = millis();
  }
  if (millis() - lastTempOnTime > TEMP_ON_TIME) {
    if (temperature >= temperatureSelection)
      digitalWrite(heaterPin, LOW);
  }

  //control the humidifier, represented by the green led
  if (humidity < humiditySelection) {
    digitalWrite(humidifierPin, HIGH);
    lastHumOnTime = millis();
  }
  if (millis() - lastHumOnTime > HUM_ON_TIME) {
    if (humidity >= humiditySelection)
	digitalWrite(humidifierPin, LOW);
  }

  //control the light, represented by the white led
  if (lightState == LIGHT_ON) {
    if (light < lightSelection) {
      digitalWrite(lightPin, HIGH);
      lastLightOnTime = millis();
    }
    if(millis() - lastLightOnTime > LIGHT_ON_TIME) {
      if (light >= lightSelection)
        digitalWrite(lightPin, LOW);
    }
  } else {
    digitalWrite(lightPin, LOW);
  }
}

uint32_t readLight(void) {
  uint32_t lum;
  uint16_t ir, full;
  
  lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  
  return tsl.calculateLux(full, ir);
}


void isrRightButtonHandler() {
	if((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
	    isrTempHumLight();
	    lastDebounceTime = millis();
	  }
}

void isrLeftButtonHandler() {
	if((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
	    isrMode();
	    lastDebounceTime = millis();
	  }
}

void isrMode() {
	switch(currentMode) {
		case MODE1:
			nextMode = MODE2;
			currentMode = nextMode;
			break;
		case MODE2:
			nextMode = MODE3;
			currentMode = nextMode;
			break;
		case MODE3:
			nextMode = MODE4;
			currentMode = nextMode;
			break;
		case MODE4:
			nextMode = MODE5;
			currentMode = nextMode;
			break;
		case MODE5:
			nextMode = MODE1;
			currentMode = nextMode;
			break;
		default:
			nextMode = MODE1;
			currentMode = nextMode;
			break;
	}
}

void isrTempHumLight() {
  if (currentMode == MODE2) {
    temperatureSelection++;
    if (temperatureSelection == TEMP_UPPER_LIMIT+1)
      temperatureSelection = TEMP_LOWER_LIMIT;
  }
  
  if (currentMode == MODE3) {
    humiditySelection++;
    if (humiditySelection == HUM_UPPER_LIMIT+1)
      humiditySelection = HUM_LOWER_LIMIT;
  }
  
  if (currentMode == MODE4) {
    lightSelection++;
    if (lightSelection == LIGHT_UPPER_LIMIT+1)
      lightSelection = LIGHT_LOWER_LIMIT;
  }
  
  if (currentMode == MODE5) {
    lightState = !lightState;
  }      
}

static void displayMode1() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Temp ");
  lcd.setCursor(5, 0); lcd.print(temperature);
  lcd.setCursor(7, 0); lcd.print("C");
  lcd.setCursor(9, 0); lcd.print("Hum ");
  lcd.setCursor(13, 0); lcd.print(humidity);
  lcd.setCursor(15, 0); lcd.print("%");
  lcd.setCursor(0, 1); lcd.print("Light");
  lcd.setCursor(6, 1); lcd.print(light);
  lcd.setCursor(9, 1); lcd.print("lux"); 
}

static void displayMode2() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Temp selection");
  lcd.setCursor(0, 1); lcd.print("Temp ");
  lcd.setCursor(5, 1); lcd.print(temperatureSelection);
  lcd.setCursor(7, 1); lcd.print("*C");
}

static void displayMode3() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Humid selection");
  lcd.setCursor(0, 1); lcd.print("Hum ");
  lcd.setCursor(5, 1); lcd.print(humiditySelection);
  lcd.setCursor(7, 1); lcd.print("%");
}

static void displayMode4() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Light selection");
  lcd.setCursor(0, 1); lcd.print("Light ");
  lcd.setCursor(7, 1); lcd.print(lightSelection);
  lcd.setCursor(10, 1); lcd.print("lux");
}

static void displayMode5() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Set light on/off");
  lcd.setCursor(0,1);
  if (lightState == LIGHT_ON) {
    lcd.print("ON");
  } else {
    lcd.print("OFF");
  }  
}

