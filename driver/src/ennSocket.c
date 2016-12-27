/**************************** (C) COPYRIGHT 2014 ENNO ****************************
 * �ļ���	��ennSocket.c
 * ����	��          
 * ʱ��     	��
 * �汾    	��
 * ���	��
 * ����	��  
**********************************************************************************/	


#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <netdb.h>
#include <linux/if_arp.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "ennAPI.h"
#include "ennDebug.h"

//head to c
#include <linux/if_arp.h>


#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif  /* __cplusplus */
#endif  /* __cplusplus */


#define ENNAPI_ETH_DEV_NAME "eth0"


/********************************************************************
* �� �� ����		ENNSock_Init
* ��������:		SOCKET��ʼ��
* �������:		��
* �������:		��
* �� �� ֵ:		�ɹ�����ENNO_SUCCESS
*					ʧ�ܷ���ENNO_FAILURE
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNSock_Init(ENN_VOID)
{
    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNSock_Socket
* ��������:		����һ��socket�ӿ�
* �������:		domain:  Э����(AF_INET��AF_INET6)
*                          	type:	ָ��socket����(SOCK_STREAM��SOCK_DGRAM��SOCK_RAW��SOCK_PACKET��SOCK_SEQPACKET)
*					protocol: ָ��Э��(IPPROTO_TCP��IPPTOTO_UDP��IPPROTO_SCTP��IPPROTO_TIPC)
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Socket(ENN_S32 domain, ENN_S32 type, ENN_S32 protocol)
{
   	 int 	sock_domain = AF_INET;
    	 int 	sock_type = SOCK_STREAM;
   	 int 	sock_protocol = IPPROTO_TCP;
   	 ENN_S32 err = 0;

	enn_api_trace(ENNAPI_TRACE_DEBUG, "domain = %x, type = %d, protocol = %d\n", domain, type, protocol);
   
	sock_domain = (int)domain;
	sock_type = (int)type;
	sock_protocol = (int)protocol;

	//printf("FUNCTION:%s, the sock_domain is %d, the sock_type is %d,the sock_protocol is %d\n", __FUNCTION__,sock_domain,sock_type, sock_protocol);
	err = (ENN_S32)socket(sock_domain, sock_type, sock_protocol);
	enn_api_trace(ENNAPI_TRACE_DEBUG, "socket return err = %d\n", err);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_Bind
* ��������:		���øýӿڽ�һ�����ص�ַ��������Э��
*					�壬�˿ں�IP��ָ������Ӧ��socket�ϣ�socket
*					�����κ�����֮ǰ��Ҫ���ñ��ӿ�ʵ��Э
*					���塢�˿ں�IP��ָ������������ͬPOSIX bind��
* �������:		sockfd:  		socket������
*                          	my_addr:		ָ��һ��cySock_SocketAddr�����ݽṹ�������ݽṹ������Ҫ���󶨵�socket�ĵ�ַ�������ַ�ĳ��Ⱥͳ�ʼ�����������socket�ĵ�ַ��
*					addrlen: 		my_addr�ĳ���
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Bind(ENN_S32 sockfd,  const ENNSock_SocketAddr *my_addr, ENN_S32 addrlen)
{
	ENN_S32 err = 0;
    ENNAPI_ASSERT(NULL != my_addr);

    err = (ENN_S32)bind((int)sockfd, (struct sockaddr *)my_addr, (socklen_t)addrlen);

	return err;
}



/********************************************************************
* �� �� ����		ENNSock_Connect
* ��������:		����ָ��IP�ķ���������������ͬPOSIX connect��
* �������:		sockfd:  		socket������
*                          	addr:		ָ��һ��cySock_SocketAddr�����ݽṹ�������ݽṹ������Ҫ���󶨵�socket�ĵ�ַ�������ַ�ĳ��Ⱥͳ�ʼ�����������socket�ĵ�ַ��
*					addrlen: 		addr�ĳ���
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Connect(ENN_S32 sockfd, const ENNSock_SocketAddr *addr, ENN_S32 addrlen)
{
	ENN_S32 err = 0;
    ENNAPI_ASSERT(NULL != addr);
	
    err = (ENN_S32)connect(sockfd, (struct sockaddr *)addr, (socklen_t)addrlen);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_Listen
* ��������:		ͨ��socket������ָ��һ��socket���ڼ�
*					��״̬�����ڼ���״̬��socket��ά��
*					һ������������У����ӵ������
*					���ɲ���backlog��������������ͬPOSIX listen��
* �������:		sockfd:  		socket������
*                          	backlog:		��������������
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Listen(ENN_S32 sockfd, ENN_S32 backlog)
{
	ENN_S32 err = 0;
	
    err = (ENN_S32)listen((int)sockfd, (int)backlog);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_Accept
* ��������:		���ڼ���״̬��socket�������������
*					��ȡ��������ǰ��һ���ͻ�����
*					���Ҵ���һ���µ�socket����ͻ�socket��
*					������ͨ����������ӳɹ����ͷ���
*					�´�����socket�����������ýӿ�ֻ��
*					�ڻ������ӵ�socketӦ�ã���������ͬPOSIX accept��
* �������:		sockfd:  		socket������
*                          	addr:		���ڴ�ſͻ���socket�����socket������ַ
*					addrlen:		�ͻ���socket��ַ����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Accept(ENN_S32 sockfd, ENNSock_SocketAddr *addr, ENN_S32 *addrlen)
{
	ENN_S32 err = 0;
    ENNAPI_ASSERT(NULL != addr);
	
    err = (ENN_S32)accept((int)sockfd, (struct sockaddr *)addr, (socklen_t *)addrlen);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_SocketSend
* ��������:		��ָ���������ӵ�socket�������ݣ���������ͬPOSIX send��
* �������:		sockfd:  		socket������
*                          	msg:		���������Ϣ��ַ
*					len:			��Ϣ���ȣ����ֽ�Ϊ��λ
*					flags:		��Ϣ�������ͱ�ʶ
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SocketSend(ENN_S32 sockfd, 
								  const ENN_VOID *msg, 
								  ENN_S32 len, 
								  ENN_S32 flags)
{
    ENN_S32 value = 0;
	ENN_S32 temp = 0;
    ENN_S32 err = 0;
    ENN_U32 i = 0;

	enn_api_trace(ENNAPI_TRACE_DEBUG, "fd = %x, flags = %x\n", sockfd, flags);

    ENNAPI_ASSERT(NULL != msg);

    value = 0;
    for(i=0; i<16; i++)
    {
        temp = flags & (0x1<<i);
        value |= temp;
    }
    flags = value;

	enn_api_trace(ENNAPI_TRACE_DEBUG, "fd = %x, flags = %x\n", sockfd, flags);
    err = (ENN_S32)send((int)sockfd, (void *)msg, (size_t)len, (int)flags);
	if(err <= 0) 
	{
		perror("cySock_SocketSend ");
		enn_api_trace(ENNAPI_TRACE_ERROR, "fd = %d, return err = %x\n", sockfd, err);
	}
	enn_api_trace(ENNAPI_TRACE_ERROR, "fd = %d, return err = %x\n", sockfd, err);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_SocketSendTo
* ��������:		��һ�����ӻ������״̬��socket����
*					���ݡ����socket�Ƿ�����ģʽ�ģ���
*					ô���ݽ�ͨ��ָ����Ŀ�ģ�to��socket��
*					���ͣ����socket������ģʽ�ģ���ô��
*					����to����������ͬPOSIX sendto��
* �������:		sockfd:  		socket������
*                          	msg:		���������Ϣ��ַ
*					len:			��Ϣ���ȣ����ֽ�Ϊ��λ
*					flags:		��Ϣ�������ͱ�ʶ
*					to:			��Ϣ���͵�Ŀ��socket��ַ
*					tplen:		Ŀ��socket��ַ����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SocketSendTo(ENN_S32 sockfd, 
									 const ENN_VOID *msg, 
									 ENN_S32 len,
									 ENN_S32 flags,
									 const ENNSock_SocketAddr *to, 
									 ENN_S32 tplen)
{
    ENN_S32 value = 0;
	ENN_S32 temp = 0;
    ENN_S32 err = 0;
    ENN_U32 i = 0;

    ENNAPI_ASSERT(NULL != msg);
    if(to == NULL)
    {
        return ENN_FAIL;
    }

    value = 0;
    for( i=0; i<16; i++)
    {
        temp = flags & (0x1<<i);
        value |= temp;
    }
    flags = value;

    err = (ENN_S32)sendto((int)sockfd, (void *)msg, (size_t)len, (int)flags, (struct sockaddr *)to, (socklen_t )tplen);
	if(err <= 0)
	{
		perror("ENNSock_SocketSendTo ");
		enn_api_trace(ENNAPI_TRACE_ERROR, "fd = %d, return err = %x\n", sockfd, err);
	}

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_SocketRecv
* ��������:		��ָ���������ӵ�socket�������ݣ���������ͬPOSIX recv��
* �������:		sockfd:  		socket������
*                          	buf:			��Ž������ݵĻ�����
*					len:			�������ݳ���
*					flags:		��Ϣ�������ͱ�ʶ
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SocketRecv(ENN_S32 sockfd, ENN_VOID *buf, ENN_S32 len, ENN_S32 flags)
{
    ENN_S32 value, temp;
    ENN_U32 i;
    ENN_S32 err;

	//enn_api_trace(ENNAPI_TRACE_DEBUG, "fd = %x, len = %d, flags = %x\n", sockfd, len, flags);


    ENNAPI_ASSERT(NULL != buf);
#if 0
    value = 0;
    for( i=0; i<16; i++)
    {
        temp = flags & (0x1<<i);
        #ifdef SOCKET_MAP
        value |= cySock_Map(CYSOCK_MAP_MSG,temp);    
        #else
        value |= temp;
        #endif
    }
    flags = value;
#endif

        err = (ENN_S32)recv((int)sockfd, (void *)buf, (size_t)len, (int)flags);
        if(err <= 0) 
	{
		perror("ENNSock_SocketRecv ");
		enn_api_trace(ENNAPI_TRACE_ERROR, "ERROR: fd = %d, return err = %x\n", sockfd, err);
	}

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_SocketRecvFrom
* ��������:		��һ�����ӻ��߷�����״̬��socket�������ݡ���������ͬPOSIX recvfrom��
* �������:		sockfd:  		socket������
*                          	buf:			��Ž������ݵĻ�����
*					len:			�������ݳ���
*					flags:		��Ϣ�������ͱ�ʶ
*					from:		Դ����socket��ַ
*					fromlen:		Դ����socket��ַ����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SocketRecvFrom(ENN_S32 sockfd,
										ENN_VOID *buf,
										ENN_S32 len,
                              			ENN_S32 flags,
                              			ENNSock_SocketAddr *from, 
                              			ENN_S32 *fromlen)
{  
    ENN_S32 value = 0;
	ENN_S32 temp = 0;
    ENN_S32 err = 0;
    ENN_U32 i = 0;

    ENNAPI_ASSERT(NULL != buf);

    value = 0;
    for( i=0; i<16; i++)
    {
        temp = flags & (0x1<<i);
        value |= temp;
    }
    flags = value;

    err = (ENN_S32)recvfrom((int)sockfd, (void *)buf, (size_t)len, (int)flags,(struct sockaddr *)from, (socklen_t *)fromlen);
	if(err <= 0) 
	{
		perror("ENNSock_SocketRecvFrom");
		enn_api_trace(ENNAPI_TRACE_ERROR, "ERROR: fd = %d, err = %x\n", sockfd, err);
	}

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_GetSockOpt
* ��������:		��ȡָ��socket��option��������������ͬPOSIX getsockopt��
* �������:		sockfd:  		socket������
*                          	level:		ָ��option���Э��
*					optname:		ָ�����Э���µ�optionѡ��
*					optval:		���ڴ�Ż�õ�ѡ������
*					optlen:		�ɹ�ʱ����optval�����ݵĳ��ȣ�����ʱ����ʱoptval��buffer����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_GetSockOpt(ENN_S32 sockfd, 
								ENN_S32 level,
								ENN_S32 optname,
								ENN_VOID *optval,
								ENN_S32  *optlen)
{
    ENN_S32 tmpLevel = 0;
	ENN_S32 tmpOptname = 0;
	ENN_S32 err = 0;
    
    ENNAPI_ASSERT(NULL != optval);
    ENNAPI_ASSERT(NULL != optlen);
    
    tmpLevel = level;    
    tmpOptname = optname;

    err = (ENN_S32)getsockopt((int)sockfd, (int)tmpLevel, (int)tmpOptname, (void *)optval, (socklen_t *)optlen);

	return err;
}


/********************************************************************
* �� �� ����		ENNSock_SetSockOpt
* ��������:		����ָ��socket��option��������������ͬPOSIX setsockopt��
* �������:		sockfd:  		socket������
*                          	level:		ָ��option���Э��
*					optname:		ָ�����Э���µ�optionѡ��
*					optval:		��Ҫ���õ�ѡ������
*					optlen:		ѡ�����ݵĳ���
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SetSockOpt(ENN_S32 sockfd, 
								ENN_S32 level, 
								ENN_S32 optname,
                                const ENN_VOID *optval,
                                ENN_S32  optlen)
{
    ENN_S32 tmpLevel = 0; 
    ENN_S32 tmpOptname = 0;
    int flags = 0; 
    
    tmpLevel = level;    
    tmpOptname = optname;    

    if(SO_NONBLOCK == optname)
    {
		/* Set socket to non-blocking */ 
		if((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
		{ 
			return -1;              
		} 
		else
		{
			return 0;
		}

		if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) 
		{ 
			return -1;               
		} 
		else
		{
			return 0;
		}
    }
    else
    {
		return setsockopt(sockfd, tmpLevel, tmpOptname, optval, optlen);
    }
}


