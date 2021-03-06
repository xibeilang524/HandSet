/**
 * \file
 *      PcapBrowserDlg.h
 *
 * \brief
 *      pcap文件浏览器实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao       2013/11/13
 */

// PcapBrowserDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "PcapBrowserDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/utils/decode.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/progressbardlg.h"
#include "src/main/utils/rnavimenu.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/smv/common_smv.h"
#include "src/main/popdlg/common_pop.h"
#include "src/main/goose/utility_goose.h"
#include "src/utils/ReportControl/ListBoxEL/TreeBoxEL.h"

extern SmvChannelTemplate* g_SmvChannelTemplate;

#define DSM_PCAP_LOAD_PARAM_OPEN_SUCCESS        10
#define OPEN_SUCCESS        0x01
#define ANALYSIS_DONE       0x02
#define ANALYSIS_FAILED     0x04
#define ANALYSIS_CANCEL     0x08
#define EXIT_ANALYSIS       0x20
#define EXIT_CANCEL         0x21


#define DSM_MSGDLG_RCTEXT	CRect(40, 100, 270, 200)
#if !SHENG_ZE
#define DSM_MSGDLG_RCTEXTT	CRect(40, 100, 280, 210)   // 弹出框文字显示的位置
#endif

DWORD WINAPI DSMAnalysisFrameListTask(WPARAM /* wParam */, LPARAM /* lParam */, void* arg)
{
    if(arg)
    {
        CDsmDataLayer* pMgr = (CDsmDataLayer*)arg;
        pMgr->StopPcapFileStatisticsAnalysis();
    }

    return 0;
}

DWORD WINAPI DSMWaveAnalysisTask(WPARAM /* wParam */, LPARAM /* lParam */, void* arg)
{
    if(arg)
    {
        CDsmDataLayer* pMgr = (CDsmDataLayer*)arg;
        pMgr->StopPcapFileWaveAnalysis();
    }

    return 0;
}



// 各业务需要更新UI数据的bool值索引 m_arrUpdate
#define DSM_PCAP_UPDATEUI_LIST          0           ///< 报文帧列表
#define DSM_PCAP_UPDATEUI_STATI         1           ///< 报文统计
#define DSM_PCAP_UPDATEUI_WAVE          2           ///< 波形分析

#if SU_FEI
#define DSM_PCAP_UPDATEUI_END           3           ///< 尾部
#else
#define	DSM_PCAP_UPDATEUI_HISTOGRAM		3			///< 报文离散度	
#define DSM_PCAP_UPDATEUI_END           4           ///< 尾部
#endif


#if CHUN_HUA
#define DSM_PCAPBROWSER_HEAD_INDEX              70
#define DSM_PCAPBROWSER_HEAD_TIME               100
#define DSM_PCAPBROWSER_HEAD_DESC               260
#define DSM_PCAPBROWSER_HEAD_TYPE               110
#define DSM_PCAPBROWSER_HEAD_LEN                60
#else
#define DSM_PCAPBROWSER_HEAD_INDEX              45
#define DSM_PCAPBROWSER_HEAD_TIME               100
#define DSM_PCAPBROWSER_HEAD_DESC               224
#define DSM_PCAPBROWSER_HEAD_TYPE               100
#define DSM_PCAPBROWSER_HEAD_LEN                60

#endif

#define WM_DSM_PCAPBROWSER_INITLOADPCAP         PRIVATE_WND_USERID + 500
#define WM_DSM_PCAPBROWSER_UPDATE_WAVE          PRIVATE_WND_USERID + 501

//////////////////////////////////////////////////////////////////////////
// smv statistics
//
#if SU_FEI
#define DSM_PCAP_STATISTICS_HEAD_SUB         120
#define DSM_PCAP_STATISTICS_HEAD_VALUE       100
#else
#define DSM_PCAP_STATISTICS_HEAD_SUB         180
#define DSM_PCAP_STATISTICS_HEAD_VALUE       349
#endif
#define STATISTICS_PILLARS          6           ///< 柱形个数

#define DSM_PCAP_STATISTICS_RC_TOP           CRect(0, 0, 640, ELT_T1_HDRHEIGHT)
#if SU_FEI
#define DSM_PCAP_STATISTICS_RC_TOP1          CRect(0, 0, 240, ELT_T1_HDRHEIGHT)
#define DSM_PCAP_STATISTICS_RC_TOP2          CRect(261, 0, 640, ELT_T1_HDRHEIGHT)
#else
#define DSM_PCAP_STATISTICS_RC_TOP1			 CRect(0, 0, 640 - 110, ELT_T1_HDRHEIGHT)
#define DSM_PCAP_STATISTICS_RC_TOP2          CRect(0, 0, 640 - 110, ELT_T1_HDRHEIGHT)
#endif
#define DSM_PCAP_STATISTICS_RC_REPROT        CRect(0, 25, 240, 414)

#define DSM_PCAP_STATISTICS_CR_UN            RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_0             RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_1             RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_2_10          RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_10_50         RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_50_250        RGB(0x26, 0xC1, 0x97)
#define DSM_PCAP_STATISTICS_CR_250           RGB(0x26, 0xC1, 0x97)


//////////////////////////////////////////////////////////////////////////
// wave
#define DSM_PCAP_WAVEFORM_MAX_TIMES      10         ///< 最大波形放大倍数
#define DSM_PCAP_WAVEFORM_MIN_TIMES      -10
#define DSM_PCAP_TIMELINE_MAX_TIMES      10         ///< 最大时间轴放大倍数
#define DSM_PCAP_TIMELINE_MIN_TIMES      -10

#define WAVEFORM_DISP_FONT_SZIE         18          ///< 波形图内字体大小

#define DSM_WFG_FONT_COLOR              RGB(128,128,135)      ///< 暖灰色


/**
 *      SMV 报文统计
 *  \note
 *      子标题、值
 */
class CDsmPcapSMVStatisticsRecord : public CELRecord
{
public:
    CDsmPcapSMVStatisticsRecord(CString csSubCaption)
    {
        AddItem(new CDsmRecordItem(csSubCaption));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmPcapSMVStatisticsRecord() {}
};


//////////////////////////////////////////////////////////////////////////
// goose statistics
//
#define DSM_PCAP_GS_STATISTICS_HEAD_SUB       160             ///< 标题栏
#define DSM_PCAP_GS_STATISTICS_HEAD_VALUE     104             ///< 值栏

#define DSM_PCAP_GS_STATISTICS_RC_TOP         CRect(0, 0, 240, 25)


/**
 *      goose报文统计
 *
 *  \note
 *      子标题、值
 */
class CDsmPcapGSStatisticsRecord : public CELRecord
{
public:
    CDsmPcapGSStatisticsRecord(const CString csSubCaption1, const CString csSubCaption2)
    {
        AddItem(new CDsmRecordItem(csSubCaption1));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(csSubCaption2));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmPcapGSStatisticsRecord() {}
};


/**
 *      帧摘要信息列表
 *
 *  \note
 *      索引、时间差、描述、长度
 */
class CDsmPackeInfoRecord : public CELRecord
{
public:
    CDsmPackeInfoRecord(size_t nIndexT, size_t nIndex, PCTSTR lpszTime, LPCTSTR lpszDesc, LPCTSTR lpszType, int nLen);
    virtual ~CDsmPackeInfoRecord() {}


public:

    /**
     *      返回记录关联的帧索引（报文的索引值）
     */
    inline size_t GetIndexT()
    {
        return m_nPacketIndex;
    }

    /**
     *      返回记录关联帧索引（控制块内的索引）
     */
    inline size_t GetIndex()
    {
        return m_nCtrlBlkIndex;
    }

    /**
     *      返回记录关联的帧描述
     */
    inline const CString& GetDesc()
    {
        return m_csDesc;
    }

    /**
     *      判断当前帧记录是否有异常信息
     */
    inline bool IsException()
    {
        return m_csDesc.IsEmpty() ? false : true;
    }


private:
    size_t          m_nCtrlBlkIndex;        ///< 帧索引（控制块）
    size_t          m_nPacketIndex;         ///< 帧索引（报文）
    CString         m_csDesc;               ///< 帧异常描述信息
};

CDsmPackeInfoRecord::CDsmPackeInfoRecord( size_t nIndexT, size_t nIndex, PCTSTR lpszTime, LPCTSTR lpszDesc,  LPCTSTR lpszType, int nLen )
{
    AddItem(new CDsmRecordItem(nIndex));
    AddItem(new CDsmRecordItem(lpszTime));
    AddItem(new CDsmRecordItem(lpszDesc));
    AddItem(new CDsmRecordItem(lpszType));
    AddItem(new CDsmRecordItem(nLen));

    m_nPacketIndex  = nIndexT;              ///< 帧索引（报文）
    m_nCtrlBlkIndex = nIndex;               ///< 帧索引（控制块）
    m_csDesc        = lpszDesc;             ///< 帧描述
}


// CPcapBrowserDlg 对话框

IMPLEMENT_DYNAMIC(CPcapBrowserDlg, DSM_BASEMODLG)

CPcapBrowserDlg::CPcapBrowserDlg(const CString csFile /* = L"" */, CWnd* pParent /*=NULL*/)
	: DSM_BASEMODLG(CPcapBrowserDlg::IDD, L"", pParent)
    , m_csPcapFile(csFile)
    , m_nMaxRows(0)
    , m_pPcap(NULL)
    , m_pFrameBuff(NULL)
    , m_pMsg92(NULL)
    , m_pMsg91(NULL)
    , m_pMsgGuoWang(NULL)
    , m_pMsgNanRui(NULL)
    , m_pMsgGoose(NULL)
    , m_pNaviBar(NULL)
    , m_eModePage(MODE_FRAME_LIST)
    , m_pRNaviCurrent(NULL)
    , m_pMsgDlg(NULL)
    , m_pDataLayer(NULL)
    , m_nCurrentAppID(0)
    , m_nCtrlBlkCmdEnd(ID_MENU_PCAP_CTRLBLK_BEGIN)
    , m_nCtrlBlkHighLight(0)
    , m_eCuttentType(FRAME_TYPE_UNKOWN)
    , m_nDetailIndex(0)
    , m_eModeWave(MODE_WAVE_WATCH)
    , m_pChannelTemplate(NULL)
    , m_hThread(NULL)
    , m_nCurrentChanns(0)
    , m_pWaveDlg(NULL)
    , m_eLastMode(MODE_FRAME_LIST)

#if SU_FEI
    , m_bHasExceptionFrame(false)

    , m_dwDirectTag(0)
    , m_nLastExcep(0)
    , m_nFirstExcep(0)
#endif

#if !CHUN_HUA
	, m_nSelectFrame(0)
	, m_nSelectFrameIndex(0)
#endif
{
    m_nIDMenu    = IDR_MENU_PCAP;
    m_pDataLayer = CDsmDataLayer::getInstance();
#if SU_FEI
    for(int i = 0; i < 3; i++)
#else
	for(int i = 0; i < 4; i++)
#endif
    {
        m_arrUpdate[i] = true;
    }
}

CPcapBrowserDlg::~CPcapBrowserDlg()
{
    m_brBackground.DeleteObject();

    m_pDataLayer->QuitPcapFileAnalysis();

    ::CloseHandle(m_hThread);

    ReleaseSav91MsgImpl(m_pMsg91);
    ReleaseSav92MsgImpl(m_pMsg92);
    ReleaseNanRuiFT3MsgImpl(m_pMsgNanRui);
    ReleaseGuoWangFT3MsgImpl(m_pMsgGuoWang);
    ReleaseGooseMsgImpl(m_pMsgGoose);

    if(m_pPcap)
    {
        m_pPcap->Close();
        ReleasePcapFileImpl(m_pPcap);
        m_pPcap = NULL;
    }

    if(m_pFrameBuff)
    {
        delete []m_pFrameBuff;
        m_pFrameBuff = NULL;
    }
}

void CPcapBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEMODLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPcapBrowserDlg, DSM_BASEMODLG)
    ON_WM_PAINT()
    ON_NOTIFY(ELNM_OKCLICK, IDC_PCAPBROWSER_REPORT, &CPcapBrowserDlg::OnItemSelFrame)
    ON_MESSAGE(WM_DSM_PCAPBROWSER_INITLOADPCAP, &CPcapBrowserDlg::OnInitLoadPcap)
    ON_COMMAND_RANGE(ID_MENU_PCAP_F1, ID_MENU_PCAP_F4, &CPcapBrowserDlg::OnF2TF4)
#if SU_FEI
    ON_COMMAND_RANGE(ID_MENU_PCAP_F5, ID_MENU_PCAP_F6, &CPcapBrowserDlg::OnNextPreviousException)
#else
	ON_COMMAND(ID_MENU_PCAP_F6, &CPcapBrowserDlg::OnMenuF6)
#endif
    ON_MESSAGE(WM_PCAPFILE_ANALYSIS_MESSAGE, &CPcapBrowserDlg::OnLoadPcapComplete)
    ON_MESSAGE(WM_DSM_PCAPBROWSER_UPDATE_WAVE, &CPcapBrowserDlg::OnUpdateWave)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
#if !SU_FEI
	ON_COMMAND(ID_MENU_PCAP_F5, &CPcapBrowserDlg::OnMenuF5)
#endif

