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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exprsolve.h"

int resolve_symbol(tSymbolTable * pST,const char * sSymbolName, float * result)
{
   int i;
   if (!strcmp(sSymbolName,"$rank"))
   {
     *result = 0.5;
     return 0;
   } else
   if (!strcmp(sSymbolName,"$rand"))
   {
     *result = (float)((rand()%1000) / 1000.);
     return 0;
   } else 
   {
     for (i = 0;i < pST->iSymbolCount;i++)
     {
         if (!strcmp(sSymbolName, pST->vSymbol[i]))
         {
           *result = pST->vValue[i];
           return 0;
         }
     }   
   }
   return 1;
} 



int expr_solve_prio(tExpr * pExpr,int iPrio)
{   
    //printf("expr_solve_prio %d o%d v%d\n",iPrio,pExpr->iOpCount,pExpr->iValueCount);
    int i,j;
    for (i = 0;i < pExpr->iOpCount;i++)
    {
      if (pExpr->vPrio[i] == iPrio)
      {
        switch(pExpr->vOp[i])
        {
          case '+' :
               //printf("%f + %f \n", pExpr->vValue[i],pExpr->vValue[i+1]);
               pExpr->vValue[i] += pExpr->vValue[i+1];
               for (j = i + 1;j < (pExpr->iValueCount - 1);j++)
                 pExpr->vValue[j] = pExpr->vValue[j+1];
               pExpr->iValueCount--;
               for (j = i;j < (pExpr->iOpCount);j++){
                 pExpr->vOp[j] = pExpr->vOp[j+1];
                 pExpr->vPrio[j] = pExpr->vPrio[j+1];
                 }
               pExpr->iOpCount--;                
               break;
          case '-' :
               pExpr->vValue[i] -= pExpr->vValue[i+1];
               for (j = i + 1;j < (pExpr->iValueCount - 1);j++)
                 pExpr->vValue[j] = pExpr->vValue[j+1];
               pExpr->iValueCount--;
               for (j = i;j < (pExpr->iOpCount);j++)
			   {
                 pExpr->vOp[j] = pExpr->vOp[j+1];
                 pExpr->vPrio[j] = pExpr->vPrio[j+1];
               }
               pExpr->iOpCount--; 
               break;
          case '*' :
               //printf("%f * %f \n", pExpr->vValue[i],pExpr->vValue[i+1]);
               pExpr->vValue[i] *= pExpr->vValue[i+1];
               for (j = i + 1;j < (pExpr->iValueCount - 1);j++)
                 pExpr->vValue[j] = pExpr->vValue[j+1];
               pExpr->iValueCount--;
               for (j = i;j < (pExpr->iOpCount);j++)
               {
                 pExpr->vOp[j] = pExpr->vOp[j+1];
                 pExpr->vPrio[j] = pExpr->vPrio[j+1];
               }
               pExpr->iOpCount--; 
               break;
          case '/' :
               pExpr->vValue[i] /= pExpr->vValue[i+1];
               for (j = i + 1;j < (pExpr->iValueCount - 1);j++)
                 pExpr->vValue[j] = pExpr->vValue[j+1];
               pExpr->iValueCount--;
               for (j = i;j < (pExpr->iOpCount);j++)
               {
                 pExpr->vOp[j] = pExpr->vOp[j+1];
                 pExpr->vPrio[j] = pExpr->vPrio[j+1];
               }
               pExpr->iOpCount--; 
               break;
        }
        return 1;            
      }    
    }
    return 0;
}
int expr_solve(tExpr * pExpr, float * result)
{
  int iHighestPrio = 0;
  int i;
  while (pExpr->iOpCount > 0)
  {
    iHighestPrio = 0;
    for (i = 0;i < pExpr->iOpCount;i++)
    {
      if ( pExpr->vPrio[i] > iHighestPrio)
         iHighestPrio =  pExpr->vPrio[i]; 
    }   
    expr_solve_prio(pExpr,iHighestPrio);
  }
  *result = pExpr->vValue[0];  
  return 1;
}

int load_expression(tExpr * pstExpr, char * sExpr, tSymbolTable * pST)
{
  int iLen;
  int i;
  int j;
  int iTmpSign;
  char sTmpValue[10];
  iTmpSign = 1;
  pstExpr->iValueCount = 0;
  pstExpr->iOpCount = 0;
  pstExpr->iCurrentPrio = 0;
  iLen = strlen(sExpr);
  
  for (i = 0;i <iLen;i++)
  {
      if (sExpr[i] == '(')
        pstExpr->iCurrentPrio += 3;
      else
        if (sExpr[i] == ')')
          pstExpr->iCurrentPrio -= 3;
      else
      if(( sExpr[i] == '+')||( sExpr[i] == '-'))
      {
        if (pstExpr->iOpCount >= pstExpr->iValueCount)
        {
          //printf("Found sign: %c\n",sExpr[i]);
           if ( sExpr[i] == '+')
             iTmpSign = 1;
           else iTmpSign = -1;
           //i++;
        } else 
        {
          //printf("Found operand: %c\n",sExpr[i]);
          pstExpr->vOp[pstExpr->iOpCount] = sExpr[i];
          pstExpr->vPrio[pstExpr->iOpCount] = pstExpr->iCurrentPrio;
          //printf("%c prio:%d\n",sExpr[i],pstExpr->vPrio[pstExpr->iOpCount]);
          pstExpr->iOpCount++;
          //i++;
        }
      } else 
      {
        if(( sExpr[i] == '*')||( sExpr[i] == '/'))    
        {
          pstExpr->vOp[pstExpr->iOpCount] = sExpr[i];
          pstExpr->vPrio[pstExpr->iOpCount] = pstExpr->iCurrentPrio + 1;
          //printf("%c prio:%d\n",sExpr[i],pstExpr->vPrio[pstExpr->iOpCount]);
          pstExpr->iOpCount++;
          //i++;            
        } else 
        {
          for (j = i;(sExpr[j] != '+')&&(sExpr[j] != '-')
          &&(sExpr[j] != '*')&&(sExpr[j] != '/')&&(sExpr[j] != 0)
          &&(sExpr[j] != '(')&&(sExpr[j] != ')');j++)
          {
            sTmpValue[j-i] = sExpr[j];

          }
          sTmpValue[j-i] = 0;
          
          if (sTmpValue[0]== '$')
          {
            if (resolve_symbol(pST,sTmpValue, &pstExpr->vValue[pstExpr->iValueCount])!= 0)
              return -1;
            pstExpr->vValue[pstExpr->iValueCount] *= iTmpSign;
            iTmpSign = 1;
            pstExpr->iValueCount++;
            i = j-1;
          } else {                   
            //printf("Value: %s\n",sTmpValue);
            sscanf_s(sTmpValue,"%f",&pstExpr->vValue[pstExpr->iValueCount]);
            pstExpr->vValue[pstExpr->iValueCount] *= iTmpSign; 
            iTmpSign = 1;
            pstExpr->iValueCount++;
            i = j-1;
          }
        }
      }
  }    
   return 0;   
}
