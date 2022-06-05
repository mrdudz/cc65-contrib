#define AMICA_KENNBYTE 0xc2
#define AMICA_REPEAT_OFFSET 1
#define AMICA_VALUE_OFFSET 2
#define AMICA_CODE_LEN 3
#define AMICA_END 0xc200

#define ZOOM_REPEAT_OFFSET 1
#define ZOOM_VALUE_OFFSET 0
#define ZOOM_CODE_LEN 3

#define E_BITMAPSIZE 0x1f40
#define E_SCRCOLRAMSIZE 0x3e8

int entp_amicapaint(unsigned char* from, unsigned char* tobmp, 
	unsigned char* toscreen, unsigned char* tovideocol,
	unsigned char* bgcolor);

int entp_zoomatic(unsigned char* endoffileadr, unsigned char* tobmp, 
	unsigned char* toscreen, unsigned char* tovideocol,
	unsigned char* bgcolor);
