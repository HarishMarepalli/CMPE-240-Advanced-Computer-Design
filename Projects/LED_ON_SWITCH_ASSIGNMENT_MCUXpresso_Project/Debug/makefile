################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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


BUILD_ARTIFACT_NAME := LED_ON_SWITCH_ASSIGNMENT
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: LED_ON_SWITCH_ASSIGNMENT.axf

# Tool invocations
LED_ON_SWITCH_ASSIGNMENT.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\SyavasyaNikhil\Documents\MCUXpressoIDE_11.6.0_8187\workspace\CMSIS_CORE_LPC17xx\Debug" -Xlinker -Map="LED_ON_SWITCH_ASSIGNMENT.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "LED_ON_SWITCH_ASSIGNMENT_Debug.ld" -o "LED_ON_SWITCH_ASSIGNMENT.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) LED_ON_SWITCH_ASSIGNMENT.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LED_ON_SWITCH_ASSIGNMENT.axf"; # arm-none-eabi-objcopy -v -O binary "LED_ON_SWITCH_ASSIGNMENT.axf" "LED_ON_SWITCH_ASSIGNMENT.bin" ; # checksum -p LPC1769 -d "LED_ON_SWITCH_ASSIGNMENT.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
