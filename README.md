# mpw6_caravel_hacks_soc_bringup

## **MPW6 silicon test: Hack SoC project inside Caravel architecture**

---
### **Architecture overview:**

![](docs/wrapped_hack_soc_design_overview.png)

### **MPRJ_IO Connections:**

| |  | |  
|---|---|---|
|RAM_CS_N |output|MPRJ_IO[8]
|RAM_CSK |output|MPRJ_IO[9]
|RAM_SIO0 |inout|MPRJ_IO[10]
|RAM_SIO1 |inout|MPRJ_IO[11]
|RAM_SIO2 |inout|MPRJ_IO[12]
|RAM_SIO3 |inout|MPRJ_IO[13]
||||
|ROM_CS_N |output|MPRJ_IO[14]
|ROM_CSK |output|MPRJ_IO[15]
|ROM_SIO0 |inout|MPRJ_IO[16]
|ROM_SIO1 |inout|MPRJ_IO[17]
|ROM_SIO2 |inout|MPRJ_IO[18]
|ROM_SIO3 |inout|MPRJ_IO[19]
||||
|VRAM_CS_N |output|MPRJ_IO[20]
|VRAM_CSK |output|MPRJ_IO[21]
|VRAM_SIO0 |inout|MPRJ_IO[22]
|VRAM_SIO1 |inout|MPRJ_IO[23]
|VRAM_SIO2 |inout|MPRJ_IO[24]
|VRAM_SIO3 |inout|MPRJ_IO[25]
||||
|HACK_EXTERNAL_RESET|input|MPRJ_IO[26]
||||
|DISPLAY_VSYNC |output|MPRJ_IO[27]
|DISPLAY_HSYNC |output|MPRJ_IO[28]
|DISPLAY_RGB |output|MPRJ_IO[29]
||||
|GPIO_I_0|input|MPRJ_IO[30]
|GPIO_I_1|input|MPRJ_IO[31]
|GPIO_I_2|input|MPRJ_IO[32]
|GPIO_I_3|input|MPRJ_IO[33]
||||
|GPIO_O_0|output|MPRJ_IO[34]
|GPIO_O_1|output|MPRJ_IO[35]
|GPIO_O_2|output|MPRJ_IO[36]
|GPIO_O_3|output|MPRJ_IO[37]


---
