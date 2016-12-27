/**************************** (C) COPYRIGHT 2014 ENNO ****************************
 * �ļ���	��ennOS.c
 * ����	��          
 * ʱ��     	��
 * �汾    	��
 * ���	��
 * ����	��  
**********************************************************************************/	

#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <sched.h>
#include <sys/reboot.h>
#include <sys/resource.h>

#include "ennDebug.h"
#include "ennAPI.h"
#include "internal.h"


#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif  /* __cplusplus */
#endif  /* __cplusplus */

/******************* global variable   ******************/
static pthread_key_t sg_ennOS_taskkey;
ENN_BOOL g_ennOS_InitFlag = ENN_FALSE;



static pthread_mutex_t sg_TaskMutex = PTHREAD_MUTEX_INITIALIZER;


//1.�����߳�˽������
//2.�����߳�ϵͳ�ȼ�
//3.ִ���߳�˽�����ݵĺ����ͺ�������
static void* m_thread(void* arg)
{
    ENNTask *task;

    if(NULL == arg)
    {
        pthread_exit(NULL);
    }

    task = (ENNTask *)arg;
    pthread_setspecific(sg_ennOS_taskkey, (void *)task);

    if(ENNOS_TASK_SUSPEND == task->mode)
    {
        pthread_kill(task->pid, SIGSTOP );
    }
    ENNTRACE("\nm_thread: task->fuc=0x%08x  PID:%d\n\n",  task->fuc, getpid());

    if(ENNOS_TASK_PRIORITY_LOW == task->prio)
    {
        if(0 != setpriority(PRIO_PROCESS, getpid(), 10))
        {
            perror("setpriority: ");
        }
    }
    else if(ENNOS_TASK_PRIORITY_MIDDLE == task->prio)
    {
	      if(0 != setpriority(PRIO_PROCESS, getpid(), 5))
        {
            perror("setpriority: ");
        }
    }
	else
    {
        if(0 != setpriority(PRIO_PROCESS, getpid(), 1))
        {
            perror("setpriority: ");
        }
    }

    task->fuc( task->param );

    //memset((void *)task, 0, sizeof(cyTask));
    //free( task );
    //pthread_setspecific( sg_cyOS_taskkey, NULL );
    //task = NULL;

    return NULL;
}

ENN_U32 gWriteTraceEnable = 0;
ENN_VOID ENNWRITETRACE(ENN_CHAR *fmt, ...)
{
	if(gWriteTraceEnable)
	{
		va_list arg_ptr;
		va_start(arg_ptr, fmt);
		vfprintf(stdout, fmt, arg_ptr);
		va_end(arg_ptr);
	}
}



ENN_U32 gTraceEnable = 0;
ENN_VOID ENNTRACE(ENN_CHAR *fmt, ...)
{
	if(gTraceEnable)
	{
		va_list arg_ptr;
		va_start(arg_ptr, fmt);
		vfprintf(stdout, fmt, arg_ptr);
		va_end(arg_ptr);
	}
}


ENN_S32 ENN_strtol(const ENN_CHAR *nptr, ENN_CHAR **endptr, ENN_S32 base)
{
	const ENN_CHAR *p = nptr;
	ENN_ULONG ret;
	ENN_S32 ch;
	ENN_ULONG Overflow;
	ENN_S32 sign = 0, flag, LimitRemainder;

	/*����ǰ�����Ŀո񣬲��ж��������š�

	���base��0��������0x��ͷ��ʮ����������

	��0��ͷ��8��������

	���base��16����ͬʱҲ������0x��ͷ��*/
	do
	{
		ch = *p++;
	}while (isspace(ch));
  
	if(ch == '-')
	{
		sign = 1;
		ch = *p++;
	}
	else if(ch == '+')
	{
		ch = *p++;
	}

	if((base == 0 || base == 16) &&
      ch == '0' && (*p == 'x' || *p == 'X'))
	{
      ch = p[1];
      p += 2;
      base = 16;
	}

	if(base == 0)
	{
		base = ch == '0' ? 8 : 10;
	}
	
	Overflow = sign ? -(unsigned long)LONG_MIN : LONG_MAX;
	LimitRemainder = Overflow % (unsigned long)base;
	Overflow /= (unsigned long)base;
	
	for(ret = 0, flag = 0;; ch = *p++)
	{
		/*�ѵ�ǰ�ַ�ת��Ϊ��Ӧ��������Ҫ��ֵ��*/
		if (isdigit(ch))
			ch -= '0';
		else if (isalpha(ch))
	        ch -= isupper(ch) ? 'A' - 10 : 'a' - 10;
		else
			break;

		if(ch >= base)
			break;

		/*���������������ñ�־λ���Ժ��������㡣*/
		if(flag < 0 || ret > Overflow || (ret == Overflow && ch > LimitRemainder))
			flag = -1;
		else
		{
			flag = 1;
			ret *= base;
			ret += ch;
		}
	}

	/*
	���������򷵻���Ӧ��Overflow�ķ�ֵ��
	û����������Ƿ���λΪ������ת��Ϊ������
	*/
	if(flag < 0)
		ret = sign ? LONG_MIN : LONG_MAX;
	else if (sign)
		ret = -ret;
	
	/*
	���ַ�����Ϊ�գ���*endptr����ָ��nptr����
	����ָ��ֵ������*endptr����nptr���׵�ַ��
	*/

	if (endptr != 0)
		*endptr = (char *)(flag ?(p - 1) : nptr);

	return ret;
}

