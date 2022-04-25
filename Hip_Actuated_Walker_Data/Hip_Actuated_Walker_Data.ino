#include <DynamixelShield.h>
#include <Wire.h> // This library allows you to communicate with I2C devices.
#include <SPI.h>
#include <SD.h>



const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;

float gyro_calibration;

float alpha = 0.9;
float yaw_prev = 0;

int16_t raw_gyroZ;


int counter = 0;





float pi = 3.14159;
const uint8_t legID = 1; //Default ID is 1
const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl;

//This namespace is required to use Control table item names
using namespace ControlTableItem;


int startOffset = 1040; //Original 820
int GOAL_SPEED = 32;
float t_ime;


//Frequency for each leg
float f_left = 1.3;
float f_right = 1.3;


//Amplitude for sinusoid
float A_left = 480; //Starting point at 440 (4096 clicks per revolution)
float A_right = 480;



const int chipSelect = SDCARD_SS_PIN;
String fileName = "testRun10.txt"; //File name

//Vectors for experimental data
float current_time;
float current_yaw;
float current_pos;
float current_velocity;
float current_actual_pos;
float current_current;

bool printHeaders = true;




void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");


  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(57600);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  // Get DYNAMIXEL information
  dxl.ping(legID);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(legID);
  dxl.setOperatingMode(legID, OP_POSITION);
  dxl.torqueOn(legID);

  dxl.setGoalPosition(legID, startOffset); //setting initial foot position

  delay(2000);


  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission

  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers

  raw_gyroZ = Wire.read() << 8 | Wire.read(); //Reading values in raw integer format
  gyro_calibration = raw_gyroZ; //Casting to a float
  gyro_calibration = gyro_calibration / 65.5; //Float division to scale units


  delay(20);

}

void loop() {

  counter++;

  t_ime = millis();
  float t = t_ime / 1000; //time in sec
  float pos = getJointAngle(t);

  //Updating position of dynamixel
  dxl.setGoalPosition(legID, pos);



  /******************
    Getting Yaw Data
  *******************/

  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers

  raw_gyroZ = Wire.read() << 8 | Wire.read(); //Reading values in raw integer format
  GyroZ = raw_gyroZ; //Casting to a float
  GyroZ = GyroZ / 65.5; //Float division to scale units

  yaw =  yaw + (GyroZ - gyro_calibration) * elapsedTime ;

  //Filtering yaw data
  //yaw = yaw_prev*alpha + yaw*(1-alpha);
  //yaw_prev = yaw;


  //Update data vectors
  current_time = t;
  current_pos = pos;
  current_actual_pos = dxl.getPresentPosition(legID, UNIT_DEGREE);
  current_velocity = dxl.getPresentVelocity(legID, UNIT_RPM);
  current_yaw = yaw;
  current_current = dxl.getPresentCurrent(legID, UNIT_MILLI_AMPERE);



  File dataFile = SD.open(fileName, FILE_WRITE);

  //Print headers
  if (printHeaders) {

    printHeaders = false;

    if (dataFile) {

      dataFile.println((String) "Amplitude Settings: " + A_right + ", " + A_left);
      dataFile.println((String) "Frequency Settings: " + f_right + "," + f_left);
      dataFile.println((String) "Bias Setting: " + startOffset);
      dataFile.println("================================================================");
      dataFile.println((String) "Time [s]" + "Pos [deg]" + "Acutal Pos [deg]" + "Velocity [RPM]" + "Yaw [deg]" + "Current [mA]");
      dataFile.close();

      Serial.println((String) "Amplitude Settings: " + A_right + ", " + A_left);
      Serial.println((String) "Frequency Settings: " + f_right + "," + f_left);
      Serial.println((String) "Bias Setting: " + startOffset);
      Serial.println("================================================================");
      Serial.println((String) "Time [s]" + "Pos [deg]" + "Acutal Pos [deg]" + "Velocity [RPM]" + "Yaw [deg]" + "Current [mA]");


    } else {
      Serial.println((String) "error printing headers to " + fileName);
    }

  } else {

//    if (!(counter % 10)) {
      //Write new data to SD Card
      if (dataFile) {
        String dataString = (String) current_time + ", " + current_pos + ", " + current_actual_pos + ", " + current_yaw + ", " + current_current;
        dataFile.println(dataString);
        dataFile.close();

        Serial.println(dataString);

      } else {
        Serial.println((String) "error writing data to " + fileName);
      }
//    }


  }

}


float getJointAngle(float t) {

  //Defining constants for sinusoid
  float phi = 0;
  float w_left = 2 * pi * f_left;
  float w_right = 2 * pi * f_right;
  float p_right = 1 / f_right;
  float p_left = 1 / f_left;
  float period_total = 1 / (2 * f_left) + 1 / (2 * f_right);

  float pos = 0;

  float tau = fmod(t, period_total); //time within one period of sinusoid


  /*Calculating sinusoidal input*/

    //First half
    if (tau < p_right / 2) {
      pos = A_right * sin(w_right * tau + phi) + startOffset;
    }
  
    //Second half
    else if (p_right / 2 <= tau && tau <= p_right / 2 + p_left / 2) {
      pos = A_left * sin(w_left * (tau - p_right / 2 + p_left / 2) + phi) + startOffset;
    }

//  pos = A_right * sin(w_right * tau + phi) + startOffset;

  return pos;
}
