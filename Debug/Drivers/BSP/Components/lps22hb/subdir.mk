################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/lps22hb/lps22hb.c 

OBJS += \
./Drivers/BSP/Components/lps22hb/lps22hb.o 

C_DEPS += \
./Drivers/BSP/Components/lps22hb/lps22hb.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/lps22hb/%.o Drivers/BSP/Components/lps22hb/%.su Drivers/BSP/Components/lps22hb/%.cyclo: ../Drivers/BSP/Components/lps22hb/%.c Drivers/BSP/Components/lps22hb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/Common" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/hts221" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/lps22hb" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/B-L475E-IOT01" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Common/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-lps22hb

clean-Drivers-2f-BSP-2f-Components-2f-lps22hb:
	-$(RM) ./Drivers/BSP/Components/lps22hb/lps22hb.cyclo ./Drivers/BSP/Components/lps22hb/lps22hb.d ./Drivers/BSP/Components/lps22hb/lps22hb.o ./Drivers/BSP/Components/lps22hb/lps22hb.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-lps22hb

