#include "types.h"


void itostr(uint8 *out_buf, uint32 num) {
    uint32 i;
    char tmp_buf[16];

    if (num == 0) {
        out_buf[0] = '0';
        out_buf[1] = 0;
        return;
    }
    
    for (i = 0; num != 0; ++i) {
        tmp_buf[i] = (char) (num % 10) + 0x30;
        num = num / 10;
    }

    uint32 j;
    for (j = 0; i-- > 0; ++j) {
        out_buf[j] = tmp_buf[i];
    }
    out_buf[j] = '\0';
}


int8 strcmp(uint8 *str1, uint8 *str2) {
    uint32 i = 0;

    while (1) {
        if (str1[i] < str2[i]) {
            return -1;
        }
        else if (str1[i] > str2[i]) {
            return 1;
        }
        else if (str1[i] == 0 || str2[i] == 0) {
            return 0;
        }

        ++i;
    }
}


int8 wstrcmp(uint16 *str1, uint16 *str2) {
    uint32 i = 0;

    while (1) {
        if (str1[i] < str2[i]) {
            return -1;
        }
        else if (str1[i] > str2[i]) {
            return 1;
        }
        else if (str1[i] == 0 || str2[i] == 0) {
            return 0;
        }

        ++i;
    }
}


uint32 strlen(uint8 *str) {
    uint32 i = 0, len_cnt = 0;

    while(str[i] != 0) {
        ++len_cnt;
        ++i;
    }

    return len_cnt;
}


void strcpy(uint8 *src, uint8 *dst, uint32 len) {
    uint32 i;

    for (i = 0; i < len && src[i] != 0; ++i) {
        dst[i] = src[i];
    }
    
    if (i == len) {
        dst[len - 1] = 0;
    }
    else {
        dst[i] = 0;
    }
}

