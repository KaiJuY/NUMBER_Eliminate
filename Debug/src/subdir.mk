################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/7seg.c \
../src/delay.c \
../src/ds18b20.c \
../src/game.c \
../src/gpio.c \
../src/helper_functions.c \
../src/keypad.c \
../src/lcd.c \
../src/led_button.c \
../src/main.c \
../src/timer.c 

OBJS += \
./src/7seg.o \
./src/delay.o \
./src/ds18b20.o \
./src/game.o \
./src/gpio.o \
./src/helper_functions.o \
./src/keypad.o \
./src/lcd.o \
./src/led_button.o \
./src/main.o \
./src/timer.o 

C_DEPS += \
./src/7seg.d \
./src/delay.d \
./src/ds18b20.d \
./src/game.d \
./src/gpio.d \
./src/helper_functions.d \
./src/keypad.d \
./src/lcd.d \
./src/led_button.d \
./src/main.d \
./src/timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32L4 -DSTM32L476RGTx -DNUCLEO_L476RG -DDEBUG -I"C:/Users/KaiRu/workspace/FinalProj/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