#if !CHUN_HUA
	ON_COMMAND_RANGE(ID_MENU_PCAP_FRAME_DETAIL_F1, ID_MENU_PCAP_FRAME_DETAIL_F4, &CPcapBrowserDlg::OnDetailF1F2)
#endif
	
END_MESSAGE_MAP()


// CPcapBrowserDlg 消息处理程序

BOOL CPcapBrowserDlg::OnInitDialog()
{
    DSM_BASEMODLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_brBackground.CreateSolidBrush(DSM_COLOR_BK);

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
    ASSERT(m_pNaviBar);

    _createReport();

    m_pFrameBuff = new uint8_t[NET_MAX_FRAME_LEN];

    // 初始加载pcap包
    PostMessage(WM_DSM_PCAPBROWSER_INITLOADPCAP, 0, 0);

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CPcapBrowserDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    dc.FillSolidRect(rcClient, DSM_COLOR_BK);

    // 离散度模式下，需要绘制柱状图的标题
#if SU_FEI
    if(m_eModePage == MODE_FRAME_STATISTICS)
#else
	if (m_eModePage == MODE_FRAME_HISTOGRAM)
#endif
    {
        CRect rcTop2 = m_rcTop;
#if SU_FEI
        rcTop2.DeflateRect(DSM_MARGIN_IN, 0, 0, 0);
#else
		rcTop2.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
#endif
        int nOriDC = dc.SaveDC();
        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(ELT_T1_HDRTEXTCOLOR);
        dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
        dc.FillSolidRect(m_rcTop, ELT_T1_HDRBKCOLOR);

        CString csSub2;
        csSub2 = dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SUB2_CAPTION) + m_csSampleInter;
        dc.DrawText(csSub2, rcTop2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        dc.RestoreDC(nOriDC);
    }
}

/**
 *      创建报文帧报表
 */
void CPcapBrowserDlg::_createReport()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(DSM_MARGIN_IN_RECT);
    m_nMaxRows = (rcClient.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT + 1;
    rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * m_nMaxRows;
    m_reportFrame.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_PCAPBROWSER_REPORT);

    // 设置字体
    m_reportFrame.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportFrame.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportFrame.PostInitTemplateStyle();
#if SHENG_ZE
    // 插入列元素 
    m_reportFrame.InsertColumn(0, L"", EL_CENTER, DSM_PCAPBROWSER_HEAD_INDEX);
    m_reportFrame.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_TIME), EL_CENTER, DSM_PCAPBROWSER_HEAD_TIME);
    m_reportFrame.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_DESC), EL_CENTER, DSM_PCAPBROWSER_HEAD_DESC);
    m_reportFrame.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_TYPE), EL_CENTER, DSM_PCAPBROWSER_HEAD_TYPE);
    m_reportFrame.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_LEN), EL_CENTER, DSM_PCAPBROWSER_HEAD_LEN);
#else
	// 插入列元素 
	m_reportFrame.InsertColumn(0, L"", EL_LEFT, DSM_PCAPBROWSER_HEAD_INDEX);
	m_reportFrame.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_TIME), EL_LEFT, DSM_PCAPBROWSER_HEAD_TIME);
	m_reportFrame.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_DESC), EL_LEFT, DSM_PCAPBROWSER_HEAD_DESC);
	m_reportFrame.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_TYPE), EL_LEFT, DSM_PCAPBROWSER_HEAD_TYPE);
	m_reportFrame.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_HEAD_LEN), EL_LEFT, DSM_PCAPBROWSER_HEAD_LEN);
#endif
    // 设置选择焦点样式
    m_reportFrame.SetRowSelectionVisible(TRUE);
    m_reportFrame.SetRowFocusEnable(TRUE);
    m_reportFrame.SetFocus();
}

struct ArgLoadPcap
{
    HWND            hwnd;
    IPcapFile*      pPcap;
    std::wstring    sFile;
    DWORD*          pLoad;
};


DWORD WINAPI DSMPcapLoadTask(void* arg)
{
    if(arg)
    {
        ArgLoadPcap* pArg = (ArgLoadPcap*)arg;

        int nRet = DSM_PCAP_LOAD_PARAM_OPEN_SUCCESS;

        // 打开pcap文件
        if(pArg->pPcap->Open(pArg->sFile, PFM_READ) != PCAP_FILE_SUCCESS)
        {
            nRet = MSG_PCAPFILE_OPEN_FAILED;
            RETAILMSG(1, (L"dsm: load pcap file failed.\n"));
        }

        if(IsWindow(pArg->hwnd))
        {
            ::PostMessage(pArg->hwnd, WM_PCAPFILE_ANALYSIS_MESSAGE, nRet, 0);
        }
    }

    return 1;
}

/**
 *      加载指定pcap文件
 */
bool CPcapBrowserDlg::_loadPcap()
{
    if(m_csPcapFile.IsEmpty())
    {
        return false;
    }

    if(!m_pPcap)
    {
        m_pPcap = CreatePcapFileImpl();
    }
    else
    {
        m_pPcap->Close();
    }

    m_dwLoadComplete = 0;
    std::wstring sfile((LPCTSTR)m_csPcapFile);
    ArgLoadPcap arg = {this->GetSafeHwnd(), m_pPcap, sfile, &m_dwLoadComplete};

    m_hThread = CreateThread(NULL, 0, DSMPcapLoadTask, &arg, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
    // 显示分析pcap文件状态
    m_pMsgDlg = new CMsgDlg(DSM_STR_MSG_DECODE_PCAP_TIPS, 0, this);

    m_pDataLayer->SetPcapFilePath(sfile);
    m_pDataLayer->SetPcapFileNotifyWnd(this);
    m_pDataLayer->StartPcapFileStatisticsAnalysis();

    std::vector<HBITMAP> v;
    for(int i = 0; i < 12; i++)
    {
        v.push_back(dsm_bmpmgr::Get(DSM_BMP_LOADING_1 + i));
    }
    m_pMsgDlg->_SetBitmap(v);
   // m_pMsgDlg->_SetPicRC(DSM_PROGRESS_BAR_RCBITMAP);
   // m_pMsgDlg->_SetTextRC(DSM_PROGRESS_BAR_RCTEXT);
	m_pMsgDlg->_SetTextRC(DSM_MSGDLG_RCTEXT);
    m_pMsgDlg->_SetMenu(IDR_MENU_ANALYSIS_PCAP);
    m_pMsgDlg->_GetCtl()->_SetTimeInterval(80);
    m_pMsgDlg->_SetCommandRange(ID_MENU_ANALYSIS_PCAP_F1, ID_MENU_ANALYSIS_PCAP_F1, DSMAnalysisFrameListTask, (void*)m_pDataLayer);
#if !SHENG_ZE
	m_pMsgDlg->_ResetF1Text(_T("取消"));
#endif
    HWND hFocus = ::GetFocus();
    INT_PTR ret = m_pMsgDlg->DoModal();
    ::SetFocus(hFocus);
    delete m_pMsgDlg;
    m_pMsgDlg = NULL;

    // 重置上一次加载的状态
    _clearStatus();

    CString csTips;
    CString csFile = m_csPcapFile.Right(m_csPcapFile.GetLength() - m_csPcapFile.ReverseFind(L'\\') - 1);

    // 加载pcap成功
    if(ret == IDOK)
    {
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_PCAP_SUCCESS), csFile);

        // 加载pcap中的控制块数据
        _initCtrlBlk();
#if SU_FEI
        // 统计分析pcap后的状态信息
        _statisticsLoadedPcap();
#endif
        _updateMenuNCaption();

        // 显示分析成功后的数据
        _updateUI();
    }
    // 加载pcap失败
    else if(ret == EXIT_ANALYSIS)
    {
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_PCAP_FAILED), csFile);
        PostMessage(WM_KEYDOWN, VK_ESCAPE, 0);
        CStatusMgr::SetTips(csTips);
        return false;
    }
    // 用户取消分析
    else if(ret == EXIT_CANCEL)
    {
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_PCAP_USER_CANCEL));
        PostMessage(WM_KEYDOWN, VK_ESCAPE, 0);
        CStatusMgr::SetTips(csTips);
        return false;
    }

    CStatusMgr::SetTips(csTips);
    return true;
}

LRESULT CPcapBrowserDlg::OnInitLoadPcap( WPARAM, LPARAM )
{
    _loadPcap();

    return 1;
}

void CPcapBrowserDlg::OnItemSelFrame( NMHDR* /* pNotifyStruct */, LRESULT *pResult )
{
    int nSelRow = m_reportFrame.GetFocusedRowIndex();

#if !CHUN_HUA
	m_nSelectFrame = nSelRow;
#endif

    if(nSelRow >= 0) 
    {
        *pResult = 1;

        CDsmPackeInfoRecord* pRecord = dynamic_cast<CDsmPackeInfoRecord*>(m_reportFrame.GetRecordRow(nSelRow));
        m_nDetailIndex = pRecord->GetIndex();
        _showFrameDetail(pRecord->GetIndexT());

#if !CHUN_HUA
		m_nSelectFrameIndex = pRecord->GetIndexT();
#endif

    }
    else
    {
        *pResult = 0;
    }
}

/**
 *      显示报文帧信息
 */
void CPcapBrowserDlg::_showFrameDetail( size_t nIndex )
{
    enumFrameType eType = FRAME_TYPE_UNKOWN;

    if(!_getFrameBuff(nIndex, eType))
    {
        return;
    }

    _switchToPage(MODE_FRAME_DETAIL);

	_udateFrameDetail(eType);

}

/**
 *      根据指定帧索引获取帧数据
 */
bool CPcapBrowserDlg::_getFrameBuff( size_t nIndex, enumFrameType& rType )
{
    PCAP_PACKET_HEADER pckhead;
    MsgRapidDecoder rapidDecoder;

    if(nIndex > m_pPcap->GetPckNum())
    {
        return false;
    }

    // 定位索引
    m_pPcap->SeekPckToRead(nIndex);

    // 获取帧头信息
    if(m_pPcap->ReadOnePckHeader(pckhead) != PCAP_FILE_SUCCESS)
    {
        DSM_DEBUG(L"dsm: read pck head data failed.\n");
        return false;
    }

    if(pckhead.capLen > NET_MAX_FRAME_LEN)
    {
        return false;
    }

    // 获取帧数据
    if(PCAP_FILE_SUCCESS != m_pPcap->ReadOnePckData(m_pFrameBuff, pckhead.capLen > NET_MAX_FRAME_LEN ? NET_MAX_FRAME_LEN : pckhead.capLen))
    {
        DSM_DEBUG(L"dsm: read pck data failed.\n");
        return false;
    }

    // 获取帧类型
    rType = rapidDecoder.GetMessageType(m_pFrameBuff, NET_MAX_FRAME_LEN);

    return true;
}

/**
 *      创建帧信息树控件
 */
void CPcapBrowserDlg::_createFrameTree()
{
    CRect rcTree;
    GetClientRect(rcTree);
    rcTree.DeflateRect(DSM_MARGIN_IN_RECT);

    int nTreeRows     = (rcTree.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcTree.bottom   = rcTree.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * nTreeRows;

    m_treeFrame.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcTree, this, IDC_SMV_MSGMONITOR_TREE);

    // 设置树样式
    m_treeFrame.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.PostInitTemplateStyle();

    m_treeFrame.InsertTreeColumn(0, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ITEM), EL_LEFT, 280);
    m_treeFrame.InsertTreeColumn(1, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VALUE), EL_LEFT, 320);

    m_treeFrame.SetFocus();
}


BOOL CPcapBrowserDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    // 传给导航菜单处理消息
    if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            if(m_eModePage == MODE_FRAME_DETAIL)
            {
                _switchToPage(MODE_FRAME_LIST);

                return TRUE;
            }
            break;

        default:
            break;
        }

        if(m_eModePage == MODE_FRAME_LIST)
        {
            if(_preTranslateMessageList(pMsg))
            {
                return TRUE;
            }
        }

        if(m_eModePage == MODE_FRAME_WAVE)
        {
            if(_preTranslateMessageWave(pMsg))
            {
                return TRUE;
            }
        }
    }

    return DSM_BASEMODLG::PreTranslateMessage(pMsg);
}

/**
 *      切换到某个页面状态时，进行菜单与标题的修改
 */
void CPcapBrowserDlg::_switchToPage( ModePage mode )
{
    PCAP_CTRLBLK ctrl;
    if(mode == m_eModePage || !_getCtrlBlkByAppID(m_nCurrentAppID, ctrl))
    {
        return;
    }

    m_eLastMode = m_eModePage;
    m_eModePage = mode;

    if(m_pRNaviCurrent)
    {
        delete m_pRNaviCurrent;
        m_pRNaviCurrent = NULL;
    }

    _updateMenuNCaption();

    switch(m_eModePage)
    {
    case MODE_FRAME_LIST:
        _switchFrameList();
        break;

    case MODE_FRAME_DETAIL:
        _switchFrameDetail();
        break;

    case MODE_FRAME_STATISTICS:
        _switchFrameStatistics();
        break;
#if !SU_FEI
	case MODE_FRAME_HISTOGRAM:
		
		_switchFrameHistogram();		
			
		break;
#endif
    case MODE_FRAME_WAVE:
        _switchFrameWave();
        break;

    default:
        break;
    }
}

