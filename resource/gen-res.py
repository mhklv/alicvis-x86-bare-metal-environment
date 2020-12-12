#!/bin/python3

import os



filenames = []
filepaths = []
filesizes = []

resource_file = open('./res.txt', 'r')
i = 0

for row in resource_file:
    space_pos = row.find(' ')
    filenames.append(row[0:space_pos])
    filepaths.append(row[space_pos + 1:].rstrip())

resource_file.close()


# drive structure:
# <metadata-size> -- 4 bytes
# <resources-number> -- 4 bytes
# indexing structures (<filename> (56 bytes), <starting LBA>, <size>), a total of <resources-number> entries
# Actual data
res_drive = open('./res-drive.img', 'wb')

# metadata_end = 8
# for i in range(len(filenames)):
#     metadata_end += 8 + len(filenames[i]) + 1

# metadata_end = 8 + 64 * len(filenames)
metadata_end = 8 + 64 * 1024

res_drive.write(metadata_end.to_bytes(4, 'little'))
res_drive.write(len(filenames).to_bytes(4, 'little'))


metadata_end += ( 0 if (metadata_end % 512 == 0) else -(metadata_end % 512) + 512 )
res_ptr = metadata_end


for i in range(len(filenames)):
    filename = filenames[i] #.encode('ascii')
    filename = (filename[:55]) if len(filename) > 55 else filename
    zeroes = [0 for i in range(0 if len(filename) >= 56 else 56 - len(filename))]
    
    res_drive.write(filename.encode('ascii'))

    for zero in zeroes:
        res_drive.write(zero.to_bytes(1, 'little'))
    
    # res_drive.write((0).to_bytes(1, 'little'))
    res_drive.write((res_ptr // 512).to_bytes(4, 'little'))

    f = open(filepaths[i], 'rb')

    fsize = os.path.getsize(filepaths[i])
    sectors = fsize // 512 + ( 0 if (fsize % 512 == 0) else 1 )
    res_drive.write(fsize.to_bytes(4, 'little'))

    metadata_cur_offset = res_drive.tell()

    res_drive.seek(res_ptr)
    res_drive.write(f.read())
    res_ptr += sectors * 512

    res_drive.seek(metadata_cur_offset)

    f.close()

    
# res_drive.seek(res_ptr + 511)
res_drive.seek(res_ptr + 512*1000 - 1)
res_drive.write((0).to_bytes(1, 'little'))


res_drive.close()
