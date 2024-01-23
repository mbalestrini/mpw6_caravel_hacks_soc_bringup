// #include "verilog/dv/caravel/defs.h"

// #define USE_DLL

#include "../defs.h"

#include <stdint.h>

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

// #include "caravel_test_hack_program.c"
// #include "../hack_programs/beatles_by_Diogo.c"
// #include "../hack_programs/FillVram_to16390.c"
// #include "../hack_programs/test_gpio.c"
// #include "../hack_programs/test_gpio_2.c"
// #include "../hack_programs/test_gpio_mem_1.c"
// #include "../hack_programs/test_gpio_mem_2.c"
// #include "../hack_programs/test_gpio_mem_3.c"
#include "../hack_programs/test_mem_1.c"
// #include "../hack_programs/test_mem_2.c"


#define MULTIPROJECT_ID 11

#define RESET__LA1_BIT 0
#define KEYCODE__LA1_BIT 1
#define KEYCODE__LA1_LENGTH 8
#define ROM_LOADER_SCK__LA1_BIT 9
#define ROM_LOADER_LOAD__LA1_BIT 10
#define ROM_LOADER_DATA__LA1_BIT 11
#define ROM_LOADER_DATA__LA1_LENGTH 16
#define ROM_LOADER_ACK__LA1_BIT 27
#define HACK_EXTERNAL_RESET__LA1_BIT 28

struct logic_analyzer_t
{
	// Outputs from Pico
	uint8_t reset;
	uint8_t keycode;
	uint8_t rom_loader_sck;
	uint8_t rom_loader_load;
	uint16_t rom_loader_data;
	uint8_t hack_external_reset;
	// Inputs to Pico
	uint8_t rom_loader_ack;

} logic_analyzer;

volatile uint32_t tmp_la1_data;

void delay(const int d)
{

    /* Configure timer for a single-shot countdown */
	reg_timer0_config = 0;
	reg_timer0_data = d;
    reg_timer0_config = 1;

    // Loop, waiting for value to reach zero
   reg_timer0_update = 1;  // latch current value
   while (reg_timer0_value > 0) {
           reg_timer0_update = 1;
   }

}

void rom_loader()
{

	uint16_t program_size = ARRAY_LENGTH(hack_program);

	// Start ROM LOADING
	logic_analyzer.rom_loader_load = 1;
	tmp_la1_data = (tmp_la1_data & ~(1 << ROM_LOADER_LOAD__LA1_BIT)) | (logic_analyzer.rom_loader_load << ROM_LOADER_LOAD__LA1_BIT);

	for (int i = 0; i < program_size; ++i)
	{

		logic_analyzer.rom_loader_data = hack_program[i];
		tmp_la1_data = (tmp_la1_data & ~(0xffff << ROM_LOADER_DATA__LA1_BIT)) | (logic_analyzer.rom_loader_data << ROM_LOADER_DATA__LA1_BIT);
		logic_analyzer.rom_loader_sck = 1;
		tmp_la1_data = (tmp_la1_data & ~(1 << ROM_LOADER_SCK__LA1_BIT)) | (logic_analyzer.rom_loader_sck << ROM_LOADER_SCK__LA1_BIT);
		reg_la1_data = tmp_la1_data;

		// TODO: Here I need to wait for the ACK. Without it only works if the code is slow enough for the HACK_SOC to process it before changing the rom_loader_sck line
		// But right now the ACK signal it lasts only one clock, so it might be hard to get. I should change that in the rtl

		logic_analyzer.rom_loader_sck = 0;
		tmp_la1_data = (tmp_la1_data & ~(1 << ROM_LOADER_SCK__LA1_BIT)) | (logic_analyzer.rom_loader_sck << ROM_LOADER_SCK__LA1_BIT);
		reg_la1_data = tmp_la1_data;
	}

	// Finished ROM LOADING
	logic_analyzer.rom_loader_load = 0;
	tmp_la1_data = (tmp_la1_data & ~(1 << ROM_LOADER_LOAD__LA1_BIT)) | (logic_analyzer.rom_loader_load << ROM_LOADER_LOAD__LA1_BIT);
	reg_la1_data = tmp_la1_data;
}

