#include <ppm.h>
#include <png.h>
#include <netpbm/pm.h>
#include <math.h>
#include "WaveSim.h"
#include "pngstuff.h"

#define COLORS ((1<<8)-1)

void
write_ppm(SimState* pState, char* szFileName)
{

	FILE* pFile=pm_openw (szFileName);
	pixel** pPixels=ppm_allocarray(pState->width, pState->height);

	for(int x=0; x<pState->width; x++)
		for(int y=0; y<pState->height; y++)
		{
			pPixels[x][y].r=min(max(0.5*(pState->pfX[ARR_INDEX(x, y, pState->width)]*COLORS+COLORS), 0), COLORS);
			pPixels[x][y].g=0;
			pPixels[x][y].b=min(max(0.5*(pState->pfV[ARR_INDEX(x, y, pState->width)]*COLORS+COLORS), 0), COLORS);
		}
	ppm_writeppm(pFile, pPixels, pState->width, pState->height, COLORS, 0);
	ppm_freearray(pPixels, pState->height);
	pm_close(pFile);
}

void
write_png(SimState* pState, char* szFileName)
{
	int code = 0;
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;

	// Open file for writing (binary mode)
	fp = fopen(szFileName, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", szFileName);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, pState->width, pState->height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * pState->width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0 ; y<pState->height ; y++) {
		for (x=0 ; x<pState->width ; x++) {
			/*row[x*3 + 0]=min(max(0.5*(pState->pfX[ARR_INDEX(x, y, pState->width)]*COLORS+COLORS), 0), COLORS);
			row[x*3 + 1]=0;
			row[x*3 + 2]=min(max(0.5*(pState->pfV[ARR_INDEX(x, y, pState->width)]*COLORS+COLORS), 0), COLORS);*/

			row[x*3 + 0]=min(max((pState->pfX[ARR_INDEX(x, y, pState->width)]*COLORS), 0), COLORS);
			row[x*3 + 1]=0;
			row[x*3 + 2]=min(max(-1.0*(pState->pfX[ARR_INDEX(x, y, pState->width)]*COLORS), 0), COLORS);

			/*row[x*3 + 0]=0;
			row[x*3 + 1]=0;
			row[x*3 + 2]=min(max(0.5*(pState->pfA[ARR_INDEX(x, y, pState->width)]*COLORS+COLORS), 0), COLORS);*/
		}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

	finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	if(code)
		printf("error.\n\n");
}



int main()
{
	int iWidth=64,
		iHeight=64;

	SimState state=init_sim(iWidth, iHeight, 0.99, 2.5);
	//create object
	for(int x=1; x<iWidth-1; x++)
		for(int y=1; y<iWidth-1; y++)
		{
		/*	if(x>=6*iWidth/10 && y>=6*iWidth/10)
				*indexi(state.piElems, x, y, &state)=TYPE_WALL;
			if(x>=6*iWidth/10 && y>=6*iWidth/10)
				*indexi(state.piElems, x, y, &state)=TYPE_WALL;
			if(x+y<5*iWidth/6)
				*indexi(state.piElems, x, y, &state)=TYPE_WALL;
		*/}

	static const double fStep=0.05;
	static const int nFrames=900;
	for(int iFrame=0; iFrame<nFrames; iFrame++)
	{
		/*if((20*iFrame)%nFrames==0)
			printf(".");*/
		//printf("frame %d\n", iFrame);
		double t=fStep*(double)iFrame;
		double fSin=sin(t);
		if(t<3.141)
			state.pfX[ARR_INDEX(iWidth/2, iHeight/2, state.width)]=fSin;
		//else
		//	fSin=1;
		sim_frame(&state, fStep);

		char buf[100];
		sprintf(buf, "%04d.png", iFrame+1);
		write_png(&state, buf);
	}
	printf("\nmaking flick...\n");

	execl("/bin/bash", "bash", "output.sh", NULL);
	return -1;
}
