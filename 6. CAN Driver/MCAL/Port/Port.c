#include "Port.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// Mapping cấu hình chân tới GPIO vật lý
typedef struct {
    GPIO_TypeDef* port;
    uint16_t      pin;
} Port_ChannelConfigType;

static const Port_ChannelConfigType Port_ChannelConfig[] = {
    // PORT_CHANNEL_LED:
    { GPIOA, GPIO_Pin_5 }
    // Thêm chân khác nếu có
};

// Khởi tạo các chân cần thiết
void Port_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    // Enable clock GPIOA (mở rộng thì phải enable từng port dùng)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA5: Output Push-Pull 2MHz (LED)
    gpio_init.GPIO_Pin = GPIO_Pin_5;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_init);

    // Thêm config cho các chân khác nếu có
}

// API nâng cao: Đổi mode pin động (ít dùng, tham khảo)
void Port_SetPinMode(Port_ChannelType channel, Port_PinModeType mode)
{
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = Port_ChannelConfig[channel].pin;

    if (mode == PORT_PIN_MODE_OUTPUT) {
        gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
        gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    } else if (mode == PORT_PIN_MODE_INPUT) {
        gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    // Có thể bổ sung các mode khác (pull-up, analog...)

    GPIO_Init(Port_ChannelConfig[channel].port, &gpio_init);
}
