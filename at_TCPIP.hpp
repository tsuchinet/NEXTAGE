#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef u_int SOCKET;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *LPIN_ADDR;
typedef struct hostent *LPHOSTENT;
typedef struct sockaddr *LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
#define Sleep(micro) usleep(micro*1000)
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#define closesocket close
#define FALSE false
#define TRUE true

class at_TCPIP
{
public:
	bool cnct;
private:
	SOCKET s;	
public :
	bool TCPConnect(char *server,u_short port)
	{
		cnct=false;
		s = socket(PF_INET,SOCK_STREAM,0);
		if(s==INVALID_SOCKET) return false;
		LPHOSTENT ipHost = gethostbyname(server);
		if(ipHost == NULL) return false;
		SOCKADDR_IN sockadd;
		memset(&sockadd,0,sizeof(sockadd));
		sockadd.sin_family = AF_INET;
		sockadd.sin_port = htons(port);
		sockadd.sin_addr = *((LPIN_ADDR)*ipHost->h_addr_list);
		if(connect(s,(sockaddr*)&sockadd,sizeof(sockadd))!=0) return false;
		cnct=true;
		return true;
	}
	bool TCPSend(char* sendWord)
	{
		int nRtn;
		int i;
		if(!cnct)return FALSE;
		//�|�C���^�œn���ꂽ���M�������sizeof���g���Ȃ�����
		//���������J�E���g���܂�
		for(i=0;sendWord[i]!='\0';i++);
		nRtn = send(s,sendWord,i,0);//���M
		if(nRtn==SOCKET_ERROR || nRtn==0) return FALSE; else return TRUE;
	}
	int TCPRead(char*buff,int bfSize)
	{
		// �f�[�^����M����܂Ńu���b�N
		{
			fd_set readfds;
			FD_ZERO( &readfds ) ;
			FD_SET( s , &readfds );

			struct timeval timeout;
			timeout.tv_sec =  2;  // �҂�����
			timeout.tv_usec = 0;

			// select Function
			// �\�P�b�g�̏�Ԃ�ݒ肷��B�����ł͓ǎ��p�̃\�P�b�g�ɑ΂��ă^�C���A�E�g���w�肷��B
			int n = select( 
				0,             // ���g�p���ۂ�
				&readfds,      // �\�P�b�g���ǎ��\�����`�F�b�N����悤�ɂ���
				NULL,
				NULL,
				&timeout       // �^�C���A�E�g����
				);
			if( n == SOCKET_ERROR )
			{
				return -1;
			}
			// �^�C���A�E�g����
			if( n == 0 )
			{
				return -1;
			}
		}

		int nRtn=false;
		nRtn = recv(s,buff,bfSize,0);//��M
		if(nRtn==SOCKET_ERROR || nRtn==0) return -1;
		else{
			buff[nRtn]='\0';
			return nRtn;
		}
	}
	bool TCPRead2(unsigned char*buff,int width,int height)
	{
		int i;
		int cnt=0;
		for(i=0;i<height;i++){
			while(cnt-i*width<width){
				if(int ccnt = recv(s,(char*)buff+cnt, width,0)>0){
					cnt+=ccnt;
				}
				Sleep(10);
			}
		}
		if(cnt>0){
			buff[cnt]='\n';
			return TRUE;
		}
		else
			return FALSE;
	}
	void TCPClose()
	{
		closesocket(s);
	}
};
