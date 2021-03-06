// GPRS.cpp: implementation of the CGPRS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GPRS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPRS::CGPRS()
{
    pCeSerial = NULL;
    ResetGlobalVarial();
}

CGPRS::~CGPRS()
{
    if (pCeSerial != NULL)
    {       
        delete pCeSerial;
    }
}

/********************************************************************************************************
函数名称: ResetGlobalVarial
描    述: 复位一些全局变量为默认值
输入参数: 无
输出参数: 无
返    回: 无
*********************************************************************************************************/
void CGPRS::ResetGlobalVarial(void)
{
    bSendATCmd = FALSE;                                                 /* 当前不处于发送命令状态       */
    m_RespCnt = 0;                            
    m_strRespBuf = _T("");
    ResetEvent(m_ATCmdRespEvent);                                       /* 复位 GPRS 响应事件           */
}

/********************************************************************************************************
函数名称: GPRS_Init
描    述: GPRS 模块初始化函数
输入参数: LPCTSTR Port   : 串口号, 如 "COM1:"
          int BaudRate   : 串口波特率, 如 115200
          DWORD UserParam: 用户传递到该类中的参数
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS:: GPRS_Init(int Port, int BaudRate, DWORD UserParam)
{    
    BOOL ret;
    DWORD Trycount = 2;

    do{                                                                 /* 尝试初始化 GPRS, 最多尝试两  */
                                                                        /* 次                           */
        Trycount--;

        pCeSerial = new CxComCE;
        ret = pCeSerial->Open(Port, 115200, 8, ONESTOPBIT, NOPARITY);        
        if (ret == FALSE)                                               /* 打开串口, 数据位为8,停止位为 */
                                                                        /* 1,无校验位                   */
        {
            AfxMessageBox(_T("串口打开失败！"), MB_OK);
            delete pCeSerial;
            return FALSE;
        }

        pCeSerial->EscapeCommFunction(SETRTS);
        pCeSerial->EscapeCommFunction(SETDTR); 

       

                                   /* 串口接收成功回调函数         */
        m_UserParam = UserParam;                                        /* 保存用户传进来的参数         */
        m_ATCmdRespEvent = CreateEvent(NULL, FALSE, FALSE, NULL);        /* 创建 GPRS 响应事件           */

        pCeSerial->BeginRecvThread(OnCommRecv,this);
        ret = GPRS_SendATCmd(_T("AT"));                                 /* 尝试模块是否处于激活状态     */
        ret = GPRS_WaitResponseOK();
        ret = GPRS_SendATCmd(_T("ATE0"));                               /* 尝试模块是否处于激活状态     */
        ret = GPRS_WaitResponseOK();
        ret = GPRS_SendATCmd(_T("AT"));                                 /* 尝试模块是否处于激活状态     */
        ret = GPRS_WaitResponseOK();
        if (ret == FALSE)
        {
            delete pCeSerial;
            if (Trycount == 0) return FALSE;
        }
        else
            Trycount = 0;

    }while(Trycount > 0);

    GPRS_SendATCmd(_T("AT+CREG?"));                
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CLIP=1"));                                    /* 设置来电显示                */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CMGF=1"));                                    /* 设置为文本模式               */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CSCS=\"GSM\""));                              /* 设置字符集                   */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CSMP=17,168,0,0"));                           /* 设置文本模式参数             */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
函数名称: OnCommRecv
描    述: 串口接收成功回调函数, 当串口正确接收到数据时, 该函数将被调用
输入参数: CString strCmd: 命令字
输出参数: DWORD UserParam : 用户传递的参数, 通常为CGPRS类的一个对象的指针
          BYTE *buf          : 串口接收数据缓冲
          DWORD buflen      : 串口接收到的数据长度
返    回: 无
*********************************************************************************************************/
void CALLBACK CGPRS::OnCommRecv(void * UserParam, const void  *buf, unsigned int buflen)
{
    UINT Length;
    CGPRS *pGPRS = (CGPRS *)UserParam;    
    
    CString strTmp = _T("");
    LPTSTR pStr = strTmp.GetBuffer(100);    
    Length=MultiByteToWideChar(CP_ACP, 0, (char *)buf, buflen, pStr, 100);
    char *p=(char *)buf;
    pStr[Length]=0;
                                                                        /* 将接收到的数据转换为 Unicode */
                                                                        /* 编码                         */
    strTmp.ReleaseBuffer();

    if (pGPRS->bSendATCmd == TRUE)
    {                                                                   /* 之前用户发送了 AT 指令       */
        pGPRS->m_strRespBuf.Insert(pGPRS->m_RespCnt, strTmp);           /* 保存数据                     */
        SetEvent(pGPRS->m_ATCmdRespEvent);
        return;
    }
    
    if ((strTmp.Find(_T("RING")) >= 0) || 
        (strTmp.Find(_T("ring")) >= 0))
    {                                                                   /* 有电话打进来                 */
        CString strCallNum = pGPRS->GPRS_GetTheCallNum(strTmp);
        pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_RECEIVE_CALL,strCallNum);
    }
    
    if ((strTmp.Find(_T("NO CARRIER")) >= 0) || 
        (strTmp.Find(_T("no carrier")) >= 0))                
    {                                                                   /* 来电后, 对方挂机             */
        pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_OTHER_SIDE_HANDUP, L"");
    }

    pGPRS->bSendATCmd = FALSE;                                          /* 复位相关全局变量             */
    pGPRS->m_RespCnt = 0;                            
    pGPRS->m_strRespBuf = _T("");
    ResetEvent(pGPRS->m_ATCmdRespEvent);
}