/********************************************************************
* �� �� ����		ENNSock_SocketClose
* ��������:		���øýӿڹر�socket����socket��������ΪID��
* �������:		sockfd:  		socket������
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_SocketClose(ENN_S32 sockfd)
{
    return close((int)sockfd);
}


/********************************************************************
* �� �� ����		ENNSock_Select
* ��������:		���øýӿ�������������Ӷ���ļ�
*					���(file descriptor)��״̬�Ƿ���ָ����
*					�仯�������ͣ�ڸýӿ��ڲ��ȴ���
*					ֱ�������ӵ��ļ������ĳһ�����
*					��������״̬�ı�󷵻ء���������
*					ͬPOSIX select��
* �������:		n:  		socket������
*                          	readfds:		�����ӵĿ����ڶ����ļ���������
*					writefds:		�����ӵĿ�����д���ļ���������
*					exceptfds:	�����ӵĿ��ܳ����쳣���ļ���������
*					timeout:		��ʱʱ�ޣ�����ͬstruct timeval
* �������:		��
* �� �� ֵ:		�ɹ�����fd_set�е���������������ʱʱ�䵽ʱ����0�����򷵻�-1��
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Select(ENN_S32 n, 
						 ENNSock_fd_set *readfds, 
						 ENNSock_fd_set *writefds, 
						 ENNSock_fd_set *exceptfds,
       					 ENNSock_Timeval *timeout)
{
    ENN_S32 err;
#if 0
#if 0
    CYAPI_ASSERT(NULL != readfds);
    CYAPI_ASSERT(NULL != writefds);
    CYAPI_ASSERT(NULL != exceptfds);
#else
    ENNSock_fd_set TmpFds;
    memset((void *)&TmpFds, 0, sizeof(ENNSock_fd_set));
    if(NULL == readfds) readfds = &TmpFds;
    if(NULL == writefds) writefds = &TmpFds;
    if(NULL == exceptfds) exceptfds = &TmpFds;
#endif

    enn_api_trace(ENNAPI_TRACE_DEBUG, "n = %d\n", n);
    err = select(n, (fd_set *)readfds->fd_array, (fd_set *)writefds->fd_array, (fd_set *)exceptfds->fd_array, (struct timeval *)timeout);
#endif
        err = select(n, (fd_set *)readfds, (fd_set *)writefds, (fd_set *)exceptfds, (struct timeval *)timeout);
	if(err < 0)
	{
		perror("ENNSock_Select ");
		enn_api_trace(ENNAPI_TRACE_ERROR, "n = %d, return err = %d, timeout = %ld - %ld\n", n, err, timeout->tv_sec, timeout->tv_usec);
	}

	return err;
}



/********************************************************************
* �� �� ����		ENNSock_Ioctl
* ��������:		���øýӿڻ��socket��ص�ѡ�
* �������:		d:  		socket�ļ�������
*                          	request:		ָ��һ����һѡ����»��    ����
*            						... �� ������ǿ�ָ�룬ָ��洢���ݻ�����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Ioctl(ENN_S32 d, ENN_S32 request, ...)
{
    void *arg;
    va_list list;

    va_start(list, request);
    arg = va_arg(list, void *);

    va_end(list);

    return ioctl(d,request,arg); 

}


/********************************************************************
* �� �� ����		ENNSock_Shutdown
* ��������:		���øýӿڹر���һ��socket���ӵĲ�
*					�ֻ���ȫ��ͨѶ����������ͬPOSIX shutdown��
* �������:		s:  		���رյ�socket������
*                          	how:	ָ���Ĺر�����
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32  ENNSock_Shutdown(ENN_S32 s, ENN_S32 how)
{
    return shutdown(s,how);
}


/********************************************************************
* �� �� ����		ENNSock_FD_ZERO
* ��������:		��ʼ��һ���ļ�������������������ͬPOSIX FD_ZERO��
* �������:		set:  		ָ���������ļ���������ָ��
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_VOID ENNSock_FD_ZERO(ENNSock_fd_set *set)
{
    ENNAPI_ASSERT(NULL != set);
    FD_ZERO((fd_set *)set->fd_array);
}


/********************************************************************
* �� �� ����		ENNSock_FD_SET
* ��������:		���һ���ļ�������ļ�������������������ͬPOSIX FD_SET��
* �������:		fd:		socket������
*					set:  	fd��Ҫ������ļ���������
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_VOID ENNSock_FD_SET(ENN_S32 fd, ENNSock_fd_set *set)
{
    ENNAPI_ASSERT(NULL != set);
    FD_SET(fd, (fd_set *)set->fd_array);
}


/********************************************************************
* �� �� ����		ENNSock_FD_SET
* ��������:		���ָ���ļ�����Ƿ����˱仯����������ͬPOSIX FD_ISSET��
* �������:		fd:		socket������
*					set:  	ȷ��fd�Ƿ��Ǹ�����������һ��������
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_FD_ISSET(ENN_S32 fd, ENNSock_fd_set *set)
{
    ENNAPI_ASSERT(NULL != set);
    return FD_ISSET(fd, (fd_set *)set->fd_array);
}


/********************************************************************
* �� �� ����		ENNSock_Inet_ntoa
* ��������:		��ָ����Internet������ַת����Internet��׼�ַ����ַ�����
* �������:		in:		��ת����������ַ
* �������:		��
* �� �� ֵ:		�ɹ�����ת������ַ���ָ�루�ַ����洢�ھ�̬�ڴ����������򷵻�NULL��
* ��          ��:
********************************************************************/
ENN_CHAR *ENNSock_Inet_ntoa(ENNSock_In_Addr in)
{
    struct in_addr addr;
    addr.s_addr = (in_addr_t)in.s_addr;
    return inet_ntoa(addr);
}