/**
 *      处理报文列表翻页操作
 */
BOOL CPcapBrowserDlg::_preTranslateMessageList( MSG* pMsg )
{
    if(!m_pPcap || m_pPcap->GetPckNum() <= 0 || m_reportFrame.GetRecordRowCount() <= 0)
    {
        return FALSE;
    }

    int nFocusIndex = m_reportFrame.GetFocusedRowIndex();

    CDsmPackeInfoRecord* pRecord = dynamic_cast<CDsmPackeInfoRecord*>(m_reportFrame.GetRecordRow(m_reportFrame.GetFocusedRowIndex()));
    if(!pRecord)
    {
        m_reportFrame.SetFocus();
        return FALSE;
    }

    // 获取当前控制块数据
    PCAP_CTRLBLK* ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, &ctrl) || !ctrl->data)
    {
        return FALSE;
    }

    switch(pMsg->wParam)
    {
        // 上下滚动
    case VK_UP:
    case VK_DOWN:
        // 向上翻页
        if(nFocusIndex == 0 && int(ctrl->first_visibility_index - m_nMaxRows) >= 0 && VK_UP == pMsg->wParam)
        {
            ctrl->first_visibility_index -= m_nMaxRows;
            _updateUIFrameList();

            if(m_reportFrame.GetRecordRowCount() > 0)
            {
                m_reportFrame.SetFocusedSelectionRowIndex(m_reportFrame.GetRecordRowCount() - 1);
            }
#if SU_FEI
            _updateNaviExcep();
#endif
            return TRUE;
        }

        // 向下翻页
        else if(nFocusIndex == m_nMaxRows - 1 && ctrl->first_visibility_index + m_nMaxRows < ctrl->data->m_vecAppidFrameIndexMap.size() && VK_DOWN == pMsg->wParam)
        {
            ctrl->first_visibility_index += m_nMaxRows;
            _updateUIFrameList();

            if(m_reportFrame.GetRecordRowCount() > 0)
            {
                m_reportFrame.SetFocusedSelectionRowIndex(0);
            }
#if SU_FEI
            _updateNaviExcep();
#endif
            return TRUE;
        }
#if SU_FEI
        m_dwDirectTag = ((pMsg->wParam == VK_UP) ? 1 : 2);

        _updateNaviExcep();
#endif
        break;

        // 左右翻页
    case VK_LEFT:
    case VK_RIGHT:
        // 向上翻页
        if((int)(ctrl->first_visibility_index - m_nMaxRows) >= 0 && VK_LEFT == pMsg->wParam)
        {
            ctrl->first_visibility_index -= m_nMaxRows;
            _updateUIFrameList();

            if(nFocusIndex >= 0 && nFocusIndex < m_reportFrame.GetRecordRowCount())
            {
                m_reportFrame.SetFocusedSelectionRowIndex(nFocusIndex);
            }
#if SU_FEI
            _updateNaviExcep();
#endif
            return TRUE;
        }
        else if(ctrl->data->m_vecAppidFrameIndexMap.size() > size_t(ctrl->first_visibility_index + m_nMaxRows) && VK_RIGHT == pMsg->wParam)
        {
            ctrl->first_visibility_index += m_nMaxRows;
            _updateUIFrameList();

            if(nFocusIndex >= 0 && nFocusIndex < m_reportFrame.GetRecordRowCount())
            {
                m_reportFrame.SetFocusedSelectionRowIndex(nFocusIndex);
            }
            else if(m_reportFrame.GetRecordRowCount() > 0)
            {
                m_reportFrame.SetFocusedSelectionRowIndex(0);
            }
#if SU_FEI
            _updateNaviExcep();
#endif
            return TRUE;
        }

        break;

    default:
        break;
    }

    return FALSE;
}
#if SU_FEI
void CPcapBrowserDlg::_updateNaviExcep()
{
    if(m_bHasExceptionFrame && _hasPrevException())
    {
        CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_EXCEPTION_PREV));
    }
    else
    {
        CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(4, L"");
    }

    if(m_bHasExceptionFrame && _hasNextException())
    {
        CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_EXCEPTION_NEXT));
    }
    else
    {
        CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(5, L"");
    }

    m_dwDirectTag = 0;

    CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
}
#endif

/**
 *      处理报文波形分析的上下左右操作
 */
BOOL CPcapBrowserDlg::_preTranslateMessageWave( MSG* pMsg )
{
    PCAP_CTRLBLK* data;

    if(_getCtrlBlkByAppID(m_nCurrentAppID, &data))
    {
        switch(pMsg->wParam)
        {
        case VK_UP:
            if(m_eModeWave == MODE_WAVE_WATCH)
            {
                _pageupChannel(data);
            }
            else
            {
                _zoominWave(data);
            }

            break;

        case VK_DOWN:
            if(m_eModeWave == MODE_WAVE_WATCH)
            {
                _pagedownChannel(data);
            }
            else
            {
                _zoomoutWave(data);
            }

            break;

        case VK_LEFT:
            if(m_eModeWave == MODE_WAVE_WATCH)
            {
                _pageleftChannel(data);
            }
            else
            {
                _zoomoutTime(data);
            }

            break;

        case VK_RIGHT:
            if(m_eModeWave == MODE_WAVE_WATCH)
            {
                _pagerightChannel(data);
            }
            else
            {
                _zoominTime(data);
            }
            break;

        default:
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    _updateUIWave();

    return TRUE;
}


LRESULT CPcapBrowserDlg::OnLoadPcapComplete( WPARAM wParam, LPARAM )
{
    if(m_eModePage == MODE_FRAME_LIST)
    {
        switch(wParam)
        {
            // 打开pcap成功
        case DSM_PCAP_LOAD_PARAM_OPEN_SUCCESS:
            m_dwLoadComplete |= OPEN_SUCCESS;
            break;

            // 分析pcap完成
        case MSG_PCAPFILE_STATUS_ANALYSIS_DONE:
            m_dwLoadComplete |= ANALYSIS_DONE;
            break;

            // 打开pcap失败
        case MSG_PCAPFILE_OPEN_FAILED:
        case MSG_PCAPFILE_ANALYSIS_FAILED:
            m_dwLoadComplete |= ANALYSIS_FAILED;
            break;

            // 用户取消分析
        case MSG_PCAPFILE_ANALYSIS_USER_CACEL:
            m_dwLoadComplete |= ANALYSIS_CANCEL;
            break;

        default:
            m_dwLoadComplete |= ANALYSIS_FAILED;
            break;
        }

        int nStatus = m_dwLoadComplete & 0xF;

        // 大于2，会判断OPEN_SUCCESS | ANALYSIS_DONE的成功组合或者其他失败标志
        if(nStatus > 0x02)
        {
            int nRet = EXIT_ANALYSIS;

            if((nStatus & ANALYSIS_DONE) == ANALYSIS_DONE)
            {
                nRet = IDOK;
            }
            else if((nStatus & ANALYSIS_CANCEL) == ANALYSIS_CANCEL)
            {
                nRet = EXIT_CANCEL;
            }

            if(m_pMsgDlg && IsWindow(m_pMsgDlg->GetSafeHwnd()))
            {
                m_pMsgDlg->EndDialog(nRet);
            }
        }
    }
    else if(m_eModePage == MODE_FRAME_WAVE)
    {
        int nRet = IDCANCEL;

        // 波形分析成功
        if(wParam == MSG_PCAPFILE_WAVE_ANALYSIS_DONE)
        {
            PostMessage(WM_DSM_PCAPBROWSER_UPDATE_WAVE, 0, 0);
            nRet = IDOK;
        }

        if(m_pWaveDlg)
        {
            m_pWaveDlg->EndDialog(nRet);
        }
    }

    return 1;
}

/**
 *      清理旧状态数据
 */
void CPcapBrowserDlg::_clearStatus()
{
    // 清理报表
    m_reportFrame.DeleteAllRecordRow();
}


/**
 *      初始化报文中的控制块信息
 */
bool CPcapBrowserDlg::_initCtrlBlk()
{
    // 加载控制块信息
    if(!_loadCtrlBlk())
    {
        return false;
    }

    // 为界面F1生成控制块选择菜单
    _initCtrlBlkMenu();

    return true;
}

/**
 *      加载报文中的控制块信息
 */
bool CPcapBrowserDlg::_loadCtrlBlk()
{
    uint32_t nCount = m_pDataLayer->GetPcapFileAppidCount();
    if(nCount <= 0)
    {
        return false;
    }

    m_vecCtrlBlk.clear();

    for(uint32_t i = 0; i < nCount; i++)
    {
        uint32_t id = m_pDataLayer->GetPcapFileOneAppid(i);

        PCAP_CTRLBLK ctrlblk = 
        {
            i, 
            id, 
            m_pDataLayer->GetPcapFileAppidFrameCount(ctrlblk.app_id), 
            0, 
            0, 
            m_pDataLayer->GetPcapFileAppidFrameType(id),
            m_pDataLayer->GetPcapFileAppidSampleMapping(id),
            ID_MENU_PCAP_CTRLBLK_BEGIN,
            {
                0, // wfirst
                0, // zoomx
                0, // zoomy
                m_pDataLayer->GetPcapFileAppidSampleRate(ctrlblk.app_id), 
                0, // begin
            }
        };

        m_vecCtrlBlk.push_back(ctrlblk);
    }

    return true;
}

/**
 *  生成控制块菜单
 */
void CPcapBrowserDlg::_initCtrlBlkMenu()
{
    CMenu  menu;
    VERIFY(menu.CreatePopupMenu());

    m_nCtrlBlkCmdEnd = ID_MENU_PCAP_CTRLBLK_BEGIN;
    CString csDesc;

    for(vecCtrlBlk::iterator it = m_vecCtrlBlk.begin(); it != m_vecCtrlBlk.end(); it++)
    {
        if(it == m_vecCtrlBlk.begin())
        {
            m_nCurrentAppID     = it->app_id;
            m_nCtrlBlkHighLight = m_nCtrlBlkCmdEnd;
            m_eCuttentType      = it->type;
        }

        it->cmdid = m_nCtrlBlkCmdEnd;
        csDesc.Format(L"AppID [0x%04X]", it->app_id);
        menu.AppendMenu(MF_STRING, m_nCtrlBlkCmdEnd++, csDesc);
    }

    CNaviMenu* pSub = new CNaviMenu;
    pSub->LoadMenu(&menu);
    menu.DestroyMenu();

    CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuSubItem(0, pSub);
    CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_CTRLBLK_DESC));
	CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
    CNaviBarMgr::GetInstance()->HighlightMenuItem(0, 0);
}

BOOL CPcapBrowserDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= ID_MENU_PCAP_CTRLBLK_BEGIN && nID < m_nCtrlBlkCmdEnd)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return DSM_BASEMODLG::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CPcapBrowserDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= ID_MENU_PCAP_CTRLBLK_BEGIN && (UINT)nMenuID < m_nCtrlBlkCmdEnd)
    {
        _switchCtrlBlk(nMenuID);

        return TRUE;
    }

    return DSM_BASEMODLG::OnCommand(wParam, lParam);
}

/**
 *     处理切换控制块消息
 */
