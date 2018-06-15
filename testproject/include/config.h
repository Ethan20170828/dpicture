#ifndef __CONFIG_H__
#define __CONFIG_H__

/***********************************DEBUG***********************************/
//#define DEBUG		// �򿪵�����Ϣ����Ŀ���
#ifdef DEBUG
#define debug(...)                                      \
        {                                               \
            fprintf(stderr, "[debug!][%s:%s:%d]",    \
                    __FILE__, __FUNCTION__, __LINE__);	\
			fprintf(stderr, __VA_ARGS__);				\
		}
#else
#define debug(...)
#endif


/**********************************ͼƬ���ݴ��ݽṹ��**********************/
// �ṹ��������װһ��ͼƬ�ĸ�����Ϣ(��Щ��Ϣ�Ѿ�ͨ��open/read��ͼƬ����Ϣ��ȡ�����ˣ����ҷŵ���
// ͼƬ���ļ�ͷ(ClBitMapFileHeader)����Ϣͷ(ClBitMapInfoHeader)�Ľṹ���У����ǿ���ͨ���ṹ��
// ��ֵ��ʽ����Ϣ���ݵ�pic_info�ṹ���У�ͨ���ṹ��ָ��ķ�ʽ�ڸ�������֮�������Ϣ����)��
// �ṹ���װ����ЩͼƬ��Ϣ�����ڸ�������֮�����ͼƬ���ݴ��ݣ�
typedef struct pic_info
{
	char *pathname;			// ͼƬ���ļ�ϵͳ�е�·����+�ļ���
	unsigned int width;		// ͼƬ�ֱ���֮��
	unsigned int height;	// ͼƬ�ֱ���֮��
	unsigned int bpp;		// ͼƬλ��
	char *pData;			// ָ��ͼƬ��Ч���ݴ洢��buffer����
}pic_info;

// BMP�ļ�ͷ�ṹ��
typedef struct  
{  
    //unsigned short	bfType;  		// �ļ�����
    unsigned long	bfSize;  		// �ļ���С
    unsigned short	bfReserved1;  	// ����������Ϊ0
    unsigned short	bfReserved2;  	// ����������Ϊ0
    unsigned long	bfOffBits;  	// ��ͷ��λͼ���ݵ�ƫ��
} ClBitMapFileHeader; 

// BMP ��Ϣͷ
typedef struct  
{  
    unsigned long	biSize;   		// ��Ϣͷ�Ĵ�С
    long			biWidth;   		// ������Ϊ��λ˵��ͼ��Ŀ��
    long			biHeight;   	// ������Ϊ��λ˵��ͼ��ĸ߶�
    unsigned short	biPlanes;   	// ΪĿ���豸˵����ɫƽ�������ܱ��豸Ϊ1
    unsigned short	biBitCount;  	// ˵��������/��������ֵ��1/2/4/8/16/24/32
    unsigned long	biCompression;  // ˵��ͼ���ѹ�����ͣ���õľ���0����ʾ��ѹ��
    unsigned long	biSizeImage;   	// ˵��λͼ���ݵĴ�С������BI_RGB��ʽʱ����������Ϊ0��
    long			biXPelsPerMeter;// ��ʾˮƽ�ֱ��ʣ���λ������/�ף��з���������
    long			biYPelsPerMeter;// ��ʾ��ֱ�ֱ��ʣ���λ������/�ף��з���������
    unsigned long	biClrUsed; 		// ˵��λͼʹ�õĵ�ɫ���е���ɫ��������Ϊ0˵��ʹ�����У�
    unsigned long	biClrImportant; // ˵����ͼ����ʾ����ҪӰ�����ɫ��������Ϊ0˵������Ҫ��
} ClBitMapInfoHeader;

int is_bmp(const char *path);
int display_bmp(const char *pathname);
int is_jpg(const char *path);
int display_jpg(const char *pathname);
int is_png(const char *path);
int display_png(const char *pathname);

int scan_image(const char *path);
void show_images(void);
int ts_updown(void);


// ���ǹ涨���֧��1920*1080��ô���ͼƬ��BPP���24
#define BMP_MAX_RESOLUTION	(1920*1080)
//#define BMP_MAX_RESOLUTION	(5000*5000)
#define RBG_BUF_SIZE		(BMP_MAX_RESOLUTION * 3)

/************************ȫ�ֱ�������*************************************/
extern char rgb_buf[RBG_BUF_SIZE];

// ���ǹ涨���֧��100��ͼƬ
#define MAX_IMAGE_CNT		100

// ����ϵͳ�еĴ������豸���豸��
#define DEVICE_TOUCHSCREEN		"/dev/input/event1"
#define TOUCH_WIDTH				200		// ���崥����ҳ����Ŀ��


#endif
