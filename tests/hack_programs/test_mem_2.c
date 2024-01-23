const uint16_t hack_program[] = {
    0x0100, //	@256
    0xec10, //	D=A
    0x0005, //	@5
    0xe308, //	M=D
    0x0005, //	@5
    0xfdc8, //	M=M+1
    0x0004, //	@4
    0xea87, //	0;JMP
};