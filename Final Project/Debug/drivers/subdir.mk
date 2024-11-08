################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_enet.c \
../drivers/fsl_ftfx_cache.c \
../drivers/fsl_ftfx_controller.c \
../drivers/fsl_ftfx_flash.c \
../drivers/fsl_ftfx_flexnvm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_enet.o \
./drivers/fsl_ftfx_cache.o \
./drivers/fsl_ftfx_controller.o \
./drivers/fsl_ftfx_flash.o \
./drivers/fsl_ftfx_flexnvm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_enet.d \
./drivers/fsl_ftfx_cache.d \
./drivers/fsl_ftfx_controller.d \
./drivers/fsl_ftfx_flash.d \
./drivers/fsl_ftfx_flexnvm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DSERIAL_PORT_TYPE_UART=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINT_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DSDK_DEBUGCONSOLE=0 -DDEBUG -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\ucosiii_config" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Source" -I../header -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source" -I../ -I../SDK/CMSIS -I../SDK/startup -I../jpec-master/include -I../jpec-master/src -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\board" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\mdio" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\phy" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\drivers" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\port" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src\include" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\utilities" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\device" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\uart" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\serial_manager" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\lists" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


