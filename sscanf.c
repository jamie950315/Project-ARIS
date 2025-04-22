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


int main(){

    char armInput[100];
    char y[10], x[10], finger[10];

    ArmCommand PrevARM;
    PrevARM=(ArmCommand){MIDDLE, CENTER, RELEASE};

    ArmCommand ARM;

while(1){

    printf("\n\nEnter command (Y X FINGER): ");
    strcpy(armInput, "UP LEFT PINCH"); // For testing purposes
    fgets(armInput, sizeof(armInput), stdin);
    armInput[strcspn(armInput, "\n")] = 0; // Remove newline character
    


    sscanf(armInput, "%s %s %s",y, x, finger);
    

    ARM.Y=parseCommand(y);
    ARM.X=parseCommand(x);
    ARM.FINGER=parseCommand(finger);

    // if(ARM.Y>=LEFT&&ARM.Y<=RIGHT){
    //     ARM.X=ARM.Y;
    //     ARM.Y=PrevARM.Y;
    // }
    // if(ARM.Y>=PINCH&&ARM.Y<=RELEASE){
    //     ARM.FINGER=ARM.Y;
    //     ARM.Y=PrevARM.Y;
    // }
    // if(ARM.X>=UP&&ARM.X<=DOWN){
    //     ARM.Y=ARM.X;
    //     ARM.X=PrevARM.X;
    // }
    // if(ARM.X>=PINCH&&ARM.X<=RELEASE){
    //     ARM.FINGER=ARM.X;
    //     ARM.X=PrevARM.X;
    // }
    // if(ARM.FINGER>=UP&&ARM.FINGER<=DOWN){
    //     ARM.Y=ARM.FINGER;
    //     ARM.FINGER=PrevARM.FINGER;
    // }
    // if(ARM.FINGER>=LEFT&&ARM.FINGER<=RIGHT){
    //     ARM.X=ARM.FINGER;
    //     ARM.FINGER=PrevARM.FINGER;
    // }

    if(ARM.Y>=LEFT&&ARM.Y<=RIGHT){
        ARM.X=parseCommand(y);
        ARM.Y=PrevARM.Y;
    }else if(ARM.Y>=PINCH&&ARM.Y<=RELEASE){
        ARM.FINGER=parseCommand(y);
        ARM.Y=PrevARM.Y;
    }

    if(ARM.X>=UP&&ARM.X<=DOWN){
        ARM.Y=parseCommand(x);
        ARM.X=PrevARM.X;
    }else if(ARM.X>=PINCH&&ARM.X<=RELEASE){
        ARM.FINGER=parseCommand(x);
        ARM.X=PrevARM.X;
    }

    if(ARM.FINGER>=UP&&ARM.FINGER<=DOWN){
        ARM.Y=parseCommand(finger);
        ARM.FINGER=PrevARM.FINGER;
    }else if(ARM.FINGER>=LEFT&&ARM.FINGER<=RIGHT){
        ARM.X=parseCommand(finger);
        ARM.FINGER=PrevARM.FINGER;
    }


    printf("Y: %s, X: %s, FINGER: %s\n", y, x, finger);
    printf("Parsed Y: %d, X: %d, FINGER: %d\n", ARM.Y, ARM.X, ARM.FINGER);
    //print ARM.Y, ARM.X, ARM.FINGER by %s




    if(ARM.Y != PrevARM.Y){
        ARMExecuteY(ARM.Y);
        PrevARM.Y = ARM.Y;
    }
    if(ARM.X != PrevARM.X){
        ARMExecuteX(ARM.X);
        PrevARM.X = ARM.X;
    }
    if(ARM.FINGER != PrevARM.FINGER){
        ARMExecuteFINGER(ARM.FINGER);
        PrevARM.FINGER = ARM.FINGER;
    }



}



    return 0;
}