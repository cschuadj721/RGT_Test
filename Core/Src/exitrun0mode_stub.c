#include "stm32h7xx.h"

// Matches the prototype called by the startup
__weak void ExitRun0Mode(void)
{
    // nothing — assume default RUN‑mode power supply is already OK
}