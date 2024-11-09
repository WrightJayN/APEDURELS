#ifndef MAIN_H
#define MAIN_H

//Function prototypes
void print_text(int pause);

//Line modification functions
void edit_line(int, const char *);
void add_line(int , const char *);
void delete_line(int );

//User interface functions 
void edit_interface();
void add_interface();
void delete_interface();
int main_menu();
int load_save_interface();

//Stack push and pop functions
void push_undo(char [][], int , int );
void push_redo(char [][], int );
int pop_undo(char [][], int *);
int pop_redo(char [][], int *);

//Undo and redo functions
int undo(char [][], int *);
int redo(char [][], int *);

//Load and save functions
int load_text(const char *, char [][], int *);
int save_text(const char *, char [][], int );

#endif