ENN_CHAR *ENN_Int_To_Format(ENN_S32 num, ENN_CHAR *str, ENN_S32 radix)   
{    
    /* ������ */   
    ENN_CHAR index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";   
    ENN_U32 unum; /* �м���� */   
    ENN_S32 i=0,j,k; 
	
    /* ȷ��unum��ֵ */   
    if((10 == radix) && (num < 0)) /* ʮ���Ƹ��� */   
    {   
        unum = (ENN_U32)-num;   
        str[i++] = '-';   
    }   
    else 
	{
		unum=(ENN_U32)num; /* ������� */  
	}
	
    /* ���� */   
    do    
    {   
        str[i++] = index[unum%(ENN_U32)radix];   
        unum /= radix;   
    }while(unum);
	
    str[i] = '\0';   
    /* ת�� */   
    if('-' == str[0]) 
	{
		k = 1; /* ʮ���Ƹ��� */ 
	}
    else 
	{
		k = 0;
	}
	
    /* ��ԭ���ġ�/2����Ϊ��/2.0������֤��num��16~255֮�䣬radix����16ʱ��Ҳ�ܵõ���ȷ��� */   
    ENN_CHAR temp;   
    for(j=k; j<=(i-k-1)/2.0; j++)   
    {   
        temp = str[j];   
        str[j] = str[i-j-1];   
        str[i-j-1] = temp;   
    }
	
    return str;   
}


ENN_U32 ENNStr_To_Hex(ENN_CHAR *str)
{
	ENN_U32 i,sum = 0;
	
	ENNAPI_ASSERT(NULL != str);
	
	for(i = 0;str[i] != '\0';i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
	        	sum = sum*16 + str[i]-'0';
		}
		else if( str[i]>='a'&&str[i]<='f' )
	    	{
		        sum = sum*16 + str[i]-'a'+10;
	    	}
		else if( str[i]>='A'&&str[i]<='F' )
	    	{
		        sum = sum*16 + str[i]-'A'+10;
	    	}

	}
	return sum;
} 


ENN_U32 ENNOS_GetSysTime(ENN_VOID)
{
    struct timeval tm_now;
    gettimeofday(&tm_now, NULL);
    return (ENN_U32)((tm_now.tv_sec)*1000 + tm_now.tv_usec/1000);
}



/********************************************************************
* �� �� ����		ENNOS_Init
* ��������:		OS��ʼ��
* �������:		��
* �������:		��
* �� �� ֵ:		�ɹ�����ENNO_SUCCESS
*					ʧ�ܷ���ENNO_FAILURE
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_Init(ENN_VOID)
{
	ENNTask *task;

    if(ENN_FALSE == g_ennOS_InitFlag)
    {
        task = (ENNTask *)malloc(sizeof(ENNTask));
        if(NULL == task)
        {
            return ENN_FAIL;
        }
        memset((void *)task, 0, sizeof(ENNTask));

        /* main task for MAPI init caller */
        task->name  = "Main thread";
        task->prio  = ENNOS_TASK_PRIORITY_MIDDLE;
        task->mode  = ENNOS_TASK_START;
        task->stack = 0x10000;  //64KB
        task->fuc 	= NULL;
        task->param = NULL;
        task->pid 	= pthread_self();

        /* init TSD */
        if(0 != pthread_key_create(&sg_ennOS_taskkey, NULL))
        {
            free(task);
			task = NULL;
            return ENN_FAIL;
        }

        /* set TSD to global variable */
        pthread_setspecific(sg_ennOS_taskkey, (void *)task);
	#if 0
		if ( 0 != timer_init())
		{
			free( task );
		    return PL_FAIL;
		 }
	#endif

        g_ennOS_InitFlag = ENN_TRUE;
        ENNTRACE("os init ok!\n");
        return ENN_SUCCESS;
    }

    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_CreateTask
