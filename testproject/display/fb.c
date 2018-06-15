/**************************************************************************
 *fb.c 操作framebuffer的基础代码，包括fb的打开、ioctl获取信息
 *	   基本的测试fb显示代码	
 **************************************************************************/

// Linux系统自带的头文件
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

// 自己写的头文件
#include <fb.h>
//#include "1024600.h"
//#include "123.h"



// 全局变量
unsigned int *pfb = NULL;	// pfb指向framebuffer内存地址首地址
int fbfd = -1;				// 打开fd后得到的fd

int fb_open(void)
{
	int ret = -1;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	 
	// 第1步：打开设备
	fbfd = open(FBDEVICE, O_RDWR);
	if (fbfd < 0)
	{
		perror("open");
		exit(-1);
	}
	debug("open [%s] success\n", FBDEVICE);
	 
	// 第2步：获取设备的硬件信息
	ret = ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	if (ret < 0)
	{
		perror("ioctl");
		exit(-1);
	}
	debug("smem_start = 0x%lx, smem_len = %u.\n", finfo.smem_start, finfo.smem_len);
	 
	ret = ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
	if (ret < 0)
	{
		perror("ioctl");
		exit(-1);
	}
	debug("xres = %u, yres = %u.\n", vinfo.xres, vinfo.yres);
	debug("xres_virtual = %u, yres_virtual = %u.\n", vinfo.xres_virtual, vinfo.yres_virtual);
	debug("bpp = %u.\n", vinfo.bits_per_pixel);

	// 第3步：使用mmap进行映射
	unsigned long len =  vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
	pfb = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (NULL == pfb)
	{
		perror("mmap");
		exit(-1);
	}
	debug("pfb = %p.\n", pfb);

	return 0;
}

int fb_close(void)
{
	close(fbfd);
	return 0;
}

// 绘制屏幕背景色
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color)
{
	unsigned int x, y;
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			*(pfb + y * width + x) = color;
		}
	}
}

// 画线测试函数
void fb_draw_line(unsigned int color)
{
	unsigned int x;
	for (x=50; x<600; x++)
	{
		*(pfb + 200*WIDTH + x) = color;
		*(pfb + 300*WIDTH + x) = color;
	}
}

#if 0
// 测试显示1024*600分辨率的图片
void fb_draw_picture(void)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_1024600;
	unsigned int *p = pfb;
	unsigned int i, j, cnt;

	for (i=0; i<HEIGHT; i++)
	{
		for (j=0; j<WIDTH; j++)
		{
			// 当前像素点的编号
			//cnt = WIDTH * i + j;
			// 当前像素点的数据在数组中的下标
			//cnt *= 3;
			cnt = 3 * (WIDTH * i + j);
			// 当前像素点对应的图像数据的RGB就应该分别是:
			// pData[cnt+0] pData[cnt+1] pData[cnt+2]
			// 当前像素点的数据
			//*p= ((pData[cnt+0]<<0) | (pData[cnt+1]<<8) | (pData[cnt+2]<<16));
			// 把图片数组中的每个像素点中的三个点依次放到虚拟地址映射中，从首地址开始映射
			*p= ((pData[cnt+0]<<16) | (pData[cnt+1]<<8) | (pData[cnt+2]<<0));
			// 每次移动一个地址
			p++;
		}
	}
}

// 测试显示1024*600分辨率的图片
void fb_draw_picture2(void)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_1024600;
	unsigned int x, y, cnt;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[3*cnt+0]<<16) | (pData[3*cnt+1]<<8) | (pData[3*cnt+2]<<0));
		}
	}
}