void CPcapBrowserDlg::_switchCtrlBlk( UINT nCmdID )
{
    if(m_nCtrlBlkHighLight != nCmdID)
    {
        // 查找选中的控制块
        for(vecCtrlBlk::iterator it = m_vecCtrlBlk.begin(); it != m_vecCtrlBlk.end(); it++)
        {
            if(nCmdID == it->cmdid)
            {
                // 设置新的高亮菜单
                CNaviBarMgr::GetInstance()->HighlightMenuItem(0, m_nCtrlBlkHighLight - ID_MENU_PCAP_CTRLBLK_BEGIN, FALSE);
                CNaviBarMgr::GetInstance()->HighlightMenuItem(0, nCmdID - ID_MENU_PCAP_CTRLBLK_BEGIN);

                // 保存之前控制的信息
                PCAP_CTRLBLK* ctrl;
                if(_getCtrlBlkByAppID(m_nCurrentAppID, &ctrl))
                {
                    ctrl->focus = m_reportFrame.GetFocusedRowIndex();
                }

                m_nCurrentAppID     = it->app_id;
                m_nCtrlBlkHighLight = it->cmdid;
                m_eCuttentType      = it->type;
#if SU_FEI
                if(m_eModePage == MODE_FRAME_STATISTICS)
                {
#else
				if (m_eModePage == MODE_FRAME_HISTOGRAM)
				{	
#endif
                    _resetHistogram();          // 重置柱状图标题s
                }

                // 控制块切换后将设置数据无效
                for(int i = 0; i < DSM_PCAP_UPDATEUI_END; i++)
                {
                    m_arrUpdate[i] = true;
                }

                // 刷新切换控制块后UI显示
                _updateUI();
            }
        }
    }
}

/**
 *      刷新UI数据
 */
void CPcapBrowserDlg::_updateUI()
{
    // 根据当前页面的模式刷新不同的UI界面
    switch(m_eModePage)
    {
    case MODE_FRAME_LIST:
        {
            _updateUIFrameList();

            // 设置焦点行
            PCAP_CTRLBLK newctrl;
            int focus = -1;
            if(_getCtrlBlkByAppID(m_nCurrentAppID, newctrl) && newctrl.focus >= 0 && newctrl.focus < m_reportFrame.GetFocusedRowIndex())
            {
                focus = newctrl.focus;
            }

            if(focus < 0 && m_reportFrame.GetRecordRowCount() > 0)
            {
                focus = 0;
            }

            if(focus >= 0)
            {
                m_reportFrame.SetFocusedSelectionRowIndex(focus);
            }
#if SU_FEI
            _updateNaviExcep();
#endif
        }

        break;

    case MODE_FRAME_STATISTICS:
        {
            _updateUIStatistics();
        }

        break;
#if !SU_FEI
		///< 在离散度页面时，刷新对应数据
	case MODE_FRAME_HISTOGRAM:
		{
			_updateUIStatistics();
		}
		break;
#endif
    case MODE_FRAME_WAVE:
        {
            // 设置波形控件参数
            _setWaveform();

            _updateUIWave();
        }
        break;

    default:
        break;
    }
}

/**
 *      刷新报文帧列表界面
 */
void CPcapBrowserDlg::_updateUIFrameList()
{
    PCAP_CTRLBLK ctrlblk;

    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrlblk) || !ctrlblk.data || ctrlblk.first_visibility_index >= ctrlblk.data->m_vecAppidFrameIndexMap.size())
    {
        return;
    }

    FrameIndexInfo pre, cur = ctrlblk.data->m_vecAppidFrameIndexMap[ctrlblk.first_visibility_index];
    CString csType(_getFrameTypeDesc(ctrlblk.type));
    pre = cur;

    if(ctrlblk.first_visibility_index > 0)
    {
        pre = ctrlblk.data->m_vecAppidFrameIndexMap[ctrlblk.first_visibility_index - 1];
    }

    m_reportFrame.DeleteAllRecordRow();
    size_t maxz = ctrlblk.data->m_vecAppidFrameIndexMap.size();

    for(int i = 1; i <= m_nMaxRows; i++)
    {
        m_reportFrame.InsertRecordRow(i - 1, new CDsmPackeInfoRecord(
            cur.nPcapFileFrameIndex, 
            ctrlblk.first_visibility_index + i, 
            DSM_GetDValue(cur.stTimeStamp.ts_sec, 
            cur.stTimeStamp.ts_usec, 
            pre.stTimeStamp.ts_sec, 
            pre.stTimeStamp.ts_usec, 6), 
            _getFrameDesc(ctrlblk.type, cur.nPcapFileFrameIndex), 
            csType, 
            cur.nFramelen
            ));

        if(ctrlblk.first_visibility_index + i >= maxz)
        {
            break;
        }

        pre = cur;
        cur = ctrlblk.data->m_vecAppidFrameIndexMap[ctrlblk.first_visibility_index + i];
    }

    m_arrUpdate[DSM_PCAP_UPDATEUI_LIST] = false;

    _updateMenuNCaption();
}


/**
 *      刷新离散度统计界面
 */
void CPcapBrowserDlg::_updateUIStatistics()
{
    switch(m_eCuttentType)
    {
    case FRAME_TYPE_NET_9_2:
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_FT3_GUOWANG:
    case FRAME_TYPE_FT3_NANRUI:

        // 刷新sv报文统计界面(其中包括有离散度页面的刷新)
        _updateUIStatisticsSV();
        break;

    case FRAME_TYPE_NET_GOOSE:
        // 刷新goose报文统计

        _updateUIStatisticsGS();
        break;

    default:
        ASSERT(FALSE);
        break;
    }
}


/**
 *      刷新sv报文统计界面(包括有离散度界面)
 */
void CPcapBrowserDlg::_updateUIStatisticsSV()
{

    CString csTotalCount(g_csBlankValue);           // 报文总计数
    CString csLostCount(g_csBlankValue);            // 丢点计数
    CString csErrorCount(g_csBlankValue);           // 错序计数
    CString csJitterCount(g_csBlankValue);          // 抖动计数
    CString csRetry(g_csBlankValue);                // 重发计数
    CString csLostSync(g_csBlankValue);             // 失步计数
    CString csQuality(g_csBlankValue);              // 品质无效计数
    CString csTest(g_csBlankValue);                 // 检修计数

    float fdata[STATISTICS_PILLARS] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    CDataFrameStatistics data;

    if(m_pDataLayer->GetPcapFileSmvStatisticsResult(&data))
    {
#if SU_FEI
        // 设置报表数据
        csTotalCount.Format(L"%u", data.GetTotalFrameCount());
        csLostCount.Format(L"%u", data.GetLostFrameCount());
        csErrorCount.Format(L"%u", data.GetMisOrderFrameCount());
        csRetry.Format(L"%u", data.GetRetryFrameCount());
        csLostSync.Format(L"%u", data.GetLostSyncFrameCount());
        csTest.Format(L"%u", data.GetTestFrameCount());
        csJitterCount.Format(L"%u", data.GetJitterFrameCount());


        // 刷新报文离散度柱状图
        for ( int i = 0; i < STATISTICS_PILLARS; i++ )
        {
            fdata[i] = data.GetDispPercent(MSG_DISP_PERIOD(MSG_DISP_PERIOD_0 + i));
        }
#else
		if (m_reportSMVStatistics.GetSafeHwnd())
		{
			csTotalCount.Format(L"%u", data.GetTotalFrameCount());
			csLostCount.Format(L"%u", data.GetLostFrameCount());
			csErrorCount.Format(L"%u", data.GetMisOrderFrameCount());
			csRetry.Format(L"%u", data.GetRetryFrameCount());
			csLostSync.Format(L"%u", data.GetLostSyncFrameCount());
			csTest.Format(L"%u", data.GetTestFrameCount());
			csJitterCount.Format(L"%u", data.GetJitterFrameCount());
		}

		if (m_ctrlHistogram.GetSafeHwnd())
		{
			// 刷新报文离散度柱状图
			for ( int i = 0; i < STATISTICS_PILLARS; i++ )
			{
				fdata[i] = data.GetDispPercent(MSG_DISP_PERIOD(MSG_DISP_PERIOD_0 + i));
			}
		}


#endif
    }
	
#if SU_FEI
    m_reportSMVStatistics.SetRowItemText(0, 1, csTotalCount);
    m_reportSMVStatistics.SetRowItemText(1, 1, csLostCount);
    m_reportSMVStatistics.SetRowItemText(2, 1, csErrorCount);
    m_reportSMVStatistics.SetRowItemText(3, 1, csRetry);
    m_reportSMVStatistics.SetRowItemText(4, 1, csLostSync);
    m_reportSMVStatistics.SetRowItemText(5, 1, csTest);
    m_reportSMVStatistics.SetRowItemText(6, 1, csJitterCount);

    if(m_eCuttentType == FRAME_TYPE_NET_9_2)
    {
        csQuality.Format(L"%u", data.GetQualityInvalidFrameCount());
        m_reportSMVStatistics.SetRowItemText(7, 1, csQuality);
    }

    m_ctrlHistogram.ErasePillar();
    m_ctrlHistogram.DrawHorHistoGram(fdata,sizeof(fdata)/sizeof(float), _T("%"));

    m_ctrlHistogram.Invalidate();

    m_arrUpdate[DSM_PCAP_UPDATEUI_STATI] = false;
#else
	if (m_reportSMVStatistics.GetSafeHwnd())
	{
		m_reportSMVStatistics.SetRowItemText(0, 1, csTotalCount);
		m_reportSMVStatistics.SetRowItemText(1, 1, csLostCount);
		m_reportSMVStatistics.SetRowItemText(2, 1, csErrorCount);
		m_reportSMVStatistics.SetRowItemText(3, 1, csRetry);
		m_reportSMVStatistics.SetRowItemText(4, 1, csLostSync);
		m_reportSMVStatistics.SetRowItemText(5, 1, csTest);
		m_reportSMVStatistics.SetRowItemText(6, 1, csJitterCount);
		m_arrUpdate[DSM_PCAP_UPDATEUI_STATI] = false;
	}
	
	if (m_ctrlHistogram.GetSafeHwnd())
	{
		m_ctrlHistogram.ErasePillar();
		m_ctrlHistogram.DrawHorHistoGram(fdata,sizeof(fdata)/sizeof(float), _T("%"));

		m_ctrlHistogram.Invalidate();
		m_arrUpdate[DSM_PCAP_UPDATEUI_HISTOGRAM] = false;

	}

	
#endif
}

/**
 *      刷新gs报文统计页面
 */
void CPcapBrowserDlg::_updateUIStatisticsGS()
{
    CString     csTotal(g_csBlankValue);              ///< 报文总计数
    CString     csLostFrame(g_csBlankValue);          ///< 丢帧计数
    CString     csMisOrder(g_csBlankValue);           ///< 错序计数
    CString     csRetry(g_csBlankValue);              ///< 重发计数
    CString     csStLost(g_csBlankValue);             ///< StNum丢帧计数
    CString     csStMis(g_csBlankValue);              ///< StNum错序计数
    CString     csLostData(g_csBlankValue);           ///< 报文丢失计数
    CString     csCommBreak(g_csBlankValue);          ///< 通信中断计数
    CString     csInvalidTTL(g_csBlankValue);         ///< 存活时间无效计数
    CString     csTimeNotSync(g_csBlankValue);        ///< 时钟未同步计数
    CString     csTimeFault(g_csBlankValue);          ///< 时钟故障计数

    CGooseFrameStatistics data;
    if(m_pDataLayer->GetPcapFileGooseStatisticsResult(&data))
    {
        csTotal.Format(L"%u", data.GetTotalFrameCounts());
        csLostFrame.Format(L"%u", data.GetLostFrameCounts());
        csMisOrder.Format(L"%u", data.GetMisOrderFrameCount());
        csRetry.Format(L"%u", data.GetRetryFrameCount());
        csStLost.Format(L"%u", data.GetStLostFrameCounts());
        csStMis.Format(L"%u", data.GetStMisOrderFrameCount());
        csLostData.Format(L"%u", data.GetLostDataGramCount());
        csCommBreak.Format(L"%u", data.GetCommBreakCount());
        csInvalidTTL.Format(L"%u", data.GetInvalidTTLCount());
        csTimeNotSync.Format(L"%u", data.GetTimeQualityNotSyncCount());
        csTimeFault.Format(L"%u", data.GetTimeQualityFaultCount());
    }

    m_reportGooseStatistics.SetRowItemText(0, 1, csTotal);
    m_reportGooseStatistics.SetRowItemText(1, 1, csLostFrame);
    m_reportGooseStatistics.SetRowItemText(2, 1, csMisOrder);
    m_reportGooseStatistics.SetRowItemText(3, 1, csRetry);
    m_reportGooseStatistics.SetRowItemText(4, 1, csStLost);
    m_reportGooseStatistics.SetRowItemText(5, 1, csStMis);

    m_reportGooseStatistics.SetRowItemText(0, 3, csLostData);
    m_reportGooseStatistics.SetRowItemText(1, 3, csCommBreak);
    m_reportGooseStatistics.SetRowItemText(2, 3, csInvalidTTL);
    m_reportGooseStatistics.SetRowItemText(3, 3, csTimeNotSync);
    m_reportGooseStatistics.SetRowItemText(4, 3, csTimeFault);

    m_arrUpdate[DSM_PCAP_UPDATEUI_STATI] = false;
}

/**
 *      刷新波形分析界面
 */
void CPcapBrowserDlg::_updateUIWave()
{
    // 刷新数据
    CString csEffect(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_EFFECT));   // 获取有效值描述
    //CString csAngle(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_ANGLE));     // 获取相位角描述

    CString csValue;
    PCAP_CTRLBLK ctrl;
    COscillogramChannelInfo* pInfo = NULL;

    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl))
    {
        return;
    }

    // 擦除波形
    m_ctrlWaveform.EraseWave();

    // 填充波形数据
    for(uint32_t nGram = 0, i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++, nGram++)
    {
        pInfo = m_dataWave.GetOneChannelInfo(ctrl.wave.wfirst_visibility_index + i);

        if(pInfo)
        {
            //csValue.Format(L"%s: %0.3f\r\n%s: %0.3f°", csEffect, pInfo->m_fEffectiveValue, csAngle, pInfo->m_fPhase);
            csValue.Format(L"%s: %0.3f", csEffect, pInfo->m_fEffectiveValue);

            // 把通道的波形信息设置到波形控件
            m_ctrlWaveform.DrawWave(nGram, &pInfo->m_vecChannelData[ctrl.wave.wbegin], pInfo->m_vecChannelData.size() - ctrl.wave.wbegin, csValue);
        }
    }

    m_ctrlWaveform.Invalidate(TRUE);
}


/**
 *      获取指定AppID的控制块数据
 */
bool CPcapBrowserDlg::_getCtrlBlkByAppID( uint32_t appid, PCAP_CTRLBLK** data )
{
    for(vecCtrlBlk::iterator it = m_vecCtrlBlk.begin(); it != m_vecCtrlBlk.end(); it++)
    {
        if(appid == it->app_id)
        {
            *data = &(*it);

            return true;
        }
    }

    return false;
}

/**
 *      获取指定AppID的控制块数据
 */