/********************************************************************
* �� �� ����		ENNSock_Inet_aton
* ��������:		���ַ���ת��ΪInternet������ַ��
* �������:		cp:		XX.XX.XX.XX��ʽ�������ַ�ַ���
*					pin:  	���ڴ��ת�����������ַ
* �������:		��
* �� �� ֵ:		�ɹ�����ת�����������ַ�����򷵻أ�1��
* ��          ��:
********************************************************************/
ENN_S32 ENNSock_Inet_aton(ENN_CHAR *cp, ENNSock_In_Addr * pin)
{
    ENNAPI_ASSERT(NULL != cp);
    ENNAPI_ASSERT(NULL != pin);
    
    return inet_aton(cp,(struct in_addr *)pin);
}


/********************************************************************
* �� �� ����		ENNSoct_inet_addr
* ��������:		���ַ���ת��ΪInternet������ַ������ͬcySock_Inet_aton��
* �������:		cp:		XX.XX.XX.XX��ʽ�������ַ�ַ���
* �������:		��
* �� �� ֵ:		�ɹ�����������ַ�����򷵻�0xFFFFFFFF��
* ��          ��:
********************************************************************/
ENN_S32 ENNSoct_inet_addr(ENN_CHAR *cp)
{
    ENNAPI_ASSERT(NULL != cp);
    return inet_addr(cp);
}


