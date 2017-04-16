#include <stdio.h> 

#include <netinet/in.h> 

#include <netdb.h> 

#include <sys/time.h> 

#include <sys/types.h> 

#include <unistd.h> 

#include <stdlib.h> 

#include <signal.h> 

#include <pthread.h> 

#include <errno.h> 

#include <string.h> 

#include <sys/socket.h> 

#include <arpa/inet.h> 

#include <sys/wait.h>

#include <net/if.h>

#include <sys/ioctl.h>

#include <fcntl.h>

//#include <ifaddrs.h>







#include "common.h"

#include "sysCmdClient.h"

#include "resolve.h"

#include "socket5.h"





static sysClientInfo g_sysClientInfo = {};



int checkSysConnect(int sock)

{

    int ret = -1;

    struct timeval timeout;

    fd_set readfd;



    timeout.tv_sec = 10;

    timeout.tv_usec = 0;



    FD_ZERO(&readfd);

    FD_SET(sock, &readfd);



    ret = select(sock +1, NULL, &readfd, NULL, &timeout); 

    if(ret < 0)

    {

        return -1;

    }

    else if( ret == 0)

    {

        return -1; 

    }



    if(FD_ISSET(sock, &readfd))

    {

        int err = 0; 

        socklen_t len = sizeof(err);



        if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) < 0)

        {

            return -1; 

        }

        else if( err != 0)

        {

            return -1; 

        }

        else

        {

            return 0; 

        }

    

    }



    

    return -1;

}

int sendSysCmdReq(void)

