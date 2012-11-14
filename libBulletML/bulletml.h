/*
* Copyright (c) 2008, Nicolas My
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


#ifndef BULLETML_H
#define BULLETML_H

#include "exprsolve.h"
#include <expat.h>

#define CONTINUE 0
#define PAUSE 1
#define END 2

typedef enum {
  accel,
  action,
  actionRef,
  bullet,
  bulletml,
  bulletRef,
  changeDirection,
  changeSpeed,
  direction,
  fire,
  fireRef,
  horizontal,
  param,
  repeat,
  speed,
  term,
  times,
  wait,
  vanish,
  vertical
} tMark;


typedef struct {
  tMark mType;
  char sAttribute[20];
  char sValue[256];
  int iValue;
  int iNext;
  int iDown; 
  int iUp;
} tNode;

typedef struct {
  int iRef;
  char sLabel[20];
} tLabelRef;

typedef struct {
  XML_Parser p; 
  int iNodePosition;
  int stack[255];
  int iStackPos;
  tNode listNode[200];
  tLabelRef vLabelLookupTable[200];
  int iLabelTableCount;
  int iTopRef;
} tParsingData;

typedef enum{
  absolute,
  aim,
  relative,
  sequence
} tDirectionType;

#define ABSOLUTE 1
#define AIM 2
#define RELATIVE 3
#define SEQUENCE 4

typedef struct 
{
   tParsingData * pstData;
   int iCurrentItem; 
   int value[200];       
   int iStatus;
   tSymbolTable stST;
   float fDirection;
   float fSpeed;
   float fStepSpeed;
   float fStepDirection;
   int iDirectionType;
   int iSpeedType;
   float fChangeDirectionStep;
   float fChangeDirectionTarget;
   float fChangeSpeedStep;
   float fChangeSpeedTarget;   

   /* The script encounters a bullet creation command
   *  Parameters : speed, direction and index where to start 
   *  interpretation, -1 if no further interpretation is needed
   */
   void(*create_bullet_callback)(void * pUserData, float fSpeed, float fDirection,int iStartValue);
   void(*set_speed_callback)(void * pUserData, float fSpeed);
   void(*set_direction_callback)(void * pUserData, float fDirection);
   void(*get_aim_direction_callback)(void *pUserData, float *pfDirection);
   void(*get_direction_callback)(void *pUserData, float *pfDirection);
   void(*get_speed_callback)(void *pUserData, float *pfSpeed);
   void * pUserData;
 
} tExecutionContext;


int run_bulletml( tExecutionContext * pCtxt);
int load_bulletml(char * sFileName, tParsingData * pstData);
void init_bulletmlcontext(tExecutionContext * pstCtxt, tParsingData * pstData);
#endif
