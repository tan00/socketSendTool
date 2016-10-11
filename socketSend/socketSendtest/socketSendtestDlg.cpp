
// socketSendtestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "socketSendtest.h"
#include "socketSendtestDlg.h"
#include "afxdialogex.h"
#include "cstringt.h"
#include "UnionThreadLock.h"

extern int initSocket(char* ip , int port , SOCKET *s , SOCKADDR_IN  *ServerAddr);
extern void destorySocket(SOCKET *fd );
extern int mysend(SOCKET *fd , SOCKADDR_IN  *ServerAddr ,unsigned char *msg ,int lenofMsg);
extern int myrecv(SOCKET *fd, unsigned char *buf ,int len);
extern bool ConnectWithTimeout(SOCKET socket,struct sockaddr *address, int timeout);




#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define MAX_THREAD_NUM  2000

char asctohex(char ch1,char ch2)
{
	char ch;
	if (ch1>='A') ch=(char )((ch1-0x37)<<4);
	else ch=(char)((ch1-'0')<<4);
	if (ch2>='A') ch|=ch2-0x37;
	else ch|=ch2-'0';
	return ch;
}
char hexlowtoasc(int xxc)
{
	xxc&=0x0f;
	if (xxc<0x0a) xxc+='0';
	else xxc+=0x37;
	return (char)xxc;
}

char hexhightoasc(int xxc)
{
	xxc&=0xf0;
	xxc = xxc>>4;
	if (xxc<0x0a) xxc+='0';
	else xxc+=0x37;
	return (char)xxc;
}

int aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
	int i,j;

	if (len % 2 == 0)
		j = len / 2;
	else
		j = len / 2 + 1;

	for (i = 0; i < j; i++)
		bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);

	return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
	int i;

	for (i=0;i<len;i++)
	{
		aschex[2*i]   = hexhightoasc(bcdhex[i]);
		aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
	}

	return(len*2);
}
int IsBCDStr(char *str1)
{
	int	i;
	int	len;

	for (i = 0,len = strlen(str1); i < len; i++)
	{
		if ((str1[i] >= '0') && (str1[i] <= '9'))
			continue;
		if ((str1[i] >= 'a') && (str1[i] <= 'f'))
			continue;
		if ((str1[i] >= 'A') && (str1[i] <= 'F'))
			continue;
		return(0);
	}
	return(1);
}

void threadfuc(void*)
{

}



// CsocketSendtestDlg 对话框




CsocketSendtestDlg::CsocketSendtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CsocketSendtestDlg::IDD, pParent), CStrRecData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsocketSendtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT4, sendEdit);
	DDX_Control(pDX, IDC_IPADDRESS1, ipAddr);
	DDX_Control(pDX, IDC_EDIT2, port);	
	DDX_Control(pDX, IDC_EDIT3, ceditRecData);
	DDX_Text(pDX, IDC_EDIT3, CStrRecData);
	DDV_MaxChars(pDX, CStrRecData, 8192);
	DDX_Control(pDX, IDC_EDIT1, CeditHexLine);
}

