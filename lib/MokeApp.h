#ifndef MOKEAPP_H
#define MOKEAPP_H

static inline void app_draw_rect(int x, int y, int w, int h, unsigned int color){
    asm volatile(
        "int $0x80"
        :
        : "a"(1),           // SYS_DRAW_RECT
          "b"(x),
          "c"(y),
          "d"((w << 16) | h),
          "S"(color)
        : "memory"
    );
}

static inline void app_draw_string(int x, int y, char* str, unsigned int color){
    asm volatile(
        "int $0x80"
        :
        : "a"(2),           // SYS_DRAW_STRING
          "b"(x),
          "c"(y),
          "d"(str),
          "S"(color)
        : "memory"
    );
}

static inline int app_get_ticks(){
    int result;
    asm volatile(
        "int $0x80"
        : "=a"(result)
        : "a"(3)            // SYS_GET_TICKS
    );
    return result;
}

static inline char app_get_key(){
    int result;
    asm volatile(
        "int $0x80"
        : "=a"(result)
        : "a"(4)            // SYS_GET_KEY
    );
    return (char)result;
}

static inline void app_exit(){
    asm volatile(
        "int $0x80"
        :
        : "a"(5)            // SYS_EXIT
    );
}

#endif