* ��������:		���սӿڵ������������һ������
*					��Linux��Ϊһ�������̣߳����߳�
*					������Ϊ�ýӿڵĲ������룬����
*					�̺������ػ����˳�ʱ����������
*					Ҳͬʱ����
* �������:	*name:		��������
*					prio     :		�������ȼ�
*					stack   :		�����ջ��С
*					mode  :		���񴴽�ģʽ���ɲο�cyOS_TASK_CreateMode_t
*					fuc	    :		������ں���
*					*param:	���񴴽�ʱ����fuc�Ĳ���
* �������:		*id:		���óɹ�ʱ���ص�����ID
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_CreateTask(ENN_CHAR *name, 
									   	       ENNOS_TASK_Priority_t prio,
									               ENN_U32 stack, 
									               ENNOS_TASK_t *id,
									               ENNOS_TASK_CreateMode_t mode, 
									               ENNOS_TASKENTRY_f fuc,
									               ENN_VOID *param )
{
	ENNTask *task;
	pthread_attr_t attr;
	struct sched_param s_param;
	ENN_U32 stacksize = stack;
	ENN_S32 s_policy;
	static ENN_U32 taskcnt = 0;

	ENNAPI_ASSERT(NULL != id);

	pthread_mutex_lock(&sg_TaskMutex);                   //�����������ô����̹߳����е���ͼͬ��
	task = (ENNTask *)malloc(sizeof(ENNTask));
	if(NULL == task)
	{
		printf("ENNOS_CreateTask fail!\n");
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_FAIL;
	}

	memset((void *)task , 0 , sizeof(ENNTask));
	pthread_attr_init(&attr);

	if((stacksize > 0) && (stacksize < PTHREAD_STACK_MIN))
	{
		stacksize = PTHREAD_STACK_MIN;
	}
	else if(0 == stacksize)
	{
		pthread_attr_getstacksize(&attr, &stacksize);
	}

	if((prio < ENNOS_TASK_PRIORITY_LOW) || (prio > ENNOS_TASK_PRIORITY_HIGH))
	{
		prio = ENNOS_TASK_PRIORITY_MIDDLE;
	}
	printf("%s,%d,prio = %d\n",__FUNCTION__,__LINE__,prio);
	/* set sched policy */
	//s_policy = SCHED_RR;
	//s_policy = SCHED_OTHER;
	s_policy = SCHED_FIFO;
	pthread_attr_setschedpolicy(&attr, s_policy);

	/* set input task params */
	task->name	= name;
	task->prio	= prio;
	task->stack = stacksize;
	task->mode	= mode; /* Notes: mode not supported in linux! */
	task->fuc	= fuc;
	task->param = param;
        task->taskid = (ENN_U32)task | (taskcnt++ & 0x03);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
	pthread_attr_setstacksize(&attr, stacksize);

	pthread_attr_getschedparam(&attr, &s_param);
	s_param.sched_priority = prio;
	pthread_attr_setschedparam(&attr, &s_param);

	if(0 != pthread_create(&task->pid, &attr, m_thread ,task))
	{
		pthread_attr_destroy(&attr);
		free(task);
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_FAIL;
	}

	ENNTRACE("ENNOS_CreateTask task=0x%x, id = 0x%x, pid=%d name=%s priority=%d func=0x%08x\n\n", task, task->taskid, task->pid, task->name, task->prio, task->fuc);

	*id=(ENNOS_TASK_t)task->taskid;
	pthread_mutex_unlock(&sg_TaskMutex);  //�ͷ���
	return ENN_SUCCESS;
}



