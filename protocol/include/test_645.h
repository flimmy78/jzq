#ifndef _TEST_645_H_
#define _TEST_645_H_


#include     <stdio.h>      /*��׼�����������*/  
#include     <stdlib.h>     /*��׼�����ⶨ��*/  
#include     <unistd.h>     /*Unix ��׼��������*/  
#include     <sys/types.h>  
#include     <sys/stat.h>  
#include     <fcntl.h>      /*�ļ����ƶ���*/  
#include     <termios.h>    /*POSIX �ն˿��ƶ���*/  
#include     <errno.h>      /*����Ŷ���*/  
#include     <unistd.h>
#include     <strings.h>

typedef struct _T_CHANNEL_VALUE
{
  float  value;
  short status;
  short padding;
}T_CHANNEL_VALUE;


typedef struct _T_REG_DATA
{
  int fd;             //set by dca, port handle
  char address[16];
  char user[16];
  char password[16];
  short port_type;    //enum SERIAL/NET
  unsigned short channel_num;    //set by dca
  unsigned int *pchannel_code; //channel_code[MAX_CHANNEL], malloc and set by dca
  T_CHANNEL_VALUE *pchannel_data; //malloc by dca, tim set value
}T_REG_DATA;


typedef enum _T_DATA_TYPE
{
  ENERGY_A_POS_T=0x0101,   //�����й��ܵ���
  ENERGY_A_NEG_T=0x0102,   //�����й��ܵ���
  ENERGY_R_POS_T=0x0103,   //�����޹��ܵ���
  ENERGY_R_NEG_T=0x0104,   //�����޹��ܵ���
  POWER_U_A=0x0201,        //A���ѹ
  POWER_U_B=0x0202,        //B���ѹ
  POWER_U_C=0x0203,        //C���ѹ
  POWER_I_A=0x0204,        //A�����
  POWER_I_B=0x0205,        //B�����
  POWER_I_C=0x0206,        //C�����
  POWER_P_T=0x0207,        //�й�������
  POWER_Q_T=0x0208,        //�޹�������
  POWER_FACTOR=0x0209,     //��������
  POWER_FEQ=0x020A         //Ƶ��
}T_DATA_TYPE;

int tim_register_data_1997(T_REG_DATA * data);
int tim_register_data_2007(T_REG_DATA * data);


#endif 

