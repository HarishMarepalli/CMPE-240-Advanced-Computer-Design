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


BUILD_ARTIFACT_NAME := Harish_Marepalli_016707314_3D_cube_sphere
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Harish_Marepalli_016707314_3D_cube_sphere.axf

# Tool invocations
Harish_Marepalli_016707314_3D_cube_sphere.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\SyavasyaNikhil\Documents\MCUXpressoIDE_11.6.0_8187\workspace\CMSIS_CORE_LPC17xx\Debug" -Xlinker -Map="Harish_Marepalli_016707314_3D_cube_sphere.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "Harish_Marepalli_016707314_3D_cube_sphere_Debug.ld" -o "Harish_Marepalli_016707314_3D_cube_sphere.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Harish_Marepalli_016707314_3D_cube_sphere.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Harish_Marepalli_016707314_3D_cube_sphere.axf"; # arm-none-eabi-objcopy -v -O binary "Harish_Marepalli_016707314_3D_cube_sphere.axf" "Harish_Marepalli_016707314_3D_cube_sphere.bin" ; # checksum -p LPC1769 -d "Harish_Marepalli_016707314_3D_cube_sphere.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