/********************************************************************
* �� �� ����		ENNOS_ExitTask
* ��������:		���øýӿ��˳���ǰ��������У�����ǰ��������ɾ��
* �������:		��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_ExitTask(ENN_VOID)
{
	ENNTask *task = NULL;

	pthread_mutex_lock(&sg_TaskMutex);
	task = (ENNTask *)pthread_getspecific(sg_ennOS_taskkey);
	if(NULL == task)
	{
		printf("ENNOS_ExitTask fail!");
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_FAIL;
	}
	/* add by SJC for debug*/
	ENNTRACE("ENNOS_ExitTask task=0x%x, id = 0x%x, pid=%d name=%s priority=%d func=0x%08x\n\n", task, task->taskid, task->pid, task->name, task->prio, task->fuc);

	memset((void *)task, 0, sizeof(ENNTask));
	free(task);
	task = NULL;

	pthread_mutex_unlock(&sg_TaskMutex);
	pthread_exit( NULL );
	return ENN_SUCCESS;/* just make the compiler smile */
}




/********************************************************************
* �� �� ����		ENNOS_DeleteTask
* ��������:		ɾ��һ������Ӧ�ÿ��Ե��øýӿ�
*					��ֹ��������Ҳ������ֹ��ǰ����
*					��������Ĳ���Ϊÿ�������ID�ţ�
*					ɾ������ǰҪȷ����ɾ����������ռ
*					�õ���Դ�Ѿ����ͷ�
* �������:		tid:  		�߳�ID��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_DeleteTask(ENNOS_TASK_t tid)
{
	ENNTask *task = NULL;

	pthread_mutex_lock(&sg_TaskMutex);
	task = (ENNTask *)(tid & 0xfffffffc);
	if(NULL == task)
	{
		printf("ENNOS_DeleteTask fail!");
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_FAIL;
	}
	/* add by SJC for debug*/
	ENNTRACE("ENNOS_DeleteTask task=0x%x, id=0x%x, pid=%d name=%s priority=%d func=0x%08x\n\n", task, task->taskid, task->pid, task->name, task->prio, task->fuc);

	if(task->taskid != tid)
	{
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_SUCCESS;
	}

	if(task->pid == 0)
	{
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_SUCCESS;
	}

	if(task->fuc == NULL)
	{
		pthread_mutex_unlock(&sg_TaskMutex);
		return ENN_SUCCESS;
	}

	if(task->pid ==  pthread_self())
	{
		pthread_mutex_unlock(&sg_TaskMutex);
		pthread_exit(NULL);
		return ENN_SUCCESS;
	}

	pthread_cancel(task->pid);
	//pthread_join(task->pid, NULL);

	memset((void *)task, 0, sizeof(ENNTask));
	free(task);
	task = NULL;

	pthread_mutex_unlock(&sg_TaskMutex);
	
	return ENN_SUCCESS;
}



/********************************************************************
* �� �� ����		ENNOS_SuspendTask
* ��������:		��ͣһ�����е�����������IDΪ��
*					ʶ����������Ѿ���ͣ��ֱ�ӷ��سɹ�
* �������:		tid:  		�߳�ID��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_SuspendTask (ENNOS_TASK_t tid)
{
	ENNTask *task;
	
	task = (ENNTask *)(tid & 0xfffffffc);
	if(NULL == task)
	{
		printf("ENNOS_SuspendTask fail!");
		return ENN_FAIL;
	}
	/* add by SJC for debug*/
	ENNTRACE("ENNOS_SuspendTask id=%d name=%s priority=%d func=0x%08x\n\n", task->pid, task->name, task->prio, task->fuc);
	if(0 == pthread_kill(task->pid, SIGSTOP))
	{
		return ENN_SUCCESS;
	}
	else
	{
		return ENN_FAIL;
	}
}



/********************************************************************
* �� �� ����		ENNOS_ResumeTask
* ��������:		����������ͣ�������������������״̬����ֱ�ӷ���
* �������:		tid:  		�߳�ID��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_ResumeTask(ENNOS_TASK_t tid)
{
	ENNTask *task = NULL;

	task = (ENNTask *)(tid & 0xfffffffc);
	if(NULL == task)
	{
		printf("ENNOS_ResumeTask fail!");
		return ENN_FAIL;
	}
	/* add by SJC for debug*/
	ENNTRACE("ENNOS_ResumeTask id=%d name=%s priority=%d func=0x%08x\n\n", task->pid, task->name, task->prio, task->fuc);
	if(0 == pthread_kill(task->pid , SIGCONT))
	{
		return ENN_SUCCESS;
	}
	else
	{
		return ENN_FAIL;
	}
}



