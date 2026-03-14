#include "string.h"

int sameas(char *buffer, char *cmd_name){
    int i = 0;
    while(cmd_name[i] != '\0'){
        if (buffer[i] != cmd_name[i]) return 0;
        i++;
    }
    return (buffer[i] == ' ' || buffer[i] == '\0');
}

void into_string(unsigned int n, char* buffer) {
    char temp[12];
    int i = 0, j = 0;

    if (n == 0) {
        buffer[j++] = '0';
        buffer[j] = '\0';
        return;
    }

    while (n > 0) {
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }

    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}