bool CPcapBrowserDlg::_getCtrlBlkByAppID( uint32_t appid, PCAP_CTRLBLK& data )
{
    for(vecCtrlBlk::iterator it = m_vecCtrlBlk.begin(); it != m_vecCtrlBlk.end(); it++)
    {
        if(appid == it->app_id)
        {
            data = *it;

            return true;
        }
    }

    return false;
}


/**
 *      切换到报文帧列表
 */
void CPcapBrowserDlg::_switchFrameList()
{
    _hideAllControl();

    m_reportFrame.ShowWindow(SW_SHOW);
    m_reportFrame.SetFocus();

    if(m_arrUpdate[DSM_PCAP_UPDATEUI_LIST])
    {
        _updateUIFrameList();
    }
#if SU_FEI
    _updateNaviExcep();
#endif
}

/**
 *      切换到详细帧报文
 */
void CPcapBrowserDlg::_switchFrameDetail()
{
#if CHUN_HUA
	if(!m_pRNaviCurrent)
	{
		m_pRNaviCurrent = new CRNaviMenu(this, IDR_MENU_NULL);
	}
#else
    if(!m_pRNaviCurrent)
    {
        m_pRNaviCurrent = new CRNaviMenu(this, ID_MENU_PCAP_FRAME_DETAIL);
    }
#endif
    if(!IsWindow(m_treeFrame.GetSafeHwnd()))
    {
        _createFrameTree();
    }

    if(m_reportFrame.GetSafeHwnd())
    {
        m_reportFrame.ShowWindow(SW_HIDE);
    }

    m_treeFrame.ShowWindow(SW_SHOW);
    m_treeFrame.SetFocus();
}

/**
 *      切换到异常统计
 */
void CPcapBrowserDlg::_switchFrameStatistics()
{
    // 切换页面显示的控件
    _hideAllControl();

    // 显示报文统计界面
    switch(m_eCuttentType)
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            _showSMVStatistics();

            if(m_arrUpdate[DSM_PCAP_UPDATEUI_STATI])
            {
#if	SU_FEI
                _resetHistogram();          // 重置柱状图标题
#endif
                _updateUI();
            }
        }
        break;

    case FRAME_TYPE_NET_GOOSE:
        _showGOOSEStatistics();

        if(m_arrUpdate[DSM_PCAP_UPDATEUI_STATI])
        {
            _updateUI();
        }
        break;

    default:
        ASSERT(FALSE);
        break;
    }
}


#if !SU_FEI
void CPcapBrowserDlg::_switchFrameHistogram()
{
	// 切换页面显示的控件
	_hideAllControl();

	// 显示离散度界面
	switch(m_eCuttentType)
	{
	case FRAME_TYPE_NET_9_1:
	case FRAME_TYPE_NET_9_2:
	case FRAME_TYPE_FT3_NANRUI:
	case FRAME_TYPE_FT3_GUOWANG:
		{
			_showSMVRistorgram();

			if(m_arrUpdate[DSM_PCAP_UPDATEUI_HISTOGRAM])
			{

				_resetHistogram();          // 重置柱状图标题
				_updateUI();
			}
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}
#endif
/**
 *      切换报文波形分析
 */
void CPcapBrowserDlg::_switchFrameWave()
{
    // 切换页面显示的控件
    _hideAllControl();

    if(!IsWindow(m_ctrlWaveform.GetSafeHwnd()))
    {
        _createWaveform();
    }

    m_ctrlWaveform.ShowWindow(SW_SHOW);
    m_ctrlWaveform.SetFocus();

    PCAP_CTRLBLK ctrl;

    // 需要重新分析波形数据
    if(m_arrUpdate[DSM_PCAP_UPDATEUI_WAVE] && _getCtrlBlkByAppID(m_nCurrentAppID, ctrl))
    {
        if(!m_pWaveDlg)
        {
#if SHENG_ZE
            m_pWaveDlg = new CProgressBarDlg(this);
            m_pWaveDlg->_SetTips(dsm_stringMgr::Get(DSM_STR_INFO_PCAP_ANALYSIS_WAVE));
            m_pWaveDlg->_SetMenu(IDR_MENU_ANALYSIS_PCAP);
            m_pWaveDlg->_SetCommandRange(ID_MENU_ANALYSIS_PCAP_F6, ID_MENU_ANALYSIS_PCAP_F6, DSMWaveAnalysisTask, (void*)m_pDataLayer);
#else
			m_pWaveDlg = new CMsgDlg(DSM_STR_INFO_PCAP_ANALYSIS_WAVE, 0, this);
			std::vector<HBITMAP> v;
			for(int i = 0; i < 12; i++)
			{
				v.push_back(dsm_bmpmgr::Get(DSM_BMP_LOADING_1 + i));
			}
			m_pWaveDlg->_SetBitmap(v);
			m_pWaveDlg->_SetTextRC(DSM_MSGDLG_RCTEXTT);
			m_pWaveDlg->_SetMenu(IDR_MENU_ANALYSIS_PCAP);
			m_pWaveDlg->_GetCtl()->_SetTimeInterval(80);
			m_pWaveDlg->_SetCommandRange(ID_MENU_ANALYSIS_PCAP_F1, ID_MENU_ANALYSIS_PCAP_F1, DSMWaveAnalysisTask, (void*)m_pDataLayer);
			m_pWaveDlg->_ResetF1Text(_T("取消"));
#endif
        }

        m_pDataLayer->SetPcapFileNotifyWnd(this);
        m_pDataLayer->SetPcapFileWaveAnalysisIndex(0, m_pDataLayer->GetPcapFileAppidSampleCount(m_nCurrentAppID) - 1);
        m_pDataLayer->StartPcapFileWaveAnalysis();
#if SHENG_ZE
        INT_PTR ret = m_pWaveDlg->DoModal();

        delete m_pWaveDlg;
        m_pWaveDlg = NULL;
#else
		HWND hFocus = ::GetFocus();
		INT_PTR ret = m_pWaveDlg->DoModal();
		::SetFocus(hFocus);
		delete m_pWaveDlg;
		m_pWaveDlg = NULL;
#endif
        if(ret == IDOK)
        {
            m_arrUpdate[DSM_PCAP_UPDATEUI_WAVE] = false;

            if(IsWindow(m_ctrlWaveform.GetSafeHwnd()))
            {
                m_ctrlWaveform.SetFocus();
            }
        }
        else
        {
            _switchToPage(m_eLastMode);
        }
    }
}

/**
 *      获取帧类型描述
 */
CString CPcapBrowserDlg::_getFrameTypeDesc( enumFrameType type )
{
    CString csType;

    switch(type)
    {
    case FRAME_TYPE_NET_9_1:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAV91);
        break;

    case FRAME_TYPE_NET_9_2:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_SAV92);
        break;

    case FRAME_TYPE_FT3_NANRUI:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FT3_NANRUI);
        break;

    case FRAME_TYPE_FT3_GUOWANG:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_FT3_GUOWANG);
        break;

    case FRAME_TYPE_NET_GOOSE:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_GOOSE);
        break;

    default:
        csType = dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_UNKNOWN);
        RETAILMSG(1, (L"dsm: pcf type not support.\n"));
    }

    return csType;
}

/**
 *      获取每一帧异常的描述
 */
CString CPcapBrowserDlg::_getFrameStatisticsDesc( enumFrameType type, uint16_t status )
{
    CString csDesc;

    // smv
    if(type == FRAME_TYPE_NET_9_1 || type == FRAME_TYPE_NET_9_2 || type == FRAME_TYPE_FT3_GUOWANG || type == FRAME_TYPE_FT3_NANRUI)
    {
        if(status & FRAME_STATUS_SMP_LOST)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_LOST);
        }

        if(status & FRAME_STATUS_SMP_MIS_ORDER)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MIS_ORDER);
        }

        if(status & FRAME_STATUS_SMP_RETRY)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_RETRY);
        }

        if(status & FRAME_STATUS_SMP_LOST_SYNC)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_LOST_SYNC);
        }

        if(status & FRAME_STATUS_SMP_Q_INVALID)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_Q_INVALID);
        }

        if(status & FRAME_STATUS_SMP_TEST)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_TEST);
        }
    }
    // goose
    else if(type == FRAME_TYPE_NET_GOOSE)
    {
        if(status & FRAME_STATUS_GOOSE_SQ_LOST)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_SQ_LOST);
        }

        if(status & FRAME_STATUS_GOOSE_SQ_MIS_ORDER)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_SQ_MIS_ORDER);
        }

        if(status & FRAME_STATUS_GOOSE_SQ_RETRY)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_SQ_RETRY);
        }

        if(status & FRAME_STATUS_GOOSE_ST_LOST)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_ST_LOST);
        }

        if(status & FRAME_STATUS_GOOSE_ST_MIS_ORDER)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_ST_MIS_ORDER);
        }

        if(status & FRAME_STATUS_GOOSE_FRAME_LOST)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_FRAME_LOST);
        }

        if(status & FRAME_STATUS_GOOSE_COMM_BREAK)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_COMM_BREAK);
        }

        if(status & FRAME_STATUS_GOOSE_TTL_INVALID)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_TTL_INVALID);
        }

        if(status & FRAME_STATUS_GOOSE_CLK_NOT_SYNC)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CLK_NOT_SYNC);
        }

        if(status & FRAME_STATUS_GOOSE_CLK_FAILED)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CLK_FAILED);
        }
    }
    // unknown
    else
    {
        if(status != 0)
        {
            csDesc += L", " + dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_UNKNOWN_EXCEPTION);
        }
    }

    return csDesc.Right(csDesc.GetLength() - 2);
}

/**
 *      获取帧描述信息
 */
CString CPcapBrowserDlg::_getFrameDesc( enumFrameType type, uint32_t index )
{
    FrameStatus status = m_pDataLayer->GetPcapFileFrameIndexStatus(index);
    CString csStatistics = _getFrameStatisticsDesc(type, status.GetValue());
    CString csDesc;

    if(!csStatistics.IsEmpty() && status.vecSmpCnt.size() > 0)
    {
        csDesc = L" smpCnt=";
        CString csSmp;
        std::vector<uint32_t>::iterator it      = status.vecSmpCnt.begin();
        std::vector<uint32_t>::iterator itEnd   = status.vecSmpCnt.end();

        for(; it != itEnd; it++)
        {
            csSmp.Format(L"%u, ", *it);
            csDesc += csSmp;
        }

        csDesc = csDesc.Left(csDesc.GetLength() - 2);
    }

    if(!csDesc.IsEmpty())
    {
        //csDesc += csStatistics;
        csDesc = csStatistics + csDesc;
    }

    return csDesc;
}

void CPcapBrowserDlg::OnF2TF4( UINT nID )
{
    switch(nID)
    {
    case ID_MENU_PCAP_F2:
        // 报文列表
        _switchToPage(MODE_FRAME_LIST);
        break;

        // 进入报文统计
    case ID_MENU_PCAP_F3:
		
        if(m_eCuttentType != FRAME_TYPE_UNKOWN)
        {
            _switchToPage(MODE_FRAME_STATISTICS);
        }

        break;

        // 进入波形分析
    case ID_MENU_PCAP_F4:
        if(m_eCuttentType != FRAME_TYPE_NET_GOOSE && m_eCuttentType != FRAME_TYPE_UNKOWN)
        {
            _switchToPage(MODE_FRAME_WAVE);
        }

        break;

    default:
        break;
    }
}



#if !SU_FEI
void CPcapBrowserDlg::OnMenuF5()
{
	if(m_eCuttentType != FRAME_TYPE_NET_GOOSE)
	{
		_switchToPage(MODE_FRAME_HISTOGRAM);
	}	
}

void CPcapBrowserDlg::OnMenuF6()
{
	if (m_eModePage == MODE_FRAME_WAVE)
	{
		m_eModeWave = (m_eModeWave == MODE_WAVE_WATCH) ? MODE_WAVE_ZOOM : MODE_WAVE_WATCH;
		_updateMenuNCaption();
	}
}
#endif
#if SU_FEI
void CPcapBrowserDlg::OnNextPreviousException( UINT nID )
{
    // F6处理波形缩放观察
    if(m_eModePage == MODE_FRAME_WAVE && nID == ID_MENU_PCAP_F6)
    {
        m_eModeWave = m_eModeWave == MODE_WAVE_WATCH ? MODE_WAVE_ZOOM : MODE_WAVE_WATCH;
        _updateMenuNCaption();
        return;
    }


    if(m_eModePage != MODE_FRAME_LIST)
    {
        return;
    }

    int focus = 0;
    size_t first = 0;

    // 查找上一个异常信息
    if(nID == ID_MENU_PCAP_F5 && !_lookupPreviousException(first, focus))
    {
        return;
    }
    else if(nID == ID_MENU_PCAP_F6 && !_lookupNextException(first, focus))
    {
        return;
    }

    // 设置新界面数据参数
    PCAP_CTRLBLK* ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, &ctrl))
    {
        return;
    }

    ctrl->first_visibility_index = first;

    // 刷新界面显示
    _updateUIFrameList();

    if(focus >= 0 && focus < m_reportFrame.GetRecordRowCount())
    {
        m_reportFrame.SetFocusedSelectionRowIndex(focus);
    }

    _updateNaviExcep();


}
#endif

/**
 *      查找上一个异常帧的位置
 */
