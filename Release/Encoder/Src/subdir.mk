################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Encoder/Src/AS5600.c \
../Encoder/Src/Encoder.c \
../Encoder/Src/encoder_calib.c 

OBJS += \
./Encoder/Src/AS5600.o \
./Encoder/Src/Encoder.o \
./Encoder/Src/encoder_calib.o 

C_DEPS += \
./Encoder/Src/AS5600.d \
./Encoder/Src/Encoder.d \
./Encoder/Src/encoder_calib.d 


# Each subdirectory must supply rules for building sources it contributes
Encoder/Src/%.o Encoder/Src/%.su Encoder/Src/%.cyclo: ../Encoder/Src/%.c Encoder/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Encoder-2f-Src

clean-Encoder-2f-Src:
	-$(RM) ./Encoder/Src/AS5600.cyclo ./Encoder/Src/AS5600.d ./Encoder/Src/AS5600.o ./Encoder/Src/AS5600.su ./Encoder/Src/Encoder.cyclo ./Encoder/Src/Encoder.d ./Encoder/Src/Encoder.o ./Encoder/Src/Encoder.su ./Encoder/Src/encoder_calib.cyclo ./Encoder/Src/encoder_calib.d ./Encoder/Src/encoder_calib.o ./Encoder/Src/encoder_calib.su

.PHONY: clean-Encoder-2f-Src

