#include "keyboard_map.h"



/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;


#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C
#define CTRL_KEY_CODE 0x1D

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01
/* there are 25 lines each of 80 columns; each element takes 2 bytes */



struct IDT_entry {
	unsigned short offset_lowerbits;
	unsigned short selector;
    unsigned char ist;
    unsigned short offset_mid;
	unsigned int zero;
	unsigned char type_attr;
	unsigned int offset_higherbits;
};
//*********************************


extern struct IDT_entry IDT[IDT_SIZE];


extern unsigned long long int isr1;
extern void LoadIDT(void);

void load_idt_entry()
{
   for(unsigned long long int t = 0; t < 256; t++) {
    IDT[t].offset_lowerbits = (unsigned short)(((unsigned long long int)&isr1 & 0x000000000000ffff));
    IDT[t].offset_mid = (unsigned short)(((unsigned long long int)&isr1 & 0x00000000ffff0000) >> 16);
    IDT[t].offset_higherbits = (unsigned int)(((unsigned long long int)&isr1 & 0xffffffff00000000) >> 32);
    IDT[t].selector = 0x08;
    IDT[t].type_attr = 0x8e;
    IDT[t].zero = 0;
    IDT[t].ist = 0;
    
    

    RemapPic();

	outb(0x21, 0xfd);
	outb(0xa1, 0xff);
    
    
    LoadIDT();
   }
}

void outb(unsigned short port, unsigned char val){
  asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port){
  unsigned char returnVal;
  asm volatile ("inb %1, %0"
  : "=a"(returnVal)
  : "Nd"(port));
  return returnVal;
}

void RemapPic(){
  unsigned char a1, a2;

  a1 = inb(PIC1_DATA);
  a2 = inb(PIC2_DATA);
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  outb(PIC1_DATA, 0);
  outb(PIC2_DATA, 8);
  outb(PIC1_DATA, 4);
  outb(PIC2_DATA, 2);
  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  outb(PIC1_DATA, a1);
  outb(PIC2_DATA, a2);

}

void isr1_handler(void)
{
	kprintch(inb(0x60));
	/* write EOI */
	outb(0x20, 0x20);
	outb(0xa0, 0x20);
}



void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x02;
	}
}


void kprintch(char str)
{
		vidptr[current_loc++] = str;
		vidptr[current_loc++] = 0x02;
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
	current_loc = 0;
}



void main()
{
    const char *str = "my first kernel";
    char *vidptr = (char*)0xb8000;
    unsigned int i = 0;
    unsigned int j = 0;


    load_idt_entry();


	while(1) __asm__("hlt\n\t");
    return;
} 
