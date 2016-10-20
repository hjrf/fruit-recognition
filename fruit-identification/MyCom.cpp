// MyCom.cpp: implementation of the MyCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//错误信息输出
inline int MessageError(LPCSTR lpcszError, UINT nType = MB_OK | MB_ICONWARNING)
{
	CWnd *pWnd = ::AfxGetMainWnd();
	if(pWnd != NULL)
		return pWnd->MessageBox(lpcszError, "Error", nType);
	else
		return ::MessageBox(NULL, lpcszError, "Error", nType);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyCom::MyCom()
: m_bConnected(false), 
m_ePStatus(PS_NOREADY)
{
	memset(&m_Connection, 0, sizeof(m_Connection) - sizeof(CCriticalSection));
}

MyCom::~MyCom()
{

}

//读取当前通讯端口缓存中的数据
UINT MyCom::CommReadBlock(HANDLE hFile, LPOVERLAPPED lpOLRead, LPSTR lpszBlock, UINT nMaxLength)
{
	BOOL       fReadStat;
	COMSTAT    ComStat;
	DWORD      dwErrorFlags;
	DWORD      dwLength;
	DWORD      dwError;

	if(hFile == NULL)
		return false;

	//查询当前通讯缓存区的队列中的数据字节数
	::ClearCommError(hFile, &dwErrorFlags, &ComStat);
	//计算当前所需读取数据的字节数
	//如果缓存区中的数据字节数较当前内存缓存字节数小, 
	//就读取当前所有数据;  
	//如果缓存区中的数据字节数较当前内存缓存字节数大,
	//就读取当前内存缓存区所支持的最大字节数.)
	dwLength = min(nMaxLength, ComStat.cbInQue);
	
	if(dwLength <= 0)
		return 0;

	//请求从当前的通讯缓存区读取数据
	fReadStat = ::ReadFile(hFile, lpszBlock, dwLength, &dwLength, lpOLRead);
	if(fReadStat)
		return dwLength;

	if(GetLastError() == ERROR_IO_PENDING)
	{
		//当前发生I/O重叠操作，继续等待I/O操作...
		while(!GetOverlappedResult(hFile, lpOLRead, &dwLength, true))
		{
			dwError = GetLastError();
			if(dwError == ERROR_IO_INCOMPLETE)
				continue;
			else
			{
				//当前通讯有其它的错误发生
				TRACE("<CE-%u>", dwError);
				::ClearCommError(hFile, &dwErrorFlags, &ComStat);
				if(dwErrorFlags > 0)
					TRACE("<CE-%u>", dwErrorFlags);
				
				break;
			}
		}
	}
	else
	{
		//当前通讯发生了一些其它的错误
		dwLength = 0 ;
		::ClearCommError(hFile, &dwErrorFlags, &ComStat);
		if(dwErrorFlags > 0)
			TRACE("<CE-%u>", dwErrorFlags);
	}

	return dwLength;
}

//当前的通讯端口监听线程函数
UINT MyCom::CommMonitorProc(void* pData)
{
	LPCONNECTION	pMPParam = (LPCONNECTION)pData;
	int				nLength;
	char			szBuffer[MAX_BUF_BLOCK + 1];
	
	//监听当前串口数据通讯区的数据输入
	while(pMPParam->hCommDev != NULL)
	{
		DWORD dwEvtMask = 0;

		//等待当前串口通讯的一个读事件被触发
		WaitCommEvent(pMPParam->hCommDev, &dwEvtMask, NULL);
		
		if((dwEvtMask & EV_RXCHAR) != EV_RXCHAR)
			continue;
		
		//从串口读取数据并处理
		while((nLength = CommReadBlock(pMPParam->hCommDev, 
			&pMPParam->olRead, (LPSTR)szBuffer, MAX_BUF_BLOCK)) > 0)
		{
			//处理从串口读出的数据
			if(pMPParam->pCom != NULL)
			{
				pMPParam->pCom->LockThread();
				pMPParam->pCom->ProcessReceivedData(szBuffer, nLength);
				pMPParam->pCom->UnlockThread();
			}

			if(::WaitForSingleObject(pMPParam->hEventCancel, 0) == WAIT_OBJECT_0)
				break;
		}
	}

	//通知主线程当前线程已结束
	//....

	//当前线程正确结束，返回0
	return 0;
}

//建立当前的通讯连接
BOOL MyCom::Connect()
{
	//当前已经通讯连接已经建好
	if(m_bConnected)
		return true;
	
	__try
	{
		m_Connection.CSec.Lock();

		//初始化当前串口信息的设置
		if(!this->InitCommInfo())
		{
			ASSERT(false);
			__leave;
		}
		
		if(m_Connection.hCommDev != NULL)
		{
			ASSERT(false);
			__leave;
		}

		//创建当前的通讯连接

		//创建用于I/O异步通讯时的读、写和取消事件
		m_Connection.olRead.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_Connection.olWrite.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_Connection.hEventCancel = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if(m_Connection.olRead.hEvent == NULL
		|| m_Connection.olWrite.hEvent == NULL
		|| m_Connection.hEventCancel == NULL)
			__leave;

		//打开通讯端口
		if(!this->OpenComm(m_Connection.hCommDev))
			__leave;

		//设置当前串口的通信，当COMM_MASK指定的事件
		//由WaitCommEvent调用发生时，来报告这些事件
		if(!::SetCommMask(m_Connection.hCommDev, COMM_MASK))
		{
			ASSERT(false);
			__leave;
		}

		//设置串口驱动的输入和输出缓冲区的大小
		if(!::SetupComm(m_Connection.hCommDev, 1024, 1024))
		{
			ASSERT(false);
			__leave;
		}
			
		//异常终止当前在打开的端口上正进行的输入和输出操作
		if(!::PurgeComm(m_Connection.hCommDev, PURGE_FLAG))
		{
			ASSERT(false);
			__leave;
		}

		//设置当前串口的读写超时
		if(!this->SetCommTimeouts(m_Connection.hCommDev))
		{
			ASSERT(false);
			__leave;
		}

		//设定当前串口的通信参数
		if(!this->SetCommState(m_Connection.hCommDev))
		{
			ASSERT(false);
			__leave;
		}

		m_Connection.pCom = this;

		//创建侦听线程
		DWORD dwThreadID;
		m_hMonitorThread = CreateThread(
			(LPSECURITY_ATTRIBUTES)NULL,
			0, (LPTHREAD_START_ROUTINE)CommMonitorProc,
			&m_Connection, 0, &dwThreadID);
		if(m_hMonitorThread == NULL)
		{
			ASSERT(false);
			__leave;
		}

		//声明串口上的DTR线
		EscapeCommFunction(m_Connection.hCommDev, SETDTR);

		m_Connection.dwRefCount++;
		m_bConnected = true;
		m_ePStatus = PS_READY;
	}
	__finally
	{
		//当前连接创建失败
		if(!m_bConnected)
		{
			//如果端口已经打开，则关闭已打开的端口
			if(m_Connection.hCommDev != NULL)
			{
				::CloseHandle(m_Connection.hCommDev);
				m_Connection.hCommDev = NULL;
			}

			//如果读、写事件已经创建，则删除当前的读、写事件
			if(m_Connection.olRead.hEvent != NULL)
			{
				//关闭读事件
				::CloseHandle(m_Connection.olRead.hEvent);
				m_Connection.olRead.hEvent = NULL;
			}

			if(m_Connection.olWrite.hEvent != NULL)
			{
				//关闭写事件
				::CloseHandle(m_Connection.olWrite.hEvent);
				m_Connection.olWrite.hEvent = NULL;
			}

			if(m_Connection.hEventCancel != NULL)
			{
				//关闭取消事件
				::CloseHandle(m_Connection.hEventCancel);
				m_Connection.hEventCancel = NULL;
			}
		}

		m_Connection.CSec.Unlock();
		return m_bConnected;
	}
}

//重新建立当前的通讯连接
BOOL MyCom::ReConnect()
{
	//当前已经通讯连接已经建好
	if(m_bConnected)
	{
		this->DisConnect();
		this->DelayReply(RECONNECT_TIME);
	}
	
	return this->Connect();
}

//取消当前的通讯连接
void MyCom::DisConnect()
{
	m_Connection.CSec.Lock();

	//当前本没有通讯连接，直接返回
	if(!m_bConnected)
	{
		m_Connection.CSec.Unlock();
		return;
	}

	m_Connection.dwRefCount--;
	
	//关闭通讯事件
	if(m_Connection.dwRefCount == 0
	&& m_Connection.hCommDev != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_Connection.hCommDev);
		::CloseHandle(m_Connection.olRead.hEvent);
		::CloseHandle(m_Connection.olWrite.hEvent);
				
		m_Connection.hCommDev = NULL;
		m_Connection.olRead.hEvent = NULL;
		m_Connection.olWrite.hEvent = NULL;

		::SetEvent(m_Connection.hEventCancel);
		::CloseHandle(m_Connection.hEventCancel);
		m_Connection.hEventCancel = NULL;
	}

	//置当前的通讯连接标志
	m_bConnected = false;
	m_ePStatus = PS_NOREADY;
	m_Connection.CSec.Unlock();
}

