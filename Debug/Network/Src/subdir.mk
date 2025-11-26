################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Network/Src/network.c \
../Network/Src/network_data.c \
../Network/Src/network_data_params.c 

OBJS += \
./Network/Src/network.o \
./Network/Src/network_data.o \
./Network/Src/network_data_params.o 

C_DEPS += \
./Network/Src/network.d \
./Network/Src/network_data.d \
./Network/Src/network_data_params.d 


# Each subdirectory must supply rules for building sources it contributes
Network/Src/%.o Network/Src/%.su Network/Src/%.cyclo: ../Network/Src/%.c Network/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I"N:/EECS373-CubeIDE-Workspace/STSW-IMG050/STSW-IMG050_1.1.0/STSW-IMG050_F401_v1.1.0/Middlewares/ST/STM32_AI_Runtime/Inc" -I"N:/EECS373-CubeIDE-Workspace/libneai_demo-rock-paper-scissors-1_1" -I"N:/EECS373-CubeIDE-Workspace/STSW-IMG050/STSW-IMG050_1.1.0/STSW-IMG050_F401_v1.1.0/Application/Network/Inc" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"N:/EECS373-CubeIDE-Workspace/373-finalProject-tof2/Drivers/VL53L7CX_ULD_API/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Network-2f-Src

clean-Network-2f-Src:
	-$(RM) ./Network/Src/network.cyclo ./Network/Src/network.d ./Network/Src/network.o ./Network/Src/network.su ./Network/Src/network_data.cyclo ./Network/Src/network_data.d ./Network/Src/network_data.o ./Network/Src/network_data.su ./Network/Src/network_data_params.cyclo ./Network/Src/network_data_params.d ./Network/Src/network_data_params.o ./Network/Src/network_data_params.su

.PHONY: clean-Network-2f-Src

