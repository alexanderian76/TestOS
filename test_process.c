#include "types.h"
#include "consts.h"

void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void sleep(uint32 timer_count)
{
  uint32 t = 0xeeeeef * timer_count;
  wait_for_io(t);
}


void process()
{
    while(1)
    {
        
        char keycode = inb(KEYBOARD_DATA_PORT);
        if(keycode == 0x01) {
            break;
        }
        kprint('@');
        sleep(2);
    }
}