//获取当前的通讯端口名称
CString MyCom::GetCommName()
{
	return this->GetCommName(m_CommInfo.byCommPort);
}

//判断当前的通讯连接是否已经建立
BOOL MyCom::IsConnected()
{
	return m_bConnected;
}

//获取当前的通讯连接指针,并锁定于当前线程
//(此函数必须与UnLockConnection()配对调用, 时间间隔不宜太久)
LPCONNECTION MyCom::LockConnection()
{
	m_Connection.CSec.Lock();
	return &m_Connection;
}

//释放当前的通讯连接指针(解除当前线程的锁定)
//(此函数必须与LockConnection()配对调用, 时间间隔不宜太久)
void MyCom::UnLockConnection()
{
	m_Connection.CSec.Unlock();
}

//初始化当前的串口信息
BOOL MyCom::InitCommInfo()
{
	if(m_bConnected)
		return false;

	memset(&m_CommInfo, 0, sizeof(m_CommInfo));

	m_CommInfo.byByteSize = 8;
	m_CommInfo.byCommPort = 1;
	m_CommInfo.byParity = NOPARITY;
	m_CommInfo.byStopBits = TWOSTOPBITS;
	m_CommInfo.dwBaudRate = CBR_9600;
	m_CommInfo.byFlowCtrl = FC_XONXOFF;
	m_CommInfo.byXonXoff = false;

	return true;
}

