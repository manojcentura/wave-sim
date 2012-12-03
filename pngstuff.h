#pragma once

#include <png.h>
#include <memory.h>

void read_png(char *file_name, int* piWidth, int* piHeight, float** appfBuffers[3])
{
	png_structp png_ptr;
	png_infop info_ptr;
	//unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type;//, interlace_type;
	FILE *fp;

	if ((fp = fopen(file_name, "rb")) == NULL)
		return;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return ;
	}

	png_init_io(png_ptr, fp);

	png_read_png(png_ptr, info_ptr, 0, NULL);

	bit_depth=png_get_bit_depth(png_ptr, info_ptr);
	int channels = png_get_channels(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);

	if(bit_depth!=8 || channels!=3 || color_type!=PNG_COLOR_TYPE_RGB)
		return;

	width=png_get_image_width(png_ptr, info_ptr);
	height= png_get_image_height(png_ptr, info_ptr);

	png_bytepp row_pointers = png_malloc(png_ptr, height*sizeof(png_bytep));
	for (int i=0; i<height; i++)
		row_pointers[i]=png_malloc(png_ptr, width*3*sizeof(png_byte) );

	png_set_rows(png_ptr, info_ptr, row_pointers);

	for(int iBuffer=0; iBuffer<3; iBuffer++)
	{
		appfBuffers[iBuffer]=malloc(height*sizeof(float*));
		for(int iRow=0; iRow<height; iRow++)
		{
			appfBuffers[iBuffer][iRow]=malloc(width*sizeof(float));
			for(int iCol=0; iCol<width; iCol++)
			{
				appfBuffers[iBuffer][iRow][iCol]=(float)row_pointers[iRow][iCol*3+iBuffer]/(float)(1<<8);
			}
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
}
