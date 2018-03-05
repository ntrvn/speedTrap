void serial_init(unsigned short ubrr_value);
void serial_txchar(char ch);
ISR(USART_RX_vect);