#if SU_FEI
bool CPcapBrowserDlg::_lookupPreviousException( size_t& first, int& focus )
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl) || !ctrl.data)
    {
        return false;
    }

    // 确定开始查找的索引
    size_t end = (unsigned int)(0 - 1);
    size_t begin = ctrl.first_visibility_index + m_reportFrame.GetFocusedRowIndex();

    // m_dwDirectTag等于1或者2时，说明当前focus的行是旧的（在pretranslatemessage时）
    // 需要调整begin的值
    if(m_dwDirectTag == 1)
    {
        if(begin > 0)
        {
            begin--;
        }
    }
    else if(m_dwDirectTag == 2)
    {
        if(begin < end)
        {
            begin++;
        }
    }

    if(begin == 0)
    {
        return false;
    }
    else
    {
        begin--;
    }

    for(size_t i = begin; i != end; i--)
    {
        // 找到上一个异常的帧信息
        if(m_pDataLayer->GetPcapFileFrameIndexStatus(ctrl.data->m_vecAppidFrameIndexMap[i].nPcapFileFrameIndex).GetValue() != 0)
        {
            first = i / m_nMaxRows * m_nMaxRows;
            focus = i % m_nMaxRows;

            return true;
        }
    }

    return false;
}

/**
 *      查找下一个异常帧的位置
 */
bool CPcapBrowserDlg::_lookupNextException( size_t& first, int& focus )
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl) || !ctrl.data)
    {
        return false;
    }

    // 确定开始查找的索引
    size_t begin = ctrl.first_visibility_index + m_reportFrame.GetFocusedRowIndex();
    size_t end   = ctrl.data->m_vecAppidFrameIndexMap.size();

    // m_dwDirectTag等于1或者2时，说明当前focus的行是旧的（在pretranslatemessage时）
    // 需要调整begin的值
    if(m_dwDirectTag == 1)          // vk_keyup
    {
        if(begin > 0)
        {
            begin--;
        }
    }
    else if(m_dwDirectTag == 2)     // vk_keydown
    {
        if(begin < end)
        {
            begin++;
        }
    }

    // 从下一个索引开始找
    if(begin < end)
    {
        begin++;
    }

    for(size_t i = begin; i < end; i++)
    {
        // // 找到下一个异常的帧信息
        if(m_pDataLayer->GetPcapFileFrameIndexStatus(ctrl.data->m_vecAppidFrameIndexMap[i].nPcapFileFrameIndex).GetValue() != 0)
        {
            first = i / m_nMaxRows * m_nMaxRows;
            focus = i % m_nMaxRows;

            return true;
        }
    }

    return false;
}
#endif
/**
 *      隐藏所有界面控件
 */
void CPcapBrowserDlg::_hideAllControl()
{
    if(::IsWindowVisible(m_treeFrame.GetSafeHwnd()))
    {
        m_treeFrame.ShowWindow(SW_HIDE);
    }

    if(::IsWindowVisible(m_reportFrame.GetSafeHwnd()))
    {
        m_reportFrame.ShowWindow(SW_HIDE);
    }

    if(::IsWindowVisible(m_reportGooseStatistics.GetSafeHwnd()))
    {
        m_reportGooseStatistics.ShowWindow(SW_HIDE);
    }

    if(::IsWindowVisible(m_reportSMVStatistics.GetSafeHwnd()))
    {
        m_reportSMVStatistics.ShowWindow(SW_HIDE);
    }


    if(::IsWindowVisible(m_ctrlHistogram.GetSafeHwnd()))
    {
        m_ctrlHistogram.ShowWindow(SW_HIDE);
    }

    if(::IsWindowVisible(m_ctrlWaveform.GetSafeHwnd()))
    {
        m_ctrlWaveform.ShowWindow(SW_HIDE);
        Invalidate(TRUE);
    }
}

/**
 *      创建报文统计报表
 */
void CPcapBrowserDlg::_createReportSMVStatistics()
{
    CRect rcClient;
    GetClientRect(rcClient);

    CRect rcTop(DSM_PCAP_STATISTICS_RC_TOP1);
    rcTop.OffsetRect(0, DSM_MARGIN_IN);
#if SU_FEI
    rcTop.DeflateRect(DSM_MARGIN_IN, 0, 0, 0);
#else
	rcTop.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
#endif
    // 减1是隐藏报表的头线，与自绘标题重合
    rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN , rcClient.right - rcTop.right, DSM_MARGIN_IN);

    m_reportSMVStatistics.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_PCAP_STATISTICS_SMVREPORT);

    // 设置字体
    m_reportSMVStatistics.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportSMVStatistics.PostInitTemplateStyle();

    // 插入列元素
    m_reportSMVStatistics.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SUB1_CAPTION), EL_LEFT, DSM_PCAP_STATISTICS_HEAD_SUB);
    m_reportSMVStatistics.InsertColumn(1, L"", EL_LEFT, DSM_PCAP_STATISTICS_HEAD_VALUE);

    // 设置选择焦点样式
    m_reportSMVStatistics.SetRowSelectionVisible(FALSE);
    m_reportSMVStatistics.SetRowFocusEnable(FALSE);

    m_reportSMVStatistics.InsertRecordRow(0, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_TOTAL)));
    m_reportSMVStatistics.InsertRecordRow(1, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_LOSE)));
    m_reportSMVStatistics.InsertRecordRow(2, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_ERROR)));
    m_reportSMVStatistics.InsertRecordRow(3, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_RETRY)));
    m_reportSMVStatistics.InsertRecordRow(4, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_LOSTSYNC)));
    m_reportSMVStatistics.InsertRecordRow(5, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_TEST)));
    m_reportSMVStatistics.InsertRecordRow(6, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SHAKE)));

    if(m_eCuttentType == FRAME_TYPE_NET_9_2)
    {
        m_reportSMVStatistics.InsertRecordRow(7, new CDsmPcapSMVStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_QUALITY)));
    }

    m_reportSMVStatistics.SetFocus();
}

/**
 *      创建GOOSE报文统计报表
 */
void CPcapBrowserDlg::_createReportGSStatistics()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    m_reportGooseStatistics.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_STATISTICS_REPORT);

    // 设置字体
    m_reportGooseStatistics.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportGooseStatistics.PostInitTemplateStyle();

    // 插入列元素
    m_reportGooseStatistics.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_CAP), EL_LEFT, DSM_PCAP_GS_STATISTICS_HEAD_SUB);          // 子标题
    m_reportGooseStatistics.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_VAL), EL_LEFT, DSM_PCAP_GS_STATISTICS_HEAD_VALUE);        // 值
    m_reportGooseStatistics.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_CAP), EL_LEFT, DSM_PCAP_GS_STATISTICS_HEAD_SUB);          // 子标题
    m_reportGooseStatistics.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_VAL), EL_LEFT, DSM_PCAP_GS_STATISTICS_HEAD_VALUE);        // 值

    // 设置选择焦点样式
    m_reportGooseStatistics.SetRowSelectionVisible(FALSE);
    m_reportGooseStatistics.SetRowFocusEnable(FALSE);

    m_reportGooseStatistics.InsertRecordRow(0, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TOTAL), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_LOSTDATA)));
    m_reportGooseStatistics.InsertRecordRow(1, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_LOSTFRAME), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_COMMBREAK)));
    m_reportGooseStatistics.InsertRecordRow(2, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_MISORDER), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_INVALIDTTL)));
    m_reportGooseStatistics.InsertRecordRow(3, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_RETRY), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TIMENOTSYNC)));
    m_reportGooseStatistics.InsertRecordRow(4, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_STLOST), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TIMEFAULT)));
    m_reportGooseStatistics.InsertRecordRow(5, new CDsmPcapGSStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_STMIS), L""));

    m_reportGooseStatistics.SetFocus();
}

/**
 *      创建离散度柱状图
 */

void CPcapBrowserDlg::_createHorHistogram()
{
    CRect rcClient;
    GetClientRect(rcClient);

    m_rcTop = DSM_PCAP_STATISTICS_RC_TOP2;
    m_rcTop.OffsetRect(0, DSM_MARGIN_IN);
    m_rcTop.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);

    rcClient.DeflateRect(m_rcTop.left, m_rcTop.bottom, DSM_MARGIN_IN, DSM_MARGIN_IN);

    m_ctrlHistogram.Create(L"", WS_CHILD | WS_VISIBLE, rcClient, this, IDC_PCAP_STATISTICS_SMVHIS);
    m_ctrlHistogram.ModifyStyle(0, SS_OWNERDRAW);
    m_ctrlHistogram.SetBkgndColor(DSM_COLOR_BK);

    CFont* pFont = dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT);
    m_ctrlHistogram.SetTitleFont(pFont);
    m_ctrlHistogram.SetScaleValFont(pFont);
    m_ctrlHistogram.SetPillarValFont(pFont);

    // 柱状图颜色
    COLORREF crPillar[STATISTICS_PILLARS] = {
        DSM_PCAP_STATISTICS_CR_0, 
        DSM_PCAP_STATISTICS_CR_1, 
        DSM_PCAP_STATISTICS_CR_2_10, 
        DSM_PCAP_STATISTICS_CR_10_50, 
        DSM_PCAP_STATISTICS_CR_50_250, 
        DSM_PCAP_STATISTICS_CR_250, 
    };

    // 设置柱状图个数
    m_ctrlHistogram.SetPillarCount(STATISTICS_PILLARS);

    // 设置柱状图颜色和标题
    for(int i = 0; i < STATISTICS_PILLARS; i++)
    {
        m_ctrlHistogram.SetPillarColor(i, crPillar[i]);
    }

    m_ctrlHistogram.SetPillarValColor(RGB(0xFF, 0xFF, 0xFF));
    m_ctrlHistogram.SetScaleValColor(RGB(0xFF, 0xFF, 0xFF));
    m_ctrlHistogram.SetPillarAreaBorderColor(ELT_T1_HDRGRIDCOLOR);
    m_ctrlHistogram.SetSpaceFromLeftBorderToPillar(10);

    // 设置10个网格，每个长度10
    m_ctrlHistogram.SetPillarAreaGrid(10, 0);
    m_ctrlHistogram.SetXScaleInterval(10.0);

    m_ctrlHistogram.SetPillarBetween(10);           ///< 柱状图间距
    m_ctrlHistogram.SetPillarHeight(30);            ///< 柱状图宽度
    m_ctrlHistogram.SetMargin(0, 78, 0, 0);         ///< 控件间距
    m_ctrlHistogram.SetYScaleValLen(55);            ///< 左侧标题宽度

    m_ctrlHistogram.SetTitleVisible(TRUE);
    m_ctrlHistogram.SetBorderVisible(0, TRUE);
    m_ctrlHistogram.SetXScaleValVisible(FALSE);
    m_ctrlHistogram.SetYScaleValVisible(TRUE);
    m_ctrlHistogram.SetVerGridVisible(FALSE);

    m_ctrlHistogram.Init();
    m_ctrlHistogram.ShowWindow(TRUE);
}

/**
 *      显示smv报文统计界面
 */
void CPcapBrowserDlg::_showSMVStatistics()
{
 
	if(!IsWindow(m_reportSMVStatistics.GetSafeHwnd()))
    {
		_createReportSMVStatistics();
#if SU_FEI
		_createHorHistogram();
#endif
    }

    m_reportSMVStatistics.ShowWindow(SW_SHOW);
    m_reportSMVStatistics.SetFocus();

#if SU_FEI
    m_ctrlHistogram.ShowWindow(SW_SHOW);
#endif
}
#if !SU_FEI
///<  显示SMV离散度页面
void CPcapBrowserDlg::_showSMVRistorgram()
{
	if(m_ctrlHistogram.GetSafeHwnd())
	{
		m_ctrlHistogram.ShowWindow(SW_SHOW);		
		m_ctrlHistogram.SetFocus();
	}
	else
	{
		_createHorHistogram();
	}
	m_ctrlHistogram.ShowWindow(SW_SHOW);		
	m_ctrlHistogram.SetFocus();
}
#endif
/**
 *      显示goose报文统计页面
 */
void CPcapBrowserDlg::_showGOOSEStatistics()
{
    if(!IsWindow(m_reportGooseStatistics.GetSafeHwnd()))
    {
        _createReportGSStatistics();
    }

    m_reportGooseStatistics.ShowWindow(SW_SHOW);
    m_reportGooseStatistics.SetFocus();
}

/**
 *      修改离散度柱状图控件参数
 */
void CPcapBrowserDlg::_resetHistogram()
{
    if(!::IsWindow(m_ctrlHistogram.GetSafeHwnd()))
    {
        return;
    }

    uint32_t asdus  = m_pDataLayer->GetPcapFileAppidNoAsdu(m_nCurrentAppID);
    uint32_t smprat = m_pDataLayer->GetPcapFileAppidSampleRate(m_nCurrentAppID);

    // 更新离散度区间标题
    CString csRegion4, csRegion5;

    csRegion4.Format(L"%uμs", asdus * 50);
    csRegion5.Format(L"%uμs", asdus * 250);

    CString csPillarTitle[STATISTICS_PILLARS] = {
        L"0μs", 
        L"1μs", 
        L"2μs", 
        L"10μs", 
        csRegion4,
        csRegion5,
    };

    uint32_t nSamInt = asdus * 1000000 / smprat;
    m_csSampleInter.Format(L" - %s(%u μs)", dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SAMPLEINTERVAL), nSamInt);

    // 设置柱状图颜色和标题
    for(int i = 0; i < STATISTICS_PILLARS; i++)
    {
        m_ctrlHistogram.SetYScaleVal(i, csPillarTitle[i]);
    }

    m_ctrlHistogram.Init();

    Invalidate();
}

