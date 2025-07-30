#include "Port.h"
#include "Dio.h"

void delay(volatile uint32_t t)
{
    while (t--) __asm__("nop");
}
extern const Port_ConfigType PortCfg_Port; // Forward declaration of Port configuration
int main(void)
{
    Port_Init(&PortCfg_Port);     // Khởi tạo mode các chân (trước)

    while (1)
    {
        Dio_FlipChannel(DIO_CHANNEL_C13); // Toggle the state of pin C13
        delay(50000);
    }
}
