################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/hts221/hts221.c 

OBJS += \
./Drivers/BSP/Components/hts221/hts221.o 

C_DEPS += \
./Drivers/BSP/Components/hts221/hts221.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/hts221/%.o Drivers/BSP/Components/hts221/%.su Drivers/BSP/Components/hts221/%.cyclo: ../Drivers/BSP/Components/hts221/%.c Drivers/BSP/Components/hts221/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/Common" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/hts221" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/lps22hb" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/B-L475E-IOT01" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Common/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-hts221

clean-Drivers-2f-BSP-2f-Components-2f-hts221:
	-$(RM) ./Drivers/BSP/Components/hts221/hts221.cyclo ./Drivers/BSP/Components/hts221/hts221.d ./Drivers/BSP/Components/hts221/hts221.o ./Drivers/BSP/Components/hts221/hts221.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-hts221

