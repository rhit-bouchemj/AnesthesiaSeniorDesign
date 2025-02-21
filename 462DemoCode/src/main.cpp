#include <AccelStepper.h>

//TODO: set start position of motor + add to position instead of setting to 0 -> can be set to the sie of the pump 
// ^ This can be a future task <-- instead maybe set current position instead of creating a new one

// Define motor connections
#define MOTOR1_DIR 25      //S3 numbers: 4
#define MOTOR1_STEP 26     // 5
#define MOTOR2_DIR 32      // 6
#define MOTOR2_STEP 33     // 7  
#define MOTOR3_DIR 23      // 1
#define MOTOR3_STEP 22     // 2
#define MOTOR4_DIR 21     // 42
#define MOTOR4_STEP 19    // 41

// Define Input connections
// #define ENCODER_BUTTON 10
#define ENCODER_SW_PIN 11  // this is sw on encoder
#define ENCODER_DT_PIN 10  // this is dt on encoder
#define ENCODER_CLK_PIN 9 // this is clk on encoder


//Define LEDscreen connections

//Define occlusion detection connections


//Define constant values used
#define stepsPerML 1480
#define multiStepping 8   //standard is base 1/4, divide standard by this value to get used stepping (EX: 8 would represent a stepping of 1/32)


//create AccelStepper instances
AccelStepper stepper1(AccelStepper::DRIVER, MOTOR1_STEP, MOTOR1_DIR);
AccelStepper stepper2(AccelStepper::DRIVER, MOTOR2_STEP, MOTOR2_DIR);
AccelStepper stepper3(AccelStepper::DRIVER, MOTOR3_STEP, MOTOR3_DIR);
AccelStepper stepper4(AccelStepper::DRIVER, MOTOR4_STEP, MOTOR4_DIR);

//Global Variables
bool newMotorStart = false; //Flag to determine if a new motor is to be started (or old one changed)
bool motorRunning = false;  //Flag to define if any motor is running (mostly created to stop all motors)

int newMotorSpeed = 0;      //alternatively use this variable since only need to adjust max speed depending on motor
int newStepperNumber = 0;   //The number of the channel that will be used when the rotary button is pressed
int newStepperAmount = 0;   //the amount that the stepper will output (in steps, I can create a simple calculation to go from mL to this)

int UI_desiredSpeed = 0;
int UI_desiredAmount = 0;
int UI_desiredMotorNum = 0;   //kind of redundant, but for clarity of what needs to be specified by the UI


//Interupts
/*
  When the rotary encoder button is depressed the newMotorStart flag will raise
  The newMotorStart flag is used in the loop to set the speed and start rotations for the specified motor instance
*/
void buttonPressed()
{
  newMotorStart = true;
}


//Helper function - These funtions were written with the assistance of chatGPT

/*
*/
AccelStepper* getStepper(int stepperNumber) {
    switch (stepperNumber) {
        case 1: return &stepper1;
        case 2: return &stepper2;
        case 3: return &stepper3;
        case 4: return &stepper4;
        default: return nullptr; // Return null if invalid input
    }
}

/*
  returns the number of steps to give the desired mL amount
*/
int calculate_mL(float mLamount)
{
  return (mLamount * multiStepping * stepsPerML);
}

/*
  returns the number of steps/second to give the desired mL/hour amount
*/
int calculateMotorSpeed(int mLPerHour)
{
  int secondsPerHour = 3600;    //for code readability
  return ((mLPerHour * stepsPerML * multiStepping) / secondsPerHour);
}

//An idea for a method to set the stepper number if you don't want to edit it inside the code (you can make more via copy/paste if you like the idea)
void setNewStepperNumber(int settingNumber)
{
  newStepperNumber = settingNumber;
}


/*
  Inputs
  stepperNumber - The number of the channel that will be used for the current dosage <-- What you're inputting
  stepperSpeed - the speed in steps/second that will be used for 
*/
void startNewMotor(int stepperNumber, int stepperSpeed, int numberSteps)
{
  AccelStepper* currentStepper = getStepper(stepperNumber);
  currentStepper->setMaxSpeed(stepperSpeed);
  currentStepper->setAcceleration(stepperSpeed/2);
  currentStepper->moveTo(numberSteps);
  Serial.print("Stepper Number:");
  Serial.println(stepperNumber);
  Serial.print("Stepper Speed:");
  Serial.println(stepperSpeed);
  Serial.print("Stepper amount:");
  Serial.println(numberSteps);
  motorRunning = true;
}

void turnOffMotors()
{
  for(int i = 1; i <= 4; i++) 
  {
    startNewMotor(i, 0, 0);
  }
  motorRunning = false;
}

int counter = 0;
void setup() {
    // pinMode(ENCODER_BUTTON, INPUT_PULLUP);    //enabling the rotary encoder button
    // attachInterrupt(digitalPinToInterrupt(ENCODER_BUTTON), handleButtonPress, FALLING); //attach interupt trigger to the button pressing down
  // pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  // pinMode(ENCODER_DT_PIN, INPUT);
  // pinMode(ENCODER_CLK_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), buttonPressed, FALLING);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), turn, CHANGE);

  Serial.begin(115200);
  newStepperNumber = 1;
  newMotorSpeed = calculateMotorSpeed(3600/2);
  newStepperAmount = calculate_mL(10);
  startNewMotor(newStepperNumber, newMotorSpeed, newStepperAmount);

  newStepperNumber = 2;
  // newMotorSpeed = calculateMotorSpeed(UI_desiredSpeed);
  newMotorSpeed = calculateMotorSpeed(3600/2);
  // newStepperAmount = calculate_mL(UI_desiredAmount);
  newStepperAmount = calculate_mL(5);
  startNewMotor(newStepperNumber, newMotorSpeed, newStepperAmount);
  
  newStepperNumber = 3;
  newMotorSpeed = calculateMotorSpeed(3600/2);
  newStepperAmount = calculate_mL(10);
  startNewMotor(newStepperNumber, newMotorSpeed, newStepperAmount);
  
  newStepperNumber = 4;
  newMotorSpeed = calculateMotorSpeed(3600/2);
  newStepperAmount = calculate_mL(10);
  startNewMotor(newStepperNumber, newMotorSpeed, newStepperAmount);
}


void loop() {
  // stepper1.run();//Speed(); //non-blocking commands
  if(motorRunning)
  {
    stepper1.run();//Speed(); //non-blocking commands
    stepper2.run();
    stepper3.run();
    stepper4.run();
  }
  // Trigger flag checks
  // if(newMotorStart)
  // {
  //   Serial.println("button");
  //   // newStepperNumber = UI_desiredMotorNum;
  //   newStepperNumber = 2;
  //   // newMotorSpeed = calculateMotorSpeed(UI_desiredSpeed);
  //   newMotorSpeed = calculateMotorSpeed(3600);
  //   // newStepperAmount = calculate_mL(UI_desiredAmount);
  //   newStepperAmount = calculate_mL(5);
  //   startNewMotor(newStepperNumber, newMotorSpeed, newStepperAmount);
    
  //   //Then set desired values back to 0
  //   UI_desiredMotorNum = 0;
  //   UI_desiredSpeed = 0;
  //   UI_desiredAmount = 0;
  //   newMotorStart = false;
  // }
  // delay(5);


}

