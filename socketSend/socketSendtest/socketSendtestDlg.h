
// socketSendtestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define MAXLINE 256

// CsocketSendtestDlg �Ի���
class CsocketSendtestDlg : public CDialogEx
{
// ����
public:
	CsocketSendtestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOCKETSENDTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	// 16������
	CEdit CeditHexLine;
	afx_msg void OnBnClickedClearSend();
	afx_msg void OnBnClickedClearRec();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
