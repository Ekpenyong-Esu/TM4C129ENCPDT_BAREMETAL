################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=none -me --opt_for_speed=2 --include_path="C:/ti/CMSIS_5-5.9.0/CMSIS_5-5.9.0/CMSIS/Core/Include" --include_path="C:/ti/CMSIS_5-5.9.0/CMSIS_5-5.9.0" --include_path="C:/ti/CMSIS_5-5.9.0/CMSIS_5-5.9.0/Device/ARM/ARMCM4/Include" --include_path="C:/Users/mahon/workspace_v8/Interrupt" --include_path="C:/ti/tivaware_c_series_2_1_4_178" --include_path="C:/ti/tivaware_c_series_2_1_4_178/inc" --include_path="C:/ti/tivaware_c_series_2_1_4_178/utils" --include_path="C:/ti/tivaware_c_series_2_1_4_178/driverlib" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/include" --define=ccs="ccs" --define=PART_TM4C129ENCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


