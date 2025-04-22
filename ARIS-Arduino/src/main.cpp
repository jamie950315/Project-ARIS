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


void ARMExecuteY(Command command){
  switch(command){
      case FRONT:
          Serial.println("Executing FRONT command");
          break;
      case BACK:
          Serial.println("Executing BACK command");
          break;
      case ORIGIN:
          Serial.println("Executing ORIGIN command");
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
          break;
      case RIGHT:
          Serial.println("Executing RIGHT command");
          break;
      case CENTER:
          Serial.println("Executing CENTER command");
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
          break;
      case DOWN:
          Serial.println("Executing DOWN command");
          break;
      case MIDDLE:
          Serial.println("Executing MIDDLE command");
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
          break;
      case RELEASE:
          Serial.println("Executing RELEASE command");
          break;
      default:
          Serial.println("Invalid FINGER command");
          break;
  }
}

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

Servo YServo;
Servo XServo;
Servo ZServo;
Servo FingerServo;

ArmCommand PrevARM={ORIGIN, CENTER, MIDDLE, RELEASE};
ArmCommand ARM={ORIGIN, CENTER, MIDDLE, RELEASE};


void setup() {
  Serial.begin(115200);

}

void loop() {

  char armInput[50]={0};
  char arg[4][10]={0};
  char buffer[50];

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

