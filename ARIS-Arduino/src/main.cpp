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
void ARMExecuteY(Command command){
  switch(command){
      case FRONT:
          Serial.println("Executing FRONT command");
          servoWrite(Y_SERVO_CHANNEL, 0);
          break;
      case BACK:
          Serial.println("Executing BACK command");
          servoWrite(Y_SERVO_CHANNEL, 180);
          break;
      case ORIGIN:
          Serial.println("Executing ORIGIN command");
          servoWrite(Y_SERVO_CHANNEL, 90);
          break;
      default:
          Serial.println("Invalid Y command");
          break;
  }
}

void ARMExecuteX(Command command){
  switch(command){
      case LEFT:
          Serial.println("Executing LEFT command");
          servoWrite(X_SERVO_CHANNEL, 0);
          break;
      case RIGHT:
          Serial.println("Executing RIGHT command");
          servoWrite(X_SERVO_CHANNEL, 180);
          break;
      case CENTER:
          Serial.println("Executing CENTER command");
          servoWrite(X_SERVO_CHANNEL, 90);
          break;
      default:
          Serial.println("Invalid X command");
          break;
  }
}

void ARMExecuteZ(Command command){
  switch(command){
      case UP:
          Serial.println("Executing UP command");
          servoWrite(Z_SERVO_CHANNEL, 0);
          break;
      case DOWN:
          Serial.println("Executing DOWN command");
          servoWrite(Z_SERVO_CHANNEL, 180);
          break;
      case MIDDLE:
          Serial.println("Executing MIDDLE command");
          servoWrite(Z_SERVO_CHANNEL, 90);
          break;
      default:
          Serial.println("Invalid Z command");
          break;
  }
}

void ARMExecuteFINGER(Command command){
  switch(command){
      case PINCH:
          Serial.println("Executing PINCH command");
          servoWrite(FINGER_SERVO_CHANNEL, 0);
          break;
      case RELEASE:
          Serial.println("Executing RELEASE command");
          servoWrite(FINGER_SERVO_CHANNEL, 180);
          break;
      default:
          Serial.println("Invalid FINGER command");
          break;
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

    snprintf(buffer, sizeof(buffer), "\nY: %d, X: %d, Z: %d, FINGER: %d\n", PrevARM.Y, PrevARM.X, PrevARM.Z, PrevARM.FINGER);
    Serial.print(buffer);
    return;
  }
  if(strcmp(arg[0], "RESET")==0){
    ARM=(ArmCommand){ORIGIN, CENTER, MIDDLE, RELEASE};
  }else{
    parseArm(arg, &ARM, &PrevARM);
  }

  snprintf(buffer, sizeof(buffer), "\nY: %d, X: %d, Z: %d, FINGER: %d\n", ARM.Y, ARM.X, ARM.Z, ARM.FINGER);
  

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

