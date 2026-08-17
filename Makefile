TARGET    = qemu_stm32
BUILD_DIR = build

CC        = arm-none-eabi-gcc
CXX       = arm-none-eabi-g++
SIZE      = arm-none-eabi-size

SRCS_S = \
    freertos_port/src/startup.s

SRCS_C = \
    freertos_port/src/stm32f4xx_hw.c \
    freertos_port/src/syscalls.c \
    FreeRTOS/tasks.c \
    FreeRTOS/list.c \
    FreeRTOS/queue.c \
    FreeRTOS/portable/GCC/ARM_CM4F/port.c \
    FreeRTOS/portable/MemMang/heap_4.c

SRCS_CPP = \
    freertos_port/src/main.cpp


INCLUDES  = -Ifreertos_port/inc
INCLUDES += -IFreeRTOS/include
INCLUDES += -IFreeRTOS/portable/GCC/ARM_CM4F


ARCH_FLAGS = \
    -mcpu=cortex-m4 \
    -mthumb \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard


CFLAGS = \
    $(ARCH_FLAGS) \
    -O2 \
    -Wall \
    -fdata-sections \
    -ffunction-sections \
    $(INCLUDES)


CXXFLAGS = \
    $(ARCH_FLAGS) \
    -O2 \
    -Wall \
    -fdata-sections \
    -ffunction-sections \
    -fno-exceptions \
    -fno-rtti \
    -std=c++14 \
    $(INCLUDES)


LDSCRIPT = freertos_port/src/stm32f4.ld

LFLAGS = \
    $(ARCH_FLAGS) \
    -T$(LDSCRIPT) \
    -Wl,--gc-sections \
    --specs=nano.specs \
    --specs=nosys.specs


OBJS_S = $(addprefix $(BUILD_DIR)/,$(SRCS_S:.s=.o))
OBJS_C = $(addprefix $(BUILD_DIR)/,$(SRCS_C:.c=.o))
OBJS_CPP = $(addprefix $(BUILD_DIR)/,$(SRCS_CPP:.cpp=.o))

OBJS = $(OBJS_S) $(OBJS_C) $(OBJS_CPP)


all: $(BUILD_DIR)/$(TARGET).elf


$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@


$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@


$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@


$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	$(CXX) $(OBJS) $(LFLAGS) -o $@
	$(SIZE) $@


clean:
	rm -rf $(BUILD_DIR)


run: $(BUILD_DIR)/$(TARGET).elf
	qemu-system-arm \
		-M netduinoplus2 \
		-cpu cortex-m4 \
		-kernel $< \
		-nographic \
		-monitor none


debug: $(BUILD_DIR)/$(TARGET).elf
	qemu-system-arm \
		-M netduinoplus2 \
		-cpu cortex-m4 \
		-kernel $< \
		-nographic \
		-monitor none \
		-S \
		-gdb tcp::1234


.PHONY: all clean run debug