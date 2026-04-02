#ifndef SHELL_H
#define SHELL_H

<<<<<<< Updated upstream:lib/shell.h
extern void* global_mbi_ptr; 
extern int showShellText;
extern int cat_commands;
extern int shell_initialized;

void exec_command(char* command);
void exec_cat_command(char* command);
=======
extern void* global_mbi_ptr;
extern uint8_t (*current_mouse_design)[16];

void start_shell();
void change_cursor(char* cursor);
>>>>>>> Stashed changes:lib/shell/shell.h

#endif