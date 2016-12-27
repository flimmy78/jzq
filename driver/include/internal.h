#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#include <ennAPI.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K 1
#endif
#include <semaphore.h>
#include <sys/time.h>
#include <sys/sysinfo.h>


#define CYDEBUG(...) fprintf( stderr, __VA_ARGS__)

typedef struct ENNTask
{
    ENN_U8                 *name;            //�߳�����
    ENNOS_TASK_Priority_t    prio;   //�̵߳ȼ�
    ENN_U32                 stack;             //�̶߳�ջ�Ĵ�С
    ENNOS_TASK_CreateMode_t  mode;   //�߳�����Ĵ���ģʽ
    ENNOS_TASKENTRY_f        fuc;          //�߳������¼�������
    ENN_VOID               *param;                  //�߳������¼��������Ĳ���
    pthread_t              pid;                              //��ǰ�̵߳��̺߳�
    ENN_U32                taskid;                      //�̵߳�����ID
}ENNTask;


#if 1
typedef struct ENNSemaphore {
    sem_t                  sem;
    ENNOS_SEMA_CreateMode_t  semaMode;
}ENNSemaphore;


typedef struct ENNMessage {
    sem_t                  sem;                  //�ź���
    pthread_mutex_t        mutex;        //�̼߳以����
    ENN_U32                 elementSize;   //DATAQ_FRAME�ṹ��Ĵ�С
    ENN_U32                 noElements;    //DATAQ_FRAME�ṹ��ĸ�����ֵ
    ENNOS_MSG_CreateMode_t   msgMode;  //��Ϣ����ģʽ
    // msg's buf
    ENN_VOID               *msgs;//��Ϣ���ݵ�λ�õ�ַmmsg+1
    // current position
    ENN_U32                 begin;     //��Ϣ���е�ǰ������λ�ã�����Ϣ�����ж�ȡ���ݵ�ʱ���1��
    // the number of existing msgs in the message queue
    ENN_U32                 num;       //��Ϣ�����д���DATAQ_FRAME �ṹ��ĸ���������Ϣ����д�����ݵ�ʱ���1
}ENNMessage;
#endif


#endif

