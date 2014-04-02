#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

int isudp = 0;
int issrv = 0;
unsigned short srvport = 0;
const char* cltaddr=0;
unsigned short cltport = 0;

int parse_flag(int argc,char** argv)
{
	printf("before,ind:%d\n",optind);
	int c;
	while((c=getopt(argc,argv,"s:u"))!=-1)
	{
		switch(c)
		{
			case 's':
				issrv = 1;
				srvport = atoi(optarg);
				break;
			case 'u':
				isudp = 1;
				break;
			default:
				printf("optarg:%s\n",optarg);
				printf("optind:%d\n",optind);
				printf("c:%c\n",c);
				break;
		}
	}
	if(!issrv)
	{
		if(optind<argc)
		{
			cltaddr = argv[optind++];
		}
		else
		{
			printf("must specify address to connect!\n");
			return -1;
		}
		if(optind<argc)
		{
			cltport = atoi(argv[optind++]);
		}
		else
		{
			printf("must specify port to connect!\n");
			return -1;
		}
	}
	return 0;
}

void print_flag()
{
	printf("Flags:\t");
	if(isudp)
	{
		printf("<udp> ");
	}
	else
	{
		printf("<tcp> ");
	}
	if(issrv)
	{
		printf("<server> ");
	}
	else
	{
		printf("<client> ");
	}
	printf("\n");
}

int is_valid_addr(const char* addr)
{
	char* dupaddr = strdup(addr);
	char* token;
	int n=0;
	while((token=strsep(&dupaddr,"."))!=NULL)
	{
		int i=atoi(token);
		if(i==0 && errno!=0)
		{
			return 0;
		}
		if(i<0 | i>255)
		{
			return 0;
		}
		++n;
	}
	if(n!=4)
		return 0;
	return 1;
}

void do_srv_udp(unsigned short port)
{
	int sock = socket(PF_INET,SOCK_DGRAM,0);
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=INADDR_ANY;

	socklen_t addrlen = sizeof(struct sockaddr_in);
	if(bind(sock,(struct sockaddr*)&addr,addrlen)==-1)
	{
		perror("bind");
		return;
	}
	
	char buf[1024];
	for(;;)
	{
		if(-1==recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&addr,&addrlen))
		{
			perror("recvfrom");
			break;
		}
		printf("received:%s\n",buf);
	}
}

void do_clt_udp()
{
	int sock=socket(PF_INET,SOCK_DGRAM,0);

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(cltport);
	addr.sin_addr.s_addr=inet_addr(cltaddr);
	
	char buf[]="hello,udp!";
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int ret=sendto(sock,buf,sizeof(buf)+1,0,(struct sockaddr*)&addr,addrlen);
	if(ret==-1)
	{
		perror("sendto");
	}
	else
	{
		printf("write result:%d\n",ret);
	}
}

void do_srv_tcp(unsigned short port)
{
	printf("Tcp Server Not Implemented!\n");
}

void do_clt_tcp()
{
	printf("Tcp Client Not Implemented!\n");
}

int main(int argc,char** argv)
{
	if(-1==parse_flag(argc,argv))
	{
		return 1;
	}

	print_flag();

	// translate host name to ip address
	if(!issrv)
	{
		if(!is_valid_addr(cltaddr))
		{
			struct hostent* he = gethostbyname(cltaddr);
			if(he==NULL)
			{
				printf("Invalid Address!\n");
				return 1;
			}
			char str[1024];
			inet_ntop(AF_INET,he->h_addr,str,sizeof(str));
			cltaddr=strdup(str);
		}
	}

	if(issrv)
	{
		if(isudp)
			do_srv_udp(srvport);
		else
			do_srv_tcp(srvport);
	}
	else
	{
		if(isudp)
			do_clt_udp();
		else
			do_clt_tcp();
	}
	
	return 0;
}
