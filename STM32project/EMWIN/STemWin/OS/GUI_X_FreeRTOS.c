#include "GUI.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

static SemaphoreHandle_t osMutex;
static SemaphoreHandle_t osSemaphore;

static SemaphoreHandle_t KeySemaphore;
static int				 KeyPressed;
static char			     KeyIsInited;
/*
*********************************************************************************************************
*                                        TIMING FUNCTIONS
*
* Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion. 
*        Default time unit (tick), normally is 1 ms.
*********************************************************************************************************
*/
int GUI_X_GetTime(void) { 
	return ((int)xTaskGetTickCount());
}

//GUI延时函数
void GUI_X_Delay(int period) { 
	
	vTaskDelay(period);
}

/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
void GUI_X_ExecIdle(void)
{
	GUI_X_Delay(1);
}

/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/
void GUI_X_InitOS(void)
{
	osMutex=xSemaphoreCreateMutex();		//创建互斥信号量
	vSemaphoreCreateBinary(osSemaphore);	//创建二值信号量
}

//等待信号量
void GUI_X_Lock(void)
{
	xSemaphoreTake(osMutex,portMAX_DELAY);	//请求信号量
}
//发送信号量
void GUI_X_Unlock(void)
{
	xSemaphoreGive(osMutex);				//释放信号量
}

//返回任务ID
U32 GUI_X_GetTaskId(void)
{
	return ((uint32_t)xTaskGetCurrentTaskHandle()); 	//获取任务ID
}

/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/

void GUI_X_WaitEvent(void)
{
	xSemaphoreTake(osSemaphore,portMAX_DELAY);//请求信号量
}

void GUI_X_SignalEvent(void)
{
	xSemaphoreGive(osSemaphore);			 //释放信号量
}
/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/

static void CheckInit(void)
{
	if(KeyIsInited == pdFALSE){
		KeyIsInited = pdTRUE;
		GUI_X_Init();
	}
}

void GUI_X_Init(void)
{
	vSemaphoreCreateBinary(KeySemaphore);	//创建二值信号量
}

int GUI_X_GetKey(void) 
{
	int r;
	
	r = KeyPressed;
	CheckInit();
	KeyPressed = 0;
	return (r);
}

int GUI_X_WaitKey(void)
{
	int r;
	
	CheckInit();
	if(KeyPressed == 0)
	{
		xSemaphoreTake(KeySemaphore,portMAX_DELAY);//请求信号量
	}
	r = KeyPressed;
	KeyPressed = 0;
	return (r);
}

void GUI_X_StoreKey(int k)
{
	KeyPressed = k;
	xSemaphoreGive(KeySemaphore);			 //释放信号量
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

