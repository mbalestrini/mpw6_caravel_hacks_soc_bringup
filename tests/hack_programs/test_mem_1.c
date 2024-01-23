const uint16_t hack_program[] = {
    0x0005, // @5
    0xea88, // M=0
    0x0005, // @5
    0xfdc8, // M=M+1
    0x0002, // @2
    0xea87 // 0;JMP
};