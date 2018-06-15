/***************************************************************************
 *	���ļ���������JPGͼƬ��������fb.c����ʾ�ӿ�����ʾ��LCD��
 **************************************************************************/
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <string.h>



#include <fb.h>
#include <config.h>


// ��������: �ж�һ��ͼƬ�ļ��ǲ���JPEGͼƬ
// ��������: path��ͼƬ�ļ���pathname
// ����ֵ: �����JPEG�򷵻�0�������򷵻�1�����󷵻�-1
int is_jpg(const char *path)
{
	FILE *file = NULL;
	char buf[2] = {0};
	// ���ļ�
	file = fopen(path, "rb");
	if (NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n", path);
		fclose(file);
		return -1;
	}
	// ����ǰ2���ֽ�
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// �ж��ǲ���0xffd8
	if (!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		// ����jpgͼƬ
	}
	// ��0xffd8��ͷ���ͼ���
	// �ļ�ָ���ƶ�������2���ַ���λ��
	fseek(file, -2, SEEK_END);
	// ����2���ֽ�
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// �ж��ǲ���0xffd9
	if (!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
		fclose(file);
		return 1;		// ����jpgͼƬ
	}

	fclose(file);

	
	return 0;
}

struct my_error_mgr 
{
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

// �Լ�����Ĵ�������
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
	#if 1
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	// output_message���������������ĺ���
	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
	#endif
}

/*
 *	��������: ����JPGͼƬ������������������ݴ洢��
 *	��������: pPic����¼ԴJPGͼƬ�����������ͼƬ��ߡ�ͼƬ���ݻ���������Ϣ��
 *	����ֵ: �ɹ������򷵻�0��ʧ���򷵻�-1��
 ********************************************************************************/
static int jpg_analyze(struct pic_info *pPic)
{
	// cinfo�ᴩ����������̵���Ϣ��¼�ʹ��ݵ����ݽṹ
	struct jpeg_decompress_struct cinfo;
	// ���������
	struct my_error_mgr jerr;
	// ����ʽ�����ļ��������ڷǻ���ʽ��fd��
	// ָ��fopen��ԴJPGͼƬ�ļ���ָ��
	FILE * infile;
	// �����һ��buffer������������������ݴ洢�ģ�
	// ָ����������ݵ�ָ��
	//JSAMPARRAY buffer = NULL;
	char *buffer = NULL;
	// ���������һ��ͼƬ��Ϣ���ֽ���
	int row_stride;

	// rb��ʾֻ���Ķ������ļ�
	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{
		fprintf(stderr, "can't open %s\n", pPic->pathname);
	   	return -1;
	}
	
	/* Step 1: allocate and initialize JPEG decompression object */
	// �����ڴ棬���ҳ�ʼ������
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
	   	jpeg_destroy_decompress(&cinfo);
	   	fclose(infile);
	   	return 0;
	 }
	// ������������Ҫ���ڴ��������ݽṹ�ĳ�ʼ��
	jpeg_create_decompress(&cinfo);
	
	/* Step 2: specify data source (eg, a file) */
	// ��infile�ļ��е���Ϣ��ֵ��cinfo�ṹ���е���Ӧ����
	jpeg_stdio_src(&cinfo, infile);
	
	/* Step 3: read file parameters with jpeg_read_header() */
	// ��JPEGͼƬ��ͷ��Ϣ
	jpeg_read_header(&cinfo, TRUE);

	/* Step 4: Start decompressor */
	// ��ʼ����
	jpeg_start_decompress(&cinfo);
	debug("image resolution: %d * %d, bpp/8=%d.\n", 
		cinfo.output_width, cinfo.output_height, cinfo.output_components);

	/* JSAMPLEs per row in output buffer */
	// �������������һ�е����ж��ٸ��ֽ�
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	// bufferָ����һ��������ռ�Ŀռ��С��
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if (NULL == buffer)
	{
		fprintf(stderr, "cinfo.mem->alloc_sarray is error.\n");
	}
	
	/* Step 5: while (scan lines remain to be read) */
	/* 		  jpeg_read_scanlines(...); */
	// ���н��н��룬��������������ݶ�������׼���õĻ�����ȥ
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		// ����һ����Ϣ�����Ҷ���buffer�У�
	    //jpeg_read_scanlines(&cinfo, buffer, 1);
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		
		#if 0
		// ��Ӵ�ӡ����֤�ǲ��ǽ�����������ݾͲ���
		int i;
		for (i=0; i<2000; i++)
		{
			printf("%x ", pPic->pData[i]);
		}
		#endif
		
		// ��buffer����һ���������ߵ���ĵط�ȥ�ݴ����ʹ�ã���֮��Ҫ�ڳ�buffer�ռ�
		// ����ѭ������һ�ν���һ����ʹ�ã�
	   	memcpy(pPic->pData + (cinfo.output_scanline-1) * row_stride, buffer, row_stride);
	}
	 
	/* Step 6: Finish decompression */
	// ��ɽ���
	jpeg_finish_decompress(&cinfo);

	/* Step 7: Release JPEG decompression object */
	// ��malloc������ڴ��ͷŵ�
	jpeg_destroy_decompress(&cinfo);

	/*Step 8: close file*/
	fclose(infile);

	pPic->width = cinfo.output_width;
	pPic->height = cinfo.output_height;
	pPic->bpp = cinfo.output_components * 8;

	return 0;
}

// ��װ��һ������ʹ�õ�JPG��ʾ����
// ����������ֻ��Ҫһ��JPGͼƬ��pathname���ɣ���Щ���ӵ����ݽṹ����JPG��ʾģ���ڲ������
// ��ȷ��ʾͼƬ����0����ʾ�����г����򷵻�-1
int display_jpg(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;				// Ҫ��ʾ��ͼƬ
	
	// ��һ��: ������ͼƬ�ǲ���JPGͼƬ
	ret = is_jpg(pathname);
	if (ret != 0)
	{
		return -1;
	}

	// �ڶ���: ��ʾ��JPGͼƬ
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	fb_draw2(&picture);

	return 0;
}
