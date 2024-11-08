################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/rtos/uCOSIII/src/uCOS-III/Source/subdir.mk
-include source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/subdir.mk
-include source/rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU/subdir.mk
-include source/rtos/uCOSIII/src/uC-CPU/subdir.mk
-include source/rtos/uCOSIII/BSP/subdir.mk
-include source/subdir.mk
-include phy/subdir.mk
-include mdio/subdir.mk
-include lwip/src/netif/ppp/polarssl/subdir.mk
-include lwip/src/netif/ppp/subdir.mk
-include lwip/src/netif/subdir.mk
-include lwip/src/core/ipv6/subdir.mk
-include lwip/src/core/ipv4/subdir.mk
-include lwip/src/core/subdir.mk
-include lwip/src/apps/tftp/subdir.mk
-include lwip/src/apps/sntp/subdir.mk
-include lwip/src/apps/snmp/subdir.mk
-include lwip/src/apps/smtp/subdir.mk
-include lwip/src/apps/netbiosns/subdir.mk
-include lwip/src/apps/mqtt/subdir.mk
-include lwip/src/apps/mdns/subdir.mk
-include lwip/src/apps/lwiperf/subdir.mk
-include lwip/src/apps/httpd/subdir.mk
-include lwip/src/apps/altcp_tls/subdir.mk
-include lwip/src/api/subdir.mk
-include lwip/port/subdir.mk
-include jpec-master/src/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include SDK/startup/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := TPF Grupo2 - V6.10
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: TPF\ Grupo2\ -\ V6.10.axf

# Tool invocations
TPF\ Grupo2\ -\ V6.10.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="TPF Grupo2 - V6.10.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T "TPF_Grupo2_-_V6_9_Debug.ld" -o "TPF Grupo2 - V6.10.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) "TPF Grupo2 - V6.10.axf"
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TPF Grupo2 - V6.10.axf"; # arm-none-eabi-objcopy -v -O binary "TPF Grupo2 - V6.10.axf" "TPF Grupo2 - V6.10.bin" ; # checksum -p MK64FN1M0xxx12 -d "TPF Grupo2 - V6.10.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
