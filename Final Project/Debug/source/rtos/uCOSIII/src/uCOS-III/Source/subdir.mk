################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_core.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_int.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_q.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_task.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_time.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_var.c 

OBJS += \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.o 

C_DEPS += \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.d 


# Each subdirectory must supply rules for building sources it contributes
source/rtos/uCOSIII/src/uCOS-III/Source/%.o: ../source/rtos/uCOSIII/src/uCOS-III/Source/%.c source/rtos/uCOSIII/src/uCOS-III/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DSERIAL_PORT_TYPE_UART=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINT_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DSDK_DEBUGCONSOLE=0 -DDEBUG -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\ucosiii_config" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source\rtos\uCOSIII\src\uCOS-III\Source" -I../header -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\source" -I../ -I../SDK/CMSIS -I../SDK/startup -I../jpec-master/include -I../jpec-master/src -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\board" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\mdio" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\phy" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\drivers" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\port" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\lwip\src\include" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\utilities" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\device" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\uart" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\serial_manager" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\component\lists" -I"C:\Users\alero\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TPF Grupo2 - V6.10\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


