#include "resource-manager.h"
#include "misc/allocator.h"
#include "drivers/disk.h"
#include "misc/string.h"
#include "types.h"



static struct ResInfo* res_info;
static uint32 *resource_cnt;
static uint32 *resource_index_len;
static uint32 cursor_lba;



uint8 store_resource_index() {
    uint32 metadata_sectors =
        *resource_index_len / 512 + ((*resource_index_len % 512 == 0) ? 0 : 1);

    return disk_write(resource_index_len, RESOURCES_STARTING_SECTOR,
                      metadata_sectors);
}

uint32 highest_lba_res_index() {
    uint32 i, max_lba = 0, max_lba_i = 0;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] != 0 && res_info[i].start_lba > max_lba) {
            max_lba = res_info[i].start_lba;
            max_lba_i = i;
        }
    }

    return max_lba_i;
}


uint32 free_res_index() {
    uint32 i;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] == 0) {
            return i;
        }
    }

    return 0;
}


// UINT32_MAX means that no such resource found
uint32 get_res_id(uint8 *file_name) {
    uint32 i;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] != 0 && strcmp(file_name, res_info[i].file_name) == 0) {
            return i;
        }
    }

    return UINT32_MAX;    
}


void read_filename(uint8** ptr, uint32 cnt) {
    uint32 i = 0;
    
    for (; **ptr != 0; ++(*ptr), ++i){
        res_info[cnt].file_name[i] = **ptr;
    }
    res_info[cnt].file_name[i] = 0;
    ++(*ptr);
}



uint8 init_resource_manager() {
    union metaDataBuffer {
        uint8* bytes;
        uint32* dwords;
    } metadata_buffer;
        
    metadata_buffer.bytes = malloc(512);
    uint8 res = 1;

    res &= disk_read(metadata_buffer.bytes, RESOURCES_STARTING_SECTOR, 1);

    if (!res) {
        free(metadata_buffer.bytes);
        return 0;
    }

    resource_index_len = metadata_buffer.dwords;
    resource_cnt = metadata_buffer.dwords + 1;

    if (*resource_index_len > 512) {
        uint32 metadata_sectors = *resource_index_len / 512 + ((*resource_index_len % 512 == 0) ? 0 : 1);

        free(metadata_buffer.bytes);
        metadata_buffer.bytes = malloc(8 + (512 * metadata_sectors));
        res &= disk_read(metadata_buffer.bytes, RESOURCES_STARTING_SECTOR,
                         metadata_sectors);

        if (!res) {
            free(metadata_buffer.bytes);
            return 0;
        }
    }

    // res_info = malloc(sizeof(struct ResInfo) * (resource_index_len - 8) / 64);

    res_info = (void*) (metadata_buffer.bytes + 8);

    uint32 highest_lba_i = highest_lba_res_index();
    uint32 sz = res_info[highest_lba_i].size / 512 + ((res_info[highest_lba_i].size % 512 == 0) ? 0 : 1);
    
    cursor_lba = res_info[highest_lba_i].start_lba + sz;
    
    // uint32 res_cnt = 0;
    // uint8* ptr = metadata_buffer.bytes + 8;
    // uint8 *break_ptr = metadata_buffer.bytes + resource_index_len;

    // while (ptr < break_ptr) {
    //     read_filename(&ptr, res_cnt);

    //     res_info[res_cnt].start_lba = *( (uint32*) ptr );
    //     ptr += 4;

    //     res_info[res_cnt].size = *( (uint32*) ptr );
    //     ptr += 4;

    //     ++res_cnt;
    // }

    // free(metadata_buffer.bytes);

    // if (res_cnt != resource_cnt) {
    //     return 0;
    // }
    

    return 1;
}


uint32 resource_size(uint8* file_name) {
    uint32 i;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] != 0 && strcmp(file_name, res_info[i].file_name) == 0) {
            return res_info[i].size;
        }
    }

    return 0;
}


uint8 resource_exists(uint8 *file_name) {
    uint32 i;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] != 0 && strcmp(file_name, res_info[i].file_name) == 0) {
            return 1;
        }
    }

    return 0;
}


uint8 load_resource(uint8* file_name, void* buffer) {
    uint32 i;

    for (i = 0; i < (*resource_index_len - 8) / 64; ++i) {
        if (res_info[i].file_name[0] != 0 && strcmp(file_name, res_info[i].file_name) == 0) {
            uint32 res_full_sectors = res_info[i].size / 512;
            uint32 res_remainder_bytes = res_info[i].size - res_full_sectors;
            uint32 res_start_lba = res_info[i].start_lba;
            uint32 i;
            uint8 res = 1;

            if (res_full_sectors != 0) {
                res &= disk_read(buffer, RESOURCES_STARTING_SECTOR + res_start_lba, res_full_sectors);
            }

            if (!res) {
                return 0;
            }

            uint8* tmp_buffer = malloc(512);

            res &= disk_read(tmp_buffer, RESOURCES_STARTING_SECTOR + res_start_lba + res_full_sectors, 1);
            
            if (!res) {
                free(tmp_buffer);
                return 0;
            }

            uint8* dst_rem_buffer = (uint8*) buffer + 512 * res_full_sectors;

            for (i = 0; i < res_remainder_bytes; ++i) {
                dst_rem_buffer[i] = tmp_buffer[i];
            }

            free(tmp_buffer);

            return 1;
        }
    }

    return 0;
}



uint8 create_resource(uint8 *file_name, void *buffer, uint32 buffer_size) {
    uint32 new_res_sectors = buffer_size / 512 + ((buffer_size % 512 == 0) ? 0 : 1);
    uint32 new_res_i = free_res_index();
    uint8 res;
    

    strcpy(file_name, (uint8*) &(res_info[new_res_i].file_name), 56);
    res_info[new_res_i].size = buffer_size;
    res_info[new_res_i].start_lba = cursor_lba;

    *resource_cnt += 1;

    res = store_resource_index();

    if (!res) {
        return 0;
    }
    
    res = disk_write(buffer, RESOURCES_STARTING_SECTOR + cursor_lba, new_res_sectors);

    if (!res) {
        return 0;
    }
    
    cursor_lba += new_res_sectors;

    return 1;
}



uint8 delete_resource(uint8 *file_name) {
    uint32 res_id = get_res_id(file_name);
    uint32 i;
    uint8 res;

    if (res_id == UINT32_MAX) {
        return 0;
    }
    
    for (i = 0; i < 56; ++i) {
        res_info[res_id].file_name[i] = 0;
    }

    res_info[res_id].size = res_info[res_id].start_lba = 0;

    res = store_resource_index();

    if (!res) {
        return 0;
    }

    return 1;
}


