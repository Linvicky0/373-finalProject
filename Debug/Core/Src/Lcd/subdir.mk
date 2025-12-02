################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Lcd/ili9488.c \
../Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.c \
../Core/Src/Lcd/stm32_ili9488_lcd.c \
../Core/Src/Lcd/stm32_ili9488_ts.c 

OBJS += \
./Core/Src/Lcd/ili9488.o \
./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.o \
./Core/Src/Lcd/stm32_ili9488_lcd.o \
./Core/Src/Lcd/stm32_ili9488_ts.o 

C_DEPS += \
./Core/Src/Lcd/ili9488.d \
./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.d \
./Core/Src/Lcd/stm32_ili9488_lcd.d \
./Core/Src/Lcd/stm32_ili9488_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Lcd/%.o Core/Src/Lcd/%.su Core/Src/Lcd/%.cyclo: ../Core/Src/Lcd/%.c Core/Src/Lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Lcd

clean-Core-2f-Src-2f-Lcd:
	-$(RM) ./Core/Src/Lcd/ili9488.cyclo ./Core/Src/Lcd/ili9488.d ./Core/Src/Lcd/ili9488.o ./Core/Src/Lcd/ili9488.su ./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.cyclo ./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.d ./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.o ./Core/Src/Lcd/lcdts_io_xpt2046_spi_hal.su ./Core/Src/Lcd/stm32_ili9488_lcd.cyclo ./Core/Src/Lcd/stm32_ili9488_lcd.d ./Core/Src/Lcd/stm32_ili9488_lcd.o ./Core/Src/Lcd/stm32_ili9488_lcd.su ./Core/Src/Lcd/stm32_ili9488_ts.cyclo ./Core/Src/Lcd/stm32_ili9488_ts.d ./Core/Src/Lcd/stm32_ili9488_ts.o ./Core/Src/Lcd/stm32_ili9488_ts.su

.PHONY: clean-Core-2f-Src-2f-Lcd

