// Reads from gpio_i and writes the value to memory position 5
// Use logic analyzer on ram pins to check if it's writing what you put on the input pins
const uint16_t hack_program[] = {
    0x6001, //	@24577
    0xfc10, //	D=M
    0x0005, //	@5
    0xe308, //	M=D
    0x0000, //	@0
    0xea87 //	0;JMP
};