/********************************************************************
* �� �� ����		ENNOS_DelayTask
* ��������:		���õ������������ʱ�䡣������õ�ʱ��С��ϵͳ��Сʱ��Ƭ���Ҳ�����0��Linux��Сʱ��ƬΪ10���룩����������Ϊϵͳ��Сʱ��Ƭ������Ĳ����Ժ���Ϊ��λ
* �������:		delay:  		��ʱʱ�䣬�Ժ���Ϊ��λ
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_DelayTask(ENN_U32 delay)
{
	if(0 == delay)
	{
		sched_yield();
	}
	else
	{
		usleep(delay*1000);
	}

	return ENN_SUCCESS;
}



/********************************************************************
* �� �� ����		ENNOS_GetTaskPriority
* ��������:		��ȡָ����������ȼ������ָߡ��С����������ȼ������嶨��ɲο�cyOS_TASK_Priority_t
* �������:		tid:  		�߳�ID��
* �������:		*priority:		��ȡ���߳����ȼ�
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_GetTaskPriority(ENNOS_TASK_t tid, ENNOS_TASK_Priority_t *priority)
{
	ENNTask *task;
	ENNAPI_ASSERT(NULL != priority);

	task  = (ENNTask *)(tid & 0xfffffffc);
	if ((task->prio < ENNOS_TASK_PRIORITY_LOW)||(task->prio > ENNOS_TASK_PRIORITY_HIGH))
	{
		printf("tid:%d task prio error!\n",tid);
		return ENN_FAIL;
	}

	*priority = task->prio;
	return ENN_SUCCESS;
}



/********************************************************************
* �� �� ����		ENNOS_SetTaskPriority
* ��������:		����ָ����������ȼ�
* �������:		tid:  		�߳�ID��
*                          	newPriority:	�µ��߳����ȼ�
* �������:		��
* �� �� ֵ:		�ɹ�����0
*					ʧ�ܷ���-1
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_SetTaskPriority(ENNOS_TASK_t tid, ENNOS_TASK_Priority_t newPriority)
{
	ENNTask *task;

	task = (ENNTask *)(tid & 0xfffffffc);

	if ((newPriority < ENNOS_TASK_PRIORITY_LOW)||(newPriority > ENNOS_TASK_PRIORITY_HIGH))
	{
		printf("tid:%d task prio error!\n",tid);
		return ENN_FAIL;
	}

	task->prio = newPriority;

	return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_CreateSemaphore
* ��������:		�����ź�������ΪFIFO���ͺ����ȼ����ͣ����óɹ����ȡ�����ź�����ID
* �������:		value: 		��ʼֵ
*                          	      semaMode:	�ź���ģʽ
* �������:		semaId:		��ȡ���ź���
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_CreateSemaphore(ENN_U32 value,
                                              ENNOS_SEMA_CreateMode_t semaMode,
                                              ENNOS_SEMA_t *semaId)
{
    ENNSemaphore *msem;

    msem = (ENNSemaphore *)malloc(sizeof(ENNSemaphore));
    if(NULL == msem)
    {
        return ENN_FAIL;
    }
    memset((void *)msem, 0, sizeof(ENNSemaphore));

    msem->semaMode = semaMode ;
    if(0 != sem_init(&msem->sem , 0, value))
    {
        free(msem);
		msem = NULL;
        return ENN_FAIL;
    }

    *semaId = (ENNOS_SEMA_t)msem;
    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_DeleteSemaphore
* ��������:		ɾ���ź��������ź�����IDΪ��ʶ
* �������:		semaphore: 		�ź���ID
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_DeleteSemaphore(ENNOS_SEMA_t semaphore)
{
    ENNSemaphore *msem = NULL;
    int rt = -1;

	if(0 == semaphore) 
	{
		printf("semaphore is NULL\n");
		return ENN_FAIL;
	}
    msem = (ENNSemaphore *)semaphore;

    rt = sem_destroy(&msem->sem);

    if(0 == rt)
    {
        free(msem);
		msem = NULL;
        return ENN_SUCCESS;
    }
    else
    {
        perror("sema del::");
        return ENN_FAIL;
    }
}


/********************************************************************
* �� �� ����		ENNOS_WaitSemaphore
* ��������:		�ȴ�ָ��ID���ź��������ָ���ĳ�ʱʱ�䵽ʱ�ź�����Ȼû�л�þͷ���ʧ��
* �������:		semaphore: 		�ź���ID
*                          	timeout:			�ź����ȴ���ʱʱ��,�Ժ���Ϊ��λ
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_WaitSemaphore(ENNOS_SEMA_t semaphore, ENN_U32 timeout)
{
    struct timeval   now;
    struct timespec  ts;
    int              rt;
    ENNSemaphore      *msem = NULL;

	if(0 == semaphore)
	{
		printf("semaphore is NULL\n");
		return ENN_FAIL;
	}

    msem = (ENNSemaphore *)semaphore;
    switch(timeout)
    {
        case ENNOS_TIMEOUT_IMMEDIATE:
        {
            rt = sem_trywait(&msem->sem);
            break;
        }
        case ENNOS_TIMEOUT_INFINITY:
        {
            rt = sem_wait(&msem->sem);
            break;
        }
        default:
        {
            gettimeofday( &now, NULL );
            ts.tv_sec   = now.tv_sec + timeout/1000 ;
            ts.tv_nsec  = now.tv_usec * 1000 + timeout%1000*1000000;
            ts.tv_sec  += ts.tv_nsec/1000000000;
            ts.tv_nsec = ts.tv_nsec%1000000000;
            rt     = sem_timedwait( &msem->sem ,&ts );
            break;
        }
    }

    if(0 == rt)
    {
        return ENN_SUCCESS;
    }
    else
    {
        return ENN_FAIL;
    }
}


/********************************************************************
* �� �� ����		ENNOS_SignalSemaphore
* ��������:		�ͷ�һ���ź��������ź���ID��Ϊ��ʶ
* �������:		semaphore: 		�ź���ID
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_SignalSemaphore (ENNOS_SEMA_t semaphore)
{
    ENNSemaphore   *msem = NULL;
    int          rt = -1;

	if(0 == semaphore) 
	{
		printf("semaphore is NULL\n");
		return ENN_FAIL;
	}

    msem = (ENNSemaphore *)semaphore;
    rt = sem_post(&msem->sem);
    if(0 == rt)
    {
        return ENN_SUCCESS;
    }
    else
    {
        return ENN_FAIL;
    }
}


/********************************************************************
* �� �� ����		ENNOS_CreateMessage
* ��������:		����һ����Ϣ���У��û�����Ҫ��������Ϣ���е���Ϣ��С����Ϣ��������Ϣ�ȴ���ʽ��ִ�гɹ�ʱ����õ�����Ϣ����ID
* �������:		elementSize:DATAQ_FRAME�ṹ��Ĵ�С
*                          		noElements:	DATAQ_FRAME�ṹ��ĸ���
*                          		msgMode:	��Ϣ���е�ģʽ
* �������:		message:
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_CreateMessage (ENN_U32 elementSize,ENN_U32 noElements, 
												    ENNOS_MSG_CreateMode_t msgMode, ENNOS_MSG_t *message)
{
    ENNMessage *mmsg = NULL;

    ENNAPI_ASSERT(NULL != message);

    mmsg = (ENNMessage *)malloc(sizeof(ENNMessage) + elementSize * noElements);
    if(NULL== mmsg)
    {
        return ENN_FAIL;
    }
    memset((void *)mmsg , 0 , sizeof(ENNMessage) );

    mmsg->elementSize = elementSize;
    mmsg->noElements  = noElements;
    mmsg->msgMode     = msgMode;
    mmsg->msgs        	= (ENN_VOID*)(mmsg+1);
    mmsg->begin             = 0;
    mmsg->num              = 0;

    pthread_mutex_init(&mmsg->mutex , NULL);  //��������ʼ��
    sem_init(&mmsg->sem, 0, 0);                          //�ź�����ʼ��

    *message = (ENNOS_MSG_t)mmsg;
    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_DeleteMessage
* ��������:		ɾ��ָ������Ϣ���У�����Ϣ����ID��Ϊ��ʶ������Ϣ����ɾ���Ժ��������ڵȴ����ո���Ϣ������Ӧ�÷���ʧ��
* �������:		messageQueue: 
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_DeleteMessage(ENNOS_MSG_t messageQueue)
{
    ENNMessage   *mmsg = NULL;

    mmsg = (ENNMessage *)messageQueue;
    if(0 != sem_destroy(&mmsg->sem))
    {
        perror("msg del::");
        return ENN_FAIL;
    }

    pthread_mutex_destroy(&mmsg->mutex);
    free(mmsg);
	mmsg = NULL;
    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_ReceiveMessage
* ��������:		��ָ������Ϣ���н���һ����Ϣ�������ָ����ʱ����û���յ���Ϣ��������ʧ��
* �������:		messageQueue: 	������ϢID��
*                          	      timeout:			������Ϣ��ʱʱ��
* �������:		msg:			���յ�����Ϣ���ָ��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t	ENNOS_ReceiveMessage(ENNOS_MSG_t messageQueue,
													ENN_U32 timeout, ENN_VOID *msg)
{
	int 		rt = ENN_SUCCESS;
	ENNMessage	 *mmsg = NULL;
	char		*dst = NULL;

	struct timeval	 now;
	struct timespec  ts;

	mmsg = (ENNMessage *)messageQueue;

	switch (timeout)
	{
		case ENNOS_TIMEOUT_IMMEDIATE:
		{
			rt = sem_trywait(&mmsg->sem);
			break;
		}
		case ENNOS_TIMEOUT_INFINITY:
		{
			rt = sem_wait(&mmsg->sem);
			break;
		}
		default:
		{
			gettimeofday( &now, NULL );
			ts.tv_sec	= now.tv_sec + timeout/1000 ;
			ts.tv_nsec	= now.tv_usec * 1000 + timeout%1000*1000000;
			ts.tv_sec  += ts.tv_nsec/1000000000;
			ts.tv_nsec = ts.tv_nsec%1000000000;
			rt	   = sem_timedwait(&mmsg->sem, &ts);
			break;
		}
	}

	if (0 != rt)
	{
		return ENN_FAIL;
	}

	pthread_mutex_lock(&mmsg->mutex);
	dst = (char *)mmsg->msgs;
	dst += mmsg->begin * mmsg->elementSize;

	memcpy(msg, dst, mmsg->elementSize);

	mmsg->begin = (mmsg->begin + 1) % mmsg->noElements;
	mmsg->num--;
	pthread_mutex_unlock(&mmsg->mutex);

	return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_SendMessage
* ��������:		��ָ���Ķ��з���һ����Ϣ������Ϣ����IDΪ��
                    ʶ�������������������ʧ��
* �������:		messageQueue:  		������ϢID��
*                          	msg:	���͵���Ϣ���ָ��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_SendMessage(ENNOS_MSG_t messageQueue, ENN_VOID * msg)
{
	ENNMessage		*mmsg = NULL;
	ENN_ErrorCode_t	rt = ENN_SUCCESS;
	char		   *dst = NULL;

	mmsg = (ENNMessage *)messageQueue;

	pthread_mutex_lock(&mmsg->mutex);

	// too many msgs ?
	if(mmsg->num >= mmsg->noElements)
	{
		rt = ENN_FAIL;
	}
	else
	{
		/* look for the position that can store msg */
		dst = (char *) mmsg->msgs;
		if(mmsg->msgMode == ENNOS_MSG_CREATE_FIFO)
		{
			dst += ( (mmsg->begin + mmsg->num) % mmsg->noElements ) * mmsg->elementSize;
		}
		else
		{
			/* CYOS_MSG_CREATE_PRIORITY  ??? */
			mmsg->begin = (mmsg->begin>0) ? (mmsg->begin-1) : (mmsg->noElements-1);
			dst += (mmsg->begin) * mmsg->elementSize;
		}

		memcpy( dst, msg, mmsg->elementSize );
		mmsg->num++;
	}

	pthread_mutex_unlock(&mmsg->mutex);

	if(ENN_SUCCESS == rt)
	{
		sem_post(&mmsg->sem);
	}
	return rt;
}


