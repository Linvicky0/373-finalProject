################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.c \
../Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.c \
../Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.c \
../Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.c 

OBJS += \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.o \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.o \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.o \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.o 

C_DEPS += \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.d \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.d \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.d \
./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/VL53L7CX_ULD_API/src/%.o Drivers/VL53L7CX_ULD_API/src/%.su Drivers/VL53L7CX_ULD_API/src/%.cyclo: ../Drivers/VL53L7CX_ULD_API/src/%.c Drivers/VL53L7CX_ULD_API/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I"N:/EECS373-CubeIDE-Workspace/STSW-IMG050/STSW-IMG050_1.1.0/STSW-IMG050_F401_v1.1.0/Middlewares/ST/STM32_AI_Runtime/Inc" -I"N:/EECS373-CubeIDE-Workspace/libneai_demo-rock-paper-scissors-1_1" -I"N:/EECS373-CubeIDE-Workspace/STSW-IMG050/STSW-IMG050_1.1.0/STSW-IMG050_F401_v1.1.0/Application/Network/Inc" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"N:/EECS373-CubeIDE-Workspace/373-finalProject-tof2/Drivers/VL53L7CX_ULD_API/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-VL53L7CX_ULD_API-2f-src

clean-Drivers-2f-VL53L7CX_ULD_API-2f-src:
	-$(RM) ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.cyclo ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.d ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.o ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_api.su ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.cyclo ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.d ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.o ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_detection_thresholds.su ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.cyclo ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.d ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.o ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_motion_indicator.su ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.cyclo ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.d ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.o ./Drivers/VL53L7CX_ULD_API/src/vl53l7cx_plugin_xtalk.su

.PHONY: clean-Drivers-2f-VL53L7CX_ULD_API-2f-src

