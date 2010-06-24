# AHNS flash the chip with ____________

PART=ATmega8
PROGRAMMER=stk500v2
PORT=/dev/ttyS0
BAUD=115200
FILE=groundpulse.hex

avrdude -p $PART -e -c $PROGRAMMER -P $PORT -b $BAUD -U flash:w:$FILE

