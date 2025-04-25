#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <Adafruit_PWMServoDriver.h>

//------SERVO DRIVER CONFIGURATION------
#define Y_SERVO_CHANNEL 0
#define X_SERVO_CHANNEL 1
#define Z_SERVO_CHANNEL 2
#define FINGER_SERVO_CHANNEL 3
#define SERVO_MIN 150
#define SERVO_MAX 600
#define SERVO_FREQ 50
#define SERVO_OSCILLATOR 27000000

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

Command parseCommand(const char* command) {
  for(int i=0;i<int(sizeof(commandNames)/sizeof((PGM_P)commandNames[0]));++i){
    if(strcmp(command, (PGM_P)commandNames[i])==0){
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
      if(arr[i]>=FRONT&&arr[i]<=BACK){
          ARM->Y=arr[i];
      }else if(arr[i]>=LEFT&&arr[i]<=RIGHT){
          ARM->X=arr[i];
      }else if(arr[i]>=UP&&arr[i]<=DOWN){
          ARM->Z=arr[i];
      }else if(arr[i]>=PINCH&&arr[i]<=RELEASE){
          ARM->FINGER=arr[i];
      }
  }
}
const ArmCommand ResetARM PROGMEM={ORIGIN, CENTER, MIDDLE, RELEASE};
ArmCommand PrevARM;
ArmCommand ARM;
//------PARSE COMMANDS END------

//------ARM MOVEMENT------
void logCommand(const char* axis, Command command) {
  char buffer[50]={0};
  snprintf(buffer, sizeof(buffer), "Executing %s command: %s", axis, (PGM_P)commandNames[command]);
  Serial.println(buffer);
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
      Serial.println("Invalid Y command");
  }else{
      logCommand("Y", command);
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
      Serial.println("Invalid X command");
  }else{
      logCommand("X", command);
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
      Serial.println("Invalid Z command");
  }else{
      logCommand("Z", command);
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
      Serial.println("Invalid FINGER command");
  }else{
      logCommand("FINGER", command);
  }
}

void ARMAction(ArmCommand* ARM, ArmCommand* PrevARM){
  char buffer[50]={0};
  snprintf(buffer, sizeof(buffer), "\nY: %s, X: %s, Z: %s, FINGER: %s", (PGM_P)commandNames[ARM->Y], (PGM_P)commandNames[ARM->X], (PGM_P)commandNames[ARM->Z], (PGM_P)commandNames[ARM->FINGER]);
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
                char buffer[], size_t bufSize) {
  memset(armInput, 0, inputSize);
  memset(arg, 0, argSize);
  memset(buffer, 0, bufSize);

  Serial.println();
  Serial.println();
  Serial.print("Enter command (Y X Z FINGER): ");
}







void setup() {
  Serial.begin(115200);
  ServoDriver.begin();
  ServoDriver.setOscillatorFrequency(SERVO_OSCILLATOR);  
  ServoDriver.setPWMFreq(SERVO_FREQ);

  Serial.println();
  Serial.println();
  Serial.print("Enter command (Y X Z FINGER): ");

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
  
    sscanf(armInput, "%s %s %s %s", arg[0], arg[1], arg[2], arg[3]);
    if(strcmp(arg[0], "LIST") == 0) {
      snprintf(buffer, sizeof(buffer), "\nY: %s, X: %s, Z: %s, FINGER: %s", (PGM_P)commandNames[PrevARM.Y], (PGM_P)commandNames[PrevARM.X], (PGM_P)commandNames[PrevARM.Z], (PGM_P)commandNames[PrevARM.FINGER]);
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

