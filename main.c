#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINES 1024
#define MAX_LINE_LENGTH 121
#define MAX_STACK_SIZE 50
#define MAX_FILEPATH_SIZE 50
#define TEST_FILENAME "test_file.txt"

//Global variables and structures
char text[MAX_LINES][MAX_LINE_LENGTH] = {0};
int total_lines = 0;

typedef struct {
    char text[MAX_LINES][MAX_LINE_LENGTH];
    int total_lines;
} TextState;

TextState undo_stack[MAX_STACK_SIZE];
TextState redo_stack[MAX_STACK_SIZE];
int undo_top = -1;
int redo_top = -1;

//Function prototypes
void print_text(int pause);

//Line modification functions
void edit_line(int line_number, const char *new_text);
void add_line(int line_number, const char *new_text);
void delete_line(int line_number);

//User interface functions 
void edit_interface();
void add_interface();
void delete_interface();
int main_menu();
int load_save_interface();

//Stack push and pop functions
void push_undo(char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines, int clear_redo);
void push_redo(char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines);
int pop_undo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines);
int pop_redo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines);

//Undo and redo functions
int clear_buffer_flag = 0;
int undo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines);
int redo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines);

//Load and save functions
int load_text(const char *filename, char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines);
int save_text(const char *filename, char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines);
void get_filepath(char *filepath);

//Test functions
void setup_text();  
void test_add_line();
void test_edit_line();
void test_delete_line();
void test_undo();
void test_redo();
void test_save_text();
void test_load_text();
void run_tests();

int main(){
    main_menu();
    return 0;
}

void print_text(int pause){
    //Display each line in the 2D array 
    //clear screen when the calling from main menu
    if(pause){
        system("cls");
    }
    printf("\n");
    for (int i = 0; i < total_lines; i++) {
        printf("\t%d  %s\n", i, text[i]);
    }
    //pause screen when calling from main menu
    if(pause){
        printf("\n");
        getchar();
    }
}

int main_menu(){
    int choice;
    while (1) {
        system("cls");
        printf("\n\tAPEDURE Text Editor\n");
        printf("\t1.  Add line\n\t2.  Print text\n\t3.  Edit line\n\t4.  Delete line\n\t5.  Undo\n\t6.  Redo\n\t7.  Load File\n\t8.  Save File\n\t9.  Test\n\t10. Exit\n\tEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: add_interface(); break;
            case 2: print_text(1); break;
            case 3: edit_interface(); break;
            case 4: delete_interface(); break;
            case 5: undo(text, &total_lines); break;
            case 6: redo(text, &total_lines); break;
            case 7: load_save_interface(1); break;
            case 8: load_save_interface(0); break;
            case 9: run_tests(); break;
            case 10: return 0; break;
            default: printf("\n\n\tInvalid choice.\n");
        }

        printf("\tPress Enter...");
        if(clear_buffer_flag){
            getchar();
            clear_buffer_flag = 0;
        }
        getchar();
    }
}

void edit_line(int line_number, const char *new_text) {
    //overwrites a selected line in the 2D array
    if (line_number >= total_lines) {
        // Extending the text buffer to accommodate the new line
        total_lines = line_number + 1;
    }
    // Insert the new line at the specified position with null termination
    strncpy(text[line_number], new_text, MAX_LINE_LENGTH - 1);
    text[line_number][MAX_LINE_LENGTH - 1] = '\0'; 
}

void add_line(int line_number, const char *new_text) {
    //Adds a string as a line to 2D array 
    if (line_number >= total_lines) {
        // Extending the text buffer to accommodate the new line
        total_lines = line_number + 1;
    } else {
        // Shift lines down to make space for the new line
        for (int i = total_lines; i > line_number; i--) {
            strncpy(text[i], text[i - 1], MAX_LINE_LENGTH - 1);
            text[i][MAX_LINE_LENGTH - 1] = '\0';  // Ensure null-termination
        }
        total_lines++;
    }

    // Insert the new line at the specified position with null termination
    strncpy(text[line_number], new_text, MAX_LINE_LENGTH - 1);
    text[line_number][MAX_LINE_LENGTH - 1] = '\0';
}

