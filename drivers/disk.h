#ifndef DISK_H
#define DISK_H

#include "types.h"

    

enum ATAStatusRegFlags {
    ATA_STATUS_ERR  = 0b00000001,
    ATA_STATUS_IDX  = 0b00000010,
    ATA_STATUS_CORR = 0b00000100,
    ATA_STATUS_DRQ  = 0b00001000,
    ATA_STATUS_SRV  = 0b00010000,
    ATA_STATUS_DF   = 0b00100000,
    ATA_STATUS_RDY  = 0b01000000,
    ATA_STATUS_BSY  = 0b10000000
};


#define ATA_PB_BASE_PORT        0x1F0  // PB -- Primary Bus
#define ATA_PB_DATA_REG         ATA_PB_BASE_PORT + 0
#define ATA_PB_ERROR_REG        ATA_PB_BASE_PORT + 1
#define ATA_PB_FEATURES_REG     ATA_PB_BASE_PORT + 1
#define ATA_PB_SECTOR_COUNT_REG ATA_PB_BASE_PORT + 2
#define ATA_PB_LBALO_REG        ATA_PB_BASE_PORT + 3
#define ATA_PB_LBAMID_REG       ATA_PB_BASE_PORT + 4
#define ATA_PB_LBAHI_REG        ATA_PB_BASE_PORT + 5
#define ATA_PB_DRIVE_HEAD_REG   ATA_PB_BASE_PORT + 6
#define ATA_PB_STATUS_REG       ATA_PB_BASE_PORT + 7
#define ATA_PB_COMMAND_REG      ATA_PB_BASE_PORT + 7

#define ATA_PB_CONTROL_BASE_PORT    0x3F6
#define ATA_PB_ALT_STATUS_REG       ATA_PB_CONTROL_BASE_PORT + 0
#define ATA_PB_CONTROL_REG          ATA_PB_CONTROL_BASE_PORT + 0
#define ATA_PB_FRIVE_ADDR_REG       ATA_PB_CONTROL_BASE_PORT + 1


uint8 init_disk();
uint8 disk_read(void *buffer, uint32 lba, uint32 sectors);
uint8 disk_write(void* buffer, uint32 lba, uint32 sectors);

uint8 pio24_read(void* buffer, uint32 lba, uint8 sectors_count);
uint8 pio24_write(void *buffer, uint32 lba, uint32 sectors_count);






#endif // DISK_H