{

	int ret = -1;

	int sock = -1;

	int nSend = 0, nRecv = 0;

	struct sockaddr_in addrCenter;

	SYS_CMD_HEAD sysCmdReqHead = {};

	SYS_CMD_REQ_BODY_EX sysCmdReqBody = {};

	SYS_CMD_HEAD sysCmdResHead = {};

	SYS_CMD_RES_BODY sysCmdResBody = {};

	int optval = 1;

	int CenterPort = 0;

	char ip[32];

    int flag = 0;

	



	if(!g_sysClientInfo.init)

	{

		 DEBUG(DEBUG_FATAL,"g_sysClientInfo is not init\n");

		 return -1;

	}





	sysCmdReqHead.mark[0] =  MAGIC_NUM_1;

	sysCmdReqHead.mark[1] =  MAGIC_NUM_2_EX;

	sysCmdReqHead.version = SYS_CMD_VERSION;

	sysCmdReqHead.modId   =  -1;

	sysCmdReqHead.modType =  SYS_USER_REQ;



	sprintf(sysCmdReqBody.userActionEx.userPort,"%d", g_sysClientInfo.userPort);

	strcpy(sysCmdReqBody.userActionEx.userName, g_sysClientInfo.userName);

	strcpy(sysCmdReqBody.userActionEx.userPass, g_sysClientInfo.userPass);

	DEBUG(DEBUG_FATAL,"userPort is %s\n", sysCmdReqBody.userActionEx.userPort);	

	

	





	 /* 在初始化的时候如果ADMIND_NETUPLOAD_EINCREATESOCKET，在这里再次尝试创建 */

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)

    {

        DEBUG(DEBUG_FATAL, "creat sock for upload failed !!!");

        ret = -1;

        goto EXIT0;

    }



    if((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&optval,sizeof(int))) < 0) 

    {

        DEBUG(DEBUG_FATAL, "setsockopt for upload failed !!!");

        ret = -1;

        goto EXIT0;

    }

    

    /* set NOBLOCK */

    flag = fcntl(sock, F_GETFL, 0);



    fcntl(sock, F_SETFL, flag | O_NONBLOCK);



    /*设置超时

       */

     struct timeval timeout;

    timeout.tv_sec = 5;

    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout)) < 0) 

    {

        DEBUG(DEBUG_FATAL, "setsockopt for upload failed !!!\n");

        ret = -1;

        goto EXIT0;

    }



    if (!resolve_ipEx(g_sysClientInfo.serverAddr, ip, SRV_ADDR))

    {

    	if(!resolve_ipEx(g_sysClientInfo.serverAddr, ip, SRV_ADDR_1))

    	{

    		DEBUG(DEBUG_FATAL, "can't get the resolve_ip \n");

			ret = -1;

        	goto EXIT0;

		}

       

    }

 

    bzero(&addrCenter, sizeof(struct sockaddr_in));

    addrCenter.sin_family = AF_INET;

    addrCenter.sin_port = htons(g_sysClientInfo.serverPort);

	inet_aton(ip, &addrCenter.sin_addr);

	



	 DEBUG(DEBUG_FATAL,"send request to %s\n", ip);

    //dbgPrint(TRACE, "alarmCenterIp=0x%x\n", htonl(pNetCfg->advance.nsAlertCfg.alertServerList[0].addr));

    

	ret =connect(sock,(struct sockaddr *)&addrCenter,sizeof(struct sockaddr));

	if(ret < 0 && errno == EINPROGRESS)

	{

        ret = checkSysConnect(sock); 

        if(ret < 0)

        {

             DEBUG(DEBUG_FATAL, "checkSysConnect error is %s !!!\n", strerror(errno));

            ret = -1;

            goto EXIT0;

        }

    }

    else if(ret < 0)

    {

          DEBUG(DEBUG_FATAL, "connect error is %s !!!\n", strerror(errno));

        ret = -1;

        goto EXIT0;

     

    }



    /* set BLOCK */

    flag = fcntl(sock, F_GETFL, 0);



    fcntl(sock, F_SETFL, flag & ~O_NONBLOCK);



    nSend = send(sock,&sysCmdReqHead,sizeof(SYS_CMD_HEAD),0);

    /*发送数据出错*/

    if(nSend !=  sizeof(SYS_CMD_HEAD))

    {

        DEBUG(DEBUG_FATAL, "send failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



	 nSend = send(sock,&sysCmdReqBody,sizeof(SYS_CMD_REQ_BODY_EX),0);

    /*发送数据出错*/

    if(nSend !=  sizeof(SYS_CMD_REQ_BODY_EX))

    {

        DEBUG(DEBUG_FATAL, "send failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }





    nRecv = recv(sock, &sysCmdResHead, sizeof(SYS_CMD_HEAD), 0);

    if(nRecv !=  sizeof(SYS_CMD_HEAD))

    {

        DEBUG(DEBUG_FATAL, "recv failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



	nRecv = recv(sock, &sysCmdResBody, sizeof(SYS_CMD_RES_BODY), 0);

    if(nRecv !=  sizeof(SYS_CMD_RES_BODY))

    {

        DEBUG(DEBUG_FATAL, "recv failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }

	

	g_sysClientInfo.modeId = sysCmdResHead.modId;



	ret = sysCmdResBody.cmdResult;



EXIT0:		  

	close(sock);



	return ret;



}

int sendUserState(void)

{

	int ret = -1;

	int sock = -1;

	int nSend = 0, nRecv = 0;

	struct sockaddr_in addrCenter;

	SYS_CMD_HEAD sysCmdReqHead = {};

	SYS_CMD_REQ_BODY_EX sysCmdReqBody = {};

	SYS_CMD_HEAD sysCmdResHead = {};

	SYS_CMD_RES_BODY sysCmdResBody = {};

	int optval = 1;

	int CenterPort = 0;

	char ip[32] = {0};

    int flag = 0;



	if(!g_sysClientInfo.init)

	{

		 DEBUG(DEBUG_FATAL,"g_sysClientInfo is not init\n");

		 return -1;

	}



	sysCmdReqHead.mark[0] =  MAGIC_NUM_1;

	sysCmdReqHead.mark[1] =  MAGIC_NUM_2_EX;

	sysCmdReqHead.version = SYS_CMD_VERSION;

	sysCmdReqHead.modId   =  g_sysClientInfo.modeId;

	sysCmdReqHead.modType =  SYS_USER_HEAT;

	//sysCmdInfo.userActionEx.userTime = time(NULL);

	



	// 只需要传送端口信息

	sprintf(sysCmdReqBody.userActionEx.userPort,"%d", g_sysClientInfo.userPort);

	strcpy(sysCmdReqBody.userActionEx.userName, g_sysClientInfo.userName);

	strcpy(sysCmdReqBody.userActionEx.userPass, g_sysClientInfo.userPass);

	DEBUG(DEBUG_FATAL,"userPort is %s\n", sysCmdReqBody.userActionEx.userPort);	





	 /* 在初始化的时候如果ADMIND_NETUPLOAD_EINCREATESOCKET，在这里再次尝试创建 */

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)

    {

        DEBUG(DEBUG_FATAL, "creat sock for upload failed !!!");

        ret = -1;

        goto EXIT0;

    }





    if((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&optval,sizeof(int))) < 0) 

    {

        DEBUG(DEBUG_FATAL, "setsockopt for upload failed !!!");

        ret = -1;

        goto EXIT0;

    }



 	 /*设置超时

       */

    struct timeval timeout;

    timeout.tv_sec = 5;

    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout)) < 0) 

    {

        DEBUG(DEBUG_FATAL, "setsockopt for upload failed !!!\n");

        ret = -1;

        goto EXIT0;

    }

	   

    /* set NOBLOCK */

    flag = fcntl(sock, F_GETFL, 0);



    fcntl(sock, F_SETFL, flag | O_NONBLOCK);



	if (!resolve_ipEx(g_sysClientInfo.serverAddr, ip, SRV_ADDR))

   {

	   if(!resolve_ipEx(g_sysClientInfo.serverAddr, ip, SRV_ADDR_1))

	   {

		   DEBUG(DEBUG_FATAL, "can't get the resolve_ip \n");

		   ret = -1;

		   goto EXIT0;

	   }

	  

   }

 

    bzero(&addrCenter, sizeof(struct sockaddr_in));

    addrCenter.sin_family = AF_INET;

    addrCenter.sin_port = htons(g_sysClientInfo.serverPort);

	inet_aton(ip, &addrCenter.sin_addr);

	



	DEBUG(DEBUG_FATAL,"send user state to %s\n", ip);

    

	ret =connect(sock,(struct sockaddr *)&addrCenter,sizeof(struct sockaddr));

	if(ret < 0 && errno == EINPROGRESS)

	{

        ret = checkSysConnect(sock); 

        if(ret < 0)

        {

             DEBUG(DEBUG_FATAL, "checkSysConnect error is %s !!!\n", strerror(errno));

            ret = -2;

            goto EXIT0;

        }

    }

    else if(ret < 0)

    {

          DEBUG(DEBUG_FATAL, "connect error is %s !!!\n", strerror(errno));

        ret = -2;

        goto EXIT0;

     

    }





    /* set BLOCK */

    flag = fcntl(sock, F_GETFL, 0);



    fcntl(sock, F_SETFL, flag & ~O_NONBLOCK);





    nSend = send(sock,&sysCmdReqHead,sizeof(SYS_CMD_HEAD),0);

    /*发送数据出错*/

    if(nSend !=  sizeof(SYS_CMD_HEAD))

    {

        DEBUG(DEBUG_FATAL, "send failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



	 nSend = send(sock,&sysCmdReqBody,sizeof(SYS_CMD_REQ_BODY_EX),0);

    /*发送数据出错*/

    if(nSend !=  sizeof(SYS_CMD_REQ_BODY_EX))

    {

        DEBUG(DEBUG_FATAL, "send failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



    nRecv = recv(sock, &sysCmdResHead, sizeof(SYS_CMD_HEAD), 0);

    if(nRecv !=  sizeof(SYS_CMD_HEAD))

    {

        DEBUG(DEBUG_FATAL, "recv failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



	 nRecv = recv(sock, &sysCmdResBody, sizeof(SYS_CMD_RES_BODY), 0);

    if(nRecv !=  sizeof(SYS_CMD_RES_BODY))

    {

        DEBUG(DEBUG_FATAL, "recv failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }

	

    if(sysCmdResBody.cmdResult != SYS_CMD_SUCCESS)

    {

		ret = -2;

    }



EXIT0:		  

	close(sock);



	return ret;



}

#if 0

void getIpAddr(char *ip)

{

	struct ifaddrs * ifAddrStruct=NULL;

	char tmpAddrBuff[64] = {0};

    void * tmpAddrPtr=NULL;



    getifaddrs(&ifAddrStruct);



    while (ifAddrStruct!=NULL) 

	{

        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) 

		{ // check it is IP4

            // is a valid IP4 Address

            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;

           

            inet_ntop(AF_INET, tmpAddrPtr, tmpAddrBuff, 64);



			if(strcmp(tmpAddrBuff, "**.**.**.**") != 0 &&\

			   strncmp(tmpAddrBuff, "192.168", 7) != 0)

			{

f(nRecv !=  sizeof(sysCmdResInfo))

    {

        DEBUG(DEBUG_FATAL, "recv failed !!! error:%s \n", strerror(errno));

        ret = -1;

        goto EXIT0;

    }



        g_sysClientInfo.modeId = sysCmdResInfo.modId;



        ret = sysCmdResInfo.cmdResult;				strcpy(ip, tmpAddrBuff);

				break;

			}

			

            //printf("%s IP Address %s/n", ifAddrStruct->ifa_name, tmpAddrBuff); 

        }

		

        ifAddrStruct=ifAddrStruct->ifa_next;

    }

}





void getIpAddr(char *ip)

{

	char hname[128];

    struct hostent *hent;

    int i;



    gethostname(hname, sizeof(hname));

	

    hent = gethostbyname(hname);



	// 只用只一个ip

    sprintf(ip, "%s", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])));



	printf("hostname: %s/naddress list: ", hent->h_name);

    for(i = 0; hent->h_addr_list[i]; i++) {

        printf("%s\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));

    }

	

    return;



}





void getIpAddr(char *netInterface, char *ip)

{

    int fd;

    struct ifreq ifr;

    struct sockaddr_in *our_ip = NULL;

	struct in_addr addr = {0};



    memset(&ifr, 0, sizeof(struct ifreq));

    if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) 

    {

        ifr.ifr_addr.sa_family = AF_INET;

        strncpy(ifr.ifr_name, netInterface, sizeof(ifr.ifr_name) - 1);



        /*ip*/

        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) 

        {

            our_ip = (struct sockaddr_in *) &ifr.ifr_addr;

			memcpy(&addr, &our_ip->sin_addr.s_addr, sizeof(our_ip->sin_addr.s_addr));

        }



        /* close socket */

        close(fd);

    }



	sprintf(ip, "%s", inet_ntoa(addr));



    return;

}





// Http Get协议

static int WebGetFull(void)

{

	int sock;

	int ret = 0;

	char check_tag[64] = {};

	char ip[32];

	char response_buff[10240] = {};

	char *host = "**.**.**.**";

	char hostIp[64] = {0};

	char *request_html ="GET http://**.**.**.**/ip_search138.asp?ip=**.**.**.** HTTP/1.1\r\n"

						"Host:**.**.**.**\r\n"	

						"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"

						"Connection:keep-alive\r\n\r\n";





    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sock == -1)

        return sock;



    struct sockaddr_in sin;



   	if (!resolve_ip(host, ip))

        return 0;



    inet_aton(ip, &sin.sin_addr);



    sin.sin_family = AF_INET;

    sin.sin_port = htons(80);



	DEBUG(DEBUG_FATAL, "connect  ip is  %s\n", ip);



    if(connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)

    {

    	DEBUG(DEBUG_FATAL, "connect  is error %s\n", strerror(errno));

        close(sock);

        return -1;

    }



	struct timeval timeout;

    timeout.tv_sec = 50;

    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout)) < 0) 

    {

        DEBUG(DEBUG_FATAL, "setsockopt for upload failed !!!\n");

        return -1;

    }

	

	

    ret = send(sock,request_html,strlen(request_html) ,MSG_NOSIGNAL);

	if(ret < strlen(request_html))

	{

		DEBUG(DEBUG_FATAL, "send request_html is error %s\n", strerror(errno));

		return -1;

	}



	DEBUG(DEBUG_FATAL, "send buffer is  %s\n", request_html);

	ret = recv(sock, response_buff, sizeof(response_buff), 0);

	if(ret < 0)

	{

		DEBUG(DEBUG_FATAL, "recv response_buff is error %s\n", strerror(errno));

		return -1;

	}



	DEBUG(DEBUG_FATAL, "%s", response_buff);



	char *p, *begain, *end;

	sprintf(check_tag, "<div>%s<br/>", "**.**.**.**");

	p = strstr(response_buff, check_tag);

	if(p != NULL)

	{

		end = strchr(p, ' ');

		*end = 0;

		

		printf("#######%s#########\n", p+ strlen("<div>**.**.**.**<br/>") + 18);

	}



	close(sock);





cleanup:



    return 0;

}

#endif



void getMacAddr(char *mac)

{

	int fd;

	struct ifreq ifrbuff[16] = {0};

	struct ifconf ifc = {0};

	struct sockaddr_in *addr = {0};

	char *address;

	int intrface = 0;



    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 

    {

	ifc.ifc_len = sizeof(ifrbuff);

	ifc.ifc_buf = (caddr_t)ifrbuff;



	if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc))

	{

		intrface = ifc.ifc_len / sizeof (struct ifreq);

		while(intrface-- > 0)

		{

			if(strcmp(ifrbuff[intrface].ifr_name, "lo") != 0)

			{

				if (ioctl(fd, SIOCGIFHWADDR, (char *) &ifrbuff[intrface]) == 0) 

				{

					sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x\n",

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[0],

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[1],

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[2],

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[3],

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[4],

					(unsigned char)ifrbuff[intrface].ifr_hwaddr.sa_data[5]);

					break;

				}

			}

		}

	}



        /* close socket */

        close(fd);

    }



	DEBUG(DEBUG_FATAL, "mac is %s\n", mac);



    return;

}



