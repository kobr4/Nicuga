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


#include "exprsolve.h"
#include "bulletml.h"
#include <string.h>
#include <stdio.h>


#define BUFFSIZE	8192

//#define DEBUGLOG

static char Buff[BUFFSIZE];


tMark GetMarkType(const char * sName)
{
  if (!strcmp("accel",sName))
     return accel;
  else if (!strcmp("action",sName))
     return action;
  else if (!strcmp("actionRef",sName))
     return actionRef;
  else if (!strcmp("bullet",sName))
     return bullet;  
  else if (!strcmp("bulletml",sName))
     return bulletml;  
  else if (!strcmp("bulletRef",sName))
     return bulletRef;  
  else if (!strcmp("changeDirection",sName))
     return changeDirection;  
  else if (!strcmp("changeSpeed",sName))
     return changeSpeed;
  else if (!strcmp("direction",sName))
     return direction;  
  else if (!strcmp("fire",sName))
     return fire;  
  else if (!strcmp("fireRef",sName))
     return fireRef;  
  else if (!strcmp("horizontal",sName))
     return horizontal;
  else if (!strcmp("param",sName))
     return param;  
   else if (!strcmp("repeat",sName))
     return repeat;
  else if (!strcmp("speed",sName))
    return speed;
  else if (!strcmp("term",sName))
    return term;
  else if (!strcmp("times",sName))
    return times;
  else if (!strcmp("wait",sName))
    return wait;  
  else if (!strcmp("vanish",sName))
    return vanish;
  else if (!strcmp("vertical",sName))
    return vertical;
  else //if (!strcmp("wait",sName))
    return wait;                    
}


void setup_item(tParsingData * pPData)
{
         
         pPData->listNode[pPData->iNodePosition].iNext = -1;
         pPData->listNode[pPData->iNodePosition].iDown = -1;
         pPData->listNode[pPData->iNodePosition].iUp = pPData->stack[pPData->iStackPos-1];
         int iUp = pPData->listNode[pPData->iNodePosition].iUp;
         if (pPData->listNode[iUp].iDown == -1)
           pPData->listNode[iUp].iDown = pPData->iNodePosition;
         for (int i = pPData->iNodePosition-1;i > 0;i--){
           if (pPData->listNode[i].iUp == pPData->stack[pPData->iStackPos-1])
             if (pPData->listNode[i].iNext == -1){
              pPData->listNode[i].iNext = pPData->iNodePosition;                     
             }
         }
#ifdef DEBUGLOG
         printf("UP : %d | DOWN : %d | NEXT : %d\n",pPData->listNode[pPData->iNodePosition].iUp
         ,pPData->listNode[pPData->iNodePosition].iDown,pPData->listNode[pPData->iNodePosition].iNext);
#endif
         //pPData->listNode[pPData->iNodePosition].iDown = pPData->iNodePosition + 1;
         pPData->stack[pPData->iStackPos] = pPData->iNodePosition;
         pPData->iStackPos++;
         pPData->iNodePosition++;
}

int add_lookupref(tParsingData * pPData, const char **attr,int iAttrCount, const char * sAttrName)
{
    int i;
#ifdef DEBUGLOG
    printf("add_lookupref: %s ",sAttrName); 
#endif
    for(i = 0;i < iAttrCount;i++)
    {
       if (!strcmp(attr[i<<1],sAttrName))
       {
         pPData->vLabelLookupTable[pPData->iLabelTableCount].iRef 
         = pPData->iNodePosition;
         strcpy_s(pPData->vLabelLookupTable[pPData->iLabelTableCount].sLabel, 20, attr[(i<<1) + 1]);    
         pPData->iLabelTableCount++;  
#ifdef DEBUGLOG 
         printf("added: %s\n",attr[(i<<1) + 1])  ;  
#endif               
         return 1;                                   
       }
    }
#ifdef DEBUGLOG
    printf("Not found!\n",sAttrName);
#endif
    return 0;
}