BEGIN_MESSAGE_MAP(CsocketSendtestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT2, &CsocketSendtestDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT4, &CsocketSendtestDlg::OnEnChangeEdit4)
	ON_BN_CLICKED(IDC_BUTTON1, &CsocketSendtestDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDOK, &CsocketSendtestDlg::OnBnClickedTestConnection)
	ON_BN_CLICKED(IDC_BUTTON3, &CsocketSendtestDlg::OnBnClickedClearSend)
	ON_BN_CLICKED(IDC_BUTTON2, &CsocketSendtestDlg::OnBnClickedClearRec)
	ON_BN_CLICKED(IDC_RADIO1, &CsocketSendtestDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CsocketSendtestDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CsocketSendtestDlg 消息处理程序

BOOL CsocketSendtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//默认radio
	m_radio01=(CButton*)GetDlgItem(IDC_RADIO1);
	m_radio02=(CButton*)GetDlgItem(IDC_RADIO2);
	m_radio01->SetCheck(1);
	m_isAllHex = false;

	//设置对话框等宽字体
	//LOGFONT lf;
	//memset(&lf,0,sizeof(LOGFONT));	
	//strcpy_s(lf.lfFaceName, _countof(lf.lfFaceName), _T("Consolas"));
	//
	//CFont Font ;
	//Font.CreateFontIndirect(&lf);
	//sendEdit.SetFont(&Font);
	//ceditRecData.SetFont(&Font);




	//socket库启动
	AfxSocketInit();	

	//默认ip
	DWORD defalutIP;
	unsigned char *ipfield = (unsigned char *)&defalutIP;
	*ipfield = 234;
	*(ipfield+1) = 2;
	*(ipfield+2) = 1;
	*(ipfield+3) = 192;
	ipAddr.SetAddress(defalutIP);
	port.SetWindowTextA("1818");



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CsocketSendtestDlg::OnPaint()
{
	if (/*IsIconic()*/ 0)
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CsocketSendtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CsocketSendtestDlg::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CsocketSendtestDlg::OnEnChangeEdit4()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}



int CsocketSendtestDlg::getEditData(CEdit &sendEdit , bool HexLineNum[MAXLINE] ,unsigned char *outdata)
{
	int i = 0;
	//全部为16进制
	if (m_isAllHex)
	{
		sendEdit.GetWindowText(this->senddata);
		this->senddata.Remove(' ');
		this->senddata.Remove('\t');
		this->senddata.Remove('\r');
		this->senddata.Remove('\n');	
		int lenofSenddata = this->senddata.GetLength();
		lenofSenddata = aschex_to_bcdhex( senddata.GetBuffer() ,lenofSenddata , (char*)outdata+2);
		//添加数据长度		
		outdata[0] = lenofSenddata/(256);
		outdata[1] = lenofSenddata%(256);
		return lenofSenddata;	
	}

	else	
	m_hasHexLine = false;
	for ( i=0 ; i<9 ;i++)
	{
		if ( HexLineNum[i+1]==true )
		{
			m_hasHexLine = true;
			break;
		}
	}

	//没有16进制行
	if (m_hasHexLine==false)
	{
		sendEdit.GetWindowText(this->senddata);
		this->senddata.Remove(' ');
		this->senddata.Remove('\t');
		this->senddata.Remove('\r');
		this->senddata.Remove('\n');
		//添加数据长度
		int lenofSenddata = this->senddata.GetLength();
		outdata[0] = lenofSenddata/(256);
		outdata[1] = lenofSenddata%(256);
		memcpy(outdata+2,senddata.GetBuffer() , lenofSenddata);
		return lenofSenddata;
	}
	//有16进制行
	else
	{
		int offset = 2;
		char linebuf[4096] = {0};		
		int lenofLine = 0;	
		
		sendEdit.GetWindowText(this->senddata);
		this->senddata.Remove(' ');
		this->senddata.Remove('\t');
		sendEdit.SetWindowText(this->senddata);

		LPTSTR msg = 0;
		for (i=0;i<sendEdit.GetLineCount();i++)
		{			
			if(( lenofLine = sendEdit.GetLine( i , linebuf  ,sizeof(linebuf)  )) <0)  //空行
				continue;	

			if ( HexLineNum[i+1] == true )//是16进制行
			{			
				if (lenofLine%2 !=0  )
				{
					sprintf(msg , "第%d行 ,16进制数据长度 lenofLine=[%d] ",i+1, lenofLine);
					MessageBox(msg , "输入数据长度不是偶数" , MB_OK | MB_ICONERROR);
					return -1;
				}
				if ( IsBCDStr(linebuf) !=1  )
				{
					sprintf(msg , "第%d行 不是16进制数据 ",i+1);
					MessageBox(linebuf , "输入数据错误" , MB_OK | MB_ICONERROR);
					return -2;
				}
				
				aschex_to_bcdhex(linebuf , lenofLine , (char*)outdata+offset);
				offset += lenofLine/2;

				//MessageBox( (char*)outdata+2 ,"送入数据" ,MB_OK );
			}
			else
			{
				strncpy((char*)outdata+offset ,linebuf ,lenofLine);
				offset += lenofLine;
				//MessageBox( (char*)outdata+2 ,"送入数据" ,MB_OK );
			}
			memset(linebuf,0,sizeof(linebuf));
		}
		//MessageBox( (char*)outdata+2 ,"送入数据" ,MB_OK );

		offset -= 2;
		outdata[0] = offset/(256);
		outdata[1] = offset%(256);

		return offset;
	}
	

}



void CsocketSendtestDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码
	//清除返回控件中的内容
	OnBnClickedClearRec();

	UpdateData(TRUE);
	int ret = 0;


	bool HexLine[MAXLINE+1] = {false} ;  //16进制	
	int lenofSenddata = 0;	

	if (m_isAllHex == false)
	{
		//获取16进制行号
		char strHexLine[9] = {0};
		CeditHexLine.GetWindowTextA(strHexLine ,9);
		for (int i=0;i<9;i++)
		{
			if ( (strHexLine[i]-'0') >0 && (strHexLine[i]-'0')<9 )
			{
				HexLine[strHexLine[i]-'0'] = true;
			}		
		}
	}
	lenofSenddata = getEditData(sendEdit ,HexLine , m_pBsendData);	


	//socket处理
	DWORD ipfiled;	
	ipAddr.GetAddress( ipfiled);
	unsigned  char  *strIP;
	CString cstrIP;
	strIP  =  (unsigned  char*)&ipfiled;  
	cstrIP.Format("%u.%u.%u.%u",*(strIP+3),  *(strIP+2),  *(strIP+1),  *strIP);
	
	CString cstrport;
	port.GetWindowText(cstrport);
	
	memset(m_ip , 0 ,sizeof(m_ip));	
	memcpy(m_ip, cstrIP.GetBuffer() ,cstrIP.GetLength());
	m_iport = atoi(cstrport.GetBuffer());



	SOCKET fd[MAX_THREAD_NUM];
	SOCKADDR_IN sockAddr[MAX_THREAD_NUM];

	for (int i=0;i<1;i++) //一百个线程
	{
		if (( ret=initSocket(  m_ip , m_iport , fd+i , sockAddr+i)  )<0)
		{
			//char strRet[5];
			//sprintf(strRet , "%04d" ,ret );
			//MessageBox(strRet, "initSocket错误", MB_OK|MB_ICONERROR);
			return ;
		}
	}


	for (int i=0;i<3;i++)
	{
		if (( ret=mysend( fd , sockAddr , m_pBsendData , lenofSenddata+2)  )<0)
		{
			//char strRet[5];
			//sprintf(strRet , "%04d" ,ret );
			//MessageBox(strRet, "mysend错误", MB_OK|MB_ICONERROR);
			return ;
		}	
		else
			break;
	}

	unsigned char resBuf[8192] = {0};
	if (( ret=myrecv(fd ,resBuf ,sizeof(resBuf))  )<0)
	{
		strcpy((char*)resBuf,"recv error!");
		CStrRecData.Format("%s" , resBuf+2 );	
		CStrRecData = " rec::\r\n" + CStrRecData;
		UpdateData(FALSE);
		return ;
	}


	//显示接收数据
	//没有16进制行


	if ( m_hasHexLine==false && m_isAllHex==false )
	{	
		CStrRecData.Format("%s" , resBuf+2 );	
	}
	else
	{
		int len = resBuf[0]*(256) + resBuf[1];
		char tmpbuf[8192];
		//bcdhex_to_aschex((char*)resBuf+2 ,len ,tmpbuf);
		int offset = 0;
		for(int i=0; i<len ;i++)
		{
			bcdhex_to_aschex((char*)resBuf+2+i ,1 ,tmpbuf+offset );
			offset += 2;
			tmpbuf[offset++] = ' ';
		}
		tmpbuf[offset] = '\0';
		CStrRecData.Format("%s" ,tmpbuf);	
		
	}

	CStrRecData = "rec::\r\n" + CStrRecData;
	UpdateData(FALSE);
	return;

}


