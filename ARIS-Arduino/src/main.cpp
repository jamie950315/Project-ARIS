#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>



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
  if (strcmp(command, "UP") == 0) return UP;
  if (strcmp(command, "DOWN") == 0) return DOWN;
  if (strcmp(command, "MIDDLE") == 0) return MIDDLE;
  if (strcmp(command, "LEFT") == 0) return LEFT;
  if (strcmp(command, "RIGHT") == 0) return RIGHT;
  if (strcmp(command, "CENTER") == 0) return CENTER;
  if (strcmp(command, "PINCH") == 0) return PINCH;
  if (strcmp(command, "RELEASE") == 0) return RELEASE;
  if (strcmp(command, "FRONT") == 0) return FRONT;
  if (strcmp(command, "ORIGIN") == 0) return ORIGIN;
  if (strcmp(command, "BACK") == 0) return BACK;
  return UNKNOWN;
}

typedef struct ArmCommand{
  Command Y;
  Command X;
  Command Z;
  Command FINGER;
}ArmCommand;

void ARMExecuteZ(Command command){
  switch(command){
      case UP:
          printf("Executing UP command\n");
          break;
      case DOWN:
          printf("Executing DOWN command\n");
          break;
      case MIDDLE:
          printf("Executing MIDDLE command\n");
          break;
      default:
          printf("Invalid Z command\n");
          break;
  }
}

void ARMExecuteX(Command command){
  switch(command){
      case LEFT:
          printf("Executing LEFT command\n");
          break;
      case RIGHT:
          printf("Executing RIGHT command\n");
          break;
      case CENTER:
          printf("Executing CENTER command\n");
          break;
      default:
          printf("Invalid X command\n");
          break;
  }
}

void ARMExecuteY(Command command){
  switch(command){
      case FRONT:
          printf("Executing FRONT command\n");
          break;
      case BACK:
          printf("Executing BACK command\n");
          break;
      case ORIGIN:
          printf("Executing ORIGIN command\n");
          break;
      default:
          printf("Invalid Y command\n");
          break;
  }
}

void ARMExecuteFINGER(Command command){
  switch(command){
      case PINCH:
          printf("Executing PINCH command\n");
          break;
      case RELEASE:
          printf("Executing RELEASE command\n");
          break;
      default:
          printf("Invalid FINGER command\n");
          break;
  }
}

void parseArm(const char* arg0, const char* arg1, const char* arg2, const char* arg3, Command* Y, Command* X, Command* Z, Command* FINGER, Command* PrevY, Command* PrevX, Command* PrevZ, Command* PrevFINGER) {

  Command arr[4]={UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};
  
  arr[0]=parseCommand(arg0);
  arr[1]=parseCommand(arg1);
  arr[2]=parseCommand(arg2);
  arr[3]=parseCommand(arg3);

  for(int i=0;i<4;i++){
      if(arr[i]>=UP&&arr[i]<=DOWN){
          *Z=arr[i];
      }else if(arr[i]>=LEFT&&arr[i]<=RIGHT){
          *X=arr[i];
      }else if(arr[i]>=PINCH&&arr[i]<=RELEASE){
          *FINGER=arr[i];
      }else if(arr[i]>=FRONT&&arr[i]<=BACK){
          *Y=arr[i];
      }
  }

  if(*Y==UNKNOWN){
      *Y=*PrevY;
  }
  if(*X==UNKNOWN){
      *X=*PrevX;
  }
  if(*FINGER==UNKNOWN){
      *FINGER=*PrevFINGER;
  }
  if(*Z==UNKNOWN){
      *Z=*PrevZ;
  }

}


Servo XServo;
Servo YServo;
Servo ZServo;
Servo FingerServo;



void setup() {
  Serial.begin(115200);
}

void loop() {

}