/********************************************************************
* �� �� ����		ENNSock_htonl
* ��������:		���������������ֽ���ת���������ֽ���
* �������:		hostlong:		�����ֽ����U32����
* �������:		��
* �� �� ֵ:		���������ֽ����Ӧ�����ݡ�
* ��          ��:
********************************************************************/
ENN_U32 ENNSock_htonl(ENN_U32 hostlong)
{
    return htonl(hostlong);
}


/********************************************************************
* �� �� ����		ENNSock_htons
* ��������:		�Ѷ��������������ֽ���ת���������ֽ���
* �������:		hostshort:		�����ֽ����U16����
* �������:		��
* �� �� ֵ:		���������ֽ����Ӧ�����ݡ�
* ��          ��:
********************************************************************/
ENN_U16 ENNSock_htons(ENN_U16 hostshort)
{
    return htons(hostshort);
}


/********************************************************************
* �� �� ����		ENNSock_ntohl
* ��������:		���������������ֽ���ת���������ֽ���
* �������:		netlong:		�����ֽ����U32��������
* �������:		��
* �� �� ֵ:		���������ֽ����U32���ݡ�
* ��          ��:
********************************************************************/
ENN_U32 ENNSock_ntohl(ENN_U32 netlong)
{
    return ntohl(netlong);
}


