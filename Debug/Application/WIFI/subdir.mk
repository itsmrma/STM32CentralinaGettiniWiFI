################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/WIFI/es_wifi.c \
../Application/WIFI/es_wifi_io.c \
../Application/WIFI/wifi.c 

OBJS += \
./Application/WIFI/es_wifi.o \
./Application/WIFI/es_wifi_io.o \
./Application/WIFI/wifi.o 

C_DEPS += \
./Application/WIFI/es_wifi.d \
./Application/WIFI/es_wifi_io.d \
./Application/WIFI/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
Application/WIFI/%.o Application/WIFI/%.su Application/WIFI/%.cyclo: ../Application/WIFI/%.c Application/WIFI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/Common" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/hts221" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/Components/lps22hb" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP/B-L475E-IOT01" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/BSP" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Common/Inc" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"/home/pietro/Scrivania/STM32CentralinaGettiniWiFI-1/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-WIFI

clean-Application-2f-WIFI:
	-$(RM) ./Application/WIFI/es_wifi.cyclo ./Application/WIFI/es_wifi.d ./Application/WIFI/es_wifi.o ./Application/WIFI/es_wifi.su ./Application/WIFI/es_wifi_io.cyclo ./Application/WIFI/es_wifi_io.d ./Application/WIFI/es_wifi_io.o ./Application/WIFI/es_wifi_io.su ./Application/WIFI/wifi.cyclo ./Application/WIFI/wifi.d ./Application/WIFI/wifi.o ./Application/WIFI/wifi.su

.PHONY: clean-Application-2f-WIFI

