
TOOLCHAIN_PATH=/home/mbalestrini/Apps/riscv64-unknown-elf-gcc/bin/
#TOOLCHAIN_PATH=/usr/bin/
#TOOLCHAIN_PATH=/usr/local/bin/
#TOOLCHAIN_PATH=/opt/riscv32imc/bin/
# TOOLCHAIN_PATH=/ef/apps/bin/

# Set the prefix for `riscvXX-unknown-elf-*`
# On installations using `multilib`, this will be `riscv64` even for compiling to 32-bit targets
#TOOLCHAIN_PREFIX=riscv64-unknown-elf
TOOLCHAIN_PREFIX=riscv32-unknown-elf
#ARCH=rv32i
ARCH=rv32i_zicsr

# caravel_board repo path:
CB_PATH=../../caravel_board/firmware/chipignite/


.SUFFIXES:

PATTERN = serial_plot_test

hex:  ${PATTERN:=.hex}

%.elf: %.c $(CB_PATH)sections.lds $(CB_PATH)crt0_vex.S
	#$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -O0 -march=rv32i -Wl,-Bstatic,-T,$(CB_PATH)sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ $(CB_PATH)start.s $(CB_PATH)print_io.c $<
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -I$(CB_PATH) -I$(CB_PATH)generated/ -O0 -mabi=ilp32 -march=$(ARCH) -D__vexriscv__ -Wl,-Bstatic,-T,$(CB_PATH)sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ $(CB_PATH)crt0_vex.S $(CB_PATH)isr.c $(CB_PATH)stub.c $<
	${TOOLCHAIN_PATH}$(TOOLCHAIN_PREFIX)-objdump -D serial_plot_test.elf > serial_plot_test.lst

%.hex: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O verilog $< $@
	sed -ie 's/@1000/@0000/g' $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: serial_plot_test.hex
	python3 $(CB_PATH)util/caravel_hkflash.py serial_plot_test.hex

flash2: serial_plot_test.hex
	python3 $(CB_PATH)util/caravel_flash.py serial_plot_test.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all flash

