uint8 op_read(uint16 addr);
void op_write(uint16 addr, uint8 data);

uint8 op_readpc();
uint8 op_readpci();
uint8 op_readsp();
uint8 op_readzp(uint8 addr);

void op_writesp(uint8 data);
void op_writezp(uint8 addr, uint8 data);

void op_page(uint16 x, uint16 y);
void op_page_always(uint16 x, uint16 y);
