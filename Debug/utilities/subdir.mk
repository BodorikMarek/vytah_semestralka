################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_debug_console.c 

OBJS += \
./utilities/fsl_debug_console.o 

C_DEPS += \
./utilities/fsl_debug_console.d 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DDEBUG -DFRDM_KL25Z -DFREEDOM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\board" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\source" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\drivers" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\CMSIS" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\utilities" -I"C:\Users\bodor\mcuxpresso\frdmkl25z_driver_examples_lpsci_interrupt_Vytah\startup" -O0 -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


