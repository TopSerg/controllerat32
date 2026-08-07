
import sys
import zlib 

firm_path = sys.argv[1]

with open(firm_path, mode='rb') as file:

    CRC_FIRMWARE_LOCATE 	= 0x08023ff0
    CRC_TABLE_LOCATE 		= 0x08027ff0

    INFO_SECTOR_END_SIZE 	= 0x10

    FIRST_SECTOR_LOCATE     = 0x08004000

    FIRMWARE_LOCATE 		= 0x08004000
    TABLE_LOCATE 			= (CRC_FIRMWARE_LOCATE + INFO_SECTOR_END_SIZE)

    FIRMWARE_LOCATE_LEN 	= (CRC_FIRMWARE_LOCATE - FIRMWARE_LOCATE)
    TABLE_LOCATE_LEN		= (CRC_TABLE_LOCATE - TABLE_LOCATE)
    
    fileContent = file.read()
    # crc_firm_addr = 0x08023ff0
    # tables_crc_addr = 0x08027ff0
    # crc_dev_settings = 0x0802bff0
    #fileContent = fileContent[:(0x08023ff0 - 0x08004000)]
    #print("len crc flash: {}".format(0x7300 - 4))
    #f = open("E:/mr/VisualGDB/Debug_ODC/AT32_Base.bin", "rb+")
    f = open(firm_path, "rb+")
    #f.seek(0x001f3fc)
    seek = (CRC_FIRMWARE_LOCATE - FIRST_SECTOR_LOCATE)
    print("seek: {}".format(hex(seek)))

    f.seek((CRC_FIRMWARE_LOCATE - FIRST_SECTOR_LOCATE))
    crc = zlib.crc32(fileContent[:FIRMWARE_LOCATE_LEN])
    print("crc {}, {} len {}".format(crc.to_bytes(4, byteorder='little'),bytes(crc.to_bytes(4, byteorder='little')), len(fileContent[:FIRMWARE_LOCATE_LEN])))
    f.write(crc.to_bytes(4, byteorder='little'))

    print("seek: {}".format(hex((CRC_TABLE_LOCATE - FIRST_SECTOR_LOCATE))))
    f.seek((CRC_TABLE_LOCATE - FIRST_SECTOR_LOCATE))
    crc = zlib.crc32(fileContent[(TABLE_LOCATE - FIRST_SECTOR_LOCATE):(TABLE_LOCATE - FIRST_SECTOR_LOCATE + TABLE_LOCATE_LEN)])
    print("crc {}, {} len {}".format(crc, hex(crc), len(fileContent[(TABLE_LOCATE - FIRST_SECTOR_LOCATE):(TABLE_LOCATE - FIRST_SECTOR_LOCATE + TABLE_LOCATE_LEN)])))    
    f.write(crc.to_bytes(4, byteorder='little'))

    #print("crc {}, {} len {}".format(zlib.crc32(fileContent), list(crc), len(fileContent)))

    # f.write(bytes(crc))
    f.close()
#    print(f.read(4))