/********************************************************************
* �� �� ����		ENNOS_EmptyMessage
* ��������:		���ָ������Ϣ�����е�������Ϣ������Ϣ����IDΪ��ʶ
* �������:		messageQueue:  		������ϢID��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_EmptyMessage(ENNOS_MSG_t messageQueue)
{
    ENNMessage        *mmsg = NULL;

    mmsg = (ENNMessage *)messageQueue;
    while (0 == sem_trywait(&mmsg->sem))
    {
        pthread_mutex_lock(&mmsg->mutex);
        mmsg->num--;
        pthread_mutex_unlock(&mmsg->mutex);
    }

    return  ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_AllocMemory
* ��������:		����ָ����С���ڴ棬����ʧ�ܷ��ؿ�ָ�룬���ñ��ӿڵõ����ڴ������cyOS_FreeMemory�ӿ��ͷţ����������ڴ�й©
* �������:		requested:  		������ڴ���С�����ֽ�Ϊ��λ
* �������:		��
* �� �� ֵ:		ֱ�ӽ�malloc�ķ���ֵ��Ϊ�ӿڷ���ֵ��
* ��          ��:
********************************************************************/
ENN_VOID* ENNOS_AllocMemory(ENN_U32 requested)
{
	ENN_VOID *pTmp = NULL;
    pTmp = (ENN_VOID *)malloc(requested);
	if (NULL == pTmp)
	{
		printf("alloc %d bytes fail!\n",requested);
	}
	return pTmp;
}


