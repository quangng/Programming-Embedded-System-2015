/*
  Abo Akademi University 
  Programming Embedded Systems 2015
  Final project: 
  Authors:  Vu Nguyen <quangngmetro@gmail.com>
            ....
*/

//Project headers to include user libraries and system libraries
#include <PES_dht11.h>



//user defined macros
#define DEBUG 0
#define SERIAL_BAUD_RATE        9600
#define DHT_PIN                 4
#define DEBOUNCE_TIME		150	//50 milliseconds
#define LIGHT_ON_TIME		60000	//1 minute
#define TEMP_ON_TIME		300000	//5 minutes
#define HUM_ON_TIME		300000	//5 minutes
#define TEMP_LOWER_LIMIT	20
#define TEMP_UPPER_LIMIT	30
#define HUM_LOWER_LIMIT		30
#define HUM_UPPER_LIMIT		40
#define ON			true
#define OFF			false
#define LCD_RS			10
#define LCD_ENABLE		9
#define LCD_D4			8
#define LCD_D5			7
#define LCD_D6			6
#define LCD_D7			5
#define LCD_ROW			2
#define LCD_COLUMN		16
#define IDEAL_OFFICE_LUX	450

//user defined data types
typedef enum {MODE1, MODE2, MODE3, MODE4, MODE5} Tmode;

//global variables
Tmode currentMode = MODE0;
Tmode nextMode = MODE1;
bool lightSelection = ON;
uint8_t humiditySelection = HUM_LOWER_LIMIT; 		//from 30 to 40
uint8_t temperatureSelection = TEMP_LOWER_LIMIT; 	//from 20 to 30
uint8_t humidity = 0;
int8_t 	temperature = 0;
uint32_t lum = 0;
uint32_t lux = 0;
uint16_t ir = 0;
uint16_t full = 0;
const uint8_t humidifierPin = 11; 	//green led
const uint8_t lightPin = 12; 			//white led
const uint8_t heaterPin = 13;			//red led
volatile uint32_t lastDebounceTime = 0;
volatile uint32_t lastLightOnTime = 0;
volatile uint32_t lastTempOnTime = 0;
volatile uint32_t lastHumOnTime = 0;

//object declarations
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
TSL2561 tsl(TSL2561_ADDR_LOW);
DHT dht;

//function prototypes
void isrMode();
void isrTempHumLight();
void debounceInterrupt0();
void debounceInterrupt1();



static void displayMode1();
static void displayMode2();
static void displayMode3();
static void displayMode4();
static void displayMode5();

//initialization function
void setup(void) {
	Serial.begin(SERIAL_BAUD_RATE);

	dht.setup(DHT_PIN);

	lcd.begin(LCD_COLUMN, LCD_ROW);

	if (tsl.begin()) {
		#ifdef DEBUF
		Serial.println("Found sensor");
		#endif
	} else {
		#ifdef DEBUF
		Serial.println("No sensor?");
		#endif
		lcd.setCursor(0, 0);
		lcd.print("No DHT sensor?");
		while (1);
	}
	tsl.setGain(TSL2561_GAIN_0X);         			// set no gain (for bright situtations)
	tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  	// shortest integration time (bright light)

	attachInterrupt(1, debounceInterrupt1, RISING);
	attachInterrupt(0, debounceInterrupt0, RISING);

	pinMode(lightPin, OUTPUT);
	pinMode(heaterPin, OUTPUT);
	pinMode(humidifierPin, OUTPUT);
}

//forever loop
void loop(void) {
  //read light sensor


  //read HT11 sensor
  humidity =
  temperature =

  //display humidity and temperature values on the terminal for testing/debugging
  #ifdef DEBUG
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Lux: ");
  Serial.println(lux);
  #endif

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

  #ifdef DEBUG
  Serial.print("current mode: ");
  Serial.print(currentMode);
  Serial.print('\t');
  #endif

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
  if (lightSelection == ON) {
    if (lux < IDEAL_OFFICE_LUX) {
      digitalWrite(lightPin, HIGH);
      lastLightOnTime = millis();
    }
    if (millis() - lastLightOnTime > LIGHT_ON_TIME) {
      if (lux >= IDEAL_OFFICE_LUX)
	  digitalWrite(lightPin, LOW);
      }
  } else {
    digitalWrite(lightPin, LOW);
  }

  #ifdef DEBUG
  Serial.print("Time elapsed(ms): ");
  Serial.println(millis());
  #endif

  delay(500);
}


void debounceInterrupt0() {
	if((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
	    isrTempHumLight();
	    lastDebounceTime = millis();
	  }
}

void debounceInterrupt1() {
	if((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
	    isrMode();
	    lastDebounceTime = millis();
	  }
}

void isrMode() {
	switch(currentMode) {
		case MODE0:
			nextMode = MODE1;
			currentMode = nextMode;
			break;
		case MODE1:
			nextMode = MODE2;
			currentMode = nextMode;
			break;
		case MODE2:
			nextMode = MODE3;
			currentMode = nextMode;
			break;
		case MODE3:
			nextMode = MODE0;
			currentMode = nextMode;
			break;
		default:
			nextMode = MODE1;
			currentMode = nextMode;
			break;
	}
}

void isrTempHumLight() {
	if (currentMode == MODE1) {
		temperatureSelection++;
		if (temperatureSelection == TEMP_UPPER_LIMIT+1)
			temperatureSelection = TEMP_LOWER_LIMIT;
	}
	if (currentMode == MODE2) {
		humiditySelection++;
		if (humiditySelection == HUM_UPPER_LIMIT+1)
			humiditySelection = HUM_LOWER_LIMIT;
	}
	if (currentMode == MODE3) {
		lightSelection = !lightSelection;
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
  lcd.setCursor(6, 1); lcd.print(lux);
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
  lcd.setCursor(0,0); lcd.print(“Set light on/off”);
  lcd.setCursor(0,1);
  if (lightSelection == LIGHT_ON) {
    lcd.print(“ON”);
  } else {
    lcd.print(“OFF”);
  }  
}

















