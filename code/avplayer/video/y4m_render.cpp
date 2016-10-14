#include "internal.h"
#include <stdio.h>
#include "y4m_render.h"

static
FILE* create_y4m(const char *filename, int width, int height, int fps)
{
	FILE *yuv_out = NULL;

	//z 打开文件
	yuv_out = fopen(filename, "wb");
	if (!yuv_out)
		return NULL;

	//z 写入 yuv 头
	fprintf(yuv_out, "YUV4MPEG2 W%d H%d F%d:%d I%c A%d:%d\n",
		width, height, fps, 1, 'p', 0, 0);	// A后面为0:0表示未知.

	fflush(yuv_out);

	return yuv_out;
}

//z 每帧之间采用frame隔开
static
int y4m_write(FILE *yuv_out, const void *ptr, const size_t num_bytes)
{
	fprintf(yuv_out, "FRAME\n");
	return fwrite(ptr, 1, num_bytes, yuv_out);
}

//z 关闭文件
static
int close_y4m(FILE *yuv_out)
{
	return fclose(yuv_out);
}


y4m_render::y4m_render(void)
{
}

y4m_render::~y4m_render(void)
{
}

bool y4m_render::init_render(void* ctx, int w, int h, int pix_fmt)
{
	return false;
}

bool y4m_render::init_render(void* ctx, int w, int h, int pix_fmt, float fps)
{
	char filename[1024] = { 0 };
	sprintf(filename, "%x.y4m", (size_t)ctx);	// 随便构建的文件名.
	m_yuv_out = create_y4m(filename, w, h, fps);
	//z 每个 frame 的大小
	m_image = (char*)malloc(w * h * 3 / 2);
	//z frame 的宽和高
	m_image_height = h;
	m_image_width = w;
	//z 打开文件失败或是内存分配不成功，返回。
	if (!m_yuv_out || !m_image)
		return false;
	return true;
}

bool y4m_render::render_one_frame(AVFrame* data, int pix_fmt)
{
	uint8_t *image_y = NULL;
	uint8_t *image_u = NULL;
	uint8_t *image_v = NULL;

	//z 这三份数据分别指向的是 y u v 么？
	uint8_t* src_yuv[3] = { data->data[0], data->data[1], data->data[2] };
	int i;
	int write_bytes = m_image_width * m_image_height * 3 / 2;

	//z y : w*h, u : w*h/4, v : w*h/4
	image_y = (uint8_t*)m_image;
	image_u = image_y + m_image_width * m_image_height;
	image_v = image_u + m_image_width * m_image_height / 4;

	for (i = 0; i < m_image_height; i++)
	{
		//z 从 y 中拷贝一行
		//z 如果 m_image_width == data->data->linesize[0]，那么也不用逐行拷贝的
		memcpy(image_y, src_yuv[0], m_image_width);
		src_yuv[0] += data->linesize[0];
		image_y += m_image_width;

		//z 只有0，2，4 等偶数行号时起作用。
		//z 可以不用放在循环里，拷贝一次就好了。
		if (i % 2 == 0)
		{
			//z 长度只有1/2
			memcpy(image_u, src_yuv[1], m_image_width / 2);
			src_yuv[1] += (m_image_width / 2);
			image_u += (m_image_width / 2);

			memcpy(image_v, src_yuv[2], m_image_width / 2);
			src_yuv[2] += (m_image_width / 2);
			image_v += (m_image_width / 2);
		}
	}
	
	//z 写入一帧到文件
	y4m_write(m_yuv_out, m_image, write_bytes);
	fflush(m_yuv_out);

	return true;
}

void y4m_render::re_size(int width, int height)
{

}

void y4m_render::aspect_ratio(int srcw, int srch, bool enable_aspect)
{

}

void y4m_render::destory_render()
{
	//z 关闭文件
	fclose(m_yuv_out);
	m_yuv_out = NULL;

	//z 释放内存
	free(m_image);
	m_image = NULL;
}
