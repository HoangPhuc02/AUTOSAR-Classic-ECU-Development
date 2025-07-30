#include "Port.h"
#include "Dio.h"

void delay(volatile uint32_t t)
{
    while (t--) __asm__("nop");
}

int main(void)
{
    Port_Init();     // Khởi tạo mode các chân (trước)

    while (1)
    {
        Dio_ToggleChannel(DIO_CHANNEL_LED);
        delay(500000);
    }
}
