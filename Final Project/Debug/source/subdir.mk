################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/DMA.c \
../source/FTM.c \
../source/I2C_Driver.c \
../source/I2C_FIFO_driver.c \
../source/Led_DRV.c \
../source/OV7670.c \
../source/array_compare.c \
../source/base64.c \
../source/bitmap_matrix.c \
../source/gpio.c \
../source/id_identification.c \
../source/lwip_dhcp_bm.c \
../source/magnetic.c \
../source/magnetic_app.c \
../source/main.c \
../source/my_assert.c \
../source/my_semaphores.c \
../source/semihost_hardfault.c \
../source/timer.c 

OBJS += \
./source/DMA.o \
./source/FTM.o \
./source/I2C_Driver.o \
./source/I2C_FIFO_driver.o \
./source/Led_DRV.o \
./source/OV7670.o \
./source/array_compare.o \
./source/base64.o \
./source/bitmap_matrix.o \
./source/gpio.o \
./source/id_identification.o \
./source/lwip_dhcp_bm.o \
./source/magnetic.o \
./source/magnetic_app.o \
./source/main.o \
./source/my_assert.o \
./source/my_semaphores.o \
./source/semihost_hardfault.o \
./source/timer.o 

C_DEPS += \
./source/DMA.d \
./source/FTM.d \
./source/I2C_Driver.d \
./source/I2C_FIFO_driver.d \
./source/Led_DRV.d \
./source/OV7670.d \
./source/array_compare.d \
./source/base64.d \
./source/bitmap_matrix.d \
./source/gpio.d \
./source/id_identification.d \
./source/lwip_dhcp_bm.d \
./source/magnetic.d \
./source/magnetic_app.d \
./source/main.d \
./source/my_assert.d \
./source/my_semaphores.d \
./source/semihost_hardfault.d \
./source/timer.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DSERIAL_PORT_TYPE_UART=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINT_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DSDK_DEBUGCONSOLE=0 -DDEBUG -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\ucosiii_config" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Source" -I../header -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source" -I../ -I../SDK/CMSIS -I../SDK/startup -I../jpec-master/include -I../jpec-master/src -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\board" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\mdio" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\phy" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\drivers" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\port" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src\include" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\utilities" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\device" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\uart" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\serial_manager" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\lists" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


