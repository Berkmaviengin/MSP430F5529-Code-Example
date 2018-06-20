#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "nokia_5110.h"
#include "ff.h"
#include "diskio.h"
/**
 * main.c
 */
int i=0;
FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{

    FRESULT res;
    DIRS dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);

            }

        }
    }

    return res;
}


int main (void)
{
    WDTCTL = WDTPW | WDTHOLD;
    FATFS fs;
    FRESULT res;
    char buff[1024];
    __delay_cycles(1000000);
    res = f_mount(0, &fs);
    if (res == FR_OK) {
        strcpy(buff, "/");
        res = scan_files(buff);

    }

    return res;
}

