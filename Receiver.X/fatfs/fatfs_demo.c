#include "fatfs_demo.h"

void FatFsDemo_Tasks(void)
{
    UINT actualLength;
    char data[] = "Hello World!";
    if( SD_SPI_IsMediaPresent() == false){ 
        printf("No media present.\n");
        return; 
    }

    uint8_t result = f_mount(&drive,"",1);
    if (result == FR_OK){
        if (f_open(&file, "HELLO.TXT", FA_WRITE | FA_CREATE_NEW ) == FR_OK) {
            f_write(&file, data, sizeof(data)-1, &actualLength );
            f_close(&file);
        }
        f_mount(0,"0:",0);
    }
    else{
        printf("SD: f_mount error: %u.\n", result);
    }
}