void delete_line(int line_number){
    //deletes a line from the 2D array
    // Shift lines up to fill the gap created by the deleted line
    for (int i = line_number; i < total_lines - 1; i++) {
        strncpy(text[i], text[i + 1], MAX_LINE_LENGTH);
    }

    // Clear the last line
    text[total_lines - 1][0] = '\0';

    // Decrement the total lines count
    total_lines--;
}

void edit_interface(){
    //This function handles different cases:
    //when the selected line is in range
    //when the selected line is -1
    //when the selected line out of range  
    int line_number;
    char new_text[MAX_LINE_LENGTH -1];
    
    system("cls");
    printf("\n\tEnter line number (>=0 and <%d) to edit or -1 to cancel: \n\t", MAX_LINES);
    scanf("%d", &line_number);
    
    if (line_number >= 0 && line_number < MAX_LINES) {
        printf("\n\tEnter new text: ");
        getchar(); // clear newline left in buffer
        fgets(new_text, MAX_LINE_LENGTH -1, stdin);
        
        // Strip newline character if present
        size_t len = strlen(new_text);
        if (len > 0 && new_text[len - 1] == '\n') {
            new_text[len - 1] = '\0';
        }
        //Push current 2D array before editing line
        push_undo(text, total_lines, 1);
        edit_line(line_number, new_text);
    }else if (line_number == -1){
        printf("\n\tOperation Cancelled\n");
        getchar(); // clear newline left in buffer
    }else{
        printf("\n\tInvalid line number.\n");
        getchar(); // clear newline left in buffer
    }
}

void add_interface(){
    //This function handles the following cases:
    //When the line number is in range but is:
    ////less than the total lines
    ////equal or greater than the total lines
    //When the line number is out of range
    int line_number;
    char new_text[MAX_LINE_LENGTH -1];
    
    if (total_lines < MAX_LINES) {
        system("cls");
        printf("\n\tEnter line number (>=0 and <%d) to add or -1 to cancel: \n\t", MAX_LINES);
        scanf("%d", &line_number);

        if (line_number >= 0 && line_number < MAX_LINES) {
            printf("\n\tEnter new text: ");
            getchar(); // clear newline left in buffer
            fgets(new_text, MAX_LINE_LENGTH -1, stdin);
            
            // Strip newline character if present
            size_t len = strlen(new_text);
            if (len > 0 && new_text[len - 1] == '\n') {
                new_text[len - 1] = '\0';
            }
            //Push current 2D array before adding line
            push_undo(text, total_lines, 1);
            add_line(line_number, new_text);
        }else if (line_number == -1){
            printf("\n\tOperation Cancelled\n");
            getchar(); // clear newline left in buffer
        }else{
            printf("\n\tInvalid line number.\n");
            getchar(); // clear newline left in buffer
        }
    } else {
        printf("\n\n\tCannot add more lines, maximum capacity reached.\n");
    }
}

void delete_interface(){
    //This function handles the case:
    //when the line number is:
    //// >= 0
    //// < total_lines
    //when the line number is out of the above range
    int line_number;
    
    system("cls");
    printf("\n\tEnter line number (>=0 and <%d) to delete or -1 to cancel: \n\t", total_lines);
    scanf("%d", &line_number);
    
    if (line_number >= 0 && line_number < total_lines) {
        //Push current 2D array before deleting line
        push_undo(text, total_lines, 1);
        delete_line(line_number);
    }else if (line_number == -1){
        printf("\n\tOperation Cancelled\n");
        getchar(); // clear newline left in buffer
    }else{
        printf("\n\tInvalid line number.\n");
        getchar(); // clear newline left in buffer
    }
}

