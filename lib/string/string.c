#include "string.h"
typedef char* moke_va_list;
#define moke_va_start(ap, last) (ap = (char*)&last + sizeof(last))
#define moke_va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))
#define moke_va_end(ap) (ap = (char*)0)

int sameas(char *buffer, char *cmd_name){
    int i = 0;
    while(cmd_name[i] != '\0'){
        if(buffer[i] != cmd_name[i]) return 0;
        i++;
    }
    return (buffer[i] == ' ' || buffer[i] == '\0');
}

void into_string(unsigned int n, char* buffer){
    char temp[12];
    int i = 0, j = 0;

    if(n == 0){
        buffer[j++] = '0';
        buffer[j] = '\0';
        return;
    }

    while(n > 0){
        temp[i++] = (n % 10) + '0';
        n /= 10;
    }

    while(i > 0){
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

int toint(char* str){
    int result = 0;
    int i = 0;
    while(str[i] >= '0' && str[i] <= '9'){
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result;
}

int strlen(char* s){
    int i = 0;
    while(s[i] != '\0') i++;
    return i;
}

void* memcpy(void* dest, const void* src, int n){
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for(int i = 0; i < n; i++){
        d[i] = s[i];
    }
    return dest;
}

int strcmp(const char* s1, const char* s2){
    while(*s1 && (*s1 == *s2)){
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strstr(const char* haystack, const char* needle){
    if(!*needle) return (char*)haystack;
    
    for(char* h =(char*)haystack; *h; h++){
        if(*h == *needle){
            char* h_ptr = h;
            char* n_ptr = (char*)needle;
            
            while(*h_ptr && *n_ptr && *h_ptr == *n_ptr){
                h_ptr++;
                n_ptr++;
            }
            if(!*n_ptr) return h; // Found
        }
    }
    return 0; // Nope
}

int sprintf(char* buffer, const char* format, ...){
    moke_va_list args;
    moke_va_start(args, format);
    
    char* ptr = buffer;
    const char* f = format;
    
    while(*f){
        if(*f == '%' && *(f + 1) != '\0'){
            f++;
            if(*f == 's'){
                char* s = moke_va_arg(args, char*);
                while (*s) *ptr++ = *s++;
            } else if(*f == 'd'){
                int n = moke_va_arg(args, int);
                char temp[12];
                into_string(n, temp);
                char* t = temp;
                while (*t) *ptr++ = *t++;
            }
        } else {
            *ptr++ = *f;
        }
        f++;
    }
    
    *ptr = '\0';
    moke_va_end(args);
    return (ptr - buffer);
}