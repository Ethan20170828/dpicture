#include <stdio.h>
#include <unistd.h>


#include <fb.h>
#include <config.h>


char rgb_buf[RBG_BUF_SIZE];



int main(void)
{
	int ret = -1;
	
	printf("image decode player...\n");
	debug("test for debug %d.\n", 13);

	ret = fb_open();
	if (ret < 0)
	{
		printf("fb_open error.\n");
		return -1;
	}

	//fb_draw_back(WIDTH, HEIGHT, RED);
	//fb_draw_picture();
	//fb_draw_picture2();
	//fb_draw_picture3();
	//fb_draw_picture4();
	//fb_draw_picture5();
	//fb_draw_picture6((1025-533)/2,(600-300)/2);
	//fb_draw_picture7((1025-533)/2,(600-300)/2);
	//fb_draw_picture7(900,200);
	//fb_draw_picture8(900,100);
	//fb_draw_picture9(800,100);

	// 测试bmp图片显示，ok
	//picture.pathname = "qiche.bmp";		// 指向要显示的图片
	//bmp_analyze(&picture);

	// 测试JPEG图片显示
	//picture.pathname = "gougou.jpg";		// 指向要显示的图片
	#if 0
	while (1)
	{
		display_jpg("chi.jpg");
		sleep(1);
		display_jpg("heying.jpg");
		sleep(1);
		display_jpg("dianying.jpg");
		sleep(1);
		display_jpg("shipin.jpg");
		sleep(1);
		display_jpg("yuandan.jpg");
		sleep(1);
	}
	#endif

	#if 0
	display_png("AdvancedC.png");
	debug("----------------------.\n");
	#endif

	scan_image("./image");
	//print_images();

	#if 0
	while (1)
		show_images();
	#endif

	ts_updown();

	fb_close();
	return 0;
}

