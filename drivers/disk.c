#include "disk.h"
#include "misc/io.h"
#include "drivers/timer.h"



// Only 1 disk is supported
uint8 init_disk() {
    uint8 status, i;
    
    // Select 'master' drive
    outb(ATA_PB_DRIVE_HEAD_REG, 0b11100000);

    // wait a little
    sleep(3);

    status = inb(ATA_PB_STATUS_REG);

    if ((status & (ATA_STATUS_ERR | ATA_STATUS_DF))) {
        return 0;
    }

    // Disable interrupts
    outb(ATA_PB_CONTROL_REG, 0b00000010);

    return 1;
}


uint8 pio24_write(void* buffer, uint32 lba, uint32 sectors_count) {
    uint8 status, error = 0;
    uint32 i, j;
    int32 retries;

    // select lowest 28 bit of the LBA
    lba = lba & (~0xF0000000);

    // select 'master' drive, LBA addressing mode, highest 4 bist of the LBA
    outb(ATA_PB_DRIVE_HEAD_REG, 0b11100000 | (lba >> 24));

    // load LBA and sector count
    outb(ATA_PB_SECTOR_COUNT_REG, sectors_count);
    outb(ATA_PB_LBALO_REG, lba);
    outb(ATA_PB_LBAMID_REG, lba >> 8);
    outb(ATA_PB_LBAHI_REG, lba >> 16);

    outb(ATA_PB_COMMAND_REG, 0x30);

    for (i = 0; i < sectors_count; ++i) {
        for (retries = 1000000; retries > 0; retries) {
            status = inb(ATA_PB_STATUS_REG);

	    if (status & ATA_STATUS_DRQ) {
		break;
	    }

            if (status & ATA_STATUS_BSY) {
                // sleep(1);
                continue;
            }
            else {
                if (!(status & ATA_STATUS_RDY)) {
                    // Disk needs to spin up
                    sleep(1000);
                }
                else if (status & ATA_STATUS_DRQ) {
                    break;
                }
                else if ((status & ATA_STATUS_ERR) || (status & ATA_STATUS_DF)) {
                    continue;
                    // return 0;
                }
            }
        }

        if (retries == 0) {
            return 0;
        }
        else {
            asm volatile (
                "cld;"
                "movl $256, %%ecx;"
	    	"1:"
	    	"outsw;"
	    	"nop;"
	    	"nop;"
	    	"dec %%ecx;"
	    	"cmpl $0, %%ecx;"
	    	"jne 1b;"
                : [buffer] "+S" (buffer)
                : [data_reg] "d" (ATA_PB_DATA_REG)
                : "memory", "cc", "ecx"
            );
        }
    }

    outb(ATA_PB_COMMAND_REG, 0xE7); // Flush Cache command

    // Wait after cache flush
    for (;;) {
	status = inb(ATA_PB_STATUS_REG);

	if (!(status & ATA_STATUS_BSY)) {
	    break;
	}
    }

    return 1;
}


uint8 pio24_read(void* buffer, uint32 lba, uint8 sectors_count) {
    uint8 status, error = 0;
    uint32 i, j;
    int32 retries;
    
    // truncate lba to 28 bits
    lba = lba & (~0xF0000000);
    
    // select 'master' drive, LBA addressing mode, highest 4 bist of the LBA
    outb(ATA_PB_DRIVE_HEAD_REG, 0b11100000 | (lba >> 24));

    // load LBA and sector count
    outb(ATA_PB_SECTOR_COUNT_REG, sectors_count);
    outb(ATA_PB_LBALO_REG, lba);
    outb(ATA_PB_LBAMID_REG, lba >> 8);
    outb(ATA_PB_LBAHI_REG, lba >> 16);

    // send a 'read sectors' command
    outb(ATA_PB_COMMAND_REG, 0x20);

    for (i = 0; i < sectors_count; ++i) {
        for (retries = 1000000; retries > 0; retries) {
            status = inb(ATA_PB_STATUS_REG);

            if (status & ATA_STATUS_BSY) {
                // sleep(1);
                continue;
            }
            else {
                if (!(status & ATA_STATUS_RDY)) {
                    // Disk needs to spin up
                    sleep(1000);
                }
                else if (status & ATA_STATUS_DRQ) {
                    break;
                }
                else if ((status & ATA_STATUS_ERR) || (status & ATA_STATUS_DF)) {
                    continue;
                    // return 0;
                }
            }
        }

        if (retries == 0) {
            return 0;
        }
        else {
            asm volatile (
                "cld;"
                "movl $256, %%ecx;"
                "rep insw;"
                : [buffer] "+D" (buffer)
                : [data_reg] "d" (ATA_PB_DATA_REG)
                : "memory", "cc", "ecx", "flags"
            );
        }
    }

    return 1;
}


uint8 disk_write(void *buffer, uint32 lba, uint32 sectors) {
    uint8 status;
    
    for (;;) {
        if (sectors == 0) {
            return 1;
        }
        else if (sectors >= 255) {
            status = pio24_write(buffer, lba, 255);

            if (status == 0) {
                return 0;
            }
            
            sectors -= 255;
            lba += 255;
            buffer += 512 * 255;
        }
        else {
            status = pio24_write(buffer, lba, sectors);

            if (status == 0) {
                return 0;
            }

            return 1;
        }
    }
}


uint8 disk_read(void* buffer, uint32 lba, uint32 sectors) {
    uint8 status;
    
    for (;;) {
        if (sectors == 0) {
            return 1;
        }
        else if (sectors >= 255) {
            status = pio24_read(buffer, lba, 255);

            if (status == 0) {
                return 0;
            }
            
            sectors -= 255;
            lba += 255;
            buffer += 512 * 255;
        }
        else {
            status = pio24_read(buffer, lba, sectors);

            if (status == 0) {
                return 0;
            }

            return 1;
        }
    }
}

