#pragma once
#include "afxwin.h"
#include "resource.h"

// CSolidMineDlg �Ի���

class CSolidMineDlg : public CAcUiDialog
{
	DECLARE_DYNAMIC(CSolidMineDlg)

public:
	CSolidMineDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSolidMineDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SOLIDMINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_trilayers0;
	CListBox m_trilayers1;
	CStringArray layArrs0,layArrs1;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonInt();
	afx_msg void OnBnClickedButtonUnion();
	//2017-7-5mode=0,����mode=1,����mode=2,��
	void SolidOperate(int mode);
};
