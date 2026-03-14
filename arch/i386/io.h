#ifndef IO_H
#define IO_H

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif