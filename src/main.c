#include <stdint.h>
#define PI3_GPIO_BASE       0x3F200000UL // 7E2 -> 3F2

#define PI3_IRQ_BASE        0x3F20B000UL 


#define GPFSEL0 (PI3_GPIO_BASE+0x00000000)
#define GPSET0  (PI3_GPIO_BASE+0x0000001C)
#define GPCLR0  (PI3_GPIO_BASE+0x00000028)

#define GPREN0  (PI3_GPIO_BASE+0x0000004C)  // rising edge detect enable 
#define GPFEN0  (PI3_GPIO_BASE+0x00000058)  // falling edge detect enable

#define PI3_IRQ0_EN (PI3_IRQ_BASE + 0x210)
#define PI3_IRQ1_EN (PI3_IRQ_BASE + 0x214)


#define RED 12
#define GREEN 13
#define YELLOW 16


#define MDELAY 5000000
#define SDELAY 1000000


void SEL_GPIO (uint32_t gpio_num, char mode)
{
//first find the related SEL bit 
// each FSEl register controls 10 GPIO(starring from 0). 
// the selection register is then gpio_num/10.
uint32_t gpf_sel_reg_index = gpio_num/10;

// each register is 4 byte. so times 4 and add to the base to find address
volatile uint32_t* reg_sel_addr = (uint32_t*) (GPFSEL0 + 4*gpf_sel_reg_index); 

// Each GPIO has 3 bits in sel register to select the mode.
// The first bit to control mode of gpio_I is gpio_I%10 * 3.
unsigned char gpf_bit = (gpio_num%10) * 3;


// create mask
// to set as output the 3bit should be set to 001. i.e. only bit gpf_bit should be set in gpf_sel_reg
uint32_t mask;
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
uint32_t curr_sel = *reg_sel_addr;
curr_sel|= mask;
*reg_sel_addr = curr_sel;

}

void SET_GPIO(uint32_t gpio_num)
{

//first find the related set register and bit 
// each SET register controls 32 GPIO(starring from 0). 
// the set register is then gpio_num/32.
uint32_t gpf_set_reg_index = gpio_num >> 5;

// each register is 4 byte. so times 4 to find address
volatile uint32_t* reg_set_addr = (uint32_t*) (GPSET0 + 4*gpf_set_reg_index); 

// the bit to set gpio_I is gpio_I%32.
unsigned char gpf_bit = gpio_num & 0x1F;


// create mask. write mask directly as writing 0 to each bit has no effect and writing 1 into bit I will set GPIO_I 
uint32_t mask = 0x1<<gpf_bit;
*reg_set_addr = mask;
}

void CLR_GPIO(uint32_t gpio_num)
{

//first find the related clear register and bit 
// each clear register controls 32 GPIO(starring from 0). 
// the clear register is then gpio_num/32.
uint32_t gpf_clr_reg_index = gpio_num >> 5;

// each register is 4 byte. so times 4 to find address
volatile uint32_t* reg_clr_addr = (uint32_t*) (GPCLR0 + 4*gpf_clr_reg_index); 

// the bit to clear gpio_I is gpio_I%32.
unsigned char gpf_bit = gpio_num & 0x1F;

// create mask. write mask directly as writing 0 to each bit has no effect
uint32_t mask = 0x1<<gpf_bit;
*reg_clr_addr = mask;
}


void RISE_EN_GPIO(uint32_t gpio_num,char en)
{
	uint32_t gp_ren_reg_index = gpio_num >> 5; // each bit control one GPIO

	volatile uint32_t* reg_ren_addr = (uint32_t*) (GPREN0 + 4*gp_ren_reg_index); 

	unsigned char bit_pos = gpio_num&0x1F; //remainder by 32

	uint32_t mask = 0x1 << bit_pos;
	volatile uint32_t cur_ren = *reg_ren_addr;
	if(en)
	{
		cur_ren|=mask;
	}
	else {
		mask=~mask;
		cur_ren&=mask;
	}
	*reg_ren_addr = cur_ren;

}

void FALL_EN_GPIO(uint32_t gpio_num, char en)
{
	uint32_t gp_fen_reg_index = gpio_num >> 5; // each bit control one GPIO

	volatile uint32_t* reg_fen_addr = (uint32_t*) (GPFEN0 + 4*gp_fen_reg_index); 

	unsigned char bit_pos = gpio_num&0x1F; //remainder by 32

	uint32_t mask = 0x1 << bit_pos;
	volatile uint32_t cur_ren = *reg_fen_addr;
	if(en)
	{
		cur_ren|=mask;
	}
	else {
		mask=~mask;
		cur_ren&=mask;
	}
	*reg_fen_addr = cur_ren;

}

void enable_gpio_interrupts(char en)
{

// GPIO interrupts are #49, #50, #51, #52
/// use the second register as more than 32
// set bits 17,18,19,20

	volatile uint32_t* irq_reg = (uint32_t*)PI3_IRQ1_EN;
	uint32_t mask = 0x1E00; // bits 17,18,19,20
	uint32_t cur_en = *irq_reg;

	if(en)
	{
		cur_en|=mask;
	}
	else {
		mask=~mask;
		cur_en&=mask;
	}

	*irq_reg = cur_en;

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


