/**********************************************************************************
 *	���ļ���������BMPͼƬ��������ʾ��fb��
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
 * ��������: �ж�һ��ͼƬ�ļ��ǲ���һ���Ϸ���bmpͼƬ
 * ����ֵ: ������򷵻�0.�����򷵻�-1
***************************************************************/
int is_bmp(const char *path)
{
	int fd = -1;
	unsigned char buf[2] = {0};
	ssize_t ret = -1;

	// ��һ��: ��BMPͼƬ
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", path);
		ret = -1;
		goto close;
	}

	// �ڶ���: ��ȡ�ļ�ͷ��Ϣ
	ret = read(fd, buf, sizeof(buf));
	if (ret != 2)
	{
		fprintf(stderr, "read file header error.\n");
		ret = -1;
		goto close;
	}

	// ����ͷ
	// ������: �ж��ǲ���BMPͼƬ
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
 *	path: Ҫ������bmpͼƬ��pathname
 *  ��������: �ú�������path���bmpͼƬ�����ҽ���������ͼƬ���ݶ���bmp_buf��ȥ
 *	����ֵ:	  ����ʱ����-1��������ȷ����0
 **********************************************************************************/
static int bmp_analyze(struct pic_info *pPic)
{
	int fd = -1;
	ClBitMapFileHeader fHeader;
	ClBitMapInfoHeader iHeader;
	unsigned short tmp;
	unsigned long len;

	// ��һ��: ��BMPͼƬ
	fd = open(pPic->pathname, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "open %s error.\n", pPic->pathname);
		return -1;
	}

	// �ڶ���: ��ȡ�ļ�ͷ��Ϣ(Ҳ���ǽ�fd�е�ͼƬͷ��Ϣ�ŵ�fHeader�ṹ����)
	read(fd, &tmp, 2);
	read(fd, &fHeader, sizeof(fHeader));
	debug("bfSize = %ld.\n",fHeader.bfSize);
	debug("bfOffBits = %ld.\n",fHeader.bfOffBits);

	read(fd, &iHeader, sizeof(iHeader));
	debug("picture resolution: %ld * %ld.\n", iHeader.biWidth, iHeader.biHeight);
	debug("biWidth: %ld.\n", iHeader.biWidth);
	debug("biHeight: %ld.\n", iHeader.biHeight);
	debug("picture bpp: %d.\n", iHeader.biBitCount);

	// ��ͼƬ����Ϣͷ�ṹ���е����ݸ�ֵ�������ں���֮�䴫����Ϣ�Ľṹ����
	// ��������Ͳ���������
	pPic->width = iHeader.biWidth;
	pPic->height= iHeader.biHeight;
	pPic->bpp= iHeader.biBitCount;

	// ������: ��ȡͼƬ��Ч��Ϣ
	// �Ȱ��ļ�ָ���ƶ�����Ч��Ϣ��ƫ������
	lseek(fd, fHeader.bfOffBits, SEEK_SET);
	// off_t lseek(int fd, off_t offset, int whence);
	// Ȼ�����iHeader.biWidth*iHeader.biHeight*iHeader.biBitCount/3 ��ô���ֽڼ���
	len = iHeader.biWidth * iHeader.biHeight * iHeader.biBitCount / 3;
	read(fd, rgb_buf, len);
	
	// ���رմ򿪵��ļ�
	close(fd);
	return 0;
}

// ��װ��һ������ʹ�õ�bmp��ʾ����
// ����������ֻ��Ҫһ��bmpͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����bmp��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1
int display_bmp(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;				// Ҫ��ʾ��ͼƬ
	
	// ��һ��: ������ͼƬ�ǲ���bmpͼƬ
	ret = is_bmp(pathname);
	if (ret != 0)
	{
		return -1;
	}

	// �ڶ���: ��ʾ��bmpͼƬ
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	bmp_analyze(&picture);

	fb_draw(&picture);

	return 0;
}

