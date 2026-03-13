#ifndef SHELL_H
#define SHELL_H

extern void* global_mbi_ptr; 
extern int showShellText;
extern int cat_commands;
extern int shell_initialized;

void exec_command(char* command);
void exec_cat_command(char* command);

#endif