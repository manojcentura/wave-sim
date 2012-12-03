#pragma once

#include "WaveHelpers.h"

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
	double* pfA;
	double* pfTempX;
	double* pfTempV;
	double* pfTemp1;
	double* pfTemp2;
	double* pfTemp3;
	int width, height;
	double fAtt;
	double fC2;
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

	double** toInit[]={&(state.pfX),
			&(state.pfV),
			&(state.pfA),
			&(state.pfTempX),
			&(state.pfTempV),
			&(state.pfTemp1),
			&(state.pfTemp2),
			&(state.pfTemp3)};

	for(int iToInit=0; iToInit<sizeof(toInit)/sizeof(double**); iToInit++)
	{
		*toInit[iToInit]=malloc(iSize*sizeof(double));
		memset(*toInit[iToInit], 0, iSize*sizeof(double));
	}

	state.width=width;
	state.height=height;

	state.fAtt=fAtt;
	state.fC2=(fC*fC);

	//set rim to open
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

void
sim_frame(SimState* pState, double deltaT)
{
	//v'=v+a*t
	ScalarMul(pState->pfA, pState->width, pState->height, deltaT, pState->pfTemp1);
	AddArrays(pState->pfTemp1, pState->pfV, pState->width, pState->height, pState->pfTempV);
	//x'=x+v*t
	ScalarMul(pState->pfTempV, pState->width, pState->height, deltaT, pState->pfTemp1);
	AddArrays(pState->pfTemp1, pState->pfX, pState->width, pState->height, pState->pfTemp2);
	//apply attenuation
	ScalarMul(pState->pfTemp2, pState->width, pState->height, -(1.0-pState->fAtt)*deltaT, pState->pfTemp3);
	AddArrays(pState->pfTemp2, pState->pfTemp3, pState->width, pState->height, pState->pfTempX);

	//calc new derivatives
	SpacialAccelSums(pState->pfTempX, &(pState->bSquareStep), pState->width, pState->height, pState->pfTemp1);
	//get our accelerations
	ScalarMul(pState->pfTemp1, pState->width, pState->height, pState->fC2, pState->pfA);

	/*for(int x=1; x<pState->width-1; x++)
		for(int y=1; y<pState->height-1; y++)
		{
			pState->pfA=pState->fC2*;
		}*/
	//switch buffers
	double* pftmpX=pState->pfTempX;
	double* pftmpV=pState->pfTempV;
	pState->pfTempX=pState->pfX;
	pState->pfTempV=pState->pfV;
	pState->pfX=pftmpX;
	pState->pfV=pftmpV;
}
