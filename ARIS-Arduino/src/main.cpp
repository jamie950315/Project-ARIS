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

const char* commandNames[] = {
  "FRONT", "ORIGIN", "BACK", "LEFT", "CENTER", "RIGHT",
  "UP", "MIDDLE", "DOWN", "PINCH", "RELEASE", "UNKNOWN"
};

Command parseCommand(const char* command) {
  if (strcmp(command, "FRONT") == 0) return FRONT;
  if (strcmp(command, "ORIGIN") == 0) return ORIGIN;
  if (strcmp(command, "BACK") == 0) return BACK;
  if (strcmp(command, "LEFT") == 0) return LEFT;
  if (strcmp(command, "CENTER") == 0) return CENTER;
  if (strcmp(command, "RIGHT") == 0) return RIGHT;
  if (strcmp(command, "UP") == 0) return UP;
  if (strcmp(command, "MIDDLE") == 0) return MIDDLE;
  if (strcmp(command, "DOWN") == 0) return DOWN;
  if (strcmp(command, "PINCH") == 0) return PINCH;
  if (strcmp(command, "RELEASE") == 0) return RELEASE;
  
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

ArmCommand PrevARM={ORIGIN, CENTER, MIDDLE, RELEASE};
ArmCommand ARM={ORIGIN, CENTER, MIDDLE, RELEASE};
//------PARSE COMMANDS END------

//------ARM MOVEMENT------
void logCommand(const char* axis, Command command) {
  char buffer[50]={0};
  snprintf(buffer, sizeof(buffer), "Executing %s command: %s", axis, commandNames[command]);
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
//------ARM MOVEMENT END------





void setup() {
  Serial.begin(115200);
  ServoDriver.begin();
  ServoDriver.setOscillatorFrequency(SERVO_OSCILLATOR);  
  ServoDriver.setPWMFreq(SERVO_FREQ);


  delay(10);
}

void loop() {

  char armInput[50]={0};
  char arg[4][10]={0};
  char buffer[50]={0};

  Serial.println();
  Serial.println();
  Serial.print("Enter command (Y X Z FINGER): ");
  while(!Serial.available()) {
    // Wait for user input
  }
  Serial.readBytesUntil('\n', armInput, sizeof(armInput)-1);
  armInput[strcspn(armInput, "\r\n")]=0;

  sscanf(armInput, "%s %s %s %s", arg[0], arg[1], arg[2], arg[3]);
  if (strcmp(arg[0], "LIST") == 0) {
    snprintf(buffer, sizeof(buffer), "\nY: %s, X: %s, Z: %s, FINGER: %s", commandNames[PrevARM.Y], commandNames[PrevARM.X], commandNames[PrevARM.Z], commandNames[PrevARM.FINGER]);
    Serial.println(buffer);
    return;
  }
  if(strcmp(arg[0], "RESET")==0){
    ARM=(ArmCommand){ORIGIN, CENTER, MIDDLE, RELEASE};
  }else{
    parseArm(arg, &ARM, &PrevARM);
  }

  snprintf(buffer, sizeof(buffer), "\nY: %s, X: %s, Z: %s, FINGER: %s", commandNames[ARM.Y], commandNames[ARM.X], commandNames[ARM.Z], commandNames[ARM.FINGER]);
  Serial.println(buffer);

  if(ARM.Y!=PrevARM.Y){
    ARMExecuteY(ARM.Y);
  }
  if(ARM.X!=PrevARM.X){
    ARMExecuteX(ARM.X);
  }
  if(ARM.Z!=PrevARM.Z){
    ARMExecuteZ(ARM.Z);
  }
  if(ARM.FINGER!=PrevARM.FINGER){
    ARMExecuteFINGER(ARM.FINGER);
  }

  PrevARM=ARM;



}