/********************************************************************
* �� �� ����		ENNOS_ReallocMemory
* ��������:		��ԭ������ڴ���������»�ȡ�µ��ڴ��С������Ϊԭ���ڴ����׵�ַ����Ҫ���·�����ڴ��С������ɹ��򷵻��·�����ڴ��ָ�룬���򷵻ؿ�ָ��
* �������:		block:			 ԭ�ڴ��ָ��
*					requested:  		������ڴ���С�����ֽ�Ϊ��λ
* �������:		��
* �� �� ֵ:		�ɹ�����׷���ڴ������ڴ��ָ�롣
* ��          ��:
********************************************************************/
ENN_VOID* ENNOS_ReallocMemory(ENN_VOID *block, ENN_U32 requested)
{
    ENNAPI_ASSERT(block != NULL);
    return realloc(block, requested);
}


/********************************************************************
* �� �� ����		ENNOS_FreeMemory
* ��������:		�ͷ�ָ�����ڴ�飬���ڴ�����ʱͨ��ENNOS_AllocMemory��cyOS_ReallocMemory����õ���
* �������:		block:			 �ͷŵ��ڴ��ָ��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_ErrorCode_t ENNOS_FreeMemory(ENN_VOID * block)
{
    ENNAPI_ASSERT(block != NULL);
    free(block);
	block = NULL;
    return ENN_SUCCESS;
}


/********************************************************************
* �� �� ����		ENNOS_GetFreeMemory
* ��������:		��ȡϵͳ��ǰ�Ŀ����ڴ��С�����ֽ�Ϊ��λ
* �������:		��
* �������:		��
* �� �� ֵ:		�ɹ����ص�ǰϵͳ���õ����ڴ��С��
* ��          ��:
********************************************************************/
ENN_S32 ENNOS_GetFreeMemory(ENN_VOID)
{
    struct sysinfo info;

    if (0 != sysinfo(&info))
    {
        return ENN_FAIL;
    }
    return (ENN_S32)info.freeram;
}