int get_lookupref(tParsingData * pPData, const char * sLabel)
{
  int ret;
  ret = -1;
  int i;
#ifdef DEBUGLOG
  printf("get_lookupref: %s ",sLabel); 
#endif
  for(i = 0;i < pPData->iLabelTableCount;i++)
  {
     if (!strcmp(pPData->vLabelLookupTable[i].sLabel,sLabel))
     {
#ifdef DEBUGLOG
       printf("-> %d\n",pPData->vLabelLookupTable[i].iRef);
#endif
       return pPData->vLabelLookupTable[i].iRef ;                                 
     }
  }  
#ifdef DEBUGLOG
  printf("-> not found\n");
#endif
  return ret;
}

void GetTimesContent(void *userData,const XML_Char *s,int len)
{
     tParsingData * pPData = (tParsingData *)userData;
     char sTimes[32];
     strncpy_s(sTimes,32,s,len);
     sTimes[len] = 0;
     pPData->listNode[pPData->iNodePosition-1].iValue = atoi(sTimes);
     if (atoi(sTimes) == 0){
        printf("Error");
     }
}
void GetDirectionContent(void *userData,const XML_Char *s,int len)
{
     tParsingData * pPData = (tParsingData *)userData;
     char sDirection[256];
     strncpy_s(sDirection,256,s,len);
     sDirection[len] = 0;
     strcpy_s(pPData->listNode[pPData->iNodePosition-1].sValue,256,sDirection);
}

void GetSpeedContent(void *userData,const XML_Char *s,int len)
{
     tParsingData * pPData = (tParsingData *)userData;
     char sSpeed[256];
     strncpy_s(sSpeed,256,s,len);
     sSpeed[len] = 0;
     strcpy_s(pPData->listNode[pPData->iNodePosition-1].sValue,256,sSpeed);
}

void GetNumberContent(void *userData,const XML_Char *s,int len)
{
     tParsingData * pPData = (tParsingData *)userData;
     char sNumber[256];
     strncpy_s(sNumber,256,s,len);
     sNumber[len] = 0;
     strcpy_s(pPData->listNode[pPData->iNodePosition-1].sValue,256,sNumber);
}



