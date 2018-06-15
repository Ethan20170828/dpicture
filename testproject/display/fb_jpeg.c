/***************************************************************************
 *	本文件用来解码JPG图片，并调用fb.c的显示接口来显示到LCD上
 **************************************************************************/
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>
#include <string.h>



#include <fb.h>
#include <config.h>


// 函数功能: 判断一个图片文件是不是JPEG图片
// 函数传参: path是图片文件的pathname
// 返回值: 如果是JPEG则返回0，不是则返回1，错误返回-1
int is_jpg(const char *path)
{
	FILE *file = NULL;
	char buf[2] = {0};
	// 打开文件
	file = fopen(path, "rb");
	if (NULL == file)
	{
		fprintf(stderr, "fopen %s error.\n", path);
		fclose(file);
		return -1;
	}
	// 读出前2个字节
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// 判断是不是0xffd8
	if (!((buf[0] == 0xff) && (buf[1] == 0xd8)))
	{
		fclose(file);
		return 1;		// 不是jpg图片
	}
	// 是0xffd8开头，就继续
	// 文件指针移动到倒数2个字符的位置
	fseek(file, -2, SEEK_END);
	// 读出2个字节
	fread(buf, 1, 2, file);
	debug("read: 0x%x%x\n", buf[0], buf[1]);
	// 判断是不是0xffd9
	if (!((buf[0] == 0xff) && (buf[1] == 0xd9)))
	{
		fclose(file);
		return 1;		// 不是jpg图片
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

// 自己定义的错误处理函数
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
	#if 1
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	// output_message函数是输出错误处理的函数
	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer, 1);
	#endif
}

/*
 *	函数功能: 解码JPG图片，并将解码出来的数据存储；
 *	函数传参: pPic，记录源JPG图片，解码出来的图片宽高、图片数据缓冲区等信息；
 *	返回值: 成功解码则返回0；失败则返回-1；
 ********************************************************************************/
static int jpg_analyze(struct pic_info *pPic)
{
	// cinfo贯穿整个解码过程的信息记录和传递的数据结构
	struct jpeg_decompress_struct cinfo;
	// 管理错误处理
	struct my_error_mgr jerr;
	// 缓冲式访问文件，类似于非缓冲式的fd；
	// 指向fopen打开源JPG图片文件的指针
	FILE * infile;
	// 输出的一行buffer，用来做解码出来数据存储的；
	// 指向解码行数据的指针
	//JSAMPARRAY buffer = NULL;
	char *buffer = NULL;
	// 解码出来的一行图片信息的字节数
	int row_stride;

	// rb表示只读的二进制文件
	if ((infile = fopen(pPic->pathname, "rb")) == NULL) 
	{
		fprintf(stderr, "can't open %s\n", pPic->pathname);
	   	return -1;
	}
	
	/* Step 1: allocate and initialize JPEG decompression object */
	// 申请内存，并且初始化解码
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
	   	jpeg_destroy_decompress(&cinfo);
	   	fclose(infile);
	   	return 0;
	 }
	// 给解码器做必要的内存分配和数据结构的初始化
	jpeg_create_decompress(&cinfo);
	
	/* Step 2: specify data source (eg, a file) */
	// 将infile文件中的信息赋值给cinfo结构体中的相应变量
	jpeg_stdio_src(&cinfo, infile);
	
	/* Step 3: read file parameters with jpeg_read_header() */
	// 读JPEG图片的头信息
	jpeg_read_header(&cinfo, TRUE);

	/* Step 4: Start decompressor */
	// 开始解码
	jpeg_start_decompress(&cinfo);
	debug("image resolution: %d * %d, bpp/8=%d.\n", 
		cinfo.output_width, cinfo.output_height, cinfo.output_components);

	/* JSAMPLEs per row in output buffer */
	// 解码出来的数据一行到低有多少个字节
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	// buffer指的是一行数据所占的空间大小；
	//buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	buffer = (char *)malloc(row_stride);
	if (NULL == buffer)
	{
		fprintf(stderr, "cinfo.mem->alloc_sarray is error.\n");
	}
	
	/* Step 5: while (scan lines remain to be read) */
	/* 		  jpeg_read_scanlines(...); */
	// 按行进行解码，并将解码出的数据丢到事先准备好的缓冲区去
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		// 解码一行信息，并且丢到buffer中；
	    //jpeg_read_scanlines(&cinfo, buffer, 1);
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		
		#if 0
		// 添加打印来验证是不是解码出来的数据就不对
		int i;
		for (i=0; i<2000; i++)
		{
			printf("%x ", pPic->pData[i]);
		}
		#endif
		
		// 将buffer中这一行数据移走到别的地方去暂存或者使用，总之是要腾出buffer空间
		// 来给循环的下一次解码一行来使用；
	   	memcpy(pPic->pData + (cinfo.output_scanline-1) * row_stride, buffer, row_stride);
	}
	 
	/* Step 6: Finish decompression */
	// 完成解码
	jpeg_finish_decompress(&cinfo);

	/* Step 7: Release JPEG decompression object */
	// 将malloc申请的内存释放掉
	jpeg_destroy_decompress(&cinfo);

	/*Step 8: close file*/
	fclose(infile);

	pPic->width = cinfo.output_width;
	pPic->height = cinfo.output_height;
	pPic->bpp = cinfo.output_components * 8;

	return 0;
}

// 封装的一个对外使用的JPG显示函数
// 本函数对外只需要一个JPG图片的pathname即可，那些复杂的数据结构都是JPG显示模块内部处理的
// 正确显示图片返回0，显示过程中出错则返回-1
int display_jpg(const char *pathname)
{
	int ret = -1;
	struct pic_info picture;				// 要显示的图片
	
	// 第一步: 检查给的图片是不是JPG图片
	ret = is_jpg(pathname);
	if (ret != 0)
	{
		return -1;
	}

	// 第二步: 显示该JPG图片
	picture.pathname = pathname;
	picture.pData = rgb_buf;
	jpg_analyze(&picture);

	fb_draw2(&picture);

	return 0;
}
