#pragma once
#include "afxwin.h"
#include "resource.h"

// CSolidMineDlg 对话框

class CSolidMineDlg : public CAcUiDialog
{
	DECLARE_DYNAMIC(CSolidMineDlg)

public:
	CSolidMineDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSolidMineDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SOLIDMINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_trilayers0;
	CListBox m_trilayers1;
	CStringArray layArrs0,layArrs1;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonInt();
	afx_msg void OnBnClickedButtonUnion();
	//2017-7-5mode=0,减；mode=1,交；mode=2,并
	void SolidOperate(int mode);
};
