/**********************************************************************************
 *	本文件用来解析BMP图片，并且显示到fb中
 *
 **********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#include <fb.h>
#include <config.h>



/***************************************************************
 * 函数功能: 判断一个图片文件是不是一个合法的bmp图片
 * 返回值: 如果是则返回0.不是则返回-1
***************************************************************/
int is_bmp(const char *path)
{
	int fd = -1;
	unsigned char buf[2] = {0};
	ssize_t ret = -1;

	// 第一步: 打开BMP图片
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", path);
		ret = -1;
		goto close;
	}

	// 第二步: 读取文件头信息
	ret = read(fd, buf, sizeof(buf));
	if (ret != 2)
	{
		fprintf(stderr, "read file header error.\n");
		ret = -1;
		goto close;
	}

	// 解析头
	// 第三步: 判断是不是BMP图片
	if ((buf[0] != 'B') || (buf[1] != 'M'))
	{
		//fprintf(stderr, "file %s is not a bmp picture.\n", path);
		ret = -1;
		goto close;
	}
	else
	{
		ret = 0;
	}

close:
	close(fd);
	return ret;
}


/**********************************************************************************
 *	path: 要解析的bmp图片的pathname
 *  函数功能: 该函数解析path这个bmp图片，并且解析出来的图片数据丢到bmp_buf中去
 *	返回值:	  错误时返回-1，解析正确返回0
 **********************************************************************************/
static int bmp_analyze(struct pic_info *pPic)
{
	int fd = -1;
	ClBitMapFileHeader fHeader;
	ClBitMapInfoHeader iHeader;
	unsigned short tmp;
	unsigned long len;

	// 第一步: 打开BMP图片
	fd = open(pPic->pathname, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", pPic->pathname);
		return -1;
	}

	// 第二步: 读取文件头信息(也就是将fd中的图片头信息放到fHeader结构体中)
	read(fd, &tmp, 2);
	read(fd, &fHeader, sizeof(fHeader));
	debug("bfSize = %ld.\n",fHeader.bfSize);
	debug("bfOffBits = %ld.\n",fHeader.bfOffBits);

	read(fd, &iHeader, sizeof(iHeader));
	debug("picture resolution: %ld * %ld.\n", iHeader.biWidth, iHeader.biHeight);
	debug("biWidth: %ld.\n", iHeader.biWidth);
	debug("biHeight: %ld.\n", iHeader.biHeight);
	debug("picture bpp: %d.\n", iHeader.biBitCount);

	// 将图片的信息头结构体中的数据赋值到负责在函数之间传递信息的结构体中
	// 利用输出型参数的特征
	pPic->width = iHeader.biWidth;
	pPic->height= iHeader.biHeight;
	pPic->bpp= iHeader.biBitCount;

	// 第三步: 读取图片有效信息
	// 先把文件指针移动到有效信息的偏移量处
	lseek(fd, fHeader.bfOffBits, SEEK_SET);
	// off_t lseek(int fd, off_t offset, int whence);
	// 然后读出iHeader.biWidth*iHeader.biHeight*iHeader.biBitCount/3 这么多字节即可
	len = iHeader.biWidth * iHeader.biHeight * iHeader.biBitCount / 3;
	read(fd, rgb_buf, len);
	
	// 最后关闭打开的文件
	close(fd);
	return 0;
}

// 封装的一个对外使用的bmp显示函数
// 本函数对外只需要一个bmp图片的pathname即可，那些复杂的数据结构都是bmp显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1
int display_bmp(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;				// 要显示的图片
	
	// 第一步: 检查给的图片是不是bmp图片
	ret = is_bmp(pathname);
	if (ret != 0)
	{
		return -1;
	}

	// 第二步: 显示该bmp图片
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	bmp_analyze(&picture);

	fb_draw(&picture);

	return 0;
}

