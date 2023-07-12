# UART_stdio

This example shows how the stdio.h facilities can be used in avr-libc.  Note that this is a pretty minimal subset of the standard library on a real OS.

See [the avr-libc manual](https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html) for details.

In particular, there is no input editing (backspace, etc) are just put in the buffer.  See example code [uart.c](https://www.nongnu.org/avr-libc/examples/stdiodemo/uart.c) for how to fix this.
