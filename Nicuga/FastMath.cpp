/*
* Copyright (c) 2012, Nicolas My
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Nicolas My ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Nicolas My BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FastMath.h"
#include <math.h>
#include <stdio.h>
float * FastMath::coslookup = new float[360];
float * FastMath::sinlookup = new float[360];
double FastMath::M_PI_4 = 3.141592653589793238462643383/4.0;
double FastMath::M_180_PI = 180.0/3.141592653589793238462643383;
void FastMath::init()
{
	for (int i = 0;i < 360;i++)
	{
		coslookup[i] = (float)cos(((float)i) * 2. *3.1416 /360.);
		sinlookup[i] = (float)sin(((float)i) * 2. *3.1416 /360.);
	}
}

float FastMath::fastCos(int deg)
{
	if (deg > 0)
	{
		return coslookup[deg%360];
	}
	else
	{
		return coslookup[(-deg)%360];
	}
}

float FastMath::fastSin(int deg)
{
	if (deg > 0)
	{
		return sinlookup[deg%360];
	}
	else
	{
		return -sinlookup[(-deg)%360];
	}
}

float FastMath::fastArcCos(float x) {
   return (float)((-0.69813170079773212 * (double)x * (double)x - 0.87266462599716477) * (double)x + 1.5707963267948966);
}

float FastMath::fastArcTan(float x)
{
	return atan(x);
    //return M_PI_4*x - x*(fabs(x) - 1)*(0.2447 + 0.0663*fabs(x));
}