HBRUSH CPcapBrowserDlg::OnCtlColor( CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor*/ )
{
//     HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
// 
//     // TODO:  在此更改 DC 的任何属性
// 
//     // TODO:  如果默认的不是所需画笔，则返回另一个画笔
//     return hbr;

    return m_brBackground;
}

/**
 *      更新不同业务对应的菜单与标题
 */
void CPcapBrowserDlg::_updateMenuNCaption()
{
    CString csCaption, csAppID;

    PCAP_CTRLBLK data;
    if(_getCtrlBlkByAppID(m_nCurrentAppID, data))
    {
        csAppID.Format(L"0x%04X", m_nCurrentAppID);
    }
    else
    {
        csAppID = L"UNKNOWN";
    }

    CNaviMenu* pNavi = CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu();

	pNavi->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
    
	if(m_eModePage == MODE_FRAME_STATISTICS)
    {
        csCaption.Format(L"%s->%s [%s]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MENU_FRAME_STATISTICS), csAppID);

        // 报文统计，隐藏『上一个』『下一个』
        pNavi->SetMenuItemBitmaps(5, NULL, NULL, NULL);
#if SU_FEI
        pNavi->SetMenuItemCaption(4, L"");
#else
		if (m_eCuttentType == FRAME_TYPE_NET_GOOSE)
		{
			pNavi->SetMenuItemCaption(4, L"");
		}
		else
		{
			pNavi->SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_PILLAR_UNSEL));
		}
		
#endif
        pNavi->SetMenuItemCaption(5, L"");
        pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_LIST_UNSEL));
        pNavi->SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_STAT_SEL));

        if(m_eCuttentType == FRAME_TYPE_NET_GOOSE)
        {
            pNavi->SetMenuItemBitmaps(3, NULL);
            pNavi->SetMenuItemCaption(3, L"");
        }
        else
        {
            pNavi->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_WA_UNSEL));
        }
        CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
    }
    else if(m_eModePage == MODE_FRAME_LIST)
    {
        // 计算当前页
        int page  = data.first_visibility_index / m_nMaxRows + 1;
        int total = data.max_frame / m_nMaxRows;
        total += (data.max_frame % m_nMaxRows > 0) ? 1 : 0;

        csCaption.Format(L"%s->%s [%s %d/%d]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MENU_FRAME_LIST), csAppID, page, total);

        // 报文列表，显示『上一个』『下一个』
        pNavi->SetMenuItemBitmaps(5, NULL, NULL, NULL);
#if	SU_FEI
        if(m_bHasExceptionFrame)
        {
            pNavi->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_EXCEPTION_PREV));
            pNavi->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_EXCEPTION_NEXT));
        }
        else
        {
            pNavi->SetMenuItemCaption(4, L"");
            pNavi->SetMenuItemCaption(5, L"");
        }
#else
		if (m_eCuttentType == FRAME_TYPE_NET_GOOSE)
		{
			pNavi->SetMenuItemCaption(4, L"");
		}
		else
		{
			pNavi->SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_PILLAR_UNSEL));
		}
#endif
        pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_LIST_SEL));
        pNavi->SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_STAT_UNSEL));

        if(m_eCuttentType == FRAME_TYPE_NET_GOOSE)
        {
            pNavi->SetMenuItemBitmaps(3, NULL);
            pNavi->SetMenuItemCaption(3, L"");
        }
        else
        {
            pNavi->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_WA_UNSEL));
        }

        CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
    }
    else if(m_eModePage == MODE_FRAME_DETAIL)
    {
        csCaption.Format(L"%s->%s [%s : %u/%u/%u]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MENU_FRAME_DETAIL), csAppID, m_nDetailIndex, data.max_frame, m_pDataLayer->GetPcapFileTotalFrameCount());
    }
    else if(m_eModePage == MODE_FRAME_WAVE)
    {
        csCaption.Format(L"%s->%s [%s]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MENU_WAVE), csAppID);
#if SU_FEI
        pNavi->SetMenuItemCaption(4, L"");
#else
		if (m_eCuttentType == FRAME_TYPE_NET_GOOSE)
		{
			pNavi->SetMenuItemCaption(4, L"");
		}
		else
		{
			pNavi->SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_PILLAR_UNSEL));
		}
#endif
        pNavi->SetMenuItemCaption(5, L"");

        // 切换波形的模式为『缩放』
        if(m_eModeWave == MODE_WAVE_ZOOM)
        {
            pNavi->SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STRETCH_SEL), NULL, NULL);
        }
        // 切换波形的模式为『选择』
        else
        {
            pNavi->SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
        }

        pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_LIST_UNSEL));
        pNavi->SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_STAT_UNSEL));

        if(m_eCuttentType == FRAME_TYPE_NET_GOOSE)
        {
            pNavi->SetMenuItemBitmaps(3, NULL);
            pNavi->SetMenuItemCaption(3, L"");
        }
        else
        {
            pNavi->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_WA_SEL));
        }

        CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
    }
#if !SU_FEI
	else if (m_eModePage == MODE_FRAME_HISTOGRAM)
	{
		CString csStr = L"离散度";
		csCaption.Format(L"%s->%s [%s]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), csStr, csAppID);

		// 报文统计，隐藏『上一个』『下一个』
		pNavi->SetMenuItemBitmaps(5, NULL, NULL, NULL);

		if (m_eCuttentType == FRAME_TYPE_NET_GOOSE)
		{
			pNavi->SetMenuItemCaption(4, L"");
			
		}
		else
		{
			pNavi->SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_PILLAR_SEL));
			
		}
		pNavi->SetMenuItemCaption(5, L"");
		pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_LIST_UNSEL));
		pNavi->SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_STAT_UNSEL));

		if(m_eCuttentType == FRAME_TYPE_NET_GOOSE)
		{
			pNavi->SetMenuItemBitmaps(3, NULL);
			pNavi->SetMenuItemCaption(3, L"");
		}
		else
		{
			pNavi->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SRA_WA_UNSEL));
		}
		CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
	}	
#endif

    CStatusMgr::SetTitle(csCaption);
}

/**
 *      创建波形控件
 */
void CPcapBrowserDlg::_createWaveform()
{
    CRect rcClient;
    GetClientRect(rcClient);
   // rcClient.DeflateRect(CRect(10, 10, 10, 10));

    // 创建波形控件
    m_ctrlWaveform.Create(NULL, WS_CHILD | WS_VISIBLE, rcClient, this, IDC_PCAP_WAVFORM_CTRL);

    // 设置波形控件属性
    m_ctrlWaveform.ModifyStyle(0, SS_OWNERDRAW | WS_TABSTOP);
    m_ctrlWaveform.SetXScaleValVisible(TRUE);
    m_ctrlWaveform.SetTitleVisible(TRUE);
    m_ctrlWaveform.SetGramCount(DSM_PCAP_WAVEFORM_GRAM_X, DSM_PCAP_WAVEFORM_GRAM_Y);
    m_ctrlWaveform.SetHorGridVisible(FALSE);
    m_ctrlWaveform.SetBkgndColor(DSM_COLOR_BK);
    m_ctrlWaveform.SetWaveAreaGrid(3, 2);
    m_ctrlWaveform.SetTitleColor(DSM_WFG_FONT_COLOR);
    m_ctrlWaveform.SetYIntervalMaxCharNum(10);

    for(int i = 0; i < 3; i++)
    {
        m_ctrlWaveform.SetWaveDescColor(i, 0, DSM_WFG_FONT_COLOR);
    }

    // 设置波形空间内需要的字体
    CFont* pFont = dsm_fontmgr::GetInstance()->GetFont(WAVEFORM_DISP_FONT_SZIE);
    m_ctrlWaveform.SetScaleValFont(pFont);
    m_ctrlWaveform.SetTitleFont(pFont);
    m_ctrlWaveform.SetWaveDescFont(pFont);

    m_ctrlWaveform.SetFocus();
}

/**
 *      设置波形控件属性
 */
void CPcapBrowserDlg::_setWaveform()
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl))
    {
        return;
    }

    // 根据类型与通道数获取一个可用的通道模板进行设置波形的通道类型
    _getTemplate(ctrl.type);

    CString csDesc;
    COLORREF color;
    uint32_t nChanIndex = ctrl.wave.wfirst_visibility_index;

    if(!m_pDataLayer->GetPcapFileWaveOscillogramData(&m_dataWave))
    {
        return;
    }

    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        // 若有通道模板，则从模板中获取信息（相别与名称）
        if(m_pChannelTemplate && (nChanIndex + i < m_pChannelTemplate->nTemplateSize))
        {
            _getChannelInfo(nChanIndex + i, color);
            csDesc.Format(L"%d: %s", nChanIndex + i + 1, m_pChannelTemplate->pChannelTempalte[nChanIndex + i].strChannelDesc.c_str());
        }
        else
        {
            color = DSM_PHASE_COLOR_UN;
            csDesc.Format(L"%d", nChanIndex + i + 1);
        }

        // 设置通道关联的波形图标题
        m_ctrlWaveform.SetTitle(i, csDesc);

        // 设置通道关联的XY轴刻度信息
        m_ctrlWaveform.SetScaleInterval(i, 
            _calcXGridCounts(&ctrl),                                                        // zoom x
            _calcAmplitude(nChanIndex + i) * (float)pow(2, ctrl.wave.wzoomy), L"");         // zoom y

        // 设置波形下方的X轴显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)_calcXGridCounts(&ctrl));

        // 设置x轴起始点描述
        m_ctrlWaveform.SetX0ScaleVal(i, (float)ctrl.wave.wbegin);

        // 把颜色信息设置到波形控件
        m_ctrlWaveform.SetWaveColor(i, 0, color);
    }

    m_ctrlWaveform.Init();
}

/**
 *      根据类型与通道数获取一个模板对象
 */
void CPcapBrowserDlg::_getTemplate( enumFrameType type )
{
    m_pChannelTemplate  = NULL;
     m_nCurrentChanns   = m_pDataLayer->GetPcapFileAppidChannelCount(m_nCurrentAppID);

    switch(type)
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        {
            // 查找一个合适的通道模板类型
            uint32_t n = sizeof(g_SmvChannelTemplate) / sizeof(g_SmvChannelTemplate[0]);
            for(uint32_t i = 0; i < n; i++)
            {
                if(g_SmvChannelTemplate[i].nTemplateSize == m_nCurrentChanns && 
                    (g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2 || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK))
                {
                    m_pChannelTemplate = &g_SmvChannelTemplate[i];
                    break;
                }
            }
        }
        break;

    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            // 查找一个合适的通道模板类型
            uint32_t n = sizeof(g_SmvChannelTemplate) / sizeof(g_SmvChannelTemplate[0]);
            for(uint32_t i = 0; i < n; i++)
            {
                if(g_SmvChannelTemplate[i].nTemplateSize == m_nCurrentChanns && 
                    (g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8 || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO || 
                    g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK))
                {
                    m_pChannelTemplate = &g_SmvChannelTemplate[i];
                    break;
                }
            }
        }
        break;

    default:
        {
            SmvChannelTemplate* pDefault = NULL;

            // 查找一个合适的通道模板类型
            uint32_t n = sizeof(g_SmvChannelTemplate) / sizeof(g_SmvChannelTemplate[0]);
            for(uint32_t i = 0; i < n; i++)
            {
                if(g_SmvChannelTemplate[i].nTemplateSize == m_nCurrentChanns)
                {
                    m_pChannelTemplate = &g_SmvChannelTemplate[i];
                }

                if(g_SmvChannelTemplate[i].enTemplateType == SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2)
                {
                    pDefault = &g_SmvChannelTemplate[i];
                }
            }

            if(!m_pChannelTemplate)
            {
                m_pChannelTemplate = pDefault;
            }
        }
        break;
    }
}

/**
 *      计算幅值
 */
float CPcapBrowserDlg::_calcAmplitude( uint32_t index )
{
    float fAmplitude = 10.0f;
    COscillogramChannelInfo* pInfo = m_dataWave.GetOneChannelInfo(index);

    if(pInfo)
    {
        fAmplitude = pInfo->m_fEffectiveValue;
    }

    return fAmplitude;
}

/**
 *      上翻页通道
 */
void CPcapBrowserDlg::_pageupChannel( PCAP_CTRLBLK* data )
{
    if(data->wave.wfirst_visibility_index == 0 || data->wave.wfirst_visibility_index - DSM_PCAP_WAVEFORM_NUMS < 0)
    {
        return;
    }

    data->wave.wfirst_visibility_index -= DSM_PCAP_WAVEFORM_NUMS;

    _setWaveform();
}


/**
 *      下翻页通道
 */
void CPcapBrowserDlg::_pagedownChannel( PCAP_CTRLBLK* data )
{
    if(data->wave.wfirst_visibility_index + DSM_PCAP_WAVEFORM_NUMS >= m_pDataLayer->GetPcapFileAppidChannelCount(data->app_id))
    {
        return;
    }

    data->wave.wfirst_visibility_index += DSM_PCAP_WAVEFORM_NUMS;

    _setWaveform();
}

