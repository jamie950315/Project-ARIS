#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
    UP=0,
    MIDDLE,
    DOWN,
    LEFT,
    CENTER,
    RIGHT,
    PINCH,
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
    return UNKNOWN;
}

typedef struct ArmCommand{
    Command Y;
    Command X;
    Command FINGER;
}ArmCommand;

ARMExecuteY(Command command){
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
            printf("Invalid Y command\n");
            break;
    }
}

ARMExecuteX(Command command){
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

ARMExecuteFINGER(Command command){
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

void parseArm(const char* arg0, const char* arg1, const char* arg2, Command* Y, Command* X, Command* FINGER, Command* PrevY, Command* PrevX, Command* PrevFINGER) {

    Command arr[3]={UNKNOWN, UNKNOWN, UNKNOWN};
    
    arr[0]=parseCommand(arg0);
    arr[1]=parseCommand(arg1);
    arr[2]=parseCommand(arg2);

    for(int i=0;i<3;i++){
        if(arr[i]>=UP&&arr[i]<=DOWN){
            *Y=arr[i];
        }else if(arr[i]>=LEFT&&arr[i]<=RIGHT){
            *X=arr[i];
        }else if(arr[i]>=PINCH&&arr[i]<=RELEASE){
            *FINGER=arr[i];
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

}
int main(){

    ArmCommand PrevARM;
    PrevARM=(ArmCommand){MIDDLE, CENTER, RELEASE};

    ArmCommand ARM;
    ARM=(ArmCommand){MIDDLE, CENTER, RELEASE};

while(1){

    char armInput[100]={0};
    char arg0[10]={0}, arg1[10]={0}, arg2[10]={0};

    printf("\n\nEnter command (Y X FINGER): ");
    fgets(armInput, sizeof(armInput), stdin);
    armInput[strcspn(armInput, "\n")] = 0;


    sscanf(armInput, "%s %s %s",arg0, arg1, arg2);

    parseArm(arg0, arg1, arg2, &ARM.Y, &ARM.X, &ARM.FINGER, &PrevARM.Y, &PrevARM.X, &PrevARM.FINGER);


    printf("\nY: %d, X: %d, FINGER: %d\n", ARM.Y, ARM.X, ARM.FINGER);
    

    if(ARM.Y!=PrevARM.Y){
        ARMExecuteY(ARM.Y);
    }
    if(ARM.X!=PrevARM.X){
        ARMExecuteX(ARM.X);
    }
    if(ARM.FINGER!=PrevARM.FINGER){
        ARMExecuteFINGER(ARM.FINGER);
    }

    PrevARM=ARM;



}



    return 0;
}