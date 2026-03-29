#ifndef STRING_H
#define STRING_H

int sameas(char *buffer, char *cmd_name);
void into_string(unsigned int n, char* buffer);
int toint(char* str);
int strlen(char* s);
void* memcpy(void* dest, const void* src, int n);
int strcmp(const char* s1, const char* s2);

#endif