/********************************************************************************************************
函数名称: GPRS_GetTheCallNum
描    述: 取得来电的电话号码
输入参数: CString str:  来电时, GPRS 响应字符
输出参数: 无
返    回: 来电电话号码
*********************************************************************************************************/
CString CGPRS::GPRS_GetTheCallNum(CString str)
{
    int pos1 = str.Find(_T("+CLIP: \""), 0);
    if (pos1 < 0)
        return L"";
    else
        pos1 = pos1 + wcslen(_T("+CLIP: \""));

    int pos2 = str.Find(_T("\""), pos1);
    if (pos2 < 0)
        return L"";

    CString strNum;
    strNum = str.Mid(pos1, pos2 - pos1);                                /* 取得电话号码                 */

    return strNum;
}


/********************************************************************************************************
函数名称: GPRS_SendATCmd
描    述: 发送 AT 命令
输入参数: CString strCmd: 命令字
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_SendATCmd(CString strCmd)
{
    int len = strCmd.GetLength();
    BYTE *psendbuf = new BYTE[len + 2];

    m_RespCnt = 0;
    bSendATCmd = TRUE;                                                  /* 进入发送命令状态             */

    for(int i = 0; i < len;i++)
        psendbuf[i] = (char)strCmd.GetAt(i);                            /* 转换为单字节字符             */

    psendbuf[len] = L'\r';                                              /* 加入结束符                   */
    psendbuf[len + 1] = L'\n';
    //strCmd += _T("\r\n");
    USES_CONVERSION;
//    pCeSerial->Senddata((BYTE*)W2A((LPCTSTR)strCmd), strCmd.GetLength());
        pCeSerial->Write(psendbuf, len + 2);                         /* 从串口发送数据               */

    delete[] psendbuf;
    return TRUE;
}


/********************************************************************************************************
函数名称: GPRS_WaitResponseOK
描    述: 等待 AT 命令响应 OK
输入参数: 无
输出参数: 无
返    回: 无
*********************************************************************************************************/
BOOL CGPRS::GPRS_WaitResponseOK(void)
{
    DWORD ret;
    BOOL bOK = TRUE;

    while(1)
    {    /*
          * 等待 0.5 秒, 如果得不到回复, 即为错误
          */
        ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
        if (ret == WAIT_OBJECT_0)
        {
            if ((m_strRespBuf.Find(_T("ERROR\r\n"), 0) >= 0) ||
                (m_strRespBuf.Find(_T("error\r\n"), 0) >= 0))
            {                                                           /* 响应中有"ERROR"              */
                bOK = FALSE;
                break;
            }
            if ((m_strRespBuf.Find(_T("OK\r\n"), 0) >= 0) ||
                (m_strRespBuf.Find(_T("ok\r\n"), 0) >= 0))
            {                                                           /* 响应中有"OK"                 */
                bOK = TRUE;
                break;
            }

            ResetEvent(m_ATCmdRespEvent);
        }
        else
        {
            bOK = FALSE;                                                /* 响应超时                     */
            break;
        }
    }

    ResetGlobalVarial();                                                /* 复位用到的全局变量           */
    if (FALSE == bOK)
        return FALSE;

    return TRUE;
}

int CGPRS::GPRS_GetSignal()
{
    ResetGlobalVarial();
    GPRS_SendATCmd(L"AT+CSQ");

    DWORD ret = WaitForSingleObject(m_ATCmdRespEvent, 5000);
    if (ret !=WAIT_OBJECT_0)
    {
        return -2;
    }

    int pos=m_strRespBuf.Find(L"+CSQ");
    if (pos <0)
    {
        return -1;
    }

    LPCWSTR p=m_strRespBuf;
    p+=pos;
    p+=5;
    int signal;
    swscanf(p,L"%d",&signal);
    
    return signal;
}



