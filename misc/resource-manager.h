#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "types.h"


#define RESOURCES_STARTING_SECTOR 0x81
#define RESOURCES_INDEX_LENGTH 0x10008



struct __attribute__((__packed__)) ResInfo {
    uint8 file_name[56];
    uint32 start_lba;
    uint32 size;
};



uint8 init_resource_manager();
uint32 resource_size(uint8 *file_name);
uint8 resource_exists(uint8 *file_name);
uint8 load_resource(uint8 *file_name, void *buffer);
uint8 create_resource(uint8 *file_name, void *buffer, uint32 buffer_size);
uint8 delete_resource(uint8 *file_name);





#endif // RESOURCE_MANAGER_H
