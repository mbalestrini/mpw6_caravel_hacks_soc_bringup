const uint16_t hack_program[] = {
    0x0005, //	@5
    0xea88, //	M=0
    0x0005, //	@5
    0xfdc8, //	M=M+1
    0xfc10, //	D=M
    0x6002, //	@24578
    0xe308, //	M=D
    0x0002, //	@2
    0xea87 //	0;JMP
};
