/**************************************************************************
 *fb.c ����framebuffer�Ļ������룬����fb�Ĵ򿪡�ioctl��ȡ��Ϣ
 *	   �����Ĳ���fb��ʾ����	
 **************************************************************************/

// Linuxϵͳ�Դ���ͷ�ļ�
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

// �Լ�д��ͷ�ļ�
#include <fb.h>
//#include "1024600.h"
//#include "123.h"



// ȫ�ֱ���
unsigned int *pfb = NULL;	// pfbָ��framebuffer�ڴ��ַ�׵�ַ
int fbfd = -1;				// ��fd��õ���fd

int fb_open(void)
{
	int ret = -1;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	 
	// ��1�������豸
	fbfd = open(FBDEVICE, O_RDWR);
	if (fbfd < 0)
	{
		perror("open");
		exit(-1);
	}
	debug("open [%s] success\n", FBDEVICE);
	 
	// ��2������ȡ�豸��Ӳ����Ϣ
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

	// ��3����ʹ��mmap����ӳ��
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

// ������Ļ����ɫ
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

// ���߲��Ժ���
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
// ������ʾ1024*600�ֱ��ʵ�ͼƬ
void fb_draw_picture(void)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_1024600;
	unsigned int *p = pfb;
	unsigned int i, j, cnt;

	for (i=0; i<HEIGHT; i++)
	{
		for (j=0; j<WIDTH; j++)
		{
			// ��ǰ���ص�ı��
			//cnt = WIDTH * i + j;
			// ��ǰ���ص�������������е��±�
			//cnt *= 3;
			cnt = 3 * (WIDTH * i + j);
			// ��ǰ���ص��Ӧ��ͼ�����ݵ�RGB��Ӧ�÷ֱ���:
			// pData[cnt+0] pData[cnt+1] pData[cnt+2]
			// ��ǰ���ص������
			//*p= ((pData[cnt+0]<<0) | (pData[cnt+1]<<8) | (pData[cnt+2]<<16));
			// ��ͼƬ�����е�ÿ�����ص��е����������ηŵ������ַӳ���У����׵�ַ��ʼӳ��
			*p= ((pData[cnt+0]<<16) | (pData[cnt+1]<<8) | (pData[cnt+2]<<0));
			// ÿ���ƶ�һ����ַ
			p++;
		}
	}
}

// ������ʾ1024*600�ֱ��ʵ�ͼƬ
void fb_draw_picture2(void)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_1024600;
	unsigned int x, y, cnt;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[3*cnt+0]<<16) | (pData[3*cnt+1]<<8) | (pData[3*cnt+2]<<0));
		}
	}
}

