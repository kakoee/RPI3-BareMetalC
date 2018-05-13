#include <stdint.h>
#define PI3_GPIO_BASE       0x3F200000UL

#define GPFSEL0 (PI3_GPIO_BASE+0x00000000)
#define GPSET0  (PI3_GPIO_BASE+0x0000001C)
#define GPCLR0  (PI3_GPIO_BASE+0x00000028)

#define RED 12
#define GREEN 13
#define YELLOW 16


#define MDELAY 5000000
#define SDELAY 1000000


void SEL_GPIO (unsigned int gpio_num, char mode)
{
//first find the related SEL bit 
// each FSEl register controls 10 GPIO(starring from 0). 
// the selection register is then gpio_num/10.
unsigned int gpf_sel_reg_index = gpio_num/10;

// each register is 4 byte. so times 4 and add to the base to find address
volatile unsigned int* reg_sel_addr = (unsigned int*) (GPFSEL0 + 4*gpf_sel_reg_index); 

// Each GPIO has 3 bits in sel register to select the mode.
// The first bit to control mode of gpio_I is gpio_I%10 * 3.
unsigned char gpf_bit = (gpio_num%10) * 3;


// create mask
// to set as output the 3bit should be set to 001. i.e. only bit gpf_bit should be set in gpf_sel_reg
unsigned int mask;
switch(mode){
case 0:// input
   mask = 0x0; 
   break;
case 1:// output
   mask = 0x1 << gpf_bit; 
   break;
default: // To DO
   mask = 0x0;
   break;
}

// we should read the existing content and mask it to avoid overwriting other bits
unsigned int curr_sel = *reg_sel_addr;
curr_sel|= mask;
*reg_sel_addr = curr_sel;

}

void SET_GPIO(unsigned int gpio_num)
{

//first find the related set register and bit 
// each SET register controls 32 GPIO(starring from 0). 
// the set register is then gpio_num/32.
unsigned int gpf_set_reg_index = gpio_num >> 5;

// each register is 4 byte. so times 4 to find address
volatile unsigned int* reg_set_addr = (unsigned int*) (GPSET0 + 4*gpf_set_reg_index); 

// the bit to set gpio_I is gpio_I%32.
unsigned char gpf_bit = gpio_num & 0x1F;


// create mask. write mask directly as writing 0 to each bit has no effect and writing 1 into bit I will set GPIO_I 
unsigned int mask = 0x1<<gpf_bit;
*reg_set_addr = mask;
}

void CLR_GPIO(unsigned int gpio_num)
{

//first find the related clear register and bit 
// each clear register controls 32 GPIO(starring from 0). 
// the clear register is then gpio_num/32.
unsigned int gpf_clr_reg_index = gpio_num >> 5;

// each register is 4 byte. so times 4 to find address
volatile unsigned int* reg_clr_addr = (unsigned int*) (GPCLR0 + 4*gpf_clr_reg_index); 

// the bit to clear gpio_I is gpio_I%32.
unsigned char gpf_bit = gpio_num & 0x1F;

// create mask. write mask directly as writing 0 to each bit has no effect
unsigned int mask = 0x1<<gpf_bit;
*reg_clr_addr = mask;
}


void delay(unsigned long mdelay)
{

volatile unsigned long tim;
for(tim = 0; tim < mdelay; tim++)
            ;

}


int main()
{

//Set mode to be output 
SEL_GPIO(RED,1);
SEL_GPIO(YELLOW,1);
SEL_GPIO(GREEN,1);
CLR_GPIO(RED);
CLR_GPIO(YELLOW);
CLR_GPIO(GREEN);

while(1)
{
SET_GPIO(RED);
CLR_GPIO(GREEN);

delay(MDELAY);

SET_GPIO(YELLOW);
CLR_GPIO(RED);

delay(SDELAY);

SET_GPIO(GREEN);
CLR_GPIO(YELLOW);

delay(MDELAY);

}

return 1;
}


