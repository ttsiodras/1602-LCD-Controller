#include <unistd.h>

#include "Phoney_uino.h"

void delay(int ms)
{
    usleep(ms*1000);

}

void delayMicroseconds(int us)
{
    usleep(us);
}