/********************************************************************************************************
函数名称: GPRS_WriteMsg
描    述: 将短信内容写入 GPRS 模块
输入参数: CString Msg:  短信内容
输出参数: 无
返    回: 无
*********************************************************************************************************/
BOOL CGPRS::GPRS_WriteMsg(CString Msg)
{
    int len = Msg.GetLength();
    BYTE *psendbuf = new BYTE[len + 3];

    for(int i = 0; i < len;i++)
        psendbuf[i] = (char)Msg.GetAt(i);                               /* 转换为单字节字符             */

    psendbuf[len] = '\r';                                               /* 加入结束符                   */
    psendbuf[len + 1] = '\n';
    psendbuf[len + 2] = 0x1A;
    pCeSerial->Write(psendbuf, len + 3);                             /* 从串口发送数据               */

    delete[] psendbuf;
    return TRUE;
}


/********************************************************************************************************
函数名称: GPRS_DialUp
描    述: 拨打电话(由本机打出)
输入参数: CString strTelNum: 电话号码
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_DialUp(CString strTelNum)
{
    CString strDialup = _T("ATD");
    strDialup += strTelNum + L";";                                      /* 以分号作为结束               */

    GPRS_SendATCmd(strDialup);                                          /* 发送拨号命令                 */

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
函数名称: GPRS_DialDown
描    述: 挂机(由本机打出的挂机)
输入参数: 无
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_DialDown(void)
{
    GPRS_SendATCmd(L"ATH");

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
函数名称: GPRS_TakeTheCall
描    述: 接听电话(有电话打进来, 接听该电话)
输入参数: 无
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_TakeTheCall(void)
{
    GPRS_SendATCmd(L"ATA");

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
函数名称: GPRS_TakeTheCall
描    述: 挂断电话(有电话打进来时, 挂断电话)
输入参数: 无
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_HangUpTheCall(void)
{
    GPRS_SendATCmd(L"AT+CHUP");
    
    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
函数名称: GPRS_SetShortMSGCenterTel
描    述: 设置短信中心号码
输入参数: CString strTelNum:  短信中心号码
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_SetShortMSGCenterTel(CString strTelNum)
{
    CString strTmp;

    strTmp = strTelNum + L",145";

    GPRS_SendATCmd(strTelNum);

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
函数名称: FindMsgPos
描    述: 设置短信中心号码
输入参数: CString strTelNum:  短信中心号码
输出参数: 无
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
int CGPRS::FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen)
{
    /*
     * 短信开始位置
     */
    int length = m_strRespBuf.GetLength();
    int posStart = m_strRespBuf.Find(_T("+CMGR:"), 0);                    
    if (posStart < 0)
        return -1;

    /*
     * 电话号码开始位置
     */
    *posnum = m_strRespBuf.Find(_T("\"REC READ\",\""), posStart);    
    if (*posnum < 0)
        return -1;

    *posnum = *posnum + wcslen(_T("\"REC READ\",\""));            

    /*
     * 电话号码结束位置
     */
    int posnumend = m_strRespBuf.Find(_T("\""), *posnum + 1);
    if (posnumend < 0)
        return -1;

    *numlen = posnumend - *posnum;                                      /* 电话号码长度                 */
    
    *posmsg = m_strRespBuf.Find(_T("\r\n"), posStart);                  /* 短信开始位置                 */
    if (*posmsg < 0)
        return -1;

    *posmsg = *posmsg + wcslen(_T("\r\n"));    
    
    int posmsgend = m_strRespBuf.Find(_T("\r\n\r\nOK"), *posmsg);       /* 短信结束位置                 */
    *msglen = posmsgend - *posmsg;
    
    return 1;
}


/********************************************************************************************************
函数名称: GPRS_ReadShortMessage
描    述: 读取短信内容
输入参数: DWORD num        : 短信序号
输出参数: CString *Msg    : 短信内容
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg)
{
    CString strReadNum;
    BOOL ret, retvalue;

    strReadNum.Format(_T("AT+CMGR=%d"), num);
    bSendATCmd = TRUE;
    GPRS_SendATCmd(strReadNum);                                         /* 命令: 读第 num 条短信        */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
        if (ret == WAIT_OBJECT_0)
        {
            if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) > 0)              /* 响应为 ERROR                 */
            {
                //AfxMessageBox(_T("读取短信发现错误"));
                retvalue = FALSE;
                break;
            }
            
            int posnum, numlen, posmsg, msglen;    
            int pos = FindMsgPos(&posnum, &numlen, &posmsg, &msglen);   /* 寻找短信位置                 */
            if (pos > 0)
            {
                //char buffer[1024];

                //for (int i = 0; i < m_strRespBuf.GetLength(); i++)
                //    buffer[i] = (char)m_strRespBuf.GetAt(i);

                *strTelCode = m_strRespBuf.Mid(posnum, numlen);
                *Msg = m_strRespBuf.Mid(posmsg, msglen);                /* 取出短信内容                 */
                retvalue = TRUE;
                break;
            }
            
            ResetEvent(m_ATCmdRespEvent);
            
        }//end of if (ret == WAIT_OBJECT_0)
        else
        {
            //AfxMessageBox(_T("读取短信超时"));
            retvalue = FALSE;
            break;
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
函数名称: GPRS_SendShortMessage
描    述: 发送短信
输入参数: CString strTelNum : 对方电话号码
输出参数: CString *Msg        : 短信内容
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_SendShortMessage(CString strTelNum, CString Msg)
{
    BOOL ret, retvalue;
    int pos;

    /* 
    *    1. 发送电话号码
    */
    CString strNum;
    strNum.Format(_T("AT+CMGS=\"%s\""), strTelNum);
    bSendATCmd = TRUE;
    GPRS_SendATCmd(strNum);                                             /* 发送电话号码                 */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
        if (ret == WAIT_OBJECT_0)    
        {    
            pos = m_strRespBuf.Find(_T('>'), 0);
            if (pos > 0)
            {                                                           /* 收到提示符, 现在可以往模块写 */
                                                                        /* 入短信内容了                 */
                retvalue = TRUE;
                break;
            }
            pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;                                       /* GPRS 响应字符中包含有"ERROR" */
                break;
            }
        }
        else
        {
            retvalue = FALSE;
            break;
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE) return FALSE;

    /* 
    *    2. 等待 '>' 提示符, 发送短信内容
    */
    bSendATCmd = TRUE;
    GPRS_WriteMsg(Msg);    

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent,500);
        if (ret == WAIT_OBJECT_0)    
        {
            int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;
                break;
            }
            pos = m_strRespBuf.Find(_T("CMGS"), 0);
            if (pos > 0)
            {                                                           /* 响应正确, 发送成功           */
                retvalue = TRUE;
                break;
            }
        }
    }
    
    ResetGlobalVarial();
    if (retvalue == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
函数名称: CheckMsgNum
描    述: 检查短信数量: SIN卡中有效短信数量及可纳容的最大短信数量(private函数)
输入参数: CString str       : GPRS 响应字符
输出参数: CString *strNum  : 有效短信数量
          CString *strTotal: 可纳容的最大短信数量
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CheckMsgNum(CString str, CString *strNum, CString *strTotal)
{
    int pos1 = str.Find(_T("\"SM\","), 0);                              /* 标识字符                     */
    if (pos1 < 0)
        return FALSE;

    int pos2 = str.Find(_T(','), pos1 + 5);                             /* 分隔逗号                     */
    if (pos2 < 0)
        return FALSE;

    *strNum = str.Mid(pos1 + 5, pos2 - (pos1 + 5));                     /* 现有短信数量                 */

    int pos3 = str.Find(_T(","), pos2 + 1);
    if (pos3 < 0)
        return FALSE;

    *strTotal = str.Mid(pos2 + 1, pos3 - pos2 - 1);                     /* 可纳容的最大短信数量         */

    return TRUE;
}


/********************************************************************************************************
函数名称: GPRS_CheckMsgNum
描    述: 检查短信数量: SIN卡中有效短信数量及可纳容的最大短信数量(public函数)
输入参数: 无
输出参数: CString *strNum  : 有效短信数量
          CString *strTotal: 可纳容的最大短信数量
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_CheckMsgNum(CString *strNum, CString *strTotal)
{    
    BOOL ret, retvalue;
    CString strTmp;

    bSendATCmd = TRUE;
    GPRS_SendATCmd(_T("AT+CPMS?"));                                     /* 查询命令                     */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
        if (ret == WAIT_OBJECT_0)    
        {
            int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;
                break;
            }

            if (CheckMsgNum(m_strRespBuf, strNum, strTotal) == TRUE)
            {                                                           /* 查询成功                     */
                retvalue = TRUE;
                break;
            }
            ResetEvent(m_ATCmdRespEvent);
        }
        else
        {
            retvalue = FALSE;
            break;            
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE) 
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
函数名称: GPRS_DeleteShortMsg
描    述: 删除短信
输入参数: 无
输出参数: DWORD num  : 短信序号
返    回: TRUE: 成功   FALSE: 失败
*********************************************************************************************************/
BOOL CGPRS::GPRS_DeleteShortMsg(DWORD num)
{
    CString strCmd;
    BOOL ret;

    strCmd.Format(_T("AT+CMGD=%d"), num);

    GPRS_SendATCmd(strCmd);
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    return TRUE;
}

void CGPRS::GPRS_Deinit()
{
    if (pCeSerial != NULL)
    {       
        delete pCeSerial;
    }
    pCeSerial= NULL;
    ResetGlobalVarial();
}