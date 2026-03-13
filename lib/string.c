#include "string.h"

int sameas(char *buffer, char *cmd_name){
    int i = 0;
    while(cmd_name[i] != '\0'){
        if (buffer[i] != cmd_name[i]) return 0;
        i++;
    }
    return (buffer[i] == ' ' || buffer[i] == '\0');
}