#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Enumerations
typedef enum { EMPTY, MONSTER, TREASURE } RoomType;

// Structures
typedef struct Room {
    RoomType type;
    int value; 
    struct Room *next;
} Room;

typedef struct {
    char* name;
    int strength;
    int health;
    int coins;
} Player;

// Function Prototypes
Room* createRoom(int index);
Room* createDungeon(int numRooms);
void deleteDungeon(Room *dungeon);
void displayRoom(const Room* room);
void playerAction(Player* player, Room* room, int roomIndex);
void fightMonster(Player* player, int monsterStrength, int roomLevel);
void gameLoop(Player* player, Room *dungeonHead);
void playGame(int numRooms);

// Main Function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_rooms>\n", argv[0]);
        return 1;
    }

    int numRooms = atoi(argv[1]);
    if (numRooms <= 0) {
        printf("Number of rooms should be a positive integer.\n");
        return 1;
    }

    playGame(numRooms);

    return 0;
}

// Function Implementations
Room* createRoom(int index) {
    Room *room = malloc(sizeof(Room));
    room->type = rand() % 3; // Randomly assign room type
    
    // Progressive difficulty: Scale monster strength and treasure value based on room depth (index)
    if (room->type == MONSTER) {
        room->value = ((rand() % 5 + 1) * 10) + (index * 5); // Base strength + depth scaling
    } else if (room->type == TREASURE) {
        room->value = ((rand() % 50 + 10)) + (index * 10);   // Base coins + depth scaling
    } else {
        room->value = 0; // Empty room
    }
    
    room->next = NULL;
    return room;
}

Room* createDungeon(int numRooms) {
    Room *head = NULL, *current = NULL;
    for(int i = 0; i < numRooms; ++i) {
        if (i == 0) {
            head = current = createRoom(i);
        } else {
            current->next = createRoom(i);
            current = current->next;
        }
    }
    return head;
}

void deleteDungeon(Room *dungeon) {
    while (dungeon != NULL) {
        Room *temp = dungeon;
        dungeon = dungeon->next;
        free(temp);
    }
}

void displayRoom(const Room* room) {
    const char* roomType = room->type == EMPTY ? "an empty room"
                             : room->type == MONSTER ? "a room with a monster"
                             : "a room with treasure";
    printf("You enter %s.\n", roomType);
}

void playerAction(Player* player, Room* room, int roomIndex) {
    switch (room->type) {
        case MONSTER:
            fightMonster(player, room->value, roomIndex + 1);
            break;
        case TREASURE:
            player->coins += room->value; // Update player's coin count
            printf("You found a treasure worth %d coins! Total coins: %d\n", room->value, player->coins);
            break;
        case EMPTY:
            printf("There's nothing here...\n");
            break;
    }
}

void fightMonster(Player* player, int monsterStrength, int roomLevel) {
    int fleeFee = roomLevel * 10; 
    int choice = 1;

    printf("A monster appears! It has a strength of %d!\n", monsterStrength);
    printf("Choose your action:\n");
    printf("1. Fight\n");
    printf("2. Flee (Cost: %d coins)\n", fleeFee);
    printf("Enter your choice: ");
    
    if (scanf("%d", &choice) != 1) {
        choice = 1; // Default to fighting if input is invalid
    }

    if (choice == 2) {
        if (player->coins >= fleeFee) {
            player->coins -= fleeFee;
            printf("You successfully fled from the monster, paying a fee of %d coins. Remaining coins: %d\n", fleeFee, player->coins);
            return; // Successfully fled without combat or health loss
        } else {
            printf("You cannot afford the flee fee (%d coins required, you have %d)! You are forced to fight!\n", fleeFee, player->coins);
        }
    }

    // Combat mechanism
    if (player->strength >= monsterStrength) {
        printf("You defeated the monster!\n");
    } else {
        printf("The monster overpowers you. You flee back, taking damage.\n");
        player->health -= monsterStrength;
        if (player->health <= 0) {
            printf("You have died...\n");
            return;
        } else {
            printf("You survived the encounter, but you are wounded. Current Health: %d\n", player->health);
        }
    }
}

void gameLoop(Player* player, Room *dungeonHead) {
    Room *currentRoom = dungeonHead;
    int roomIndex = 0;
    while (currentRoom != NULL && player->health > 0) {
        printf("\n----------------------------------------\n");
        printf("You are in room %d.\n", roomIndex + 1);
        displayRoom(currentRoom);
        playerAction(player, currentRoom, roomIndex);
        
        if (player->health <= 0) {
            printf("%s has fallen in battle...\n", player->name);
            break;
        }
        currentRoom = currentRoom->next;
        roomIndex++;
    }
    if (player->health > 0) {
        printf("\nCongratulations, %s! You've cleared the dungeon with %d coins remaining!\n", player->name, player->coins);
    }
}

void playGame(int numRooms) {
    srand((unsigned int)time(NULL)); // Seed the random number generator

    Room *dungeonHead = createDungeon(numRooms); // Create the dungeon

    Player player = {"Adventurer", 20, 100, 0}; // Name, strength, health, starting coins

    gameLoop(&player, dungeonHead); // Start the game loop

    deleteDungeon(dungeonHead); // Cleanup memory
}