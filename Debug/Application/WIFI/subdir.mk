################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32L475E_IOT01 -DSTM32L475xx -c -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Inc" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/Common" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/hts221" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/Components/lps22hb" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP/B-L475E-IOT01" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/BSP" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Common/Inc" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/CMSIS/Include" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/Pietro/Documents/GitHub/STM32CentralinaGettiniWiFI/Drivers/STM32L4xx_HAL_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-WIFI

clean-Application-2f-WIFI:
	-$(RM) ./Application/WIFI/es_wifi.cyclo ./Application/WIFI/es_wifi.d ./Application/WIFI/es_wifi.o ./Application/WIFI/es_wifi.su ./Application/WIFI/es_wifi_io.cyclo ./Application/WIFI/es_wifi_io.d ./Application/WIFI/es_wifi_io.o ./Application/WIFI/es_wifi_io.su ./Application/WIFI/wifi.cyclo ./Application/WIFI/wifi.d ./Application/WIFI/wifi.o ./Application/WIFI/wifi.su

.PHONY: clean-Application-2f-WIFI

