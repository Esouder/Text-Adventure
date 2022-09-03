#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

//It would be wise to split this up into multiple files once I get off this plane
//and can look up how to use the linker lol.

void welcomePlayer(void);
int resumeOrNew(void);
void openSavedGame(void);

//Saving
void saveGame(char *saveName);
void recallGame(char *saveName);

//navigating
void describeSituation(void);
void getPlayerInput(void);


typedef struct inventoryObject{
    char description[256];
};

//In-Room Object
typedef struct roomObject{
    char description[256];

};

//Room
struct room{
    struct room* north;
    struct room* east;
    struct room* south;
    struct room* west;

    int numObjects;
    char description[256];
    char notes[256];

};

typedef enum cardinalDirection{
    NORTH,
    EAST,
    SOUTH,
    WEST
} cardinalDirection_t;

cardinalDirection_t getRoomOppositeDirection(cardinalDirection_t direction);

struct room* generateRoom(struct room* preceedingRoom, cardinalDirection_t direction, int depth);

#define ROOM_GEN_DEPTH 10

//Game Data
#define PLAYER_NAME_SIZE 32
char playerName[PLAYER_NAME_SIZE];
#define SAVE_NAME_SIZE 32
char saveName[SAVE_NAME_SIZE];

int firstTimeInRoom = TRUE;



#define GAME_DATA_SIZE 64

struct room* currentRoom;

//adjectives
#define LIGHTING_ADJ_LENGTH 22
#define SIZE_ADJ_LENGTH 10
#define ROOM_WORDS_LENGTH 15
#define NOTE_FLAVOR_NOUNS_LENGTH 20
char lightingAdjs[LIGHTING_ADJ_LENGTH][64];
char sizeAdjs[SIZE_ADJ_LENGTH][64];
char roomWords[ROOM_WORDS_LENGTH][64];
char noteFlavorNouns[NOTE_FLAVOR_NOUNS_LENGTH][64];
void loadLists(void);

int main(void){
    loadLists();
    if(resumeOrNew()){
        welcomePlayer();
        printf("Generating world...");
        currentRoom = generateRoom((struct room*)NULL,SOUTH, 0);
        printf("done\n");

    }else{
        openSavedGame();
    }

    while(TRUE){
        describeSituation();
        getPlayerInput();
    }
    
}

void welcomePlayer(void){
    printf("Enter Your Name: ");
    scanf("%s", &playerName);
    printf("Welcome, %s, to a game of as-yet-undecided name\n", playerName);
    printf("You can save the game at any point by entering the 'savegame' command.\n");
    printf("Enter a name for this save: ");
    scanf("%s", &saveName);
    saveGame(saveName);

}

void saveGame(char *saveName){
    char fullSaveName[64];
    sprintf(fullSaveName, "saves/%s.savegame",saveName);
    FILE* saveFile = fopen(fullSaveName, "w");
    fprintf(saveFile, playerName);
    fprintf(saveFile, "\n");
    fprintf(saveFile, saveName);
    fprintf(saveFile, "\n");
    fclose(saveFile);
    
    printf("A savefile has been created with the name '%s'.\n", saveName);
}

void recallGame(char *saveName){
    char fullSaveName[64];
    sprintf(fullSaveName, "saves/%s.savegame",saveName);
    FILE* saveFile = fopen(fullSaveName, "r");
    fscanf(saveFile,"%s", playerName);
    fclose(saveFile);
    printf("Opened save file '%s'. Welcome back, %s.\n", saveName, playerName);
}

int resumeOrNew(void){
    for(;;){
        printf("(N)ew game or (R)esume saved game?\n");
        char response;
        scanf("%c", &response);
        if(response == 'N'){
            return TRUE;
        } else if(response == 'R'){
            return FALSE;
        }
    }
}

void openSavedGame(void){
    printf("Enter the savefile name [Note: bad names will crash game]:");
    scanf("%s", saveName);
    recallGame(saveName);
}

cardinalDirection_t getRoomOppositeDirection(cardinalDirection_t direction){
    if(direction == NORTH){
        return SOUTH;
    } else if(direction == EAST){
        return WEST;
    } else if(direction == SOUTH){
        return NORTH;
    } else if(direction == WEST){
        return EAST;
    }
}

struct room* generateRoom(struct room* preceedingRoom, cardinalDirection_t direction, int depth){
    struct room* thisRoom = (struct room*)malloc(sizeof(struct room));
    //printf("test 1\n");
    cardinalDirection_t sourceDirection = getRoomOppositeDirection(direction);
    //gen North
    int random = rand()%ROOM_GEN_DEPTH;
    //printf("random: %d\n", random);
    if(sourceDirection == NORTH){
        thisRoom->north = preceedingRoom;
    } else if(random>depth){
        thisRoom->north = generateRoom(thisRoom, NORTH, depth+1);
    } else {
        thisRoom->north = (struct room*)NULL;
    }