/********************************************************************
* �� �� ����		ENNOS_GetUsedMemory
* ��������:		��ȡϵͳ��ǰʹ�õ��ڴ��С�����ֽ�Ϊ��λ
* �������:		��
* �������:		��
* �� �� ֵ:		�ɹ�����ENN_SUCCESS
*					ʧ�ܷ���ENN_FAIL
* ��          ��:
********************************************************************/
ENN_S32 ENNOS_GetUsedMemory(ENN_VOID)
{
    struct sysinfo info;

    if (0 != sysinfo(&info))
    {
        return ENN_FAIL;
    }
    return (ENN_S32)(info.totalram - info.freeram);
}


/********************************************************************
* �� �� ����		ENNOS_GetMaxFreeMemory
* ��������:		��õ�ǰ����������ڴ�Ĵ�С������ǰϵͳ��������������ڴ棬���ֽ�Ϊ��λ
* �������:		��
* �������:		��
* �� �� ֵ:		���ص�ǰϵͳ���ڴ�������
* ��          ��:
* ��	      ��: 	�ݲ�֧�ֱ�����
********************************************************************/
ENN_S32 ENNOS_GetMaxFreeMemory(ENN_VOID)
{
    printf("dummy call!\n");
    return ENN_FAIL;
}


/********************************************************************
* �� �� ����		ENNOS_SwReset
* ��������:		��λϵͳ������������
* �������:		��
* �������:		��
* �� �� ֵ:		��
* ��          ��:
********************************************************************/
ENN_VOID ENNOS_SwReset(ENN_VOID)
{
    //sleep(1);
	printf("%s, %d\n",__FUNCTION__,__LINE__);
    	//reboot(RB_AUTOBOOT);
    	system("reboot");
}



ENN_VOID cyTRACE_Enable(ENN_BOOL bEnable)
{
	if (bEnable)
	{
		gTraceEnable = ENN_TRUE;
	}
	else
	{
		gTraceEnable = ENN_FALSE;
	}
}




#ifdef __cplusplus
#if __cplusplus
    }
#endif /* __cpluscplus */
#endif /* __cpluscplus */



