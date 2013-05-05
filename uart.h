void uart_init(void);

/* uart_receive: Get one octet from UART. A negative number
is returned if there was no new data. */
int16_t uart_receive(void);