//根据串口的序号获取当前串口的名称
CString MyCom::GetCommName(BYTE bySN)
{
	CString strOut;
	
	if(bySN <= 0 || bySN > 20)
	{
		ASSERT(false);
		bySN = 1;
	}

	strOut.Format("COM%d", bySN);
	return strOut;
}

//设定当前串口的通信参数
BOOL MyCom::SetCommState(HANDLE hFile)
{
	DCB dcb;
	::ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	
	if(!::GetCommState(hFile, &dcb))
		return false;
	
	dcb.BaudRate = this->m_CommInfo.dwBaudRate;
	dcb.ByteSize = this->m_CommInfo.byByteSize;
	dcb.Parity = this->m_CommInfo.byParity;
	dcb.StopBits = this->m_CommInfo.byStopBits;
	
	//setup hardware flow control
/*	dcb.fOutxDsrFlow = (BYTE)((m_CommInfo.byFlowCtrl & FC_DTRDSR) != 0);
	if(dcb.fOutxDsrFlow)
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	else
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
	
	dcb.fOutxCtsFlow = (BYTE)((m_CommInfo.byFlowCtrl & FC_RTSCTS) != 0);
	if(dcb.fOutxCtsFlow)
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	else
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	
	//setup software flow control
	dcb.fInX = dcb.fOutX = (BYTE)((m_CommInfo.byFlowCtrl & FC_XONXOFF) != 0);
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;
	
	//other various settings
	dcb.fBinary = TRUE;
	dcb.fParity = m_CommInfo.byParity;
*/	
	return ::SetCommState(hFile, &dcb);
}

//设置当前串口的读写超时
BOOL MyCom::SetCommTimeouts(HANDLE hFile)
{
	COMMTIMEOUTS CommTimeOuts;
	
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 20;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	
	return ::SetCommTimeouts(hFile, &CommTimeOuts);
}

//打开串口
BOOL MyCom::OpenComm(HANDLE &hFile)
{
	CString strError;

	//找开串口
	HANDLE h = ::CreateFile(
		this->GetCommName(), 
		GENERIC_READ|GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, 
		NULL);
			
	//打开指定串口失败
	if(h == INVALID_HANDLE_VALUE)
	{
		strError.Format("打开通讯端口%s出错!\n", 
			this->GetCommName());
		MessageError(strError);
		return false;
	}

	hFile = h;
	return true;
}

