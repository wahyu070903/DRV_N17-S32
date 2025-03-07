################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ImuSensor/Src/ADXL345.c \
../ImuSensor/Src/ImuSensor.c 

OBJS += \
./ImuSensor/Src/ADXL345.o \
./ImuSensor/Src/ImuSensor.o 

C_DEPS += \
./ImuSensor/Src/ADXL345.d \
./ImuSensor/Src/ImuSensor.d 


# Each subdirectory must supply rules for building sources it contributes
ImuSensor/Src/%.o ImuSensor/Src/%.su ImuSensor/Src/%.cyclo: ../ImuSensor/Src/%.c ImuSensor/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ImuSensor-2f-Src

clean-ImuSensor-2f-Src:
	-$(RM) ./ImuSensor/Src/ADXL345.cyclo ./ImuSensor/Src/ADXL345.d ./ImuSensor/Src/ADXL345.o ./ImuSensor/Src/ADXL345.su ./ImuSensor/Src/ImuSensor.cyclo ./ImuSensor/Src/ImuSensor.d ./ImuSensor/Src/ImuSensor.o ./ImuSensor/Src/ImuSensor.su

.PHONY: clean-ImuSensor-2f-Src

