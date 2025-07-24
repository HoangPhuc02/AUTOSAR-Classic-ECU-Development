
# AUTOSAR Classic ECU Development - Ví dụ ADC Driver cho STM32F103

## Giới thiệu

Dự án này cung cấp ví dụ chuẩn về **ADC Driver** theo tiêu chuẩn AUTOSAR Classic MCAL dành cho vi điều khiển STM32F103 (Blue Pill). Dự án sử dụng **CMSIS** và **STM32 Standard Peripheral Library (SPL)**, với module ADC được thiết kế theo chuẩn AUTOSAR, dễ tích hợp và mở rộng. Tất cả các file đầu ra của quá trình build (`.o`, `.elf`, `.bin`) được lưu trữ trong thư mục `Tools/` để quản lý hiệu quả.

Ví dụ bao gồm chương trình đọc giá trị ADC từ hai chân khác nhau (PA0, PA1), minh họa cách sử dụng ADC driver theo cấu trúc AUTOSAR.


## Yêu cầu hệ thống

- **Toolchain**: [GNU Arm Embedded Toolchain (arm-none-eabi-gcc)](https://developer.arm.com/downloads/-/gnu-rm)
- **OpenOCD**: Công cụ để nạp firmware qua ST-Link
- **Make**: Có sẵn trên Linux/macOS; với Windows, cài đặt qua MinGW hoặc WSL
- **Phần cứng**: Board STM32F103C8T6 (Blue Pill)
- **Thiết bị lập trình**: ST-Link (v2 hoặc tương thích)
- **Hệ điều hành**: Linux, macOS, hoặc Windows (khuyến nghị dùng WSL trên Windows để đơn giản hóa)


## Cấu trúc thư mục

```
AUTOSAR-Classic-ECU-Development/
└── 2. ADC Driver/
    ├── CMSIS/
    ├── Config/
    ├── Linker/
    ├── MCAL/
    │   └── Adc/
    │       ├── Adc.c
    │       └── Adc.h
    ├── SPL/
    ├── Startup/
    ├── Tools/
    ├── main.c
    └── makefile
```

- **MCAL/Adc/**: Driver ADC chuẩn AUTOSAR
- **main.c**: Mã ứng dụng mẫu đọc ADC hai chân


## Hướng dẫn build và nạp firmware

Các bước build và nạp firmware tương tự như phần DIO Driver. Thay đổi đường dẫn sang thư mục `2. ADC Driver/`.

## API của ADC Driver AUTOSAR

Dưới đây là các API chuẩn của ADC Driver AUTOSAR:

### 1. Khởi tạo ADC
```c
void Adc_Init(const Adc_ConfigType* ConfigPtr);
```
Khởi tạo module ADC với cấu hình truyền vào.

### 2. Bắt đầu chuyển đổi ADC cho một group
```c
Std_ReturnType Adc_StartGroupConversion(Adc_GroupType Group);
```
Bắt đầu chuyển đổi ADC cho một nhóm kênh.

### 3. Đọc giá trị ADC sau chuyển đổi
```c
Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr);
```
Đọc giá trị ADC đã chuyển đổi cho một nhóm kênh.

### 4. Dừng chuyển đổi ADC
```c
void Adc_StopGroupConversion(Adc_GroupType Group);
```
Dừng chuyển đổi ADC cho một nhóm kênh.

### 5. Callback khi chuyển đổi xong (nếu dùng interrupt)
```c
void Adc_GroupConversionComplete(Adc_GroupType Group);
```
Được gọi khi chuyển đổi ADC hoàn tất.

## Ví dụ sử dụng ADC Driver AUTOSAR cho 2 chân khác nhau

```c
#include "Adc.h"

Adc_ConfigType AdcConfig = {
    // Cấu hình cho 2 kênh ADC (ví dụ: PA0 và PA1)
};

int main(void) {
    Adc_ValueGroupType adcValues[2];

    // Khởi tạo ADC
    Adc_Init(&AdcConfig);

    while (1) {
        // Bắt đầu chuyển đổi cho group 0 (gồm 2 kênh)
        Adc_StartGroupConversion(ADC_GROUP_0);

        // Đợi chuyển đổi xong (polling hoặc interrupt)
        // ...existing code...

        // Đọc giá trị ADC
        Adc_ReadGroup(ADC_GROUP_0, adcValues);

        // Xử lý giá trị adcValues[0] và adcValues[1]
        // ...existing code...

        // Dừng chuyển đổi nếu cần
        Adc_StopGroupConversion(ADC_GROUP_0);
    }
}
```

## Giải thích các tham số

- **Adc_ConfigType**: Cấu trúc chứa thông tin cấu hình ADC (số kênh, sample time, chế độ hoạt động, v.v.).
- **Adc_GroupType**: Kiểu dữ liệu xác định nhóm kênh ADC.
- **Adc_ValueGroupType**: Kiểu dữ liệu chứa giá trị ADC đọc được (thường là mảng uint16_t).
- **Std_ReturnType**: Kiểu trả về chuẩn của AUTOSAR (E_OK hoặc E_NOT_OK).

## Tích hợp với SPL STM32F103

Các API AUTOSAR sẽ ánh xạ sang các hàm SPL như: ADC_Init, ADC_RegularChannelConfig, ADC_GetConversionValue, ...

Nếu cần code mẫu cụ thể cho từng API, hãy bổ sung file Adc.h và Adc.c theo chuẩn AUTOSAR.

## Liên hệ

Vui lòng liên hệ **Hala Academy** để gửi phản hồi hoặc đề xuất cải tiến.
