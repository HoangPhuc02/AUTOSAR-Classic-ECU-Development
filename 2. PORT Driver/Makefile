# Makefile build cho AUTOSAR-Classic-ECU-Development/1. DIO Driver
# Output (.o, .elf, .bin) sẽ nằm trong thư mục Tools/

# Tên thư mục chứa file build
BUILDDIR = Tools
# Tên file đầu ra (không có phần mở rộng)
TARGET = $(BUILDDIR)/blinkled

# Toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Include path và define macro
CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -Wall -ffreestanding -nostdlib \
         -ICMSIS \
         -IMCAL/Dio \
         -ISPL/inc \
         -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
CFLAGS += -IConfig/Port  \
		  -IConfig

# Linker script
LDSCRIPT = Linker/stm32f103.ld
LDFLAGS = -T$(LDSCRIPT) -nostdlib -Wl,--gc-sections

# Source files
SRCS_C = main.c \
         MCAL/Port/Port.c \
         $(wildcard SPL/src/*.c)
SRCS_C += $(wildcard Config/Port/*.c)

SRCS_S = Startup/startup_stm32f103.s

# List of object files (đặt trong BUILDDIR)
OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS_C)) $(patsubst %.s,$(BUILDDIR)/%.o,$(SRCS_S))

# Tự tạo thư mục build nếu chưa có
$(shell mkdir -p $(BUILDDIR))

# Mục tiêu mặc định
all: $(TARGET).bin

# Biên dịch file C (object .o nằm trong Tools/)
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Biên dịch file ASM (object .o nằm trong Tools/)
$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link thành ELF (trong Tools/)
$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Tạo file .bin từ .elf (trong Tools/)
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Nạp firmware vào Blue Pill (cần openocd, ST-Link)
flash: $(TARGET).bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).bin 0x08000000 verify reset exit"

# Xóa tất cả file build trong Tools/
clean:
	rm -rf $(BUILDDIR)/*

.PHONY: all clean flash
