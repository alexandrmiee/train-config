#ifndef _JEDEC_H_
#define _JEDEC_H_

#define WRITE_ENABLE	0x06
#define WRITE_DISABLE 0x04
#define READ_IDENTIFICATION_H 0x9F
#define READ_IDENTIFICATION_L 0x9E
#define READ_STATUS_REGISTER 0x05
#define WRITE_STATUS_REGISTER 0x01
#define READ_DATA_BYTES 0x03
#define READ_DATA_BYTES_HS 0x0B
#define PAGE_PROGRAM 0x02
#define SECTOR_ERASE 0xD8
#define BULK_ERASE 0xC7
#define DEEP_PD 0xB9
#define RELEASE_DEEP_PD 0xAB
#define READ_ELECTRONIC_SIGNATURE	0xAB


#define STATUS_REGISTER_WIP	 (1<<0) /*<- The write in progress (WIP) bit indicates whether the memory is busy*/
#define STATUS_REGISTER_WEL	 (1<<1) /*<-The write enable latch (WEL) bit indicates the status of the internal write enable latch*/
#define STATUS_REGISTER_BP0	 (1<<2)
#define STATUS_REGISTER_BP1  (1<<3)
#define STATUS_REGISTER_BP2  (1<<4)
#define STATUS_REGISTER_BP   (7<<2)
#define STATUS_REGISTER_ZERO (3<<5)
#define STATUS_REGISTER_SWDR (1<<7)

#define SECTOR_PROTECT_HALF	0x001F0000
#define SECTOR_PROTECT_4TH	0x002F0000
#define SECTOR_PROTECT_8TH	0x00380000
#define SECTOR_PROTECT_16TH	0x003B0000
#define SECTOR_PROTECT_32TH	0x003D0000
#define SECTOR_PROTECT_64TH	0x003E0000



#define SET		1
#define RESET	0

typedef struct{
	uint8_t manufactureId;
	uint8_t memoryType;
	uint8_t memoryCapacity;
	uint8_t uid[17];
}__attribute__ ((__packed__)) Jedec_st;

typedef enum  WriteStatus{init=0,sendData,write,wait,error} WriteStatus_en;

uint8_t writeFlash(uint8_t* buf, uint32_t adr, uint16_t len);
uint8_t eraseFlash(void);
uint8_t eraseSector(uint32_t adr);
uint8_t readFlash(uint8_t *buf, uint32_t adr, uint16_t len);
void changeProtection(uint8_t isWp);


#endif /*_JEDEC_H_*/
