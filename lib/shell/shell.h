#ifndef SHELL_H
#define SHELL_H
#include "../stdint/types.h"

extern int showShellText;
extern int cat_commands;
extern int shell_initialized;
extern void* global_mbi_ptr;
extern uint8_t (*current_mouse_design)[16];

void init_shell();
void exec_command(char* command);
void exec_cat_command(char* command);
void start_shell();
void change_cursor(char* cursor);
void shell_main();

#endif  