static void start(void *data, const char *el, const char **attr) {
  tParsingData * pPData = (tParsingData *)data;
//  tExpr stExpr;
#ifdef DEBUGLOG
  printf("%s %d\n",el,pPData->iNodePosition);
#endif
  int k;
  switch (GetMarkType(el))
  {
    case action :
         pPData->listNode[pPData->iNodePosition].mType = action;
         k = XML_GetSpecifiedAttributeCount(pPData->p);
		 /*
         for (int i = 0;i < k;i++){
             printf("attr[%d]= %s\n",i,attr[i]);
         }
		 */
         add_lookupref(pPData,attr,k,"label");
         setup_item(pPData);
         break;
    case bulletml :
         pPData->listNode[pPData->iNodePosition].mType = bulletml;
         pPData->listNode[pPData->iNodePosition].iNext = -1;
         pPData->listNode[pPData->iNodePosition].iUp = -1;
         pPData->listNode[pPData->iNodePosition].iDown = pPData->iNodePosition + 1;
         pPData->stack[pPData->iStackPos] = pPData->iNodePosition;
         pPData->iStackPos++;
         pPData->iNodePosition++;
         break;
    case bullet :
         pPData->listNode[pPData->iNodePosition].mType = bullet;
         k = XML_GetSpecifiedAttributeCount(pPData->p);
		 /*
         for (int i = 0;i < k;i++){
             printf("attr[%d]= %s\n",i,attr[i]);
         }
		 */
         add_lookupref(pPData,attr,k,"label");
         setup_item(pPData);
         break;
    case bulletRef :
         pPData->listNode[pPData->iNodePosition].mType = bulletRef;
         strcpy_s(pPData->listNode[pPData->iNodePosition].sValue,256,attr[1]);
         setup_item(pPData);
         break;   
    case changeSpeed :
         pPData->listNode[pPData->iNodePosition].mType = changeSpeed;
         setup_item(pPData);
         break;   
    case changeDirection :
         pPData->listNode[pPData->iNodePosition].mType = changeDirection;
         setup_item(pPData);
         break;    
    case repeat :
         pPData->listNode[pPData->iNodePosition].mType = repeat;
         setup_item(pPData);
         break;   
    case times :
         pPData->listNode[pPData->iNodePosition].mType = times;
         XML_SetCharacterDataHandler(pPData->p,GetNumberContent);
         pPData->listNode[pPData->iNodePosition].iValue = -1;
         setup_item(pPData);
         break;
    case term :
         pPData->listNode[pPData->iNodePosition].mType = term;
         XML_SetCharacterDataHandler(pPData->p,GetNumberContent);
         pPData->listNode[pPData->iNodePosition].iValue = -1;
         setup_item(pPData);
         break;
    case fire :
         pPData->listNode[pPData->iNodePosition].mType = fire;
         setup_item(pPData);
         break;   
    case direction :
         pPData->listNode[pPData->iNodePosition].mType = direction;
         XML_SetCharacterDataHandler(pPData->p,GetDirectionContent);
         k = XML_GetSpecifiedAttributeCount(pPData->p);
         if (k > 0){
           if (!strcmp("absolute",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = ABSOLUTE;
           if (!strcmp("aim",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = AIM;
           if (!strcmp("relative",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = RELATIVE;
           if (!strcmp("sequence",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = SEQUENCE;
         } else pPData->listNode[pPData->iNodePosition].iValue = 0;
         setup_item(pPData);
         break;    
    case speed :
         pPData->listNode[pPData->iNodePosition].mType = speed;
         XML_SetCharacterDataHandler(pPData->p,GetSpeedContent);
         k = XML_GetSpecifiedAttributeCount(pPData->p);
         if (k > 0){
           if (!strcmp("absolute",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = ABSOLUTE;
           if (!strcmp("aim",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = AIM;
           if (!strcmp("relative",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = RELATIVE;
           if (!strcmp("sequence",attr[1]))
              pPData->listNode[pPData->iNodePosition].iValue = SEQUENCE;
         } else pPData->listNode[pPData->iNodePosition].iValue = 0;
         setup_item(pPData);
         break; 
    case wait :
         pPData->listNode[pPData->iNodePosition].mType = wait;
         XML_SetCharacterDataHandler(pPData->p,GetNumberContent);
         pPData->listNode[pPData->iNodePosition].iValue = -1;
         setup_item(pPData);
         break; 
    default : printf("Unknown item!\n");exit(0);break;
  }
}  /* End of start handler */

static void end(void *data, const char *el) {
  tParsingData * pPData = (tParsingData *)data;
  switch (GetMarkType(el))
  {
     case term:
     case times:
     case speed:
     case direction:
     case wait:
          XML_SetCharacterDataHandler(pPData->p,NULL);
     //case bulletRef:
     //     printf("Out bullet Ref\n");
     default : 
         pPData->iStackPos--; 
         break;       
  }   
}  /* End of end handler */



int next_upitem(tExecutionContext * pCtxt)
{
    tNode * node = &pCtxt->pstData->listNode[pCtxt->iCurrentItem];
    if (node->iNext != -1){
#ifdef DEBUGLOG
      printf("Next\n");
#endif
      pCtxt->iCurrentItem = node->iNext;
      return 0;
      }
    if (node->iUp != -1){
#ifdef DEBUGLOG
      printf("Up\n");
#endif
      pCtxt->iCurrentItem = node->iUp;
      if (pCtxt->pstData->listNode[pCtxt->iCurrentItem].mType != repeat){
          return next_upitem(pCtxt);
        }
      return 0;
      }
#ifdef DEBUGLOG
  printf("End of execution\n");
#endif
  return END;
}


int next_item(tExecutionContext * pCtxt)
{
    tNode * node = &pCtxt->pstData->listNode[pCtxt->iCurrentItem];
    if (node->iDown != -1){
#ifdef DEBUGLOG
      printf("next_item Down\n");
#endif
      pCtxt->iCurrentItem = node->iDown;
      return 0;
      }
    if (node->iNext != -1){
#ifdef DEBUGLOG
      printf("next_item Next\n");
#endif
      pCtxt->iCurrentItem = node->iNext;
      return 0;
      }
    if (node->iUp != -1){
#ifdef DEBUGLOG
      printf("next_item Up\n");
#endif
      pCtxt->iCurrentItem = node->iUp;
      if (pCtxt->pstData->listNode[pCtxt->iCurrentItem].mType != repeat){
          return next_upitem(pCtxt);
        }
      return 0;
      }
#ifdef DEBUGLOG
  printf("End of execution\n");
#endif
  return END;
}

int run_item(tExecutionContext * pCtxt)
{
  int iTmp;
  tExpr stExpr;
  float fTmpFloat;
  tNode * node = &pCtxt->pstData->listNode[pCtxt->iCurrentItem];
  switch(node->mType)
  {
    case action :
#ifdef DEBUGLOG
         printf("%d - action\n",pCtxt->iCurrentItem);
#endif
         pCtxt->iStatus = next_item(pCtxt);
         break;
    case bullet :
#ifdef DEBUGLOG
         printf("%d - bullet\n",pCtxt->iCurrentItem);
#endif
      
         if(pCtxt->pstData->listNode[node->iUp].mType == fire)
           pCtxt->create_bullet_callback(pCtxt->pUserData,pCtxt->fSpeed,pCtxt->fDirection,-1);

         pCtxt->iStatus = next_upitem(pCtxt);
         break;
    case bulletRef :
#ifdef DEBUGLOG
         printf("%d - bulletRef %d\n",pCtxt->iCurrentItem,pCtxt->create_bullet_callback);
#endif

         pCtxt->create_bullet_callback(pCtxt->pUserData,pCtxt->fSpeed,pCtxt->fDirection,
           pCtxt->pstData->listNode[get_lookupref(pCtxt->pstData,node->sValue)].iDown);
         
//pCtxt->iCurrentItem = get_lookupref(pCtxt->pstData,node->sValue);
         pCtxt->iStatus = next_item(pCtxt);
         //pCtxt->iStatus = END;
         break;
    case changeSpeed :
#ifdef DEBUGLOG
         printf("%d - changeSpeed\n",pCtxt->iCurrentItem);
#endif
         pCtxt->iStatus = next_item(pCtxt);
         break;
    case changeDirection :
#ifdef DEBUGLOG
         printf("%d - changeDirection\n",pCtxt->iCurrentItem);
#endif
         pCtxt->iStatus = next_item(pCtxt);
         break;

    case direction :
#ifdef DEBUGLOG
         printf("%d - direction\n",pCtxt->iCurrentItem);
#endif   
         load_expression(&stExpr, node->sValue, &pCtxt->stST);
         expr_solve(&stExpr, &pCtxt->fStepDirection);
#ifdef DEBUGLOG
         printf("%f\n",pCtxt->fDirection);
#endif  
         pCtxt->iDirectionType = node->iValue;
         if(pCtxt->pstData->listNode[node->iUp].mType == fire){
              if (pCtxt->iDirectionType == SEQUENCE){
                pCtxt->fDirection = pCtxt->fDirection + pCtxt->fStepDirection; 
              } else {
				    
			switch (pCtxt->iDirectionType)
            {
            case RELATIVE :
               //pCtxt->get_direction_callback(pCtxt->pUserData, &fTmpFloat);
               //pCtxt->fDirection = fTmpFloat;
               //pCtxt->fStepDirection = fTmpFloat + pCtxt->fStepDirection;
				pCtxt->fDirection = pCtxt->fDirection + pCtxt->fStepDirection; 
               break;
            case AIM : 
				  //puts("AIMING");
				  fTmpFloat = pCtxt->fDirection;
				  pCtxt->get_aim_direction_callback(pCtxt->pUserData, &fTmpFloat);
				  pCtxt->fDirection = fTmpFloat;
				  break;
			default :
				pCtxt->fDirection = pCtxt->fStepDirection;
				break;
			}
           }     
         }                                    
         if((pCtxt->pstData->listNode[node->iUp].mType != fire)&&
              (pCtxt->pstData->listNode[node->iUp].mType != changeDirection))
          {
              if (pCtxt->iDirectionType == SEQUENCE){
                pCtxt->fDirection = pCtxt->fDirection + pCtxt->fStepDirection; 
              } else {
                pCtxt->fDirection = pCtxt->fStepDirection;                                          
                pCtxt->set_direction_callback(pCtxt->pUserData,pCtxt->fDirection);
              }
           }
         if (pCtxt->pstData->listNode[node->iUp].mType == changeDirection)
              switch (pCtxt->iDirectionType)
              {
             
            case AIM : 
			   
			   pCtxt->get_direction_callback(pCtxt->pUserData, &pCtxt->fDirection);
			   fTmpFloat = pCtxt->fDirection;
               pCtxt->get_aim_direction_callback(pCtxt->pUserData, &fTmpFloat);
               
               pCtxt->fStepDirection = fTmpFloat + pCtxt->fStepDirection;
			   //pCtxt->fStepDirection = fTmpFloat;
               break; 
            case RELATIVE :
               pCtxt->get_direction_callback(pCtxt->pUserData, &fTmpFloat);
               pCtxt->fDirection = fTmpFloat;
               pCtxt->fStepDirection = fTmpFloat + pCtxt->fStepDirection;
               break;
            case SEQUENCE : 
               break;
            case ABSOLUTE : 
            default :
               pCtxt->get_direction_callback(pCtxt->pUserData, &pCtxt->fDirection);
               //pCtxt->fStepDirection = fTmpFloat + pCtxt->fStepDirection;
               break;
           }


         pCtxt->iStatus = next_item(pCtxt);
         break;
    case fire :
#ifdef DEBUGLOG
         printf("%d - fire\n",pCtxt->iCurrentItem);
#endif
         pCtxt->iStatus = next_item(pCtxt);
         break;
    case repeat :
#ifdef DEBUGLOG
         printf("%d - repeat %d\n",pCtxt->iCurrentItem,pCtxt->value[pCtxt->iCurrentItem]);
#endif
         if (pCtxt->value[pCtxt->iCurrentItem] == -1){
           iTmp = node->iDown;
           load_expression(&stExpr, pCtxt->pstData->listNode[iTmp].sValue, &pCtxt->stST);
           expr_solve(&stExpr, &fTmpFloat);          
           pCtxt->value[pCtxt->iCurrentItem] = (int)fTmpFloat;
#ifdef DEBUGLOG
           printf("repeat loaded %s\n",pCtxt->pstData->listNode[iTmp].sValue);
#endif
         }
         iTmp = node->iDown;
         if (pCtxt->value[pCtxt->iCurrentItem] > 0){
           pCtxt->value[pCtxt->iCurrentItem]--;
           pCtxt->iStatus = next_item(pCtxt);
         } else {
           pCtxt->value[pCtxt->iCurrentItem] = -1;
           pCtxt->iStatus = next_upitem(pCtxt);
           }
         break;
    case speed :
#ifdef DEBUGLOG
         printf("%d - speed\n",pCtxt->iCurrentItem);
#endif
         load_expression(&stExpr, node->sValue, &pCtxt->stST);
         expr_solve(&stExpr, &pCtxt->fStepSpeed);
#ifdef DEBUGLOG
         printf("%f\n",pCtxt->fSpeed);
#endif
         pCtxt->iSpeedType = node->iValue;
         if(pCtxt->pstData->listNode[node->iUp].mType == fire)
           pCtxt->fSpeed = pCtxt->fStepSpeed;                                                
         if((pCtxt->pstData->listNode[node->iUp].mType != fire)&&
              (pCtxt->pstData->listNode[node->iUp].mType != changeSpeed))
         {
           pCtxt->fSpeed = pCtxt->fStepSpeed;                                   
           pCtxt->set_speed_callback(pCtxt->pUserData,pCtxt->fStepSpeed);
         }
         if (pCtxt->pstData->listNode[node->iUp].mType == changeSpeed)
             switch (pCtxt->iSpeedType)
             {
                    case RELATIVE :
                         pCtxt->get_speed_callback(pCtxt->pUserData, &fTmpFloat);
                         pCtxt->fSpeed = fTmpFloat;
                         pCtxt->fStepSpeed = fTmpFloat + pCtxt->fStepSpeed;
                         break;
                    case SEQUENCE : 
                         break;
                    case ABSOLUTE : 
                    default : 
                     pCtxt->get_speed_callback(pCtxt->pUserData, &pCtxt->fSpeed);
                         break;
             }

         pCtxt->iStatus = next_item(pCtxt);
         break;
    case term :
#ifdef DEBUGLOG
         printf("%d - term %d\n",pCtxt->iCurrentItem,pCtxt->value[pCtxt->iCurrentItem]);
#endif
         if (pCtxt->value[pCtxt->iCurrentItem] == -1){
           //iTmp = node->iDown;
           load_expression(&stExpr, node->sValue, &pCtxt->stST);
           expr_solve(&stExpr, &fTmpFloat);    
           if(pCtxt->pstData->listNode[node->iUp].mType == changeSpeed){ 
             pCtxt->fChangeSpeedTarget = pCtxt->fStepSpeed;     
             pCtxt->fStepSpeed = (pCtxt->fStepSpeed - pCtxt->fSpeed) / fTmpFloat; 
             //pCtxt->fSpeed += pCtxt->fStepSpeed;
             pCtxt->fChangeSpeedStep = pCtxt->fStepSpeed;

           } else {
             pCtxt->fChangeDirectionTarget = pCtxt->fStepDirection;
             pCtxt->fStepDirection = (pCtxt->fStepDirection - pCtxt->fDirection) / fTmpFloat;
             pCtxt->fChangeDirectionStep = pCtxt->fStepDirection;
             //pCtxt->fDirection += pCtxt->fStepDirection;
           }
          // pCtxt->value[pCtxt->iCurrentItem] = (int)fTmpFloat;
           //pCtxt->iStatus = PAUSE;
         }
/*
         if (pCtxt->value[pCtxt->iCurrentItem] > 0){
           pCtxt->value[pCtxt->iCurrentItem]--;
           pCtxt->iStatus = PAUSE;
*/  
         if(pCtxt->pstData->listNode[node->iUp].mType == changeSpeed)
          {
/*
             pCtxt->fSpeed += pCtxt->fStepSpeed;
             pCtxt->set_speed_callback(pCtxt->pUserData,pCtxt->fSpeed); 
*/                                           
          }   
         else if(pCtxt->pstData->listNode[node->iUp].mType == changeDirection)
           {
/*
             pCtxt->fDirection += pCtxt->fStepDirection;
             pCtxt->set_direction_callback(pCtxt->pUserData,pCtxt->fDirection);       
 */            
           }
           
         //  if (pCtxt->value[pCtxt->iCurrentItem] == 0){
             pCtxt->value[pCtxt->iCurrentItem] = -1;
             pCtxt->iStatus = next_upitem(pCtxt);
           //} 
      //   }
         break;
    case times :

#ifdef DEBUGLOG
         printf("%d - times\n",pCtxt->iCurrentItem);
#endif
         pCtxt->iStatus =next_item(pCtxt);
         break;
    case wait :
#ifdef DEBUGLOG
         printf("%d - wait\n",pCtxt->iCurrentItem);
#endif
         if (pCtxt->value[pCtxt->iCurrentItem] == -1){
           //iTmp = node->iDown;
           load_expression(&stExpr, node->sValue, &pCtxt->stST);
           expr_solve(&stExpr, &fTmpFloat);          
           pCtxt->value[pCtxt->iCurrentItem] = (int)fTmpFloat;
         }
         if (pCtxt->value[pCtxt->iCurrentItem] > 0){
           pCtxt->value[pCtxt->iCurrentItem]--;
           pCtxt->iStatus = PAUSE;
         } else {
           pCtxt->value[pCtxt->iCurrentItem] = -1;
           pCtxt->iStatus = next_item(pCtxt);}
         break;
    default : 
#ifdef DEBUGLOG
            printf("DEFAULT\n");
#endif
            pCtxt->iStatus = next_item(pCtxt);
            break;     
  }
  return pCtxt->iStatus;
}

int run_bulletml( tExecutionContext * pCtxt)
{ 
    int ret = 0;
    float fReg;

    if (pCtxt->fChangeDirectionStep != 0.)
    {

       pCtxt->get_direction_callback(pCtxt->pUserData, &fReg);
       fReg += pCtxt->fChangeDirectionStep;
       pCtxt->set_direction_callback(pCtxt->pUserData, fReg);
       if (((pCtxt->fChangeDirectionStep + fReg)> pCtxt->fChangeDirectionTarget)
       && ((fReg) <  pCtxt->fChangeDirectionTarget))
       {
         pCtxt->fChangeDirectionStep = 0.;
       } else 
       if (((pCtxt->fChangeDirectionStep + fReg)< pCtxt->fChangeDirectionTarget)
       && ((fReg) >  pCtxt->fChangeDirectionTarget))
       {
         pCtxt->fChangeDirectionStep = 0.;
       }
       
    }
    if (pCtxt->fChangeSpeedStep  != 0.)
    {
       pCtxt->get_speed_callback(pCtxt->pUserData, &fReg);
       fReg += pCtxt->fChangeSpeedStep;
       pCtxt->set_speed_callback(pCtxt->pUserData, fReg);
       if (((pCtxt->fChangeSpeedStep + fReg)> pCtxt->fChangeSpeedTarget)
       && ((fReg) <  pCtxt->fChangeSpeedTarget))
       {
         pCtxt->fChangeSpeedStep = 0.;
       } else 
       if (((pCtxt->fChangeSpeedStep + fReg)< pCtxt->fChangeSpeedTarget)
       && ((fReg) >  pCtxt->fChangeSpeedTarget))
       {
         pCtxt->fChangeSpeedStep = 0.;
       }
    }

/*
   float fChangeDirectionStep;
   float fChangeDirectionTarget;
   float fChangeSpeedStep;
   float fChangeSpeedTarget; 
*/
    while (ret == CONTINUE){
      ret = run_item(pCtxt);
    }
    return ret;
}

int load_bulletml(char * sFileName, tParsingData * pstData)
{
  XML_Parser p = XML_ParserCreate(NULL);
  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    exit(-1);
  }
  printf("filename: %s\n",sFileName);
  FILE * f; 
  fopen_s(&f,sFileName,"rb"); 
  if (f == NULL)
    printf("error opening file\n");
  //stParsingData stData;
  pstData->iNodePosition = 0;
  pstData->iStackPos = 0;
  pstData->p = p;
  pstData->iLabelTableCount = 0;

  XML_SetUserData(p, pstData);
  XML_SetElementHandler(p, start, end);

  for (;;) {
    int done;
    int len;
    len = fread(Buff, 1, BUFFSIZE, f);
    if (ferror(stdin)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(f);
    if (! XML_Parse(p, Buff, len, done)) {
      fprintf(stderr, "Parse error at line %d:\n%s\n",
	      XML_GetCurrentLineNumber(p),
	      XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (done)
      break;
  }
  XML_ParserFree(p);
#ifdef DEBUGLOG
  for (int i = 0;i < pstData->iNodePosition;i++)
  printf("NODE : %d | UP : %d | DOWN : %d | NEXT : %d | SVALUE : %s\n",i,pstData->listNode[i].iUp
         ,pstData->listNode[i].iDown,pstData->listNode[i].iNext,pstData->listNode[i].sValue);
#endif

  for (int i = 0;i < pstData->iLabelTableCount;i++)
  {
      if (!strcmp(pstData->vLabelLookupTable[i].sLabel,"top")){
         pstData->iTopRef= pstData->vLabelLookupTable[i].iRef;
          //printf("Start position: %d\n",pstCtxt->iCurrentItem);
        }
  }
  return 0;
    
}

void init_bulletmlcontext(tExecutionContext * pstCtxt, tParsingData * pstData)
{
  pstCtxt->pstData = pstData;
  pstCtxt->stST.iSymbolCount = 0;
  pstCtxt->fSpeed = 0.;
  pstCtxt->fDirection = 0.;
  pstCtxt->fStepDirection = 0.;
 
  pstCtxt->fChangeDirectionStep = 0.;
  pstCtxt->fChangeDirectionTarget = 0.;
  pstCtxt->fChangeSpeedStep = 0.;
  pstCtxt->fChangeSpeedTarget = 0.;  

  for (int i = 0;i < 200;i++)
    pstCtxt->value[i] = -1;
  pstCtxt->iCurrentItem = pstData->iTopRef;   
}
