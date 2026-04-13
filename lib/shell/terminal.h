#ifndef TERMINAL_H
#define TERMINAL_H

void print_ram(void* mbi);
char* get_argument(char* full_command);
char* next_arg(char* str);
void tick_command(char* arg);
void print_moke_logo();
void change_time(int h, int m, int s);
void change_date(int day, int month, int year);
void exec_command(char* command);
void exec_cat_command(char* command);
void power(char* options);

#endif