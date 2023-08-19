#include "novel.h"
#include "common.h"
int play_next=0;
int x=0;
int y=0;
int screen_start=0;
static int screen_save=0;
static string end_string = {"END OF NOVEL, PRESS Q TO EXIT\n", 30, 30, 0};
bool toSave=false;
FILE* novel;
gal_text* head;
gal_text* tail;
char font8x8_basic[128][8];

static uint8_t local_vbuf[SCR_SIZE * 0xff];

uint8_t *local_vmem = local_vbuf; 

static void display_buffer_(void)
{
	asm volatile("cld; rep movsl"
				 :
				 : "c"(SCR_SIZE / 4), "S"(local_vmem + screen_start * (SCR_WIDTH)), "D"(VMEM_ADDR));
}

static inline void incre_start(){
	for(int i = 0;i < 8;++i, ++screen_start, display_buffer_());
}

static inline void decre_start(){
	for(int i = 0;i < 8;++i, --screen_start, display_buffer_());
}

static inline void
draw_pixel_(int x, int y, int color)
{
	assert(x >= 0 && y >= 0 && x < SCR_HEIGHT && y < SCR_WIDTH);
	x += (screen_start);
	local_vmem[(x << 8) + (x << 6) + y] = color;
}

static inline void roll_screen_down(){
	if(toSave){
		incre_start();
		screen_save = screen_start;
		toSave=0;
	}else{
		if(screen_start < screen_save){
			incre_start();
		}
	}
}

static inline void roll_screen_up(){
	if(screen_start > 0){
		decre_start();
	}
}

static inline void update_x(){
    if(x+8>=(HEIGHT<<3)){
	toSave=1;
        roll_screen_down();
    }else{
        x += 8;
    }
}

static inline void update_y(){
    if(y+8>=(WIDTH<<3)){
        update_x();
        y = 0; 
    }else{
        y += 8;
    }
}

void init_novel(){
    novel = fopen("novel.txt", "r");    
    if(!novel){
        _Exit(-1);
    }
    string* p = getch(novel);
    if(p == NULL) _Exit(-1);
    head = (gal_text*)malloc(sizeof(gal_text));
    gal_text *node = (gal_text*)malloc(sizeof(gal_text));
    head->next = node;
    node->str = p;
    tail = node;
    while(!feof(novel)){
        node = (gal_text*)malloc(sizeof(gal_text));
        node->str = getch(novel);
        if(!node->str)break;
        tail->next = node;
        tail = node;
    }
}

static void putch_(char ch, int x, int y, int color){
	int i, j;
    if(ch & 0x80){
        Log("invalid char : %.2x", ch);
	    assert((ch & 0x80) == 0);
    }
	char *p = font8x8_basic[(int)ch];
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if ((p[i] >> j) & 1)
			{
				draw_pixel_(x + i, y + j, color);
			}
		}
	}
}

void put(char c, int color){
    while(screen_start != screen_save){screen_start+=8;display_buffer_();}
    if(c=='\n'){
        y = 0;
        update_x();
    }else if(c=='\t'){
        update_y();
        update_y();    
    }else{
        putch_(c, x, y, color);
        update_y();
        
    }
    display_buffer_();
}

string *init_string(unsigned size) {
  string *s = malloc(sizeof(string));
  s->str = NULL;
  s->str = malloc(sizeof(char) * size);
  memset(s->str, '\0', sizeof(char) * size);
  if (!s->str) {
    return NULL;
  }
  s->length = 0;
  s->cap = size;
  return s;
}

static bool isspace(string* s){
    bool is=true;
    for(int i = 0;i < s->length;++i){
        if(!strchr(" \t\n",s->str[i])){
            is=false;
            break;
        }
    }
    return is;
}

