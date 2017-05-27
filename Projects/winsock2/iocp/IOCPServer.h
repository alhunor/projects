#ifndef _SERVER_IOCP_H_
#define _SERVER_IOCP_H_

//Disable deprecation warnings
#pragma warning(disable: 4996)

//Op codes for IOCP
#define OP_READ     0
#define OP_WRITE    1

#define WORKER_THREADS_PER_PROCESSOR 2

//Buffer Length 
#define MAX_BUFFER_LEN 256

//Time out interval for wait calls
#define WAIT_TIMEOUT_INTERVAL 100


class CClientContext  //To store and manage client related information
{
private:

	OVERLAPPED        *m_pol;
	WSABUF            *m_pwbuf;

	int               m_nTotalBytes;
	int               m_nSentBytes;

	SOCKET            m_Socket;  //accepted socket
	int               m_nOpCode; //will be used by the worker thread to decide what operation to perform
	char              m_szBuffer[MAX_BUFFER_LEN];

public:

	//Get/Set calls
	void SetOpCode(int n)
	{
		m_nOpCode = n;
	}

	int GetOpCode()
	{
		return m_nOpCode;
	}

	void SetTotalBytes(int n)
	{
		m_nTotalBytes = n;
	}

	int GetTotalBytes()
	{
		return m_nTotalBytes;
	}

	void SetSentBytes(int n)
	{
		m_nSentBytes = n;
	}

	void IncrSentBytes(int n)
	{
		m_nSentBytes += n;
	}

	int GetSentBytes()
	{
		return m_nSentBytes;
	}

	void SetSocket(SOCKET s)
	{
		m_Socket = s;
	}

	SOCKET GetSocket()
	{
		return m_Socket;
	}

	void SetBuffer(char *szBuffer)
	{
		strcpy(m_szBuffer, szBuffer);
	}

	void GetBuffer(char *szBuffer)
	{
		strcpy(szBuffer, m_szBuffer);
	}

	void ZeroBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

	void SetWSABUFLength(int nLength)
	{
		m_pwbuf->len = nLength;
	}

	int GetWSABUFLength()
	{
		return m_pwbuf->len;
	}

	WSABUF* GetWSABUFPtr()
	{
		return m_pwbuf;
	}

	OVERLAPPED* GetOVERLAPPEDPtr()
	{
		return m_pol;
	}

	void ResetWSABUF()
	{
		ZeroBuffer();
		m_pwbuf->buf = m_szBuffer;
		m_pwbuf->len = MAX_BUFFER_LEN;
	}

	//Constructor
	CClientContext()
	{
		m_pol = new OVERLAPPED;
		m_pwbuf = new WSABUF;

		ZeroMemory(m_pol, sizeof(OVERLAPPED));

		m_Socket =  SOCKET_ERROR;

		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);

		m_pwbuf->buf = m_szBuffer;
		m_pwbuf->len = MAX_BUFFER_LEN;

		m_nOpCode = 0;
		m_nTotalBytes = 0;
		m_nSentBytes = 0;
	}

	//destructor
	~CClientContext()
	{
		//Wait for the pending operations to complete
		while (!HasOverlappedIoCompleted(m_pol)) 
		{
			Sleep(0);
		}

		closesocket(m_Socket);

		//Cleanup
		delete m_pol;
		delete m_pwbuf;
	}
};


//global functions
bool InitializeIOCP();
bool Initialize();
void CleanUp();
void DeInitialize();
DWORD WINAPI AcceptThread(LPVOID lParam);
void AcceptConnection(SOCKET ListenSocket);
bool AssociateWithIOCP(CClientContext   *pClientContext);
DWORD WINAPI WorkerThread(LPVOID lpParam);
void AddToClientList(CClientContext   *pClientContext);
void RemoveFromClientListAndFreeMemory(CClientContext   *pClientContext);
void CleanClientList();

#endif