//向当前通讯端口发送数据
BOOL MyCom::CommWriteBlock(HANDLE hFile, LPCSTR lpcByte, 
							DWORD dwBytesToWrite,
							OVERLAPPED *pOLWrite)
{
	DWORD       dwBytesWritten;
	DWORD   	dwError;
	DWORD       dwBytesSent = 0;
	COMSTAT     ComStat;
	char        szError[128];
	
	//向串口写入数据
	if(WriteFile(hFile, lpcByte, dwBytesToWrite, 
		&dwBytesWritten, pOLWrite))
		return true;
	
	//检测写数据函数运行后的全局系统错误信息
	//如果有非重叠信息错误，则报错并退出当前函数的运行
	if(GetLastError() != ERROR_IO_PENDING)
	{
		ClearCommError(hFile, &dwError, &ComStat);
		if(dwError > 0)
		{
			sprintf(szError, "<CE-%x>", dwError);
			TRACE0(szError);
		}
			
		return false;
	}

	//获取当前IO重叠操作的结果
	while(!GetOverlappedResult(hFile, pOLWrite, 
		&dwBytesWritten, true))
	{
		dwError = GetLastError();

		//依旧处于IO重叠操作过程中，继续等待
		if(dwError == ERROR_IO_INCOMPLETE)
		{
			dwBytesSent += dwBytesWritten;
			continue;
		}
		//发生了其它错误，退出当前函数的执行
		else
		{
			sprintf(szError, "<CE-%x>\n", dwError);
			TRACE0(szError);
			ClearCommError(hFile, &dwError, &ComStat);
			if(dwError > 0)
			{
				sprintf(szError, "<CE-%x>\n", dwError);
				TRACE0(szError);
			}
			break;
		}
	}
	
	//计算已写入数据的字节数，并判断是否写串口数据成功
	dwBytesSent += dwBytesWritten;
	if(dwBytesSent != dwBytesToWrite)
	{
		sprintf(szError, "Probable Write Timeout: Total of %ld bytes sent\n", dwBytesSent);
		TRACE0(szError);
		return false;
	}
	else
	{
//		sprintf(szError, "%ld bytes written\n", dwBytesSent);
//		TRACE0(szError);
		return true;
	}
}

//处理接收回来的数据
void MyCom::ProcessReceivedData(LPCSTR lpcByte, 
								 DWORD dwDataLenght)
{
#ifdef _DEBUG
	char szBuf[128];
	char szTemp[8];
	CTime timeCur = time(NULL);
	sprintf(szBuf, "(R_%2d:%2d:%2d)", timeCur.GetHour(), timeCur.GetMinute(), timeCur.GetSecond());
	for(DWORD i = 0; i < dwDataLenght; i++)
	{
		sprintf(szTemp, "%02x ", (unsigned char)lpcByte[i]);
		strcat(szBuf, szTemp);
	}

	TRACE(szBuf);
#endif
}

//向串口发送数据
BOOL MyCom::WriteData2Comm(LPCSTR lpcByte, 
							DWORD dwDataLenght)
{
#ifdef _DEBUG
	char szBuf[128];
	char szTemp[8];
	CTime timeCur = time(NULL);
	sprintf(szBuf, "(S_%2d:%2d:%2d)", timeCur.GetHour(), timeCur.GetMinute(), timeCur.GetSecond());
	for(DWORD i = 0; i < dwDataLenght; i++)
	{
		sprintf(szTemp, "%02x ", (unsigned char)lpcByte[i]);
		strcat(szBuf, szTemp);
	}

	TRACE(szBuf);
#endif

	BOOL bRet = false;
	//将当前对象锁定于当前线程
	m_CSec.Lock();

	__try
	{
		if(!m_bConnected)
		{
			TRACE("请先建立正确的串口通讯连接!\n");
			__leave;
		}
		
		LPCONNECTION lpConnection = this->LockConnection();
		
		bRet = MyCom::CommWriteBlock(lpConnection->hCommDev,
			lpcByte, dwDataLenght, &lpConnection->olWrite);
		
		this->UnLockConnection();
	}
	__finally
	{
		m_CSec.Unlock();
		return bRet;
	}
}

//设定当前延时处理
void MyCom::DelayReply(DWORD dwTime)
{
	DWORD dwStart = GetTickCount();

	while(1)
	{
		MSG msg;
		//响应其它消息的处理
		while(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if(GetTickCount() - dwStart > dwTime)
			return;
	
		::Sleep(1);
	}
}