    //gen East
    if(sourceDirection == EAST){
        thisRoom->east = preceedingRoom;
    }
    else if(rand()%ROOM_GEN_DEPTH>depth){
        thisRoom->east = generateRoom(thisRoom, EAST, depth+1);
    } else {
        thisRoom->east = (struct room*)NULL;
    }
    //gen South
    if(sourceDirection == SOUTH){
        thisRoom->south = preceedingRoom;
    } else if(rand()%ROOM_GEN_DEPTH>depth){
        thisRoom->south = generateRoom(thisRoom, SOUTH, depth+1);
    } else {
        thisRoom->south = (struct room*)NULL;
    }
    //gen West
    if(sourceDirection == WEST){
        thisRoom->west = preceedingRoom;
    } else if(rand()%ROOM_GEN_DEPTH>depth){
        thisRoom->west = generateRoom(thisRoom, WEST, depth+1);
    } else {
        thisRoom->west = (struct room*)NULL;
    }
    sprintf(thisRoom->description,"%s, %s %s", sizeAdjs[rand()%SIZE_ADJ_LENGTH], lightingAdjs[rand()%SIZE_ADJ_LENGTH], roomWords[rand()%ROOM_WORDS_LENGTH]);
    
    if(rand()%20>=10){
        //description type one... "the X is/appear to be/seem to be/looks like theyre/ made of/covered in/"
        char flavorText1[22];
        switch(rand()%5){
            case 0:
            strcpy(flavorText1,"are");
            break;
            case 1:
            strcpy(flavorText1,"appear to be");
            break;
            case 2:
            strcpy(flavorText1,"look like they are");
            break;
            case 3:
            strcpy(flavorText1,"appear as if they are");
            break;
            case 4:
            strcpy(flavorText1,"seem to be");
            break;
        }
        char flavorText2[11];
        switch(rand()%2){
            case 0:
            strcpy(flavorText2,"made of");
            break;
            case 1:
            strcpy(flavorText2,"covered in");
            break;            
        }
        char flavorLocation[17];
        switch(rand()%3){
            case 0:
            strcpy(flavorLocation,"floors");
            break;
            case 1:
            strcpy(flavorLocation,"ceilings");
            break;    
            case 2:
            strcpy(flavorLocation,"walls");
            break;          
        }
        sprintf(thisRoom->notes, "%s %s %s %s", flavorLocation,flavorText1,flavorText2,noteFlavorNouns[rand()%NOTE_FLAVOR_NOUNS_LENGTH]);
    } else {
        thisRoom->notes[0] = 0;
    }
    return thisRoom;
}

void describeSituation(void){
    char roomEntryText[13];
    firstTimeInRoom ? strcpy(roomEntryText, "You enter a") : strcpy(roomEntryText, "You are in a");
    printf("%s %s. ",roomEntryText, currentRoom->description);
    if(currentRoom->north!=NULL){
        printf("There is a passage leading North. ");
    }
    if(currentRoom->east!=NULL){
        printf("There is a passage leading East. ");
    } 
    if(currentRoom->south!=NULL){
        printf("There is a passage leading South. ");
    } 
    if(currentRoom->west!=NULL){
        printf("There is a passage leading West. ");
    }
    if(currentRoom->notes[0]){
        char noteEntryText[20];
        firstTimeInRoom ? strcpy(noteEntryText, rand()%2 ? "You notice that the" : "You note that the") : strcpy(noteEntryText, "The");
        printf("%s %s.", noteEntryText,currentRoom->notes);
    }
    printf("\n> ");
}

void getPlayerInput(void){
    while(TRUE){
        firstTimeInRoom = FALSE;
        char userChoice[32];
        scanf("%s", userChoice);
        if(!strcmp("go",userChoice)){
            scanf("%s", userChoice);
            if(!strcmp("north",userChoice) && currentRoom->north != (struct room*)NULL){
                currentRoom = currentRoom->north;
                firstTimeInRoom = TRUE;
                return;
            }
            else if(!strcmp("east",userChoice) && currentRoom->east != (struct room*)NULL){
                currentRoom = currentRoom->east;
                firstTimeInRoom = TRUE;
                return;
            }
            else if(!strcmp("south",userChoice) && currentRoom->south != (struct room*)NULL){
                currentRoom = currentRoom->south;
                firstTimeInRoom = TRUE;
                return;
            }
            else if(!strcmp("west",userChoice) && currentRoom->west != (struct room*)NULL){
                currentRoom = currentRoom->west;
                firstTimeInRoom = TRUE;
                return;
            }
            else {
                printf("You can't go that way.\n");
                return;
            }
        } else if(!strcmp("quit",userChoice)){
            exit(0);
        } else if(!strcmp("savegame",userChoice)){
            saveGame(saveName);
            return;
        } else if(!strcmp("savequit",userChoice)){
            saveGame(saveName);
            exit(0);
        } else {
            printf("You can't do that.\n");
            return;
        }
    }
    
}

void loadLists(void){
    FILE* lightingFile = fopen("gamedata/lighting.ndt", "r");
    for(int i=0; i<LIGHTING_ADJ_LENGTH; i++){
        fscanf(lightingFile,"%s",lightingAdjs[i]);
    }
    fclose(lightingFile);

    FILE* sizeFile = fopen("gamedata/size.ndt", "r");
    for(int i=0; i<SIZE_ADJ_LENGTH; i++){
        fscanf(sizeFile,"%s",sizeAdjs[i]);
    }
    fclose(sizeFile);

    FILE* roomFile = fopen("gamedata/rooms.ndt", "r");
    for(int i=0; i<ROOM_WORDS_LENGTH; i++){
        fscanf(sizeFile,"%s",roomWords[i]);
    }
    fclose(roomFile);

    FILE* noteFlavorFile = fopen("gamedata/notesFlavorNouns.ndt", "r");
    for(int i=0; i<NOTE_FLAVOR_NOUNS_LENGTH; i++){
        fscanf(noteFlavorFile,"%s",noteFlavorNouns[i]);
    }
    fclose(noteFlavorFile);
    
}