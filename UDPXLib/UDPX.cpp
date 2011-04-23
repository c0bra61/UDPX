/*
 *	Created by C0BRA (Mitchel Collins) and DrSchnz (Dimity Z)
 *	Copyright 2011
 *	Many thanks to http://realdev.co.za/code/c-network-communication-using-udp for his socket class
 */

#define PLATFORM_WIN 0
#define PLATFORM PLATFORM_WIN

#define WSAErr() do{cerr << "WSAError: " << WSAGetLastError() << endl;}while(false)
#define PORT 27015

#include <winsock2.h>
#include "UDPX.h"
#include <iostream>
#include "windows.h"

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using namespace UDPX;

namespace UDPX
{
	bool InitSockets()
	{
		WSADATA WsaData;
		return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;	 
	}	 
	void UninitSockets()
	{
		WSACleanup();
	}

	
	UDPXAddress::UDPXAddress()
	{
		m_Address = 0;
		m_Port = 0;
	}
	UDPXAddress::UDPXAddress( unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short Port )
	{
		m_Address = (a << 24) | (b << 16) | (c << 8) | d; // this is not network byte order
		m_Port = Port;
	}
	UDPXAddress::UDPXAddress( unsigned int Address, unsigned short Port )
	{
		m_Address = Address;
		m_Port = Port;
	}
	unsigned int UDPXAddress::GetAddress() const//technically an IP is an unsigned long
	{
		return m_Address;
	}
	unsigned short UDPXAddress::GetPort() const
	{
		return m_Port;
	}


	








	Socket::Socket()
	{
		this->handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if (this->handle == INVALID_SOCKET)
			WSAErr();
	}

	bool Socket::Open(unsigned short port)
	{
		//set our ports etc
		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);
		int result = bind(this->handle,(const sockaddr*) &address,sizeof(sockaddr_in));
		if(result == SOCKET_ERROR)//incase another value below zero gets reserved to mean something other than '�rror'
			WSAErr();

		DWORD nonblocking = 1;
		if (ioctlsocket( this->handle,FIONBIO,&nonblocking) !=0)
		{
			cout<<"SOCKET FAILED TO SET NON-BLOCKING\n";
			WSAErr();
		}
		return 0;
	}
	void Socket::Close()
	{
	  closesocket(this->handle);
	}

	bool Socket::Send(const UDPXAddress& destination, const char* data, int size)
	{
		unsigned int dest_addr = destination.GetAddress();
		unsigned short dest_port = destination.GetPort();

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(dest_addr);
		address.sin_port = htons(dest_port);

		int sent_bytes = sendto( this->handle, data, size, 0, (sockaddr*)&address, sizeof(address) );
		if ( sent_bytes < size || this->handle == INVALID_SOCKET)//why would the socket explode after?
		{
			WSAErr();
			return false;
		}

		return true;
	}

	int Socket::Receive(UDPXAddress& sender, void* data, int size)
	{
		typedef int socklen_t;
		socklen_t fromLength = sizeof(sockaddr_in);
		sockaddr_in pAddr;
		cout << "recieving data..." << endl;
		int received_bytes = recvfrom( this->handle, (char*)data, size, 0,  (sockaddr*)&pAddr, &fromLength);//this passed sender for the sockaddr...(the dangers of c-casts arise!)
		
		if(received_bytes == SOCKET_ERROR)
		{
			WSAErr();
			return -1;
		}
		sender = UDPXAddress(pAddr.sin_addr.s_addr,pAddr.sin_port);//seeing as there are no set methods...
		return received_bytes;
	}




	
	UDPXConnection::UDPXConnection()
	{
	}
	UDPXConnection::UDPXConnection(UDPXAddress Address)
	{
		this->m_pAddress = &Address;
	}
	UDPXConnection::UDPXConnection(UDPXAddress* Address)
	{
		this->m_pAddress = Address;
	}
	void UDPXConnection::Send(BYTE Data)
	{
	}
	void UDPXConnection::SendUnchecked(BYTE Data)
	{
	}
	void UDPXConnection::Disconnect(void)
	{
	}
	void UDPXConnection::SetKeepAlive(double Time)
	{
	}
	void UDPXConnection::SetTimeout(double Time)
	{
	}
	void UDPXConnection::SetDisconnectEvent(DisconnectedFn fp)
	{
	}
	void UDPXConnection::SetReceivedPacketEvent(ReceivedPacketFn fp)
	{
	}
	UDPXAddress* UDPXConnection::GetAddress()
	{
		return this->m_pAddress;
	}

	/*
private:
	DisconnectedFn		m_pDisconnected;
	ReceivedPacketFn	m_ReceivedPacket;
	double				m_KeepAlive;
	double				m_Timeout;
	UDPXAddress*		m_pAddress;
	*/

	
	bool Listen(int Port, ConnectionHandelerFn connection)
	{
		
	}

	bool Connect(UDPXAddress* Address)
	{
		
	}
}


/*////




	int client(Socket s)
	{
		 cout<<"CLIENT\n";
	 
		 const char d[] = "hello world!";
		 Address a = Address(127,0,0,1,PORT);//127.0.0.1 is localhost (127.0.0.0 is the windows system gateway)
		 s.Send(a, d, sizeof(d));
		 cin.get();
		 return 0;
	}
	 
	int server(Socket s)
	{
		cout<<"SERVER\n";
	 
		while (true)
		{
			Address sender;
			unsigned char buffer[1024];//this was an array of 1024 char pointers...
			int br = s.Receive(sender, buffer, sizeof(buffer));//this passed a pointer to the array, instead or a pointer the the first element...
			if (br > 0)
			{
				cout << "Data Recived..." << endl;//debugging messages ftw!
				for (int i = 0; i < br; i++)
				{
					cout << buffer[i];
				}
				cout<<"\n";
			}
			else
				WSAErr();//errors wheren't checked, which resulted in error 10014 being unnoticed...
		}
		return 0;
	}
	 
	int mainnope(int argc, char *argv[])
	{
	 
		if (!InitSock())
			return 1; //if there is an error return 1!
		Socket s;
		 //socket opened on port g_port;

		if (argc > 1)
		{
			if (strcmp(argv[1],"-s") == 0)
			{
				  //this was in the wrong place, causing double+ binding of the same port, again a WSAError will tell you this ;)
				int res = s.Open(PORT);
				if (res < 0)
				{
					WSAErr();
					cin.get();
					return 1;
				}
 				server(s);
			 }
			 else
			 {
				client(s);
			 }
		 }
		 else
		 {
			  client(s);
		 }
	 
		 s.Close();
	 
		 StopSock();
		 return 0;
	}


	///*/