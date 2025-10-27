################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2030/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=soft -mfpu=none -mlittle-endian -O2 -I"C:/Users/hneelabh/workspace_ccstheia/gpio_toggle_output_LP_MSPM33C321A_nortos_ticlang" -I"C:/Users/hneelabh/workspace_ccstheia/gpio_toggle_output_LP_MSPM33C321A_nortos_ticlang/Debug" -I"C:/ti/mspm33_sdk_1_00_00_00_internal/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm33_sdk_1_00_00_00_internal/source" -D__MSPM33C321A__ -gdwarf-3 -fcoverage-mapping -fprofile-instr-generate -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-797255455: ../blink.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2030/ccs/utils/sysconfig_1.25.0/sysconfig_cli.bat" --script "C:/Users/hneelabh/workspace_ccstheia/gpio_toggle_output_LP_MSPM33C321A_nortos_ticlang/blink.syscfg" -o "." -s "C:/ti/mspm33_sdk_1_00_00_00_internal/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

ti_msp_dl_config.c: build-797255455 ../blink.syscfg
ti_msp_dl_config.h: build-797255455
Event.dot: build-797255455

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2030/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m33 -mfloat-abi=soft -mfpu=none -mlittle-endian -O2 -I"C:/Users/hneelabh/workspace_ccstheia/gpio_toggle_output_LP_MSPM33C321A_nortos_ticlang" -I"C:/Users/hneelabh/workspace_ccstheia/gpio_toggle_output_LP_MSPM33C321A_nortos_ticlang/Debug" -I"C:/ti/mspm33_sdk_1_00_00_00_internal/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm33_sdk_1_00_00_00_internal/source" -D__MSPM33C321A__ -gdwarf-3 -fcoverage-mapping -fprofile-instr-generate -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


