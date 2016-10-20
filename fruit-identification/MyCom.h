// MyCom.h: interface for the MyCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCOM_H__BF6B4ECC_762F_48D2_9365_F123560D9CAA__INCLUDED_)
#define AFX_MYCOM_H__BF6B4ECC_762F_48D2_9365_F123560D9CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

// flow control
#define FC_DTRDSR  0x01
#define FC_RTSCTS  0x02
#define FC_XONXOFF 0x04

#define ASCII_XON  0x11
#define ASCII_XOFF 0x13

class MyCom;
typedef struct _CONNECTION 
{
	HANDLE				hCommDev;	//当前的通讯连接句柄
	HANDLE				hEventCancel;//当前退出信号事件
	DWORD				dwRefCount;	//当前通讯连接的引用次数
	MyCom*				pCom;		//MyCom类对象指针
	OVERLAPPED			olWrite;	//写重叠I/O对象
	OVERLAPPED			olRead;		//读重叠I/O对象
	CCriticalSection	CSec;		//当前结构支持多线程访问的互拆量
}CONNECTION, *LPCONNECTION;
typedef const CONNECTION* LPCCONNECTION;

typedef struct _COMM_INFO{
	BYTE	byCommPort;	//通讯端口名称，可为COM1-4
	BYTE	byByteSize;	//数据位数，必须为8
	BYTE	byParity;	//奇偶检验方式，必须为0
	BYTE	byStopBits;	//停止位，必须为1
	DWORD	dwBaudRate;	//波特率，必须为38400
	BYTE	byXonXoff;	
	BYTE	byFlowCtrl;	//流控制方式
}COMM_INFO;

//当前用到的串口通讯处理状态
typedef enum{
	PS_NOREADY,			//当前状态还没定(初始状态)
	PS_READY,			//当前处于就绪状态
	PS_WAITTING,		//当前处理数据等待状态
	PS_RECEIVINGDATA,	//当前正在接收数据
	PS_RECEIVED_FAULT	//当前接收数据错误
}PROCESS_STATUS;

class MyCom
{
public:
	enum{
		//当前与通信缓存区一次性最多读取数据的内存缓存大小
		MAX_BUF_BLOCK	=	128,
		//异常终止当前在打开的端口上正进行的输入和输出操作类型
		PURGE_FLAG		=	PURGE_TXABORT | PURGE_RXABORT 
							| PURGE_TXCLEAR | PURGE_RXCLEAR,
		//当前串口的通信时需要串口通知的事件类型
		COMM_MASK		=	EV_RXCHAR,
		//当前最长等待时间(毫秒)
		MAX_WAITTING	=	500,
		//当前最小等待时间(毫秒)
		MIN_WAITTING	=	100,
		//重新连接同一串时的时间间隔
		RECONNECT_TIME	=	1000
	};
	//当前串口的设置信息
	COMM_INFO	m_CommInfo;
	CONNECTION	m_Connection;

	MyCom();
	virtual ~MyCom();

	//建立当前的通讯连接
	virtual BOOL Connect();
	//重新建立当前的通讯连接
	virtual BOOL ReConnect();
	//取消当前的通讯连接
	virtual void DisConnect();
	//初始化当前的串口信息
	virtual BOOL InitCommInfo();
	//处理接收回来的数据
	virtual void ProcessReceivedData(LPCSTR lpcByte/*从串口接收回来的数据*/, 
		DWORD dwDataLenght/*从串口接收回来的数据的长度*/);
	//向串口发送数据
	virtual BOOL WriteData2Comm(LPCSTR lpcByte/*想要写入串口的数据*/, 
		DWORD dwDataLenght/*想要写入串口的数据的长度*/);

	//当前的通讯端口监听线程函数
	static UINT CommMonitorProc(void* pData/*主线程传递给监听(接收)线程的参数*/);
	//读取当前通讯端口缓存中的数据
	static UINT CommReadBlock(HANDLE hFile/*打开的串口文件句柄*/, 
		LPOVERLAPPED lpOL/*用于读串口时的异步通讯事件*/, 
		LPSTR lpszBlock/*用于读串口数据的缓存区*/, 
		UINT nMaxLength/*用于读串口数据的缓存区的大小*/);
	//向当前通讯端口发送数据
	static BOOL CommWriteBlock(HANDLE hFile/*打开的串口文件句柄*/, 
		LPCSTR lpcByte/*想要写往串口的数据*/, 
		DWORD dwBytesToWrite/*想要写往串口的数据的字节数*/, 
		OVERLAPPED *pOLWrite/*用于写串口时的异步通讯事件*/);
	
	//获取当前的通讯连接指针,并锁定于当前线程
	//(此函数必须与UnLockConnection()配对调用, 时间间隔不宜太久)
	LPCONNECTION LockConnection();
	//释放当前的通讯连接指针(解除当前线程的锁定)
	//(此函数必须与LockConnection()配对调用, 时间间隔不宜太久)
	void UnLockConnection();

	//判断当前的通讯连接是否已经建立
	BOOL IsConnected();
	//设定当前的处理状态
	inline void SetProcessStatus(PROCESS_STATUS ePStatus)
	{m_ePStatus = ePStatus;}
	//获取当前的处理状态
	inline PROCESS_STATUS GetProcessStatus()
	{return m_ePStatus;}
	//将当前对象锁定于当前线程
	BOOL LockThread(){return m_CSec.Lock();}
	//将当前对象解锁于当前线程
	void UnlockThread(){m_CSec.Unlock();}

protected:
	//当前通讯对象的通讯连接状态(true连接, false不连接)
	volatile BOOL m_bConnected;
	//当前的监听线程句柄
	HANDLE m_hMonitorThread;
	//当前线程通讯所处于的状态
	volatile PROCESS_STATUS m_ePStatus;
	//当前对象用于线程互锁的的临界量
	CCriticalSection	m_CSec;

	//获取当前的通讯端口名称
	CString GetCommName();
	//根据串口的序号获取当前串口的名称
	CString GetCommName(BYTE bySN);
	//设定当前延时处理
	void DelayReply(DWORD dwTime);

private:
	//设定当前串口的通信参数
	BOOL SetCommState(HANDLE hFile);
	//设置当前串口的读写超时
	BOOL SetCommTimeouts(HANDLE hFile);
	//打开串口
	BOOL OpenComm(HANDLE &hFile);
};

#endif // !defined(AFX_MYCOM_H__BF6B4ECC_762F_48D2_9365_F123560D9CAA__INCLUDED_)
