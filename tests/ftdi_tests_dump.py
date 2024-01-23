#SPI
from pyftdi.ftdi import Ftdi
from pyftdi.spi import SpiController
spi = SpiController(cs_count=2)
spi.configure('ftdi://::/1')
gpio = spi.get_gpio()
gpio.set_direction(0b110100000000, 0b110100000000)


bin(gpio.read(0b110100000000))

# turn on both leds
gpio.write(0b000100000000)

# turn on UART_EN?
gpio.write(0b110000000000)

# turn on UART_EN and 1 led
gpio.write(0b010000000000)

spi.terminate()

#UART
import pyftdi.serialext
# port = pyftdi.serialext.serial_for_url('ftdi://ftdi:232h:3:c/1', baudrate=24000)
port = pyftdi.serialext.serial_for_url('ftdi://::/1', baudrate=24000)
port.read()

port.ftdi.has_cbus




