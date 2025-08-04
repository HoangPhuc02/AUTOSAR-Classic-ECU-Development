#include "Port.h"
#include "Lin.h"
#include "system_stm32f10x.h"

extern const Port_ConfigType PortCfg_Port; 
extern const Lin_ConfigType* Lin_ConfigPtr; // Forward declaration of LIN configuration

void delay(uint32_t count)
{
    while (count--);
}
int main(void)
{
    SystemInit(); // set clock to 72MHz
    Port_Init(&PortCfg_Port);     // Khởi tạo mode các chân (trước)
    Lin_Init(Lin_ConfigPtr);      // Khởi tạo giao thức LIN

    while (1)
    {
        const Lin_PduType PduInfoPtr = {
            .Pid = 0x30, // Example PID
            .Cs = LIN_CLASSIC_CS ,
            .Drc = LIN_FRAMERESPONSE_TX,
            .Dl = 2, // Data length
            .SduPtr = (uint8_t[]){0x55, 0xAA} // Example data
        };
        Lin_SendFrame(LIN_CHANNEL_0, &PduInfoPtr);
        // Dio_FlipChannel(DIO_CHANNEL_C13); // Toggle the state of pin C13
        delay(50000); // Delay to simulate periodic transmission
    }
}
