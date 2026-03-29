void (*mouse_callback)() = 0;

void subscribe_mouse(void (*func)()){
    mouse_callback = func;
}

void on_mouse_interrupt(){
    if(mouse_callback) mouse_callback();
}