/********************************************************************
* �� �� ����		ENNSock_ntohs
* ��������:		�Ѷ��������������ֽ���ת���������ֽ���
* �������:		netlong:		�����ֽ����U16��������
* �������:		��
* �� �� ֵ:		���������ֽ����U16���ݡ�
* ��          ��:
********************************************************************/
ENN_U16 ENNSock_ntohs(ENN_U16 netlong)
{
    return ntohs(netlong);
}


/********************************************************************
* �� �� ����		ENNSock_GetHostbyName
* ��������:		ͨ�����ƻ�ȡ������ַ����������ͬPOSIX gethostbyname��
* �������:		domainName:		���������ƣ��ַ�����XX.XX.XX.XX��ʽ��
* �������:		��
* �� �� ֵ:		�ɹ�����cySock_GetHostbyName�ṹ��ָ�룬���򷵻�NULL��
* ��          ��:
********************************************************************/
ENNSock_Hostent *ENNSock_GetHostbyName(const ENN_CHAR *domainName)
{
	ENNSock_Hostent *pHostent = NULL;

    ENNAPI_ASSERT(NULL != domainName);
	pHostent = (ENNSock_Hostent *)gethostbyname(domainName);
	if(!pHostent)
	{
		perror("ENNSock_GetHostbyName");
		enn_api_trace(ENNAPI_TRACE_ERROR, "FAILED: err = %x, domainName = %s\n", errno, domainName);
	}
	return pHostent;
}

