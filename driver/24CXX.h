#ifndef __EEPROM_H
#define __EEPROM_H

/**************************************************************
EEPROM����͹���
**************************************************************/
extern void eeprom_sleep();
/**************************************************************
��EEPROM
**************************************************************/
extern char eeprom_read(unsigned short address, unsigned char *buf, unsigned short size);
/**************************************************************
дEEPROM
**************************************************************/
extern char eeprom_write(unsigned short address, unsigned char *buf, unsigned short size);

#endif /* __EEPROM_H */