/**
 *      左翻页通道
 */
void CPcapBrowserDlg::_pageleftChannel( PCAP_CTRLBLK* data )
{
    if(data->wave.wbegin == 0)
    {
        return;
    }

    uint32_t xCounts = (uint32_t)_calcXGridCounts(data);

    if(data->wave.wbegin < xCounts)
    {
        data->wave.wbegin = 0;
    }
    else
    {
        data->wave.wbegin -= xCounts;
    }

    // 设置波形控件x坐标起始点描述
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetX0ScaleVal(i, (float)data->wave.wbegin);
    }

    m_ctrlWaveform.Init();
}

/**
 *      右翻页通道
 */
void CPcapBrowserDlg::_pagerightChannel( PCAP_CTRLBLK* data )
{
    int dis = (int)_calcXGridCounts(data);

    if(data->wave.wbegin + dis >= m_pDataLayer->GetPcapFileAppidSampleCount(data->app_id))
    {
        return;
    }

    data->wave.wbegin += dis;

    // 设置波形控件x坐标起始点描述
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetX0ScaleVal(i, (float)data->wave.wbegin);
    }

    m_ctrlWaveform.Init();
}

/**
 *      放大波形
 */
void CPcapBrowserDlg::_zoominWave( PCAP_CTRLBLK* data )
{
    if(data->wave.wzoomy <= DSM_PCAP_WAVEFORM_MIN_TIMES)
    {
        return;
    }

    data->wave.wzoomy--;

    // 循环设置每个波形的刻度信息，主要是改变Y轴刻度（幅值）
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            _calcXGridCounts(data), 
            _calcAmplitude(data->wave.wfirst_visibility_index + i) * (float)pow(2, data->wave.wzoomy), 
            L"");
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      缩小波形
 */
void CPcapBrowserDlg::_zoomoutWave( PCAP_CTRLBLK* data )
{
    if(data->wave.wzoomy >= DSM_PCAP_WAVEFORM_MAX_TIMES)
    {
        return;
    }

    data->wave.wzoomy++;

    // 循环设置每个波形的刻度信息，主要是改变Y轴刻度（幅值）
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            _calcXGridCounts(data), 
            _calcAmplitude(data->wave.wfirst_visibility_index + i) * (float)pow(2, data->wave.wzoomy), 
            L"");
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      放大时间轴
 */
void CPcapBrowserDlg::_zoominTime( PCAP_CTRLBLK* data )
{
    if(data->wave.wzoomx <= DSM_PCAP_TIMELINE_MIN_TIMES)
    {
        return;
    }

    data->wave.wzoomx--;

    // 循环设置每个波形的刻度信息，主要是改变X轴刻度（每个格子采样点数）
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            _calcXGridCounts(data), 
            _calcAmplitude(data->wave.wfirst_visibility_index + i) * (float)pow(2, data->wave.wzoomy), 
            L"");

        // 设置波形下方每个格子的采样点数的显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)_calcXGridCounts(data));
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      缩小时间轴
 */
void CPcapBrowserDlg::_zoomoutTime( PCAP_CTRLBLK* data )
{
    if(data->wave.wzoomx >= DSM_PCAP_TIMELINE_MAX_TIMES)
    {
        return;
    }

    data->wave.wzoomx++;

    // 循环设置每个波形的刻度信息，主要是改变X轴刻度（每个格子采样点数）
    for(int i = 0; i < DSM_PCAP_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            _calcXGridCounts(data), 
            _calcAmplitude(data->wave.wfirst_visibility_index + i) * (float)pow(2, data->wave.wzoomy), 
            L"");

        // 设置波形下方每个格子的采样点数的显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)_calcXGridCounts(data));
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      获取通道的信息
 */
void CPcapBrowserDlg::_getChannelInfo( uint32_t index, COLORREF& color )
{
    const PREDEFINED_CHANNEL_INFO*  pTemplate = m_pChannelTemplate->pChannelTempalte;

    if(pTemplate[index].enChannelType  != CHANNEL_TYPE_SMV_VOLTAGE &&
        pTemplate[index].enChannelType != CHANNEL_TYPE_SMV_CURRENT)
    {
        color = DSM_PHASE_COLOR_UN;
        return;
    }

    switch(pTemplate[index].enChannelPhase)
    {
        // A相
    case CHANNEL_PHASE_A:
        color = DSM_PHASE_COLOR_A;
        break;

        // B相
    case CHANNEL_PHASE_B:
        color = DSM_PHASE_COLOR_B;
        break;

        // C相
    case CHANNEL_PHASE_C:
        color = DSM_PHASE_COLOR_C;
        break;

        // N相
    case CHANNEL_PHASE_N:
        color = DSM_PHASE_COLOR_N;
        break;

        // X相
    case CHANNEL_PHASE_X:
        color = DSM_PHASE_COLOR_X;
        break;

        // 未知
    default:
        color = DSM_PHASE_COLOR_UN;
        break;
    }
}

LRESULT CPcapBrowserDlg::OnUpdateWave( WPARAM, LPARAM )
{
    // 设置波形控件参数
    _setWaveform();
    _updateUIWave();

    return 1;
}

#if SU_FEI
void CPcapBrowserDlg::_statisticsLoadedPcap()
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl) || !ctrl.data)
    {
        m_bHasExceptionFrame = false;
        return;
    }

    m_bHasExceptionFrame = false;

    size_t begin = 0;
    size_t end   = ctrl.data->m_vecAppidFrameIndexMap.size();

    for(size_t i = begin; i < end; i++)
    {
        // // 找到一个异常的帧信息
        if(m_pDataLayer->GetPcapFileFrameIndexStatus(ctrl.data->m_vecAppidFrameIndexMap[i].nPcapFileFrameIndex).GetValue() != 0)
        {
            m_nFirstExcep        = i; 
            m_bHasExceptionFrame = true;
            break;
        }
    }

    // 找最后一个异常帧索引
    if(m_bHasExceptionFrame)
    {
        for(size_t i = end - 1; i >0; i--)
        {
            // // 找到一个异常的帧信息
            if(m_pDataLayer->GetPcapFileFrameIndexStatus(ctrl.data->m_vecAppidFrameIndexMap[i].nPcapFileFrameIndex).GetValue() != 0)
            {

                m_nLastExcep = i;

                break;
            }
        }
    }
}

#endif
void CPcapBrowserDlg::_ClearResource()
{
    if(m_pRNaviCurrent)
    {
        delete m_pRNaviCurrent;
        m_pRNaviCurrent = NULL;
    }

    __super::_ClearResource();
}
#if SU_FEI
bool CPcapBrowserDlg::_hasPrevException()
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl) || !ctrl.data)
    {
        return false;
    }

    // 确定开始查找的索引
    size_t end = (unsigned int)(0 - 1);
    size_t begin = ctrl.first_visibility_index + m_reportFrame.GetFocusedRowIndex();

    // m_dwDirectTag等于1或者2时，说明当前focus的行是旧的（在pretranslatemessage时）
    // 需要调整begin的值
    if(m_dwDirectTag == 1)
    {
        if(begin > 0)
        {
            begin--;
        }
    }
    else if(m_dwDirectTag == 2)
    {
        if(begin < end)
        {
            begin++;
        }
    }

    if(begin == 0)
    {
        return false;
    }
    else
    {
        begin--;
    }

    if(begin < m_nFirstExcep)
    {
        return false;
    }

    return true;
}

bool CPcapBrowserDlg::_hasNextException()
{
    PCAP_CTRLBLK ctrl;
    if(!_getCtrlBlkByAppID(m_nCurrentAppID, ctrl) || !ctrl.data)
    {
        return false;
    }

    // 确定开始查找的索引
    size_t begin = ctrl.first_visibility_index + m_reportFrame.GetFocusedRowIndex();
    size_t end   = ctrl.data->m_vecAppidFrameIndexMap.size();

    // m_dwDirectTag等于1或者2时，说明当前focus的行是旧的（在pretranslatemessage时）
    // 需要调整begin的值
    if(m_dwDirectTag == 1)          // vk_keyup
    {
        if(begin > 0)
        {
            begin--;
        }
    }
    else if(m_dwDirectTag == 2)     // vk_keydown
    {
        if(begin < end)
        {
            begin++;
        }
    }

    // 从下一个索引开始找
    if(begin < end)
    {
        begin++;
    }

    if(begin > m_nLastExcep)
    {
        return false;
    }

    return true;
}
#endif

#if !CHUN_HUA
void CPcapBrowserDlg::OnDetailF1F2( UINT nID )
{

	int nMaxIndex = m_pPcap->GetPckNum();
	int nIndex = m_nSelectFrameIndex;

	switch(nID)
	{
		case ID_MENU_PCAP_FRAME_DETAIL_F1:
			if (m_nSelectFrameIndex<1)
			{
				nIndex = 0;
			}
			else
				nIndex = --m_nSelectFrameIndex;
			break;
		case ID_MENU_PCAP_FRAME_DETAIL_F2:
			if (m_nSelectFrameIndex>nMaxIndex-2)
			{
				nIndex = nMaxIndex-1;
			}
			else
				nIndex = ++m_nSelectFrameIndex;
			break;
		case ID_MENU_PCAP_FRAME_DETAIL_F3:
			m_treeFrame.ExpandAllItem(true);
			return;
			break;
		case ID_MENU_PCAP_FRAME_DETAIL_F4:
			m_treeFrame.ExpandAllItem(false);
			return;
			break;
		default:
			break;
	}

	enumFrameType eType = FRAME_TYPE_UNKOWN;

	if(!_getFrameBuff(nIndex, eType))
	{
		return;
	}

	LBT_HTREEITEM  hItem =  m_treeFrame.GetTreeSelectedItem();
	m_treeFrame.RecordExpandStatus();
	
	_udateFrameDetail(eType);

	_updateFrameDetailCaption();

	m_treeFrame.RestoreExpandStatus();
	
	//if (hItem->firstChild)
	//{
	//	if (!hItem->isExpand)
	//	{
	//		//原来收缩
	//		m_treeFrame.ExpandTreeItem(hItem, TRUE);
	//	}
	//	else if (hItem->isExpand )
	//	{
	//		//原来展开
	//		m_treeFrame.ExpandTreeItem(hItem, FALSE);
	//	}

	//}
	//m_treeFrame.SelectTreeItem(hItem);
}

void CPcapBrowserDlg::_udateFrameDetail( enumFrameType eType )
{
	// 根据数据帧类型转交给不同的解码接口
	switch(eType)
	{
	case FRAME_TYPE_NET_9_1:
		if(!m_pMsg91)
		{
			m_pMsg91 = CreateSav91MsgImpl();
		}

		DSM_Decode91(m_treeFrame, m_pMsg91,eType, m_pFrameBuff, NET_MAX_FRAME_LEN);
		break;

	case FRAME_TYPE_NET_9_2:
		if(!m_pMsg92)
		{
			m_pMsg92 = CreateSav92MsgImpl();
		}

		DSM_Decode92(m_treeFrame, m_pMsg92, eType, m_pFrameBuff, NET_MAX_FRAME_LEN);
		break;

	case FRAME_TYPE_FT3_NANRUI:
		if(!m_pMsgNanRui)
		{
			m_pMsgNanRui = CreateNanRuiFT3MsgImpl();
		}

		DSM_DecodeNanruiFT3(m_treeFrame, m_pMsgNanRui, eType, m_pFrameBuff, NET_MAX_FRAME_LEN);
		break;

	case FRAME_TYPE_FT3_GUOWANG:
		if(!m_pMsgGuoWang)
		{
			m_pMsgGuoWang = CreateGuoWangFT3MsgImpl();
		}

		DSM_DecodeGuowangFT3(m_treeFrame, m_pMsgGuoWang, eType, m_pFrameBuff, NET_MAX_FRAME_LEN);
		break;

	case FRAME_TYPE_NET_GOOSE:
		if(!m_pMsgGoose)
		{
			m_pMsgGoose = CreateGooseMsgImpl();
		}

		DSM_DecodeGoose(m_treeFrame, m_pMsgGoose, eType, m_pFrameBuff, NET_MAX_FRAME_LEN);
		break;

	default:
		break;
	}

	if(m_treeFrame.GetCount() > 0)
	{
		LBT_HTREEITEM item = m_treeFrame.GetTreeFirstVisibleItem();

		if(item)
		{
			m_treeFrame.SelectTreeItem(item);
		}
	}
}
//更新标题
void CPcapBrowserDlg::_updateFrameDetailCaption()
{
	CString csCaption, csAppID;

	PCAP_CTRLBLK data;
	if(_getCtrlBlkByAppID(m_nCurrentAppID, data))
	{
		csAppID.Format(L"0x%04X", m_nCurrentAppID);
	}
	else
	{
		csAppID = L"UNKNOWN";
	}

	csCaption.Format(L"%s->%s [%s : %u/%u/%u]", dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_CAPTION), dsm_stringMgr::Get(DSM_STR_PCAPBROWSER_MENU_FRAME_DETAIL), csAppID, m_nSelectFrameIndex+1, data.max_frame, m_pDataLayer->GetPcapFileTotalFrameCount());
	CStatusMgr::SetTitle(csCaption);
}

#endif


