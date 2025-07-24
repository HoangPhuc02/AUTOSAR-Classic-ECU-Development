#include "stm32f10x.h"

void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_Config(void) {
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /**
     * @brief  Cấu hình các thông số cho bộ chuyển đổi ADC trên STM32F103 sử dụng SPL.
     *
     * - ADC_Mode: 
     *   + ADC_Mode_Independent: Chế độ độc lập, mỗi ADC hoạt động riêng biệt. 
     *     Sử dụng khi chỉ cần lấy mẫu từ một hoặc nhiều kênh mà không cần đồng bộ với ADC khác.
     *
     * - ADC_ScanConvMode:
     *   + ENABLE: Cho phép quét nhiều kênh ADC liên tục. 
     *     Sử dụng khi cần đọc giá trị từ nhiều chân ADC trong một lần chuyển đổi (ví dụ: đo nhiều cảm biến cùng lúc).
     *
     * - ADC_ContinuousConvMode:
     *   + ENABLE: Cho phép ADC chuyển đổi liên tục không ngừng. 
     *     Sử dụng khi cần lấy dữ liệu liên tục (ví dụ: giám sát tín hiệu analog thời gian thực).
     *
     * - ADC_ExternalTrigConv:
     *   + ADC_ExternalTrigConv_None: Không sử dụng trigger ngoài, chuyển đổi bắt đầu bằng phần mềm. 
     *     Sử dụng khi muốn chủ động bắt đầu chuyển đổi bằng phần mềm, không phụ thuộc vào tín hiệu ngoài.
     *
     * - ADC_DataAlign:
     *   + ADC_DataAlign_Right: Căn phải dữ liệu kết quả ADC. 
     *     Sử dụng khi muốn dữ liệu kết quả nằm ở phía phải của thanh ghi dữ liệu (phổ biến khi xử lý số nguyên).
     *
     * - ADC_NbrOfChannel:
     *   + 2: Số lượng kênh ADC cần chuyển đổi (ví dụ: đọc giá trị từ 2 cảm biến khác nhau).
     *     Sử dụng khi cần lấy mẫu từ nhiều kênh, giá trị này phải khớp với số lượng kênh được cấu hình tiếp theo.
     */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 2;

    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

int main(void) {
    uint16_t adc_value[2];
    GPIO_Config();
    ADC_Config();
    while(1) {
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        adc_value[0] = ADC_GetConversionValue(ADC1);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        adc_value[1] = ADC_GetConversionValue(ADC1);
        // Xử lý giá trị adc_value[0] và adc_value[1] ở đây
    }
}
