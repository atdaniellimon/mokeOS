#include "../stdint/types.h"

typedef struct {
    int pid;                // Unique ID
    char name[32];          // Name
    int x, y, w, h;         // Size & pos
    void (*render_ptr)();   // Render logic
    uint32_t* buffer;
} MokeProcess;