
// socketSendtestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define MAXLINE 256

// CsocketSendtestDlg 对话框
class CsocketSendtestDlg : public CDialogEx
{
// 构造
public:
	CsocketSendtestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOCKETSENDTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	unsigned char m_pBsendData[8192];
	char m_ip[128];
	int m_iport;
	bool m_hasHexLine;
	bool m_isAllHex;
	CButton* m_radio01;
	CButton* m_radio02;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit4();
	CEdit sendEdit;
	
	afx_msg void OnBnClickedSend();
	CString senddata;
	CIPAddressCtrl ipAddr;
	CEdit port;

	CEdit ceditRecData;
	CString CStrRecData;
	afx_msg void OnBnClickedTestConnection();

	int getEditData(CEdit &sendEdit , bool HexLineNum[MAXLINE] ,unsigned char *outdata);
	// 16进制行
	CEdit CeditHexLine;
	afx_msg void OnBnClickedClearSend();
	afx_msg void OnBnClickedClearRec();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
