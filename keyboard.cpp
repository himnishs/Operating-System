#include "keyboard.h"

void printf(char*);

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(manager,0x21),
dataport(0x60),
commandport(0x64)
{
    /*
    while(commandport.Read() & 0x1)
    {
        dataport.Read();
    }
    commandport.Write(0xAE); // activate interrupts
    commandport.Write(0x20); // get current state

    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // change the current state
    dataport.Write(status);
    dataport.Write(0xf4);
    */
    while(commandport.Read() & 0x1)
    {
        dataport.Read();

    }
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);
}
KeyboardDriver::~KeyboardDriver()
{

}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    char* foo = "KEYBOARD 0x00";
    char* hex = "0123456789ABCDEF";
    foo[11] = hex[(key >> 4) & 0x0F];
    foo[12] = hex[key & 0x0F];
    printf(foo);
    return esp;    
}