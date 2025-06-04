#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <Adafruit_PWMServoDriver.h>
#include <avr/pgmspace.h>

//------SERVO DRIVER CONFIGURATION------
#define Y_SERVO_CHANNEL 0
#define X_SERVO_CHANNEL 1
#define Z_SERVO_CHANNEL 2
#define FINGER_SERVO_CHANNEL 3
#define SERVO_MIN 150
#define SERVO_MAX 600
#define SERVO_FREQ 50
#define SERVO_OSCILLATOR 25420595 //depends on different driver

Adafruit_PWMServoDriver ServoDriver=Adafruit_PWMServoDriver();

void servoWrite(uint8_t channel, uint8_t angle){
  uint16_t pulse=map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  ServoDriver.setPWM(channel, 0, pulse);
}
//------SERVO DRIVER CONFIGURATION END------

//------PARSE COMMANDS------
typedef enum{
  
  FRONT=0,  //Y
  ORIGIN,
  BACK,
  LEFT, //X
  CENTER,
  RIGHT,
  UP, //Z
  MIDDLE,
  DOWN,
  PINCH,  //Finger
  RELEASE,
  UNKNOWN
}Command;

const char commandNames[][10] PROGMEM={
  "FRONT", "ORIGIN", "BACK", "LEFT", "CENTER", "RIGHT",
  "UP", "MIDDLE", "DOWN", "PINCH", "RELEASE", "UNKNOWN"
};

const char* getCommandName(Command cmd) {
  static char nameBuffer[4][10];
  static uint8_t bufIndex = 0;
  strcpy_P(nameBuffer[bufIndex], commandNames[cmd]);
  const char* result = nameBuffer[bufIndex];
  bufIndex = (bufIndex + 1) % 4;
  return result;
}

Command parseCommand(const char* command) {
  for(uint8_t i=0;i<uint8_t(sizeof(commandNames)/sizeof(commandNames[0]));++i){
    if(strcmp(command, getCommandName(Command(i)))==0){
      return (Command)i;
    }
  }
  
  return UNKNOWN;
}

typedef struct ArmCommand{
  Command Y;
  Command X;
  Command Z;
  Command FINGER;
}ArmCommand;

void parseArm(char arg[4][10], ArmCommand* ARM, ArmCommand* PrevARM) {
  Command arr[4]={UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};
  
  ARM->Y=PrevARM->Y;
  ARM->X=PrevARM->X;
  ARM->Z=PrevARM->Z;
  ARM->FINGER=PrevARM->FINGER;

  for(int i=0;i<4;i++){
      arr[i]=parseCommand(arg[i]);

      switch(arr[i]){
        case FRONT:
        case ORIGIN:
        case BACK:
          ARM->Y=arr[i];
          break;
        case LEFT:
        case CENTER:
        case RIGHT:
          ARM->X=arr[i];
          break;
        case UP:
        case MIDDLE:
        case DOWN:
          ARM->Z=arr[i];
          break;
        case PINCH:
        case RELEASE:
          ARM->FINGER=arr[i];
          break;
        default:
          break; 
      }
  }
}
const ArmCommand ResetARM PROGMEM={ORIGIN, CENTER, MIDDLE, RELEASE};
ArmCommand PrevARM;
ArmCommand ARM;
//------PARSE COMMANDS END------

//------ARM MOVEMENT------
void logCommand(Command command) {

  switch(command){
    case FRONT:
    case ORIGIN:
    case BACK:
      Serial.print(F("Executing Y command: "));
      break;
    case LEFT:
    case CENTER:
    case RIGHT:
      Serial.print(F("Executing X command: "));
      break;
    case UP:
    case MIDDLE:
    case DOWN:
      Serial.print(F("Executing Z command: "));
      break;
    case PINCH:
    case RELEASE:
      Serial.print(F("Executing Finger command: "));
      break;
    default:
      break;
  }

  Serial.println(getCommandName(command));
}

void ARMExecuteY(Command command){
  bool inValid=false;
  switch(command){
      case FRONT:
          servoWrite(Y_SERVO_CHANNEL, 0);
          break;
      case ORIGIN:
          servoWrite(Y_SERVO_CHANNEL, 90);
          break;
      case BACK:
          servoWrite(Y_SERVO_CHANNEL, 180);
          break;
      default:
          inValid=true;
          break;
  }
  if(inValid){
      Serial.println(F("Invalid Y command"));
  }else{
      logCommand(command);
  }
}