int load_save_interface(int load_or_save){
    char filepath[MAX_FILEPATH_SIZE];
    system("cls");

    if (load_or_save){
        get_filepath(filepath);   // Get file path from user
        load_text(filepath, text, &total_lines);    // Load file content
    } else {
        get_filepath(filepath);   // Get file path from user    
        save_text(filepath, text, total_lines);   // Save file content
    }
}

void push_undo(char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines, int clear_redo) {
    // If undo stack is full, remove the oldest entry by shifting down
    if (undo_top == MAX_STACK_SIZE - 1) {
        for (int i = 0; i < MAX_STACK_SIZE - 1; i++) {
            undo_stack[i] = undo_stack[i + 1];
        }
        undo_top--;  // Adjust the top index
    }
    
    // Push current state onto the undo stack
    undo_top++;
    memcpy(undo_stack[undo_top].text, text, sizeof(char) * MAX_LINES * MAX_LINE_LENGTH);
    undo_stack[undo_top].total_lines = total_lines;
    
    // Clear the redo stack only if a new action was performed
    if (clear_redo) {
        redo_top = -1;
    }
}

void push_redo(char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines) {
    // If redo stack is full, remove the oldest entry by shifting down
    if (redo_top == MAX_STACK_SIZE - 1) {
        for (int i = 0; i < MAX_STACK_SIZE - 1; i++) {
            redo_stack[i] = redo_stack[i + 1];
        }
        redo_top--;
    }

    // Push current state onto the redo stack
    redo_top++;
    memcpy(redo_stack[redo_top].text, text, sizeof(char) * MAX_LINES * MAX_LINE_LENGTH);
    redo_stack[redo_top].total_lines = total_lines;
}

int pop_undo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines) {
    if (undo_top == -1) {
        printf("Undo stack is empty.\n");
        return 0;  // Indicate that there's nothing to undo
    }
    
    // Save current state to redo stack before undoing
    push_redo(text, *total_lines);
    
    // Retrieve the previous state from the undo stack
    memcpy(text, undo_stack[undo_top].text, sizeof(char) * MAX_LINES * MAX_LINE_LENGTH);
    *total_lines = undo_stack[undo_top].total_lines;
    undo_top--;  // Move the stack pointer down
    
    return 1;  // Indicate that an undo operation was successful
}

int pop_redo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines) {
    if (redo_top == -1) {
        printf("Redo stack is empty.\n");
        return 0;  // Indicate that there's nothing to redo
    }

    // Save current state to undo stack before redoing
    push_undo(text, *total_lines, 0);

    // Retrieve the state to redo from the redo stack
    memcpy(text, redo_stack[redo_top].text, sizeof(char) * MAX_LINES * MAX_LINE_LENGTH);
    *total_lines = redo_stack[redo_top].total_lines;
    redo_top--;  // Move the stack pointer down

    return 1;  // Indicate that a redo operation was successful
}

int undo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines) {
    clear_buffer_flag = 1;
    if (undo_top == -1) {
        printf("\n\n\tNothing to undo.\n");
        return 0;  // Indicate that there's no undo action available
    }
    
    // Push the current state onto the redo stack
    push_redo(text, *total_lines);

    // Pop the previous state from the undo stack
    if(pop_undo(text, total_lines)){
        printf("\n\n\tUndo operation was successfully.\n");
        return 1;
    }
}

int redo(char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines) {
    clear_buffer_flag = 1;
    if (redo_top == -1) {
        printf("\n\n\tNothing to redo.\n");
        return 0;  // Indicate that there's no redo action available
    }
    
    // Push the current state onto the undo stack
    push_undo(text, *total_lines, 0);

    // Pop the state from the redo stack to redo the change
    if(pop_redo(text, total_lines)){
        printf("\n\n\tRedo operation was successfully.\n");
        return 1;
    }
}