int sysCmdClientInit(SERVER_INFO* serverInfo, short clientPort)

{

	int ret = 0;



	if(g_sysClientInfo.init)

	{

		DEBUG(DEBUG_FATAL, "Client have inited\n");

		return 0;

	}



	g_sysClientInfo.modeId = -1;

	g_sysClientInfo.onlieTime = 0;

	strcpy(g_sysClientInfo.serverAddr, serverInfo->name);

	g_sysClientInfo.serverPort = serverInfo->port;

	g_sysClientInfo.userPort = clientPort;

	strcpy(g_sysClientInfo.userName, USER_NAME);

	getRandPasswd(g_sysClientInfo.userPass);



	DEBUG(DEBUG_FATAL, "the passwd is %s\n", g_sysClientInfo.userPass);



	

	g_sysClientInfo.init = 1;





	return 0;

}





char * sysGetUserName()

{

	if(!g_sysClientInfo.init)

	{

		DEBUG(DEBUG_FATAL, "Client not inited\n");

		return 0;

	}



	return g_sysClientInfo.userName;

}



char * sysGetUserPasswd()

{

	if(!g_sysClientInfo.init)

	{

		DEBUG(DEBUG_FATAL, "Client not inited\n");

		return 0;

	}



	return g_sysClientInfo.userPass;

}



void gettmpIpAddr(char *netInterface, char *ip)

{

    int fd;

    struct ifreq ifr;

    struct sockaddr_in *our_ip = NULL;

	struct in_addr addr = {0};



    memset(&ifr, 0, sizeof(struct ifreq));

    if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) 

    {

        ifr.ifr_addr.sa_family = AF_INET;

        strncpy(ifr.ifr_name, netInterface, sizeof(ifr.ifr_name) - 1);



        /*ip*/

        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) 

        {

            our_ip = (struct sockaddr_in *) &ifr.ifr_addr;

			memcpy(&addr, &our_ip->sin_addr.s_addr, sizeof(our_ip->sin_addr.s_addr));

        }



        /* close socket */

        close(fd);

    }



	sprintf(ip, "%s", inet_ntoa(addr));



    return;

}