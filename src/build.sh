## alias arm-gcc='arm-none-eabi-gcc --specs=nosys.specs -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits'

arm-gcc -nostdlib -nostartfiles -ffreestanding -c main.c 