int load_text(const char *filename, char text[MAX_LINES][MAX_LINE_LENGTH], int *total_lines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("\n\tError opening file for reading");
        return 0;  // Return 0 if file cannot be opened
    }

    *total_lines = 0;
    while (fgets(text[*total_lines], MAX_LINE_LENGTH, file)) {
        // Remove newline character, if present
        text[*total_lines][strcspn(text[*total_lines], "\n")] = '\0';
        (*total_lines)++;
        
        // Stop if we reach the maximum allowed lines
        if (*total_lines >= MAX_LINES) {
            printf("\n\tWarning: Maximum line limit reached; additional lines will be ignored.\n");
            break;
        }
    }

    fclose(file);
    printf("\n\tLoaded %d lines from %s.\n", *total_lines, filename);
    return 1;  // Return 1 on successful load
}

int save_text(const char *filename, char text[MAX_LINES][MAX_LINE_LENGTH], int total_lines) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("\n\tError opening file for writing");
        return 0;  // Return 0 if file cannot be opened
    }

    for (int i = 0; i < total_lines; i++) {
        fprintf(file, "%s\n", text[i]);
    }

    fclose(file);
    printf("\n\tSaved %d lines to %s.\n", total_lines, filename);
    return 1;  // Return 1 on successful save
}

void get_filepath(char *filepath) {
    getchar();
    printf("\n\tEnter the file path (Press enter to return to Main Menu): \n\t");
    // Read the file path and store it in the filepath variable
    fgets(filepath, MAX_FILEPATH_SIZE, stdin);

    // Remove the newline character from fgets if it's there
    size_t len = strlen(filepath);
    if (len > 0 && filepath[len - 1] == '\n') {
        filepath[len - 1] = '\0';
    }
}

// Test setup function
void setup_text() {
    total_lines = 0; // Start with an empty text
    memset(text, 0, sizeof(text)); // Clear any previous text data
    undo_top = -1;
    redo_top = -1;
}

// Test 1: Add line
void test_add_line() {
    printf("\n\tTest 1: Adding lines\n");
    add_line(0, "Line 1");
    add_line(1, "Line 2");
    add_line(2, "Line 3");
    add_line(2, "Line 4");
    add_line(2, "Line 5");
    print_text(0);
}

// Test 2: Edit line
void test_edit_line() {
    printf("\n\tTest 2: Editing line\n");
    edit_line(2, "Edited Line");
    print_text(0);
}
    
// Test 3: Delete line
void test_delete_line() {
    printf("\n\tTest 3: Deleting line\n");
    push_undo(text, total_lines, 1);
    delete_line(0);
    print_text(0);
}

// Test 4: Undo functionality
void test_undo() {
    printf("\n\tTest 4: Undoing an edit\n");
    undo(text, &total_lines);
    print_text(0);
}

// Test 5: Redo functionality
void test_redo() {
    printf("\n\tTest 5: Redoing an edit\n");
    redo(text, &total_lines);
    print_text(0);
}

// Test for Save functionality
void test_save_text() {
    printf("\n\tTest 6: Saving text to file\n");

    // Save to file
    if (save_text(TEST_FILENAME, text, total_lines)) {
        printf("\n\tText saved successfully to %s\n", TEST_FILENAME);
    } else {
        printf("\n\tFailed to save text to %s\n", TEST_FILENAME);
    }
}

// Test for Load functionality
void test_load_text() {
    printf("\n\tTest 7: Loading text from file\n");
    
    // Load from previously saved file
    if (load_text(TEST_FILENAME, text, &total_lines)) {
        // Print loaded text to confirm contents
        print_text(0);
    } else {
        printf("\n\tFailed to load text from %s\n", TEST_FILENAME);
    }
}

// Run all tests including save and load
void run_tests() {
    system("cls");
    setup_text();
    test_add_line();
    test_edit_line();
    test_delete_line();
    test_undo();
    test_redo();
    test_save_text();
    test_load_text();
    setup_text();
    clear_buffer_flag = 1;
    printf("\n");
}