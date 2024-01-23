// #include "verilog/dv/caravel/defs.h"

#define USE_DLL

#include "../defs.h"
#include "../stub.h"

#include <stdint.h>

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

// #include "caravel_test_hack_program.c"
// #include "../hack_programs/beatles_by_Diogo.c"
// #include "../hack_programs/FillVram_to16390.c"
// #include "../hack_programs/test_gpio.c"
// #include "../hack_programs/test_gpio_2.c"
#include "../hack_programs/test_gpio_mem_1.c"
// #include "../hack_programs/test_gpio_mem_2.c"

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
	reg_timer0_update = 1; // latch current value
	while (reg_timer0_value > 0)
	{
		reg_timer0_update = 1;
	}
}

// UART code from https://github.com/efabless/caravel_mgmt_soc_litex/blob/main/verilog/dv/firmware/APIs/uart_api.h
#define UART_EV_TX 0x1
#define UART_EV_RX 0x2
/**
 * Wait receiving ASCII symbol and return it.
 *
 * Return the first ASCII symbol of the UART received queue
 *
 * RX mode have to be enabled
 *
 */
char UART_readChar()
{
	while (uart_rxempty_read() == 1)
		;
	return reg_uart_data;
}
/**
 * Pop the first ASCII symbol of the UART received queue
 *
 * UART_readChar() function would keeping reading the same symbol unless this function is called
 */
void UART_popChar()
{
	uart_ev_pending_write(UART_EV_RX);
	return;
}
/**
 * Send ASCII char through UART
 * @param c ASCII char to send
 *
 * TX mode have to be enabled
 */
void UART_sendChar(char c)
{
	while (reg_uart_txfull == 1)
		;
	reg_uart_data = c;
}
/**
 * Send int through UART
 * the int would be sent as 8 hex characters
 * @param c int to send
 *
 * TX mode have to be enabled
 */
void UART_sendInt(uint32_t data)
{
	for (int i = 0; i < 8; i++)
	{
		// Extract the current 4-bit chunk
		int chunk = (data >> (i * 4));
		if (chunk == 0)
		{
			break;
		}
		chunk = chunk & 0x0F;
		char ch;
		if (chunk >= 0 && chunk <= 9)
		{
			ch = '0' + chunk; // Convert to corresponding decimal digit character
		}
		else
		{
			ch = 'A' + (chunk - 10); // Convert to corresponding hex character A-F
		}
		UART_sendChar(ch);
	}
	UART_sendChar('\n');
}

void print_gpio_values_header() 
{
	char d0 = '0';
	char d1 = '0';

	// HEADER
	for (int32_t i = 0; i < 38; i++)
	{
		print("[");
		putchar(d1);
		putchar(d0);
		print("] ");
		d0 = d0 + 1;
		if (d0 > ('0' + 9))
		{
			d0 = '0';
			d1 = d1 + 1;
		}
	}

	print("\n");
}

void print_gpio_values()
{
	// VALUES
	for (int32_t i = 0; i < 38; i++)
	{
		uint32_t value;

		if (i < 32)
		{
			value = ((reg_mprj_datal >> i) & 0x1);
		}
		else
		{
			value = ((reg_mprj_datah >> (i - 32)) & 0x1);
		}

		print("  ");
		if (value)
		{
			print("1");
		}
		else
		{
			print("0");
		}
		print("  ");
	}
	print("\r");
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

	reg_mprj_io_5 = GPIO_MODE_MGMT_STD_INPUT_NOPULL; // UART Rx
	reg_mprj_io_6 = GPIO_MODE_MGMT_STD_OUTPUT;		 // UART Tx

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

	// Setup UART and MGMT GPIO pin for debugging purposes
	reg_uart_enable = 1;

	reg_gpio_mode1 = 1;
	reg_gpio_mode0 = 0;
	reg_gpio_ien = 1;
	reg_gpio_oe = 0;

	print("Starting HackSoC\n");

	delay(5000000);

	char c;
	c = UART_readChar();
	UART_popChar();
	print("Read:");
	putchar(c);
	print("\n");

	// c = UART_readChar();
	// UART_popChar();
	// print("Read:");
	// putchar(c);
	// print("\n");

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

	c = UART_readChar();
	UART_popChar();
	print("Read:");
	putchar(c);
	print("\n");

	rom_loader();

	c = UART_readChar();
	UART_popChar();
	print("Read:");
	putchar(c);
	print("\n");

	logic_analyzer.hack_external_reset = 0;
	tmp_la1_data = (tmp_la1_data & ~(1 << HACK_EXTERNAL_RESET__LA1_BIT)) | (logic_analyzer.hack_external_reset << HACK_EXTERNAL_RESET__LA1_BIT);
	reg_la1_data = tmp_la1_data;

	// reg_gpio_mode1 = 1;
	// reg_gpio_mode0 = 0;

	reg_gpio_ien = 0;
	reg_gpio_oe = 1;

	print_gpio_values_header();

	while (1)
	{
		// Blink
		reg_gpio_out = 1; // OFF
		delay(5000000);
		reg_gpio_out = 0; // ON
		delay(5000000);

		print_gpio_values();
	}
}
