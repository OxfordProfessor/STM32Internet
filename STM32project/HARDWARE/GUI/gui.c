#include "gui.h"
#include "stdlib.h"
#include "stdio.h"
#include "ds18b20.h"
#include "ILI93xx.h"

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_GRAPH_0 (GUI_ID_USER + 0x01)
#define ID_Text_0 (GUI_ID_USER + 0x02)
#define ID_Text_1 (GUI_ID_USER + 0x03)
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*数据对象句柄*/
GRAPH_DATA_Handle Graphdata;
GRAPH_DATA_Handle Graphdata2;
int t = 0;
short temperature;
extern short DS18B20_Get_Temp();

/*********************************************************************
*
*       _aDialogCreate，创建窗体
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 900, 450, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 440, 410, 0, 0x0, 0 },
  { TEXT_CreateIndirect,"Text",ID_Text_0,450,25,440,50,0,0x0,0},
  { TEXT_CreateIndirect,"Text",ID_Text_1,450,120,440,50,0,0x0,0}
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/**
  * @brief 对话框回调函数
  * @note 无
  * @param pMsg：消息指针
  * @retval 无
  */
static void _cbDialog(WM_MESSAGE* pMsg) {
	WM_HWIN hItem;
  GRAPH_SCALE_Handle hScaleV;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
	  /* 初始化Framewin控件 */
	  hItem = pMsg->hWin;
	  FRAMEWIN_SetText(hItem, "Made by Zhangkaiyang:The sensor");
	  FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
	  /* 初始化Graph控件 */
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
      GRAPH_SetColor(hItem, GUI_WHITE, GRAPH_CI_BK);
      GRAPH_SetColor(hItem, GUI_BLACK, GRAPH_CI_GRID);
	  GRAPH_SetBorder(hItem, 30, 10, 10, 10);
	  GRAPH_SetGridDistX(hItem, 30);
	  GRAPH_SetGridDistY(hItem, 30);
	  GRAPH_SetLineStyleH(hItem, GUI_LS_DOT);
	  GRAPH_SetLineStyleV(hItem, GUI_LS_DOT);
	  GRAPH_SetGridVis(hItem, 1);
	  /* 创建垂直刻度对象 */
	  hScaleV = GRAPH_SCALE_Create(15, GUI_TA_HCENTER | GUI_TA_LEFT,
	                               GRAPH_SCALE_CF_VERTICAL, 50);
	  GRAPH_AttachScale(hItem, hScaleV);
	  GRAPH_SCALE_SetFactor(hScaleV, 0.1);
	  /* 创建数据对象 */
	  Graphdata = GRAPH_DATA_YT_Create(GUI_RED, 500, 0, 0);
	  Graphdata2 = GRAPH_DATA_YT_Create(GUI_BLUE, 500, 0, 0);
	  GRAPH_AttachData(hItem, Graphdata);
	  GRAPH_AttachData(hItem, Graphdata2);
	  /*创建文本显示对象*/
	  /* 初始化Text0 */
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_Text_0);
      TEXT_SetFont(hItem, GUI_FONT_COMIC24B_ASCII);
      TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
      TEXT_SetText(hItem, "The Temperature");
	  /* 初始化Text1 */
      hItem = WM_GetDialogItem(pMsg->hWin, ID_Text_1);
      TEXT_SetFont(hItem, GUI_FONT_8X16X1X2);
      TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
      TEXT_SetDec(hItem, 0, 4, 0, 0, 0);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
 /**
  * @brief 以对话框方式间接创建控件
  * @note 无
  * @param 无
  * @retval hWin：资源表中第一个控件的句柄
  */
WM_HWIN CreateFramewin(void);  
WM_HWIN CreateFramewin(void) {
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	return hWin;
}

/**
  * @brief 十进制数显示
  * @note 无
  * @param 无
  * @retval 无
  */
