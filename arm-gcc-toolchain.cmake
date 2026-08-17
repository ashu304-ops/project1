set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Target ARM Cortex-M4 flags for STM32
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -Wextra --specs=nosys.specs" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} -Wall -Wextra --specs=nosys.specs" CACHE STRING "" FORCE)