// ������ʾ1024*600�ֱ��ʵ�ͼƬ
void fb_draw_picture3(void)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_1024600;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=0; y<HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// ������ʾ533*300�ֱ���(��ʾ����ĻС��)��ͼƬ
void fb_draw_picture4(void)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=0; y<300; y++)
	{
		for (x=0; x<533; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// ������ʾ533*300�ֱ���(��ʾ����ĻС��)��ͼƬ
void fb_draw_picture5(void)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=0; y<300; y++)
	{
		for (x=0; x<533; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * y + x;
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}

// ������ɹ���: ��ͼƬ��ʾ���������(x0,y0)��
void fb_draw_picture6(unsigned int x0, unsigned int y0)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=y0; y<300+y0; y++)
	{
		for (x=x0; x<533+x0; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * (y - y0) + (x - x0);

			// ��ʽ����ֵ����ֵ�ֱ�Ҫȥ���ǣ�
			// ��ֵ���ǵ�ǰ���ص���fb�ڴ��е�ƫ������
			// ��ֵ���ǵ�ǰ���ص���ͼ�����������е��±ꣻ
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}


// ������ʾ533*300�ֱ���(��ʾ����ĻС��)��ͼƬ
void fb_draw_picture7(unsigned int x0, unsigned int y0)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=y0; y<300+y0; y++)
	{
		for (x=x0; x<533+x0; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

// x��y�������򳬳���Ļ��Ĳ��ֶ�����ʾ
void fb_draw_picture8(unsigned int x0, unsigned int y0)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt;
	unsigned int a = 0;

	for (y=y0; y<300+y0; y++)
	{
		if (y >= HEIGHT)
		{
			// y���򳬳���Ļ����
			break;
		}
		for (x=x0; x<533+x0; x++)
		{
			if (x >= 1024)
			{
				// x���򳬳���Ļ���ˣ��ѳ�����ȥ���������Դ�����ʾ����
				a += 3;
				// continue����������ǰѭ������Ĳ���
				continue;
			}
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
}

void fb_draw_picture9(unsigned int x0, unsigned int y0)
{
	// ָ��ָ��ͼ������
	const unsigned char *pData = gImage_123;
	unsigned int x, y, cnt1, cnt2;

	for (y=y0; y<300+y0; y++)
	{
		if (y >= HEIGHT)
		{
			// y���򳬳���Ļ����
			break;
		}
		for (x=x0; x<533+x0; x++)
		{
			if (x >= 1024)
			{
				// x���򳬳���Ļ���ˣ��ѳ�����ȥ���������Դ�����ʾ����
				// continue����������ǰѭ������Ĳ���
				continue;
			}
			// cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = 533 * (y - y0) + (x - x0);

			// ��ʽ����ֵ����ֵ�ֱ�Ҫȥ���ǣ�
			// ��ֵ���ǵ�ǰ���ص���fb�ڴ��е�ƫ������
			// ��ֵ���ǵ�ǰ���ص���ͼ�����������е��±ꣻ
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
}
#endif

// 
void fb_draw(struct pic_info *pPic)
{
	// ָ��ָ��ͼ������
	const char *pData = (const char *)pPic->pData;
	unsigned int x, y, cnt1, cnt2;

	
	debug("image resolution: %d * %d, bpp=%d.\n", 
			pPic->width, pPic->height, pPic->bpp);

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	// ��ӡ������ʾ�����ݣ������ǲ��Ǻܶ඼��0
	#if 0
	int i;
	for (i=0; i<2000; i++)
	{
		printf("%x ", pPic->pData[i]);
	}
	#endif

	#if 0
	// bmpͼƬ��ʾ
	unsigned int cnt;
	int a = 0;
	for (y=pPic->height; y>0; y--)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+2]<<16) | (pData[a+1]<<8) | (pData[a+0]<<0));
			a += 3;
		}
	}
	#endif

	#if 1
	// bmpͼƬ��ʾ
	for (y=pPic->height; y>0; y--)
	{
		for (x=pPic->width; x>0; x--)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = (pPic->width) * ((pPic->height) - y) + x;
			*(pfb + cnt1)= ((pData[3*cnt2+2]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+0]<<0));
		}
	}
	#endif

}

void fb_draw2(struct pic_info *pPic)
{
	// ָ��ָ��ͼ������
	const char *pData = (const char *)pPic->pData;
	unsigned int x, y, cnt1, cnt2;

	
	debug("image resolution: %d * %d, bpp=%d.\n", 
			pPic->width, pPic->height, pPic->bpp);

	if ((pPic->bpp != 32) && (pPic->bpp != 24))
	{
		fprintf(stderr, "BPP %d is not support.\n", pPic->bpp);
		return;
	}

	// ��ӡ������ʾ�����ݣ������ǲ��Ǻܶ඼��0
	#if 0
	int i;
	for (i=0; i<2000; i++)
	{
		printf("%x ", pPic->pData[i]);
	}
	#endif

	#if 1
	// bmpͼƬ��ʾ
	unsigned int cnt;
	int a = 0;
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt = WIDTH * y + x;
			*(pfb + cnt)= ((pData[a+0]<<16) | (pData[a+1]<<8) | (pData[a+2]<<0));
			a += 3;
		}
	}
	#endif


	#if 0
	// JPEGͼƬ��ʾ
	for (y=0; y<pPic->height; y++)
	{
		for (x=0; x<pPic->width; x++)
		{
			// cnt��ʾ��ǰ���ص�ı��
			cnt1 = WIDTH * y + x;
			cnt2 = (pPic->width) * y + x;
			*(pfb + cnt1)= ((pData[3*cnt2+0]<<16) | (pData[3*cnt2+1]<<8) | (pData[3*cnt2+2]<<0));
		}
	}
	#endif
}

