#ifndef novel_h
#define novel_h
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "device/video.h"
#define true 1
#define false 0

#define WIDTH 40
#define HEIGHT 25

#define K_UP 0x48
#define K_DOWN 0x50
#define K_LEFT 0x4b
#define K_RIGHT 0x4d
#define K_ESCAPE 0x01
#define K_RETURN 0x1c
#define K_SPACE 0x39
#define K_PAGEUP 0x49
#define K_PAGEDOWN 0x51
#define K_r 0x13
#define K_a 0x1e
#define K_d 0x20
#define K_e 0x12
#define K_w 0x11
#define K_q 0x10
#define K_s 0x1f
#define K_f 0x21
#define K_p 0x19
typedef struct string{
    char* str;
    unsigned length;
    unsigned cap;
    int overflow;
}string;

typedef struct gal_text{
    string* str;
    struct gal_text* next;
}gal_text;
extern int x;
extern int y;
extern int screen_start;
extern int play_next;
extern FILE* novel;
extern gal_text* head;
extern gal_text* tail;
extern uint8_t* local_vmem;

string *getch(FILE* f);

string* init_string(unsigned size);

bool empty(string* s);

bool full(string* s);

void free_string(string* s);

bool push_back(string* s, char c);

bool pop_back(string* s, char* c);

bool resize(string* s, unsigned size);

void print_string(string* s, int);

bool string_cpy(string* dest, char* src);

void put(char c, int color);

void init_novel();

typedef struct{
  string* name;
  string* say;
}sentence;

sentence* make_sentence(string* source);
void play_sentence(sentence* s);
void keyboard_event(void);
void timer_event();
#endif