int is_a_ip(char *ipaddr)
{
  char *pnum,*pdot=ipaddr;
  int i=0;

  for(;*ipaddr;ipaddr=pdot++,i++)
  {
    int t=0,e=1;
    if(*(pnum=pdot)=='.') return 1;

    while(*pdot!='.'&&*pdot)
    {
        ++pdot;
    }

    ipaddr = pdot-1;
    while(ipaddr>=pnum)
    {
        t+=e*(*ipaddr---'0');
        e*=10;
    }
    //if(t<0||t>255||(pdot-pnum==3&&t<100)||(pdot-pnum==2&&t<10))
    if(t<0||t>255)
        return 1;
  }

  if (i !=4 ) return 1;

  return 0;
}

static char* skip_space(char* line)
{
    char* p = line;
    while (*p == ' ' || *p == '\t')
    {
        p++;
    }
    return p;
}

static char* get_word(char* line, char* value)
{
    char* p = line;

    p = skip_space(p);
    while (*p != '\t' && *p != ' ' && *p != '\n' && *p != 0)
    {
        *value++ = *p++; 
    }
    *value = 0;
    return p;
}

ENN_ErrorCode_t ENNSock_EthernetIPAddressGet(ENN_CHAR *ipAdd)
{
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_in *s_in;

    ENNAPI_ASSERT(NULL != ipAdd);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "socket create fail!\n");
        return ENN_FAIL;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ENNAPI_ETH_DEV_NAME);
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "ioctl fail!\n");
        return ENN_FAIL;
    }
    
    s_in = (struct sockaddr_in *)(&ifr.ifr_addr);
    memcpy((void *)ipAdd, inet_ntoa(s_in->sin_addr), 15);

	enn_api_trace(ENNAPI_TRACE_ERROR, "ip = %s\n", ipAdd);
    close(sockfd);
    return ENN_SUCCESS;
}

