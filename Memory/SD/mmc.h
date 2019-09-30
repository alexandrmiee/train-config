//uint8_t detectCard(void);

DSTATUS disk_status_mmc (
    BYTE drv            /* Drive number (0) */
);


DSTATUS disk_initialize_mmc (
	BYTE drv		/* Physical drive nmuber (0) */
);

DRESULT disk_read_mmc (
    BYTE drv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
);


DRESULT disk_write_mmc (
    BYTE drv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
);
		
DRESULT disk_ioctl_mmc (
    BYTE drv,        /* Physical drive nmuber (0) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
);
		