void CsocketSendtestDlg::OnBnClickedTestConnection()
{
	// TODO: 在此添加控件通知处理程序代码
	//socket处理
	int ret = 0;
	DWORD ipfiled;	
	ipAddr.GetAddress( ipfiled);
	unsigned  char  *strIP;
	CString cstrIP;
	strIP  =  (unsigned  char*)&ipfiled;  
	cstrIP.Format("%u.%u.%u.%u",*(strIP+3),  *(strIP+2),  *(strIP+1),  *strIP);
	CString cstrport;
	port.GetWindowText(cstrport);

	SOCKET fd;
	SOCKADDR_IN sockAddr;
	if (( ret=initSocket(  cstrIP.GetBuffer() , atoi(cstrport.GetBuffer()) , &fd , &sockAddr)  )<0)
	{
		char strRet[5];
		sprintf(strRet , "%04d" ,ret );
		MessageBox(strRet, "连接错误", MB_OK|MB_ICONERROR);
		return ;
	}
	else 
		MessageBox("测试连接成功", "连接测试", MB_OK);

	//destorySocket(&fd);
	return;
}


void CsocketSendtestDlg::OnBnClickedClearSend()
{
	// TODO: 在此添加控件通知处理程序代码
	sendEdit.SetSel(0,-1);
	sendEdit.Clear();
}


void CsocketSendtestDlg::OnBnClickedClearRec()
{
	// TODO: 在此添加控件通知处理程序代码
	ceditRecData.SetSel(0,-1);
	ceditRecData.Clear();
}



void threadFunc(void*)
{

}





void CsocketSendtestDlg::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_isAllHex = false;
}


void CsocketSendtestDlg::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
		m_isAllHex = true;
}
