// SCLAnalysisDLLTest.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CSCLAnalysisDLLTestApp:
// 有关此类的实现，请参阅 SCLAnalysisDLLTest.cpp
//

class CSCLAnalysisDLLTestApp : public CWinApp
{
public:
	CSCLAnalysisDLLTestApp();
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSCLAnalysisDLLTestApp theApp;
