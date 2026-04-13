#ifndef DATE_H
#define DATE_H

extern int seconds;
extern int minutes;
extern int hours;
extern int day;
extern int month;
extern int year;

void get_date();
void set_time(int h, int m, int s);
void set_date(int day, int month, int year);

#endif