void ARMExecuteX(Command command){
  bool inValid=false;
  switch(command){
      case LEFT:
          servoWrite(X_SERVO_CHANNEL, 0);
          break;
      case CENTER:
          servoWrite(X_SERVO_CHANNEL, 90);
          break;
      case RIGHT:
          servoWrite(X_SERVO_CHANNEL, 180);
          break;
      default:
          inValid=true;
          break;
  }
  if(inValid){
      Serial.println(F("Invalid X command"));
  }else{
      logCommand(command);
  }
}

void ARMExecuteZ(Command command){
  bool inValid=false;
  switch(command){
      case UP:
          servoWrite(Z_SERVO_CHANNEL, 0);
          break;
      case MIDDLE:
          servoWrite(Z_SERVO_CHANNEL, 90);
          break;
      case DOWN:
          servoWrite(Z_SERVO_CHANNEL, 180);
          break;
      default:
          inValid=true;
          break;
  }
  if(inValid){
      Serial.println(F("Invalid Z command"));
  }else{
      logCommand(command);
  }
}

void ARMExecuteFINGER(Command command){
  bool inValid=false;
  switch(command){
      case PINCH:
          servoWrite(FINGER_SERVO_CHANNEL, 0);
          break;
      case RELEASE:
          servoWrite(FINGER_SERVO_CHANNEL, 180);
          break;
      default:
          inValid=true;
          break;
  }
  if(inValid){
      Serial.println(F("Invalid FINGER command"));
  }else{
      logCommand(command);
  }
}

void ARMAction(ArmCommand* ARM, ArmCommand* PrevARM){
  char buffer[50]={0};
  snprintf(buffer, sizeof(buffer), "Y: %s, X: %s, Z: %s, FINGER: %s", getCommandName(ARM->Y), getCommandName(ARM->X), getCommandName(ARM->Z), getCommandName(ARM->FINGER));
  Serial.println(buffer);
  if(ARM->Y!=PrevARM->Y){
    ARMExecuteY(ARM->Y);
  }
  if(ARM->X!=PrevARM->X){
    ARMExecuteX(ARM->X);
  }
  if(ARM->Z!=PrevARM->Z){
    ARMExecuteZ(ARM->Z);
  }
  if(ARM->FINGER!=PrevARM->FINGER){
    ARMExecuteFINGER(ARM->FINGER);
  }

  *PrevARM=*ARM;
}
//------ARM MOVEMENT END------


void startOver( char armInput[], size_t inputSize, 
                char arg[][10], size_t argSize,
                char buffer[], size_t bufSize){
  memset(armInput, 0, inputSize);
  memset(arg, 0, argSize);
  memset(buffer, 0, bufSize);

  Serial.println(F("Arduino Ready"));
}










void setup() {
  Serial.begin(115200);
  ServoDriver.begin();
  ServoDriver.setOscillatorFrequency(SERVO_OSCILLATOR);  
  ServoDriver.setPWMFreq(SERVO_FREQ);

  Serial.println(F("Arduino Ready"));

  memcpy_P(&PrevARM, &ResetARM, sizeof(ArmCommand));
  memcpy_P(&ARM, &ResetARM, sizeof(ArmCommand));

  delay(10);
}

void loop() {

  static char armInput[50]={0};
  static char arg[4][10]={0};
  static char buffer[50]={0}; 

  if(Serial.available()){
    Serial.readBytesUntil('\n', armInput, sizeof(armInput)-1);
    armInput[strcspn(armInput, "\r\n")]=0;
  
    // Limit each argument to 9 characters to avoid buffer overflows
    sscanf(armInput, "%9s %9s %9s %9s", arg[0], arg[1], arg[2], arg[3]);
    if(strcmp(arg[0], "LIST")==0) {
      snprintf(buffer, sizeof(buffer), "Y: %s, X: %s, Z: %s, FINGER: %s", getCommandName(PrevARM.Y), getCommandName(PrevARM.X), getCommandName(PrevARM.Z), getCommandName(PrevARM.FINGER));
      Serial.println(buffer);

      startOver(armInput, sizeof(armInput),
                arg, sizeof(arg),
                buffer, sizeof(buffer));
      return;

    }
    if(strcmp(arg[0], "RESET")==0){
      memcpy_P(&ARM, &ResetARM, sizeof(ArmCommand));
      ARMAction(&ARM, &PrevARM);

      startOver(armInput, sizeof(armInput),
                arg, sizeof(arg),
                buffer, sizeof(buffer));
      return;

    }else{
      parseArm(arg, &ARM, &PrevARM);
    }
  

    ARMAction(&ARM, &PrevARM);
  
    

    startOver(armInput, sizeof(armInput),
                arg, sizeof(arg),
                buffer, sizeof(buffer));


  }


  



}