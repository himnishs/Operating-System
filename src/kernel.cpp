#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "driver.h"
#include "mouse.h"

void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*) 0xb8000;

    static uint8_t x = 0, y = 0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
        }
        
        if(x >= 80)
        {
            y++;
            x = 0;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
            {
                for(x = 0; x < 80; x++)
                {
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}


// Put on keyboard.h to be more concise
class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
    public:
    void OnKeyDown(char c)
    {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

// Move this to the mouse.h file
class MouseToConsole : public MouseEventHandler
{
    int8_t x,y;
    public:

        MouseToConsole()
        {
            uint16_t* VideoMemory = (uint16_t*)0xb8000;
            x = 40;
            y = 12;
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                | (VideoMemory[80*y+x] & 0xF000) >> 4
                                | (VideoMemory[80*y+x] & 0x00FF);   
        }
        void OnMouseButton(int xoffset, int yoffset)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

            x += xoffset;
            if(x >= 80) 
            {
                x = 79;
            }
            else if(x < 0)
            {
                x = 0;
            } 

            y += yoffset;

            if(y >= 25) 
            {
                y = 24;
            }
            else if(y < 0) 
            {
                y = 0;
            }

            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                | (VideoMemory[80*y+x] & 0xF000) >> 4
                                | (VideoMemory[80*y+x] & 0x00FF);
    }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}


extern "C" void kernelMain(void* multiboot_structure, uint32_t)
{
    printf("jdsajdjsajdjsadjsajdjsajdsajdjasjdjsajd\n");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    printf("Stage 1\n");
    DriverManager drvManager;
    PrintfKeyboardEventHandler kbhandler;

    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
    drvManager.AddDriver(&mouse);
    
    printf("Stage 2\n");
    interrupts.Activate();
    printf("Stage 3\n");
    drvManager.ActivateAll();

    while(1);
}