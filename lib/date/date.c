#include "date.h"
#include "../../arch/i386/io.h"
#define BCD_TO_BIN(val) ((val & 0xF0) >> 4) * 10 + (val & 0x0F)
#define BIN_TO_BCD(val) ((val / 10) << 4) | (val % 10)

int seconds = 0;
int minutes = 0;
int hours = 0;
int day = 0;
int month = 0;
int year = 0;

void get_date(){
    outb(0x70, 0x00);
    seconds = BCD_TO_BIN(inb(0x71));

    outb(0x70, 0x02);
    minutes = BCD_TO_BIN(inb(0x71));

    outb(0x70, 0x04);
    hours = BCD_TO_BIN(inb(0x71));

    outb(0x70, 0x07);
    day = BCD_TO_BIN(inb(0x71));

    outb(0x70, 0x08);
    month = BCD_TO_BIN(inb(0x71));

    outb(0x70, 0x09);
    year = BCD_TO_BIN(inb(0x71));
}

void set_time(int h, int m, int s){
    if(s != -1){
        outb(0x70, 0x00);
        outb(0x71, BIN_TO_BCD(s));
    }
    if(m != -1){
        outb(0x70, 0x02);
        outb(0x71, BIN_TO_BCD(m));
    }
    if(h != -1){
        outb(0x70, 0x04);
        outb(0x71, BIN_TO_BCD(h));
    }
}

void set_date(int day, int month, int year){
    if(day != -1){
        outb(0x70, 0x07);
        outb(0x71, BIN_TO_BCD(day));
    }

    if(month != -1){
        outb(0x70, 0x08);
        outb(0x71, BIN_TO_BCD(month));
    }

    if(year != -1){
        outb(0x70, 0x09);
        outb(0x71, BIN_TO_BCD(year));
    }
}