static void play(){
    static int claimed_end = 0;
    while(isspace(head->next->str) && head != tail){
        head=head->next;
    }
    if(head==tail){
        if(!claimed_end){
            print_string(&end_string, 10);     
            claimed_end = 1;
        }    
        return;        
    }
    play_sentence(make_sentence(head->next->str));
    head = head->next;
}
void keyboard_event(void){
    static int32_t last_key_code=0xabcd;
    int32_t code = in_byte(0x60) & 0x7f;
    static int32_t updown_speed=0;
    if(code == K_q){
        HIT_GOOD_TRAP;   
    }else if(code == K_UP){
	if(!play_next){
		if(last_key_code == code){
			updown_speed++;		
		}else updown_speed=0;
		for(int i = 0;i <= updown_speed/5;++i){
			roll_screen_up();
		}
		++play_next;
	}else play_next=0;
    	last_key_code = code;
	return;
    }else if(code == K_DOWN){
	if(!play_next){
		toSave=0;
		if(last_key_code == code){
			updown_speed++;		
		}else updown_speed=0;
		for(int i = 0;i <= updown_speed/5;++i){
			roll_screen_down();
		}
		++play_next;
	}else play_next=0;
    	last_key_code = code;
	return;
    }    
    last_key_code = code;
    play_next++;
    if(play_next==2){
        play_next=0;
        play();
    }
}

void timer_event(){
	
}

bool empty(string *s) { return s->length == 0; }

bool full(string *s) { return s->cap == s->length; }

void free_string(string *s) {
  free(s->str);
  s = NULL;
}

bool push_back(string *s, char c) {
  if (full(s)) {
    resize(s, s->cap * 2);
  }
  s->str[s->length] = c;
  s->length++;
  return true;
}

bool pop_back(string *s, char *c) {
  if (empty(s)) {
    return false;
  }
  *c = s->str[s->length];
  s->str[s->length] = '\0';
  s->length--;
  return true;
}

bool resize(string *s, unsigned size) {
  char *n = malloc(sizeof(char) * size);
  memset(n, 0, sizeof(char) * size);
  if (n == NULL) {
    return false;
  }
  char *old = s->str;
  s->str = n;
  for (int i = 0; i < size && i < s->length; i++) {
    n[i] = old[i];
  }
  free(old);
  if (s->length > size) {
    s->length = size;
  }
  s->cap = size;
  return true;
}

void print_string(string *s, int color) {
  for (int i = 0; i < s->length; i++) {
    if (s->str[i] != '\\') {
      put(s->str[i], color);
    }
  }
}

bool string_cpy(string *dest, char *src) {
  free(dest->str);
  dest->length = 0;
  dest->str = malloc(sizeof(char) * dest->cap);
  memset(dest->str, 0, sizeof(char) * dest->cap);
  if (dest->cap < strlen(src)) {
    resize(dest, strlen(src));
  }
  char *dummy = strncpy(dest->str, src, strlen(src));
  if (dummy == NULL) {
    return false;
  }
  dest->length = strlen(src);
  return true;
}

string *getch(FILE *f) {
  char buf[114514];
  fgets(buf, 114514, f);
  if(feof(f))return NULL;
  string* p = init_string(10);
  string_cpy(p, buf);
  return p;
}

sentence *make_sentence(string *source) {
  char *src = source->str;
  if (src == NULL) {
    return NULL;
  }
  char *split = src;
  do {
    if (*split == '\0') {
      split = NULL;
      break;
    }
    if (*split == '\\') {
      split += 2;
    } else {
      split++;
    }
  } while (*split != ':');
  sentence *s = malloc(sizeof(sentence));
  if (split != NULL) {
    s->say = init_string(strlen(split + 1));
    if (!string_cpy(s->say, split + 1)) {
      return NULL;
    }
    *split = '\0';
    s->name = init_string(strlen(src));
    if (!string_cpy(s->name, src)) {
      return NULL;
    }
  } else {
    s->name = NULL;
    s->say = init_string(strlen(src));
    string_cpy(s->say, src);
  }
  return s;
}



void play_sentence(sentence *s) {
  if (s->name) {
    print_string(s->name, 11);
  }
  put('\n', 15);
  put('\t',15);
  print_string(s->say, 15);
  put('\n',15);
}
