################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MotorDriver/Src/common.c \
../MotorDriver/Src/motion.c \
../MotorDriver/Src/tmc2209.c 

OBJS += \
./MotorDriver/Src/common.o \
./MotorDriver/Src/motion.o \
./MotorDriver/Src/tmc2209.o 

C_DEPS += \
./MotorDriver/Src/common.d \
./MotorDriver/Src/motion.d \
./MotorDriver/Src/tmc2209.d 


# Each subdirectory must supply rules for building sources it contributes
MotorDriver/Src/%.o MotorDriver/Src/%.su MotorDriver/Src/%.cyclo: ../MotorDriver/Src/%.c MotorDriver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MotorDriver-2f-Src

clean-MotorDriver-2f-Src:
	-$(RM) ./MotorDriver/Src/common.cyclo ./MotorDriver/Src/common.d ./MotorDriver/Src/common.o ./MotorDriver/Src/common.su ./MotorDriver/Src/motion.cyclo ./MotorDriver/Src/motion.d ./MotorDriver/Src/motion.o ./MotorDriver/Src/motion.su ./MotorDriver/Src/tmc2209.cyclo ./MotorDriver/Src/tmc2209.d ./MotorDriver/Src/tmc2209.o ./MotorDriver/Src/tmc2209.su

.PHONY: clean-MotorDriver-2f-Src

