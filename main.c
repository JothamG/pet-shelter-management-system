#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Outline, File handling, function tweaking by Gavin Campbell 1002233506
// searchPets(), addPet(), and removePet() Functions by Jotham Golingo 1001817327
// Donation Platform for Pet Supplies feature, fixed compiler warning, and added safe enum reading by Edna Mateo 1001773697

//--------------------------    STRUCTURES / ENUMS    --------------------------

enum AnimalType{
    dog,
    cat,
    rodent,
    bird,
    reptile,
    fish,
    other // Specific type can be given in Animal Breed variable
};

#define NAME_SIZE 50
#define ANIMALBREED_SIZE 100
#define SIZE_SIZE 10
#define NOTES_SIZE 200
#define SHELTER_COUNT 3

struct PetInfo{
    char name[NAME_SIZE];
    enum AnimalType animalType;                 // Type of animal (dog, cat, bird, etc.)
    char animalBreed[ANIMALBREED_SIZE];         // Animal breed (specifies animal in the case of Other)
    int age;
    char size[SIZE_SIZE];
    float weight;
    char notes[NOTES_SIZE];                     // Additional notes if needed
    int shelter;                                // Shelter index
    int ID;
};

struct Node{
    struct PetInfo pet;
    struct Node* next;  // Pointer to next node in chain
};

//-------------------------    GLOBAL VARIABLES    ---------------------------

struct Node *petListHead = NULL;
int petListSize = 0; //We can keep track of how many animals there are
int nextID = 1;

FILE *PETFILE;

const char* shelters[SHELTER_COUNT][5] = 
{   //  Name                    Hours               Donation Preferences             Location            Contact Info
    {"Cooper Street Shelter", "9 A.M. -> 5 P.M.", "Any Food, Bowls, Towels", "123 South Cooper St.", "cooperstpets@cst.com / 817-555-2010"},
    {"Club Hill Animal Shelter", "9 A.M. -> 6 P.M.", "Toys, Bowls, Leashes", "456 Pecan Ave.", "chillshelter@mail.com / 682-555-1450"},
    {"Paws On Rescue", "8 A.M. -> 4 P.M.", "Beds, Litter, Dog and Cat Food", "7890 Milan Dr.", "915-555-0360"}
};

//--------------------------    HELPER FUNCTIONS    --------------------------

// Reusable prompt functions to reduce clutter and make debugging easier
// Values will be put into pointer variable given to function

void intPrompt(int *uc, char prompt[], int maximum) { // If you don't want a maximum, just set it to the max integer
    printf("%s", prompt);
    do {
        scanf("%d", uc);
        getchar(); // Remove newline character
        if(*uc > maximum || *uc < 0) {
            printf("Invalid input. Try again: ");
        }
    }
    while(*uc > maximum || *uc < 0);
}

void floatPrompt(float *uc, char prompt[]) {
    printf("%s", prompt);
    scanf("%f", uc);
    getchar(); // Remove newline character
}

void charPrompt(char *uc, char prompt[]) {
    printf("%s", prompt);
    scanf(" %c", uc);
    getchar(); // Remove newline character
}

void stringPrompt(char uc[], char prompt[], int size) {
    printf("%s", prompt);
    fgets(uc, size, stdin);
    int len = strlen(uc);
    if (len > 0 && uc[len-1] == '\n'){ // Did we include the newline?
        uc[len-1] = '\0'; // Remove it
    }
    else { // Newline was not stored in our string, meaning there is more input left in stdin that we need to remove to avoid it affecting future stdin readings
        int test = 0;
        while((test = getchar()) != '\n' && test != EOF);
    }
}

void returnPrompt() {
    char returnToMenu[1];
    stringPrompt(returnToMenu, "\nPress Enter to continue. ", 1);
}

// Used for reading pet data only
void stringFromFile(char string[], int size) {
    fgets(string, size, PETFILE);
    int len = strlen(string);
    if (len > 0 && string[len-1] == '\n'){ // Did we include the newline?
        string[len-1] = '\0'; // Remove it
    }
    else { // Newline was not stored in our string, so we need to remove it. Only needs to execute once since we know the format of the file
        getc(PETFILE);
    }
}

// Print out the information for a given Pet struct.
void printPet(struct Node *node){
    struct PetInfo *inPet = &node->pet;
    char animalTypes[7][10] = {"Dog", "Cat", "Rodent", "Bird", "Reptile", "Fish", "Other"}; //Array for enum values so we can print them out

    printf("----------\n   ID: %d   Shelter: %s\n", inPet->ID, shelters[inPet->shelter][0]);
    printf("Name: %s\n", inPet->name);
    printf("Type: %s\n", animalTypes[inPet->animalType]);
    printf("Breed: %s\n", inPet->animalBreed);
    printf("Age: %d years\n", inPet->age);
    printf("Size: %s\n", inPet->size);
    printf("Weight: %.2f lbs\n", inPet->weight);
    if (strlen(inPet->notes)> 0){
        printf("Notes: %s\n", inPet->notes);
    }
}

struct Node *createNewNode() {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->next = NULL;
    