// 测试显示1024*600分辨率的图片
void fb_draw_picture3(void)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_1024600;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// 测试显示533*300分辨率(显示比屏幕小的)的图片
void fb_draw_picture4(void)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=0; y<300; y++)
	{
		for (x=0; x<533; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// 测试显示533*300分辨率(显示比屏幕小的)的图片
void fb_draw_picture5(void)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=0; y<300; y++)
	{
		for (x=0; x<533; x++)
		{
			// cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * y + x;
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}

// 函数完成功能: 将图片显示到起点坐标(x0,y0)处
void fb_draw_picture6(unsigned int x0, unsigned int y0)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=y0; y<300+y0; y++)
	{
		for (x=x0; x<533+x0; x++)
		{
			// cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * (y - y0) + (x - x0);

			// 算式的左值和右值分别要去考虑，
			// 左值考虑当前像素点在fb内存中的偏移量，
			// 右值考虑当前像素点在图像数据数组中的下标；
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}


// 测试显示533*300分辨率(显示比屏幕小的)的图片
void fb_draw_picture7(unsigned int x0, unsigned int y0)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=y0; y<300+y0; y++)
	{
		for (x=x0; x<533+x0; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// x和y两个方向超出屏幕外的部分都不显示
void fb_draw_picture8(unsigned int x0, unsigned int y0)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=y0; y<300+y0; y++)
	{
		if (y >= HEIGHT)
		{
			// y方向超出屏幕外了
			break;
		}
		for (x=x0; x<533+x0; x++)
		{
			if (x >= 1024)
			{
				// x方向超出屏幕外了，把超出的去掉，不在显存中显示出来
				a += 3;
				// continue就是跳过当前循环外面的部分
				continue;
			}
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

void fb_draw_picture9(unsigned int x0, unsigned int y0)
{
	// 指针指向图像数组
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=y0; y<300+y0; y++)
	{
		if (y >= HEIGHT)
		{
			// y方向超出屏幕外了
			break;
		}
		for (x=x0; x<533+x0; x++)
		{
			if (x >= 1024)
			{
				// x方向超出屏幕外了，把超出的去掉，不在显存中显示出来
				// continue就是跳过当前循环外面的部分
				continue;
			}
			// cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * (y - y0) + (x - x0);

			// 算式的左值和右值分别要去考虑，
			// 左值考虑当前像素点在fb内存中的偏移量，
			// 右值考虑当前像素点在图像数据数组中的下标；
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}
#endif

// 
void fb_draw(struct pic_info *pPic)
{
	// 指针指向图像数组
	const char *pData = (const char *)pPic->pData;
	unsigned int x, y, cnt1, cnt2;

	
	debug("image resolution: %d * %d, bpp=%d.\n", 
			pPic->width, pPic->height, pPic->bpp);

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	// 打印出待显示的数据，看看是不是很多都是0
	#if 0
	int i;
	for (i=0; i<2000; i++)
	{
		printf("%x ", pPic->pData[i]);
	}
	#endif

	#if 0
	// bmp图片显示
	unsigned int cnt;
	int a = 0;
	for (y=pPic->height; y>0; y--)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+2]<<16) | (pData[a+1]<<8) | (pData[a+0]<<0));
			a += 3;
		}
	}
	#endif

	#if 1
	// bmp图片显示
	for (y=pPic->height; y>0; y--)
	{
		for (x=pPic->width; x>0; x--)
		{
			// cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = (pPic->width) * ((pPic->height) - y) + x;
			*(pfb + cnt1)= ((pData[3*cnt2+2]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+0]<<0));
		}
	}
	#endif

}

void fb_draw2(struct pic_info *pPic)
{
	// 指针指向图像数组
	const char *pData = (const char *)pPic->pData;
	unsigned int x, y, cnt1, cnt2;

	
	debug("image resolution: %d * %d, bpp=%d.\n", 
			pPic->width, pPic->height, pPic->bpp);

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	// 打印出待显示的数据，看看是不是很多都是0
	#if 0
	int i;
	for (i=0; i<2000; i++)
	{
		printf("%x ", pPic->pData[i]);
	}
	#endif

	#if 1
	// bmp图片显示
	unsigned int cnt;
	int a = 0;
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt表示当前像素点的编号
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
	#endif


	#if 0
	// JPEG图片显示
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt表示当前像素点的编号
			cnt1 = WIDTH * y + x;
			cnt2 = (pPic->width) * y + x;
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
	#endif
}

