#ifndef __CONFIG_H__
#define __CONFIG_H__

/***********************************DEBUG***********************************/
//#define DEBUG		// 打开调试信息输出的开关
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


/**********************************图片数据传递结构体**********************/
// 结构体用来封装一个图片的各种信息(这些信息已经通过open/read将图片的信息提取出来了，并且放到了
// 图片的文件头(ClBitMapFileHeader)和信息头(ClBitMapInfoHeader)的结构体中，我们可以通过结构体
// 赋值方式将信息传递到pic_info结构体中，通过结构体指针的方式在各个函数之间进行信息传递)；
// 结构体封装的这些图片信息可以在各个函数之间进行图片数据传递；
typedef struct pic_info
{
	char *pathname;			// 图片在文件系统中的路径名+文件名
	unsigned int width;		// 图片分辨率之宽
	unsigned int height;	// 图片分辨率之高
	unsigned int bpp;		// 图片位深
	char *pData;			// 指向图片有效数据存储的buffer数据
}pic_info;

// BMP文件头结构体
typedef struct  
{  
    //unsigned short	bfType;  		// 文件类型
    unsigned long	bfSize;  		// 文件大小
    unsigned short	bfReserved1;  	// 保留，必须为0
    unsigned short	bfReserved2;  	// 保留，必须为0
    unsigned long	bfOffBits;  	// 从头到位图数据的偏移
} ClBitMapFileHeader; 

// BMP 信息头
typedef struct  
{  
    unsigned long	biSize;   		// 信息头的大小
    long			biWidth;   		// 以像素为单位说明图像的宽度
    long			biHeight;   	// 以像素为单位说明图像的高度
    unsigned short	biPlanes;   	// 为目标设备说明颜色平面数，总被设备为1
    unsigned short	biBitCount;  	// 说明比特数/像素数，值有1/2/4/8/16/24/32
    unsigned long	biCompression;  // 说明图像的压缩类型，最常用的就是0，表示不压缩
    unsigned long	biSizeImage;   	// 说明位图数据的大小，当用BI_RGB格式时，可以设置为0；
    long			biXPelsPerMeter;// 表示水平分辨率，单位是像素/米，有符号整数；
    long			biYPelsPerMeter;// 表示垂直分辨率，单位是像素/米，有符号整数；
    unsigned long	biClrUsed; 		// 说明位图使用的调色板中的颜色索引数，为0说明使用所有；
    unsigned long	biClrImportant; // 说明对图像显示有重要影响的颜色索引数，为0说明都重要；
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


// 我们规定最大支持1920*1080这么大的图片，BPP最多24
#define BMP_MAX_RESOLUTION	(1920*1080)
//#define BMP_MAX_RESOLUTION	(5000*5000)
#define RBG_BUF_SIZE		(BMP_MAX_RESOLUTION * 3)

/************************全局变量声明*************************************/
extern char rgb_buf[RBG_BUF_SIZE];

// 我们规定最多支持100张图片
#define MAX_IMAGE_CNT		100

// 定义系统中的触摸屏设备的设备名
#define DEVICE_TOUCHSCREEN		"/dev/input/event1"
#define TOUCH_WIDTH				200		// 定义触摸翻页区域的宽度


#endif
