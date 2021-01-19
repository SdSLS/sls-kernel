#include "kernel.h"
#include "keyboard.h"

uint32 vga_index;
static uint32 next_line_index = 1;
uint8 g_fore_color = WHITE, g_back_color = BLUE;
int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) {
    uint16 ax = 0;
    uint8 ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color){
    uint32 i;
    for(i = 0; i < BUFSIZE; i++)
        (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
    next_line_index = 1;
    vga_index = 0;
}

//iniciar el buffer pada el vga mamahuebo
void init_vga(uint8 fore_color, uint8 back_color){
    vga_buffer = (uint16*)VGA_ADDRESS;
    clear_vga_buffer(&vga_buffer, fore_color, back_color);
    g_fore_color = fore_color;
    g_back_color = back_color;
}

void nl(){
    if(next_line_index >= 55){
        next_line_index = 0;
        clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
    }
    vga_index = 80*next_line_index;
    next_line_index++;
}

//iniciar ascii mamaguebo
void print_char(char ch){
    //printeando do char con vga_entry
    vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
    vga_index++;
}


uint32 strlen(const char* str){
    uint32 length = 0;
    while(str[length])
        length++;
    return length;
}

uint32 digit_count(int num){
    uint32 count = 0;
    if(num == 0)
        return 1;
    while(num > 0){
        count++;
        num = num/10;
    }
    return count;
}

void itoa(int num, char *number){
    int dgcount = digit_count(num);
    int index = dgcount - 1;
    char x;
    if(num == 0 && dgcount == 1){
        number[0] = '0';
        number[1] = '\0';
    }else{
        while(num != 0){
            x = num % 10;
            number[index] = x + '0';
            index--;
            num = num / 10;
        }
        number[dgcount] = '\0';
    }
}

//print komo en piton!!
void print(char *str){
    uint32 index = 0;
    while(str[index]){
        print_char(str[index]);
        index++;
    }
}

void print_int(int num){
    char str_num[digit_count(num)+1];
    itoa(num, str_num);
    print(str_num);
}

uint8 inb(uint16 port){
    uint8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void outb(uint16 port, uint8 data){
    asm volatile("outb %0, %1" : "=a"(data) : "d"(port));
}

char get_input_keycode(){
    char ch = 0;
    while((ch = inb(KEYBOARD_PORT)) != 0){
        if(ch > 0)
            return ch;
    }
    return ch;
}
void wait_for_io(uint32 timer_count)
{
    while(1){
        asm volatile("nop");
        timer_count--;
        if(timer_count <= 0)
            break;
    }
}

void sleep(uint32 timer_count){
    wait_for_io(timer_count);
}
char get_ascii_char(char k){
    switch(k){
        case KEY_A: return 'A';
        default: return ' ';
    }
}

void test_input(){
    char ch = 0;
    char keycode = 0;
    do{
        keycode = get_input_keycode();
        if(keycode == KEY_ENTER){
            nl();
        }else{
            ch = get_ascii_char(keycode);
            print_char(ch);
        }
        sleep(0x02FFFFFF);
    }while(ch > 0);
}

//aka inicia do kernel
void kernel_entry(){
    //iniciar vga con blancos y niggas
    init_vga(WHITE, BLACK);

    print("estan en el OS de SLS mamaguebo");
    nl();
    print_int(422);
    nl();
    sleep(0x02FFFFFF);
    print("ingresar algo");
    test_input();
}