void main()
{
	/*
	IO Control Registers
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |

	Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |


	Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |

	*/

	// ** HACK GPIO ** //
	// gpio_o
	reg_mprj_io_37 = GPIO_MODE_USER_STD_OUTPUT;
	reg_mprj_io_36 = GPIO_MODE_USER_STD_OUTPUT;
	reg_mprj_io_35 = GPIO_MODE_USER_STD_OUTPUT;
	reg_mprj_io_34 = GPIO_MODE_USER_STD_OUTPUT;

	// gpio_i
	reg_mprj_io_33 = GPIO_MODE_USER_STD_INPUT_NOPULL;
	reg_mprj_io_32 = GPIO_MODE_USER_STD_INPUT_NOPULL;
	reg_mprj_io_31 = GPIO_MODE_USER_STD_INPUT_NOPULL;
	reg_mprj_io_30 = GPIO_MODE_USER_STD_INPUT_NOPULL;

	// ** DISPLAY VSYNC< HSYNC, RGB ** //
	// rgb
	reg_mprj_io_29 = GPIO_MODE_USER_STD_OUTPUT;
	// hsync
	reg_mprj_io_28 = GPIO_MODE_USER_STD_OUTPUT;
	// vsync
	reg_mprj_io_27 = GPIO_MODE_USER_STD_OUTPUT;

	//	** HACK_EXTERNAL_RESET ** //
	reg_mprj_io_26 = GPIO_MODE_USER_STD_INPUT_NOPULL;

	//	** VRAM ** //

	// SPI VRAM SIO
	reg_mprj_io_25 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_24 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_23 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_22 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	// SPI VRAM_SCK
	reg_mprj_io_21 = GPIO_MODE_USER_STD_OUTPUT;
	// SPI VRAM_CS_N
	reg_mprj_io_20 = GPIO_MODE_USER_STD_OUTPUT;

	// 	** ROM ** //

	// SPI ROM SIO
	reg_mprj_io_19 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_18 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_17 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_16 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	// SPI ROM_SCK
	reg_mprj_io_15 = GPIO_MODE_USER_STD_OUTPUT;
	// SPI ROM_CS_N
	reg_mprj_io_14 = GPIO_MODE_USER_STD_OUTPUT;

	//	** RAM ** //

	// SPI RAM SIO
	reg_mprj_io_13 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_12 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_11 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_10 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
	// SPI RAM_SCK
	reg_mprj_io_9 = GPIO_MODE_USER_STD_OUTPUT;
	// SPI RAM_CS_N
	reg_mprj_io_8 = GPIO_MODE_USER_STD_OUTPUT;

	/* Apply configuration */
	reg_mprj_xfer = 1;
	while (reg_mprj_xfer == 1)
		;

	// Default value for LA[31:0] = OUTPUT
	reg_la1_oenb = reg_la1_iena = 0xFFFFFFFF;
	// reg_la1_iena = 0;


	// DLL mode:
	// main_clock_freq = EXTERNAL_CLOCK * reg_hkspi_pll_divider[4:0] / reg_hkspi_pll_source[2:0]
	// secondary_clock_freq = EXTERNAL_CLOCK * reg_hkspi_pll_divider[4:0] / reg_hkspi_pll_source[5:3]
	// In this case:
	// 10MHz * 10 / 4 = 25MHz
	// More info: https://caravel-harness.readthedocs.io/en/latest/housekeeping-spi.html#housekeeping-reg-pll-trim
	#ifdef USE_DLL
	reg_hkspi_pll_ena = 1;
	reg_hkspi_pll_source = 0x04; 
	reg_hkspi_pll_divider = 0x0a;
	// reg_hkspi_pll_trim = 0;
	reg_hkspi_pll_bypass = 0;
	#endif

	// rom_loader_ack is input
	// reg_la1_iena  = reg_la1_oenb  = ~( 1<< ROM_LOADER_ACK__LA1_BIT );
	reg_la1_iena = reg_la1_oenb = (reg_la1_oenb & ~(1 << ROM_LOADER_ACK__LA1_BIT));
	// reg_la1_iena = reg_la1_iena | ( 1<< ROM_LOADER_ACK__LA1_BIT );

	// system reset
	logic_analyzer.reset = 1;
	logic_analyzer.keycode = 97;
	logic_analyzer.rom_loader_load = 0;
	// hack_cpu reset
	logic_analyzer.hack_external_reset = 1;
	// logic_analyzer.rom_loader_sck = 0;
	// logic_analyzer.rom_loader_data = 0;

	// Set initial output values
	tmp_la1_data = (logic_analyzer.reset << RESET__LA1_BIT) |
				   (logic_analyzer.keycode << KEYCODE__LA1_BIT) |
				   (logic_analyzer.rom_loader_load << ROM_LOADER_LOAD__LA1_BIT) |
				   (logic_analyzer.hack_external_reset << HACK_EXTERNAL_RESET__LA1_BIT);
	;
	// (logic_analyzer.rom_loader_sck << 9) |
	// (logic_analyzer.rom_loader_data << 11);
	reg_la1_data = tmp_la1_data;

	// activate the project by setting the [project ID] bit of 2nd bank of LA
	reg_la0_iena = 0xFFFFFFFF; // input enable off
	reg_la0_oenb = 0xFFFFFFFF; // output enable on
	reg_la0_data = 1 << MULTIPROJECT_ID;

	// Release system reset
	logic_analyzer.reset = 0;
	tmp_la1_data = (tmp_la1_data & ~(1 << RESET__LA1_BIT)) | (logic_analyzer.reset << RESET__LA1_BIT);
	reg_la1_data = tmp_la1_data;

	rom_loader();

	logic_analyzer.hack_external_reset = 0;
	tmp_la1_data = (tmp_la1_data & ~(1 << HACK_EXTERNAL_RESET__LA1_BIT)) | (logic_analyzer.hack_external_reset << HACK_EXTERNAL_RESET__LA1_BIT);
	reg_la1_data = tmp_la1_data;

	// // do something with the logic analyser
	// reg_la1_iena = 0;
	// reg_la1_oenb = 0;
	// reg_la1_data |= 100;



	reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;

	while (1)
	{
		reg_gpio_out = 1; // OFF
		reg_mprj_datal = 0x00000000;
		reg_mprj_datah = 0x00000000;

		delay(5000000);
	
		reg_gpio_out = 0; // ON
		reg_mprj_datah = 0x0000003f;
		reg_mprj_datal = 0xffffffff;

		delay(5000000);
	}
}
