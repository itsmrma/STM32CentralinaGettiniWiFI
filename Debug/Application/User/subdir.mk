################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/main.c \
../Application/User/stm32l4xx_it.c \
../Application/User/syscalls.c \
../Application/User/sysmem.c \
../Application/User/system_stm32l4xx.c 

OBJS += \
./Application/User/main.o \
./Application/User/stm32l4xx_it.o \
./Application/User/syscalls.o \
./Application/User/sysmem.o \
./Application/User/system_stm32l4xx.o 

C_DEPS += \
./Application/User/main.d \
./Application/User/stm32l4xx_it.d \
./Application/User/syscalls.d \
./Application/User/sysmem.d \
./Application/User/system_stm32l4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/%.o Application/User/%.su Application/User/%.cyclo: ../Application/User/%.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/Common" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/hts221" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/lps22hb" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/B-L475E-IOT01" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Common/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User

clean-Application-2f-User:
	-$(RM) ./Application/User/main.cyclo ./Application/User/main.d ./Application/User/main.o ./Application/User/main.su ./Application/User/stm32l4xx_it.cyclo ./Application/User/stm32l4xx_it.d ./Application/User/stm32l4xx_it.o ./Application/User/stm32l4xx_it.su ./Application/User/syscalls.cyclo ./Application/User/syscalls.d ./Application/User/syscalls.o ./Application/User/syscalls.su ./Application/User/sysmem.cyclo ./Application/User/sysmem.d ./Application/User/sysmem.o ./Application/User/sysmem.su ./Application/User/system_stm32l4xx.cyclo ./Application/User/system_stm32l4xx.d ./Application/User/system_stm32l4xx.o ./Application/User/system_stm32l4xx.su

.PHONY: clean-Application-2f-User

