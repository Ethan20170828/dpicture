/***************************************************************************
 *	本文件用来管理图片
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>


#include <config.h>
#include <fb.h>
#include "image_manager.h"


// 定义存储图片信息的数组；
image_info_t images[MAX_IMAGE_CNT];
unsigned int image_index = 0;		// 数组images中的计数index


// images数组本来是空的，然后程序初始化时会去一个事先约定好的目录(image目录)
// 下去递归检索所有的文件及子文件夹，并且将所有的图片格式收集并且填充记录到
// images数组中，经过检索后，images数组中就记录了所有的图片，然后显示图片逻辑
// 部分再去这个图片库中拿出来相应的图片来显示即可。
// path是要去检索的文件夹的pathname
int scan_image(const char *path)
{
	// 在本函数中递归检索path文件夹，将其中所有图片填充到images数组中去
	DIR *dir;
	struct dirent *ptr;
	char base[1000];		// 用来存放文件路径名的
	struct stat sta;

	if ((dir=opendir(path)) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	// readdir函数每调用一次就会返回opendir打开的basepath目录下的一个文件，直到basepath目录
	// 下所有文件都被读完之后，就会返回NULL。
	while ((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    //current dir OR parrent dir
			continue;

		// 用lstat来读取文件属性并判断文件类型
		memset(base, '\0', sizeof(base));
		strcpy(base, path);
		strcat(base, "/");
		strcat(base, ptr->d_name);
		lstat(base, &sta);

		if (S_ISREG(sta.st_mode))
		{
			printf("d_name: %s/%s\n", path, ptr->d_name);
			// 如果是普通文件，就要在这里进行处理，
			// 处理思路就是: 先判定是否属于已知的某种图片格式，如果是则放在images数组中，
			// 如果都不属于则不理他。
			if (!is_bmp(base))
			{
				strcpy(images[image_index].pathname, base);
				images[image_index].type = IMAGE_TYPE_BMP;
			}
			if (!is_jpg(base))
			{
				strcpy(images[image_index].pathname, base);
				images[image_index].type = IMAGE_TYPE_JPG;
			}
			if (!is_png(base))
			{
				strcpy(images[image_index].pathname, base);
				images[image_index].type = IMAGE_TYPE_PNG;
			}
			image_index++;
		}
		if (S_ISDIR(sta.st_mode))
		{
			printf("directory.\n");
			printf("d_name: %s/%s\n", path, ptr->d_name);
			scan_image(base);
		}
		#if 0
		switch(ptr->d_type)
		{
			case DT_REG:			// 普通文件
				printf("d_name:%s/%s\n", path, ptr->d_name);
				break;
			case DT_DIR:			// 文件夹
				memset(base,'\0',sizeof(base));
				strcpy(base,path);
				strcat(base,"/");
				strcat(base,ptr->d_name);
				scan_image(base);
				break;
			case DT_UNKNOWN:		// 不识别的文件格式
				printf("unhnown file type.\n");
				break;
			default:
				break;
		}
		#endif
	}
	
	closedir(dir);
	return 0;
}


void print_images(void)
{
	int i;

	printf("image_index = %d.\n", image_index);
	for (i=0; i<image_index; i++)
	{
		printf("images[i].pathname = %s, type = %d.\n", images[i].pathname, images[i].type);
	}
}

void show_images(void)
{
	int i;

	for (i=0; i<image_index; i++)
	{
		switch (images[i].type)
		{
			case IMAGE_TYPE_BMP:
				display_bmp(images[i].pathname);	break;
			case IMAGE_TYPE_JPG:
				display_jpg(images[i].pathname);	break;
			case IMAGE_TYPE_PNG:
				display_png(images[i].pathname);	break;
			default:
				break;
		}
		sleep(2);
	}
}


static void show_image(int index)
{
	switch (images[index].type)
	{
		case IMAGE_TYPE_BMP:
			display_bmp(images[index].pathname);	break;
		case IMAGE_TYPE_JPG:
			display_jpg(images[index].pathname);	break;
		case IMAGE_TYPE_PNG:
			display_png(images[index].pathname);	break;
		default:
			break;
	}
}

// 本函数实现通过触摸屏来对图片翻页显示
int ts_updown(void)
{
	// 第一步: 触摸屏的触摸操作检测
	int fd = -1, ret = -1;
	struct input_event ev;
	int i = 0;		// 用来记录当前显示的是第几个图片

	fd = open(DEVICE_TOUCHSCREEN, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	while (1)
	{
		memset(&ev, 0, sizeof(struct input_event));
		ret = read(fd, &ev, sizeof(struct input_event));
		if (ret != sizeof(struct input_event))
		{
			perror("read");
			close(fd);
			return -1;
		}
		// 第二步: 根据触摸坐标来翻页
		if ((ev.type == EV_ABS) && (ev.code == ABS_X))
		{
			// 确定这个是x坐标
			if ((ev.value >= 0) && (ev.value < TOUCH_WIDTH))
			{
				// 上翻页
				if (i-- < 0)
				{
					i = image_index - 1;
				}
			}
			else if ((ev.value > (WIDTH - TOUCH_WIDTH)) && (ev.value <= WIDTH))
			{
				// 上翻页
				if (i++ >= image_index)
				{
					i = 1;
				}
			}
			else
			{
				// 不翻页
			}
			show_image(i - 1);
		}

		#if 0
		printf("-------------------------\n");
		printf("type: %hd\n", ev.type);
		printf("code: %hd\n", ev.code);
		printf("value: %d\n", ev.value);
		printf("\n");
		#endif
	}

	close(fd);

	return 0;
}







