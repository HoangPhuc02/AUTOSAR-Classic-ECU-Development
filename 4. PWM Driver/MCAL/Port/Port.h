#ifndef PORT_H
#define PORT_H

#include <stdint.h>

// Định nghĩa các mode chân (tối giản, có thể mở rộng)
typedef enum {
    PORT_PIN_MODE_OUTPUT = 0,
    PORT_PIN_MODE_INPUT,
    // Thêm các mode khác nếu cần
} Port_PinModeType;

// Định danh các chân cần quản lý
typedef enum {
    PORT_CHANNEL_LED = 0,   // PA5
    // Thêm các channel khác nếu cần
} Port_ChannelType;

// API khởi tạo Port (tất cả chân cần dùng)
void Port_Init(void);

// API thay đổi mode động cho từng chân (nâng cao, optional)
void Port_SetPinMode(Port_ChannelType channel, Port_PinModeType mode);

#endif // PORT_H