    if(petListHead == NULL){
        petListHead = newNode;
    }
    else {
        struct Node* current = petListHead;
        while (current->next !=NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    petListSize++;
    return(newNode);
}

// Read and copy values from PETFILE
// This should only be called at the beginning of the program
void readPetsFromFile() {
    int petNum;
    int emptyCheck = fscanf(PETFILE, "%d\n%d\n", &petNum, &nextID); // Update pet list size and the next ID from file info
    if(emptyCheck == EOF) // File is empty
    {
        printf("\nNo saved pet data to load.\n");
        petListSize = 0;
        nextID = 1;
    }
    else { // File has contents
        struct Node *current = petListHead;
        printf("\nLoading saved pet info...\n");
        for(int i = 0; i < petNum; i++) {
            current = createNewNode();
            struct PetInfo *pet = &current->pet;

            // Read current index info from file
            fscanf(PETFILE, "%d", &pet->ID);                     // ID
            getc(PETFILE); // Remove Newline

            fscanf(PETFILE, "%d", &pet->shelter);                // Shelter Index
            getc(PETFILE); // Remove Newline

            stringFromFile(pet->name, NAME_SIZE);                // Name
            
            //Read animal type as int first the assign to enum
            //Avoids compiler warnings
            int tempType;
            fscanf(PETFILE, "%d", &tempType);              // Animal Type
            getc(PETFILE); // Remove Newline

            //Reads enum safely, avoids warnings
            if(tempType >= 0 && tempType <=6)
            {
                pet->animalType = tempType;
            }
            else
            {
                pet->animalType = other; //Set to 'other' if ivalid value is found
            }

            stringFromFile(pet->animalBreed, ANIMALBREED_SIZE);   // Animal Breed

            fscanf(PETFILE, "%d", &pet->age);                     // Age
            getc(PETFILE); // Remove Newline

            stringFromFile(pet->size, SIZE_SIZE);                 // Size

            fscanf(PETFILE, "%f", &pet->weight);                  // Weight
            getc(PETFILE); // Remove Newline

            stringFromFile(pet->notes, NOTES_SIZE);               // Notes
        }
        printf("\nDone.\n");
    }
}

// Save current pet list to PETFILE
void savePetsToFile() {
    PETFILE = fopen("petSavedValues.txt", "w+"); // If the file exists, it will be overwritten; If not, it will be created
    struct Node *current = petListHead;
    fprintf(PETFILE, "%d\n%d\n\n", petListSize, nextID); // Store number of pets and the next ID at the start of the file
    for(int i = 0; i < petListSize; i++) {
        struct PetInfo *pet = &current->pet;
        // Print current index info to file
        fprintf(PETFILE, "%d\n%d\n%s\n%d\n%s\n%d\n%s\n%f\n%s\n", pet->ID, pet->shelter, pet->name, pet->animalType, pet->animalBreed, pet->age, pet->size, pet->weight, pet->notes);
        current = current->next;
    }
    fclose(PETFILE); // We are done writing
}

//--------------------------    MAIN FUNCTIONS    --------------------------

// Function for pet search menu
void searchPets() {
    int menuChoice;
    intPrompt(&menuChoice, "\n--- Pet Search ---\n 1 - Search by Animal Type\n 2 - Search by Animal ID\nSelection: ", 2);
    printf("\n\n");

    struct Node* current = petListHead;
    int index = 0;
    int found = 0;

    if (menuChoice == 1){
        int Choice;
        intPrompt(&Choice, "What Type of Animal do you want to Search for?\n 0 - Dog\n 1 - Cat\n 2 - Rodent\n 3 - Bird\n 4 - Reptile\n 5 - Fish\n 6 - OTHER\n 7 - ALL [No Filter]\n\nSelection: ", 7);
        printf("\n\n");

        while (current != NULL){
            if (current->pet.animalType == Choice || Choice == 7){
                found = 1;
                printPet(current);
            }
            current = current->next;
            index++;
        }
        if(!found){
            printf("\nNo pets found matching that filter.\n");
            returnPrompt();
        }
        else {
            printf("\n===== END OF LIST =====\n\n");
            returnPrompt();
        }
    }
    else {
        int searchID;
        intPrompt(&searchID, "Enter pet ID to Search: ", nextID);
        char animalTypes[7][10] = {"Dog", "Cat", "Rodent", "Bird", "Reptile", "Fish", "Other"};
        printf("\n\n");

        while (current != NULL){
            if (current->pet.ID == searchID){
                printPet(current);
                found = 1;
                break;
            }
            current = current->next;
            index++;
        }
        if (!found) {
            printf("\nNo pet found with ID %d.\n", searchID);
            returnPrompt();
        }
        else {
            printf("\n===== END OF LIST =====\n");
            returnPrompt();
        }
    }
}

//Function to add a pet to the Linked List
void addPet(){
    char exit; // Store user responses

    charPrompt(&exit, "\n--- Add a Pet ---\nDo you wish to add a pet? (y/n): ");
    exit = tolower(exit);
    if (exit == 'n'){
        printf("Returning to the main menu. \n");
        return;
    }
    
    struct Node *newNode = createNewNode();
    struct PetInfo *newPet = &newNode->pet;

    newNode->pet.ID = nextID;
    nextID++;

    // Prompt and Store the values for this pet
    stringPrompt(newPet->name, "\n[1/8] Enter the pet's name: ", sizeof(newPet->name));
    
    printf("\n[2/8] Which shelter is this pet at? [number]\n");
    for(int i = 0; i < SHELTER_COUNT; i++) {
        printf("%d - %s\n", i, shelters[i][0]);
    }
    intPrompt(&newPet->shelter, "Selection: ", SHELTER_COUNT);
    
    int temp;
    intPrompt(&temp, "\n[3/8] Enter the Type of Animal\n 0 - Dog\n 1 - Cat\n 2 - Rodent\n 3 - Bird\n 4 - Reptile\n 5 - Fish\n 6 - OTHER\nSelection: ", 6);
    newPet->animalType = temp;

    stringPrompt(newPet->animalBreed, "\n[4/8] Enter the Breed of the pet (e.g. Golden Retriever, Hermit Crab): ", sizeof(newPet->animalBreed));
    
    intPrompt(&newPet->age, "\n[5/8] How old is the pet? (in years) ", 999);
    
    stringPrompt(newPet->size, "\n[6/8] Is the pet Small, Medium, or Large in size? ", sizeof(newPet->size));
    
    floatPrompt(&newPet->weight, "\n[7/8] How much does the pet weigh? (in lbs) ");
    
    stringPrompt(newPet->notes, "\n[8/8] Enter any additional notes: ", sizeof(newPet->notes));
    
    printf("\nThe Pet has been added. \n");
    savePetsToFile(); // Store the new list
    returnPrompt();
}

// Remove a pet from the database
void removePet(){
    char exit;
    charPrompt(&exit, "\n--- Remove a Pet ---\nDo you wish to remove a pet? (y/n): ");
    exit = tolower(exit);
    if (exit == 'n'){
        printf("Going back to the main menu. \n");
        return; // Exit to menu
    }

    struct Node *current = petListHead, *previous = NULL;

    int petID;
    intPrompt(&petID, "Enter the ID of the animal you want to remove: ", 999999);

    while (current != NULL) { // Repeat until we reach the end of the chain
        if (current->pet.ID == petID) { // Is the current node the correct ID?
            if (previous == NULL) {          // If there's nothing before this...
                petListHead = current->next; // ...then start the chain at the next node
            }
            else {
                previous->next = current->next; // Remove this node from the chain by connecting the previous to the next
            }
            free(current);
            petListSize--;
            printf("Pet with ID %d has been removed. \n", petID);
            savePetsToFile(); // Store the new list
            returnPrompt();
            return; // Pet has been removed, we can go back now
        }
        previous = current;
        current = current->next;
    }
    printf("No pet found with ID %d.\n", petID); // Search unsucessful
    returnPrompt();
}

// Show list of shelters and related information, such as hours or things they need donated
void shelterResources() {
    int choice;
    do {
        printf("\n--- Pet Shelter Search ---\n\n");
        for (int i=0; i < SHELTER_COUNT; i++) {
            printf("%d - %s\n", i+1, shelters[i][0]);   // Print names of all shelters and their index
        }

        intPrompt(&choice, "\nEnter shelter number to view information, or 0 to return: ", SHELTER_COUNT);

        if(choice > 0) {
            printf("\n%s\n", shelters[choice-1][0]);
            printf("Location: %s\n", shelters[choice-1][3]);
            printf("Hours: %s\n", shelters[choice-1][1]);
            printf("Contact Information: %s\n", shelters[choice-1][4]);
            printf("Donation Preference: %s\n", shelters[choice-1][2]);
            returnPrompt();
        }
        else {
            printf("\nReturning to main menu.\n\n");
        }
    }
    while(choice != 0);
}

int main()
{
    int menuOption;

    PETFILE = fopen("petSavedValues.txt", "r");
    // Only read from file if it exists. Otherwise, start with fresh list.
    if(PETFILE != NULL) // File opened
    {
        readPetsFromFile(); // Auto initialize list to the saved elements in the pet file
        fclose(PETFILE);
    }
    else 
    {
        printf("No pet file found. Starting with an empty list.\n");
    }

    do{
        intPrompt(&menuOption, "\n=========== Pet Shelter Main Menu ===========\n [1] - Search Pets\n [2] - Add Pet\n [3] - Remove Pet\n [4] - Shelter Resources / Donation Info\n [0] EXIT\n\nSelection: ", 4);
        switch(menuOption) {
            case 1:
                // --- TODO ---
                searchPets();
                break;
            case 2:
                addPet();
                break;
            case 3:
                removePet();
                break;
            case 4:
                shelterResources();
                break;
            case 0: // Exit the program
                printf("Thank you for using Cooper Street Pet Adoption Services. Goodbye!\n\n");
                break;
        }
    }
    while(menuOption != 0);

    return 0; // Program exit
}