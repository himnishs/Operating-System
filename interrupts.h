#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"

class InterruptManager
{
    public:
        static uint32_t handleInterrupt(uint8_t InterruptNumber, uint32_t esp);
};


#endif