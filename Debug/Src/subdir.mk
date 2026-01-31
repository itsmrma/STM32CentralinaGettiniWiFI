################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/system_stm32l4xx.c 

OBJS += \
./Src/system_stm32l4xx.o 

C_DEPS += \
./Src/system_stm32l4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Inc" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/Common" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/hts221" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/lps22hb" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/B-L475E-IOT01" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Common/Inc" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/CMSIS/Include" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Matteo Benassi/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/system_stm32l4xx.cyclo ./Src/system_stm32l4xx.d ./Src/system_stm32l4xx.o ./Src/system_stm32l4xx.su

.PHONY: clean-Src