static void Disp_DecNum(void)
{
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_DispStringAt("GUI_DispDec():", 0, 0);
	GUI_DispNextLine();
	GUI_DispDec(12345, 6);
	GUI_GotoX(7 * 9);
	GUI_DispDec(-12345, 6);
	
	GUI_DispStringAt("GUI_DispDecAt():", 0, 13 * 2);
	GUI_DispDecAt(12345, 0, 13 * 3, 6);
	GUI_DispDecAt(-12345, 7 * 9, 13 * 3, 6);
	
	GUI_DispStringAt("GUI_DispDecMin():", 0, 13 * 4);
	GUI_DispNextLine();
	GUI_DispDecMin(12345);
	GUI_GotoX(7 * 9);
	GUI_DispDecMin(-12345);
	
	GUI_DispStringAt("GUI_DispDecShift():", 0, 13 * 6);
	GUI_DispNextLine();
	GUI_DispDecShift(12345, 7, 2);
	GUI_GotoX(7 * 9);
	GUI_DispDecShift(-12345, 7, 2);
	
	GUI_DispStringAt("GUI_DispDecSpace():", 0, 13 * 8);
	GUI_DispNextLine();
	GUI_DispDecSpace(12345, 6);
//	GUI_GotoX(12 * 9);
//	GUI_DispDecSpace(-12345, 7);

  GUI_DispStringAt("GUI_DispSDec():", 0, 13 * 10);
	GUI_DispNextLine();
	GUI_DispSDec(12345,6);
	
	GUI_DispStringAt("GUI_DispSDecShift():", 0, 13 * 12);
	GUI_DispNextLine();
	GUI_DispSDecShift(12345, 7, 2);
}

/**
  * @brief 浮点数显示
  * @note 无
  * @param 无
  * @retval 无
  */
static void Disp_FloatingNum(void)
{
	int x0 = 120;
	
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_GotoX(x0);
	GUI_DispStringAt("GUI_DispFloat():", x0, 0);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloat(123.4567, 9);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloat(-123.4567, 9);
	
	GUI_DispStringAt("GUI_DispFloatFix():", x0, 13 * 2);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloatFix(123.4567, 9, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloatFix(-123.4567, 9, 3);
	
	GUI_DispStringAt("GUI_DispFloatMin():", x0, 13 * 4);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispFloatMin(123.4567, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispFloatMin(-123.4567, 3);
	
	GUI_DispStringAt("GUI_DispSFloatFix():", x0, 13 * 6);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispSFloatFix(123.4567, 9, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispSFloatFix(-123.4567, 9, 3);
	
	GUI_DispStringAt("GUI_DispSFloatMin():", x0, 13 * 8);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispSFloatMin(123.4567, 3);
	GUI_GotoX(x0 + 5 * 12);
	GUI_DispSFloatMin(-123.4567, 3);
}

/**
  * @brief 二进制数和十六进制数显示
  * @note 无
  * @param 无
  * @retval 无
  */
static void Disp_BinHexNum(void)
{
	int x0 = 0;
	
	GUI_SetFont(GUI_FONT_13_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_DispStringAt("GUI_DispBin():", x0, 13 * 16);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispBin(10, 8);
	
	GUI_DispStringAt("GUI_DispBinAt():", x0, 13 * 18);
	GUI_DispBinAt(10, x0, 13 * 19, 8);
	
	GUI_DispStringAt("GUI_DispHex():", x0, 13 * 20);
	GUI_DispNextLine();
	GUI_GotoX(x0);
	GUI_DispHex(4779, 4);
	
	GUI_DispStringAt("GUI_DispHexAt():", 120, 13 * 16);
	GUI_DispNextLine();
	GUI_DispHexAt(4779, 120, 13 * 17, 4);
	
	GUI_DispStringAt("GUI_GetVersionString()", 120, 13 * 18);
	GUI_DispStringAt(GUI_GetVersionString(), 120, 13 * 19);
}
/**
  * @brief GUI主任务
  * @note 无
  * @param 无
  * @retval 无
  */
void MainTask(void)
{
	/* 创建窗口 */
	WM_HWIN hWin;
    WM_HWIN hItem;
	U32 temp;
	
	hWin = CreateFramewin();
	hItem = WM_GetDialogItem(hWin, ID_Text_1);
	while(1)
	{
		/* 向GRAPH数据对象添加数据 */
		GRAPH_DATA_YT_AddValue(Graphdata, DS18B20_Get_Temp());
		GRAPH_DATA_YT_AddValue(Graphdata2, DS18B20_Get_Temp()/2);
		temp = DS18B20_Get_Temp();
		TEXT_SetDec(hItem, temp, 3, 0, 0, 0);
		GUI_Delay(150);
	}
}