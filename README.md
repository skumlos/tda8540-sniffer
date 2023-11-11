TDA8540 sniffer

Used to sniff which composite channel is selected in Barco ADVM monitors to
allow automatically blanking on when a certain composite channel is selected.

It is assumed that this is coupled together with the TDA9321H RGB enabler from
https://github.com/skumlos/tda9321h-i2c-bridge but since adding two Arduino's
to the I2C bus seems to load it too much, causing erroneous I2C communication,
and since this actually doesn't need to mangle the I2C packets in any way,
a non-inverting buffer should be added to each I2C signal, SDA and SCL along
with a 100R resistor, so the Arduino *thinks* it speaks on the line, but instead
just gets the same information as the real TDA8540.

Further information in the .ino file...

(2023) Martin Hejnfelt (martin@hejnfelt.com)
