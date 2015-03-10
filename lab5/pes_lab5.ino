/*
  Abo Akademi University
  Programming Embedded Systems course
  Exercise 5: TI TivaC / ADC Compiling and booting
  Author: Vu Nguyen (71141) <quangngmetro@gmail.com>
*/

#define DEBUG 1

// Accelerometer parameters
#define VMAX 3.3         // 3.3 Volts
#define DMAX 4095        // 12-bit ADCs
#define SENSITIVITY 0.420  // 420mV/g
#define XPIN A9  // x-axis of the accelerometer
#define YPIN A8  // y-axis
#define ZPIN A10 // z-axis
#define NORMAL_ACC 9.81 // 1g
#define HIGH_ACC 14.72  // 1.5g

//#define GAIN_CONSTANT_K 0.92  // TODO: Gain constant needs to be changed later, use k = 0.92 just for now 

// LEDs on expansion board
#define LED_RED PB_7
#define LED_YELLOW PE_0
#define LED_GREEN PB_2

// Buttons on expansion board
#define BUTTON_A2 PA_2
#define BUTTON_A3 PA_3
#define BUTTON_A4 PA_4

#define BUFFER_SIZE 100

// Global variables
double xZero; // x axis voltage when subject to 0g
double yZero; // y axis voltage when subject to 0g
double zZero; // z axis voltage when subject to 0g

double rX, rY, rZ; // acceleration values of each component

double r; // total acceleration r = k * sqrt(rX^2 + rY^2 + rZ^2)
double arr_r[BUFFER_SIZE];
double r_ms2;

double k;

// Function  prototypes
void calibrate_ADXL327B(unsigned int nsamples);

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
  
  //Configure LEDs and button
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUTTON_A2, INPUT);
  
  //Initialize the array arr_r
  memset(arr_r, 0, sizeof(arr_r));
  
  //Calibrate 3 axis accelerometer
  calibrate_ADXL327B(1000);
}

void loop()
{
  int i;

  // Calculate total accleration
  //r = (double)GAIN_CONSTANT_K * sqrt(pow(rX,2) + pow(rY,2) + pow(rZ,2));
  //Calculate the average value of r to smooth out its total value since it fluctuates a lot
  for (i = 0; i < BUFFER_SIZE; i++) {
    rX = (analogRead(XPIN) * (float)VMAX / (float)DMAX - xZero) / SENSITIVITY;
    rY = (analogRead(YPIN) * (float)VMAX / (float)DMAX - yZero) / SENSITIVITY;
    rZ = (analogRead(ZPIN) * (float)VMAX / (float)DMAX - zZero) / SENSITIVITY;
    arr_r[i] = k * sqrt(pow(rX,2) + pow(rY,2) + pow(rZ,2));
    Serial.println(arr_r[i]);
  }
  r = 0;
  for (i = 0; i < BUFFER_SIZE; i++) {
    r += arr_r[i]; 
  }
  r = r / BUFFER_SIZE;
  
  #ifdef DEBUG
  Serial.print("r: "); Serial.println(r);
  #endif
  
  // Convert from total acceleration g to m/s^2
  r_ms2 = r * 9.81;
  #ifdef DEBUG
  Serial.print("r_ms2: "); Serial.println(r_ms2);
  #endif
  
  // Turn on LEDs according to accleration values
  if (r_ms2 >= 0 && r_ms2 <= NORMAL_ACC) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
  } else if (r_ms2 > NORMAL_ACC && r_ms2 <= HIGH_ACC) {
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);    
  }
}


void calibrate_ADXL327B(unsigned int nsamples) {
  int i;
  
  #ifdef DEBUG
  Serial.println("Calibrating ADXL327 accelerometer");
  Serial.println("Lie the accelerometer flat so that \r\nx and y axis are zero degrees to the horizontal");
  Serial.println("Press A2 button to start calilbrating x and y axis");
  #endif
  
  //Turn on green LED to signify calibration of x and y axis begins
  digitalWrite(LED_GREEN, HIGH);
  while(digitalRead(BUTTON_A2) == HIGH);
  //Calibrating x and y axis
  for (i = 0; i < nsamples; i++) {
    xZero += (analogRead(XPIN) * (float)VMAX) / (float)DMAX;
    yZero += (analogRead(YPIN) * (float)VMAX) / (float)DMAX;
  }
  xZero = xZero / nsamples; 
  yZero = yZero / nsamples;
  //Turn off green LED to signify that calibration of x and y axis is done
  digitalWrite(LED_GREEN, LOW);
  #ifdef DEBUG
  Serial.println("Calibration of x and y axis done!");
  #endif
  
  delay(100); //Delay to avoid button boucing problem
  
  #ifdef DEBUG
  Serial.println("Lie the accelerometer so that the z axis is perpendicular to the vertical");  
  Serial.println("Press A2 button again to start calibrating z axis");
  #endif
  
   //Turn on yellow LED to signify calibration of z axis begins
  digitalWrite(LED_YELLOW, HIGH); 
  while(digitalRead(BUTTON_A2) == HIGH);
  for (i = 0; i < nsamples; i++) {
    zZero += (analogRead(ZPIN) * (float)VMAX) / (float)DMAX;
  }
  zZero = zZero / nsamples;
  //Turn off green LED to signify that calibration of z axis is done
  digitalWrite(LED_YELLOW, LOW);
  #ifdef DEBUG
  Serial.println("Calibration of z axis done!");
  #endif
  
  delay(100); //Delay to avoid button bouncing problem
  
  //Calculate value of gain constant k
  #ifdef DEBUG
  Serial.println("Lie the accelerometer flat to calculate constant k\n");
  #endif
  digitalWrite(LED_RED, HIGH); //Turn on red LED to signify that calculation of k is in progress
  while(digitalRead(BUTTON_A2) == HIGH);
  rX = (analogRead(XPIN) * (float)VMAX / (float)DMAX - xZero) / SENSITIVITY;
  rY = (analogRead(YPIN) * (float)VMAX / (float)DMAX - yZero) / SENSITIVITY;
  rZ = (analogRead(ZPIN) * (float)VMAX / (float)DMAX - zZero) / SENSITIVITY;
  k = 1 / sqrt(pow(rX, 2) + pow(rY, 2) + pow(rZ, 2));
  digitalWrite(LED_RED, LOW);
  #ifdef DEBUG
  Serial.println("Calculation of gain constant k done!");
  #endif
  
  #ifdef DEBUG
  Serial.print("xZero: "); Serial.println(xZero);
  Serial.print("yZero: "); Serial.println(yZero);
  Serial.print("zZero: "); Serial.println(zZero);
  Serial.print("k: "); Serial.println(k);
  #endif
}