ENN_ErrorCode_t ENNSock_EthernetIPAddressSet(ENN_CHAR* ipAdd)
{
    int sockfd;
    struct ifreq    ifr;
    struct sockaddr_in *sin;

    ENNAPI_ASSERT(NULL != ipAdd);

    if(is_a_ip(ipAdd) != 0)    
    {
		enn_api_trace(ENNAPI_TRACE_ERROR, "set eth ip failed!\n");
		return ENN_FAIL;
    }
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "socket create fail!\n");
        return ENN_FAIL;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ENNAPI_ETH_DEV_NAME, sizeof(ifr.ifr_name)-1);
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;

    if (inet_pton(AF_INET, ipAdd, &sin->sin_addr) <= 0)  // fix: change from < to <= for invalid ipAddr input
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "inet_pton  fail!\n");
        return ENN_FAIL;
    }

    if(ioctl(sockfd, SIOCSIFADDR, &ifr) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "set ipaddr fail!\n");
        return ENN_FAIL;
    }

    close(sockfd);
    return ENN_SUCCESS;    
}


ENN_ErrorCode_t ENNSock_EthernetSubNetmaskGet(ENN_CHAR* subNetmask)
{
    int skfd;
    struct ifreq ifr;
    struct sockaddr_in *s_in;

    ENNAPI_ASSERT(NULL != subNetmask);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "socket create fail!\n");
        return ENN_FAIL;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ENNAPI_ETH_DEV_NAME);
    if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "ioctl fail!\n");
        return ENN_FAIL;
    }

    s_in = (struct sockaddr_in *)(&ifr.ifr_netmask);
    
    memcpy((void *)subNetmask, inet_ntoa(s_in->sin_addr), 15);

    close(skfd);
    return ENN_SUCCESS;
}


