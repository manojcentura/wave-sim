#pragma once

#define ARR_INDEX(x,y,width) ( (x) + (y) * (width))

//calculate sums of second derivatives on each axis
//no results for border pixels
void
SpacialAccelSums(double* pData, int* pbSquareStep, int iWidth, int iHeight, double* pOutput)
{
	*pbSquareStep=!*pbSquareStep;
	for(int y=1; y<iWidth-1; y++)
		for(int x=1; x<iHeight-1; x++)
		{
			double fSum=0;
			/*for(int iRow=-1; iRow<=1; iRow++)
			{
				double fFactor=iRow ? 0.3 : 0.4;
				double delta1=pData[ARR_INDEX(x, y+iRow, iWidth)] - pData[ARR_INDEX(x-1, y+iRow, iWidth)];
				double delta2=pData[ARR_INDEX(x+1, y+iRow, iWidth)] - pData[ARR_INDEX(x, y+iRow, iWidth)];
				fSum+=(delta2-delta1)*fFactor;
			}
			for(int iCol=-1; iCol<=1; iCol++)

			{
				double fFactor=iCol ? 0.3 : 0.4;
				double delta1=pData[ARR_INDEX(x+iCol, y, iWidth)] - pData[ARR_INDEX(x+iCol, y-1, iWidth)];
				double delta2=pData[ARR_INDEX(x+iCol, y+1, iWidth)] - pData[ARR_INDEX(x+iCol, y, iWidth)];
				fSum+=(delta2-delta1)*fFactor;
			}
			pOutput[ARR_INDEX(x, y, iWidth)]=fSum*0.5;
			*/
/*
			double delta1=pData[ARR_INDEX(x, y, iWidth)] - pData[ARR_INDEX(x-1, y, iWidth)];
			double delta2=pData[ARR_INDEX(x+1, y, iWidth)] - pData[ARR_INDEX(x, y, iWidth)];
			fSum+=delta2-delta1;

			delta1=pData[ARR_INDEX(x, y, iWidth)] - pData[ARR_INDEX(x, y-1, iWidth)];
			delta2=pData[ARR_INDEX(x, y+1, iWidth)] - pData[ARR_INDEX(x, y, iWidth)];
			fSum+=delta2-delta1;

			delta1=pData[ARR_INDEX(x, y, iWidth)] - pData[ARR_INDEX(x-1, y-1, iWidth)];
			delta2=pData[ARR_INDEX(x+1, y+1, iWidth)] - pData[ARR_INDEX(x, y, iWidth)];
			fSum+=(delta2-delta1);// *1.414;

			delta1=pData[ARR_INDEX(x, y, iWidth)] - pData[ARR_INDEX(x-1, y+1, iWidth)];
			delta2=pData[ARR_INDEX(x+1, y-1, iWidth)] - pData[ARR_INDEX(x, y, iWidth)];
			fSum+=(delta2-delta1);// *1.414;

			pOutput[ARR_INDEX(x, y, iWidth)]=fSum*0.5;
			*/

			int nSamples=0;
			for(int iRow=-1; iRow<=1; iRow++)
				for(int iCol=-1; iCol<=1; iCol++)
					{
						if(! (iCol || iRow))
							continue;
						if(((iCol && iRow) && *pbSquareStep) ||  ((!(iCol && iRow)) && (!*pbSquareStep)))
							continue;

						if(!(x+iCol))
							continue;
						if(!(y+iRow))
							continue;

						fSum+=pData[ARR_INDEX(x+iCol, y+iRow, iWidth)];
						nSamples++;
						//printf("step type: %d. col=%d, row=%d, sample %d\n", *pbSquareStep, iCol, iRow, nSamples);
					}
			pOutput[ARR_INDEX(x, y, iWidth)]=(fSum-((double)nSamples)*pData[ARR_INDEX(x, y, iWidth)])*4.0/((double)nSamples);
		}
}

void
AddArrays(double* pData1, double* pData2, int iWidth, int iHeight, double* pOutput)
{
	for(int y=0; y<iWidth; y++)
		for(int x=0; x<iHeight; x++)
			pOutput[ARR_INDEX(x, y, iWidth)]=pData1[ARR_INDEX(x, y, iWidth)] + pData2[ARR_INDEX(x, y, iWidth)];
}

void
ScalarMul(double* pData, int iWidth, int iHeight, double fFactor, double* pOutput)
{
	for(int y=0; y<iWidth; y++)
		for(int x=0; x<iHeight; x++)
			pOutput[ARR_INDEX(x, y, iWidth)]=pData[ARR_INDEX(x, y, iWidth)] * fFactor;
}
