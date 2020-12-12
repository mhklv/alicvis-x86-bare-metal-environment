#ifndef STRING_H
#define STRING_H

#include "..//types.h"


void itostr(uint8 *out_buf, uint32 num);
int8 strcmp(uint8 *str1, uint8 *str2);
int8 wstrcmp(uint16 *str1, uint16 *str2);
void strcpy(uint8 *src, uint8 *dst, uint32 len);
uint32 strlen(uint8 *str);


#endif // STRING_H

