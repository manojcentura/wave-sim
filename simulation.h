#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

typedef struct
{
	int* piElems;
	double* pfX;
	double* pfV;
	double* pfTempX;
	double* pfTempV;
	int width, height;
	double fAtt;
	double fC;
	int bSquareStep;
} SimState;

enum EElemType
{
	TYPE_MEDIUM=0,
	TYPE_WALL,
	TYPE_OPEN
};

SimState
init_sim(int width, int height, double fAtt, double fC)
{
	SimState state;
	int iSize=width*height;
	state.piElems=malloc(iSize*sizeof(int));
	memset(state.piElems, 0, iSize*sizeof(int));

	state.pfX=malloc(iSize*sizeof(double));
	memset(state.pfX, 0, iSize*sizeof(double));

	state.pfV=malloc(iSize*sizeof(double));
	memset(state.pfV, 0, iSize*sizeof(double));

	state.pfTempX=malloc(iSize*sizeof(double));
	memset(state.pfTempX, 0, iSize*sizeof(double));

	state.pfTempV=malloc(iSize*sizeof(double));
	memset(state.pfTempV, 0, iSize*sizeof(double));

	state.width=width;
	state.height=height;

	state.fAtt=fAtt;
	state.fC=fC;

	//set rim to wall
	for(int x=0; x<width; x++)
	{
		state.piElems[x]=TYPE_OPEN;
		state.piElems[x+(height-1)*state.width]=TYPE_OPEN;

		state.piElems[x*state.width]=TYPE_OPEN;
		state.piElems[x*state.width+state.width-1]=TYPE_OPEN;
	}
	state.bSquareStep=0;

	return state;
}

double* indexf(double* p, int x, int y, SimState* pState)
{
	x=min(max(x,0), pState->width);
	y=min(max(y,0), pState->height);

	return &p[y*pState->width+x];
}

int* indexi(int* p, int x, int y, SimState* pState)
{
	x=min(max(x,0), pState->width);
	y=min(max(y,0), pState->height);

	return &p[y*pState->width+x];
}

void
sim_frame(SimState* pState, double deltaT)
{
	pState->bSquareStep=!pState->bSquareStep;
	for(int x=1; x<pState->width-1; x++)
		for(int y=1; y<pState->height-1; y++)
		{
			double fIntegralX=0;
			int iIntegralSize=0;
			double oldX;
			//check what type of cell this is
			switch(*indexi(pState->piElems, x,y, pState))
			{
			case TYPE_OPEN:
			case TYPE_WALL:
				break;
			default://medium
				//check neighbors
				oldX=*indexf(pState->pfX, x, y, pState);

				for(int iNeigh=0; iNeigh<4; iNeigh++)
				{
					int xNeigh=0, yNeigh=0;
					if(pState->bSquareStep)
					{
						switch(iNeigh)
						{
						case 0:
							xNeigh=-1;
							break;
						case 1:
							xNeigh=1;
							break;
						case 2:
							yNeigh=-1;
							break;
						case 3:
							yNeigh=1;
							break;
						}
					}
					else
					{
						xNeigh=-1+2*(iNeigh&1);
						yNeigh=-1+2*((iNeigh&2)>>1);
					}

					switch(*indexi(pState->piElems, x+xNeigh, y+yNeigh, pState))
					{
					case TYPE_OPEN:
						break;
					case TYPE_WALL:
						iIntegralSize++;
						break;
					case TYPE_MEDIUM:
						fIntegralX+=*indexf(pState->pfX, x+xNeigh, y+yNeigh, pState)-oldX;
						iIntegralSize++;
						break;
					}
				}

				double fA=fIntegralX/(double)iIntegralSize*pState->fC;
				//calc new V
				double oldV=*indexf(pState->pfV, x, y, pState);
				*indexf(pState->pfTempV, x, y, pState)=oldV + fA*deltaT*pState->fAtt;
				//calc new X

				*indexf(pState->pfTempX, x, y, pState)=oldX + oldV*deltaT;// + .5*fA*deltaT*deltaT;

				break;
			}
		}
	//switch buffers
	double* pftmpX=pState->pfTempX;
	double* pftmpV=pState->pfTempV;
	pState->pfTempX=pState->pfX;
	pState->pfTempV=pState->pfV;
	pState->pfX=pftmpX;
	pState->pfV=pftmpV;
}