ENN_ErrorCode_t ENNSock_EthernetSubNetmaskSet(ENN_CHAR* subNetmask)
{
    int sockfd;
    struct ifreq    ifr;
    struct sockaddr_in *sin;

    ENNAPI_ASSERT(NULL != subNetmask);

    if( is_a_ip(subNetmask) != 0)   
	{
		enn_api_trace(ENNAPI_TRACE_ERROR, "set netmask failed!\n");
		return ENN_FAIL;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "socket create fail!\n");
        return ENN_FAIL;
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ENNAPI_ETH_DEV_NAME, sizeof(ifr.ifr_name)-1);
    sin = (struct sockaddr_in *)&ifr.ifr_netmask;
    sin->sin_family = AF_INET;

    if (inet_pton(AF_INET, subNetmask, &sin->sin_addr) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "inet_pton  fail!\n");
        return ENN_FAIL;
    }

    if(ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0)
    {
        enn_api_trace(ENNAPI_TRACE_ERROR, "set netmask fail!\n");
        return ENN_FAIL;
    }
    
    close(sockfd);
    return ENN_SUCCESS;
}


ENN_ErrorCode_t cySock_EthernetGatewayGet(ENN_CHAR* gateway)
{
    char line[512];
    char str[16];
    FILE* fp;

    if(gateway == NULL)
    {
    	enn_api_trace(ENNAPI_TRACE_ERROR, "gateway is NULL!\n");
        return ENN_FAIL;
    }

    fp = fopen("/proc/net/route", "r");
    if (fp)
    {
        memset(str, 0, sizeof(str));
        while (fgets(line, sizeof(line), fp) != 0)
        {
            char* p = line;
            p = strstr(p, ENNAPI_ETH_DEV_NAME);
            if (p)
            {
                p = get_word(p, str);
                p = get_word(p, str);
                if (strcmp(str, "00000000") == 0)
                {
                    p = get_word(p, str);
                    if (strcmp(str, "00000000") != 0)
                    {
                        int a, b, c, d;			
                        sscanf(str, "%02X%02X%02X%02X", &a, &b, &c, &d);
                        sprintf(gateway, "%d.%d.%d.%d", d, c, b, a);
//			hi_cyapi_trace(CYAPI_TRACE_ERROR, "gw: %02x %02x %02x %02x\n", (PL_U8)d, (PL_U8)c, (PL_U8)b, (PL_U8)a);
                        fclose(fp);
                        return ENN_SUCCESS;
                    }
                }
            }
        }
        fclose(fp);        
    }

    strcpy(gateway, "0.0.0.0");    
    return ENN_FAIL;
}


ENN_ErrorCode_t ENNSock_EthernetGatewaySet(ENN_CHAR *gateway)
{
    char str[512];
    int rt=0;

    if(gateway == NULL)
    {
        return ENN_FAIL;
    }

//	hi_cyapi_trace(CYAPI_TRACE_ERROR, "gateway = %s\n", gateway);
	
    if( is_a_ip(gateway) != 0)  
	{
		enn_api_trace(ENNAPI_TRACE_ERROR, "set gateway failed!\n");
		return ENN_FAIL;
    }

    rt=sprintf(str, "route add default gw %s", gateway);
    if( rt < 0)
        return ENN_FAIL;

    rt = system(str);
    if( rt < 0) 
	{
		perror("ENNSock_EthernetGatewaySet: ");
        return ENN_FAIL;
    }
    else 
    {
        return ENN_SUCCESS;
    }
}


#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cpluscplus */
#endif /* __cpluscplus */


