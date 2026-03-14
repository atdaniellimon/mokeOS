#ifndef SHELL_H
#define SHELL_H

extern int showShellText;
extern int cat_commands;
extern int shell_initialized;
extern void* global_mbi_ptr;

void init_shell();
void exec_command(char* command);
void exec_cat_command(char* command);

#endif