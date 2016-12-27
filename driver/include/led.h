/**************************** (C) COPYRIGHT 2014 ENNO ****************************
 * �ļ���	��uart.h
 * ����	��          
 * ʱ��     	��
 * �汾    	��
 * ���	��
 * ����	��  
**********************************************************************************/
#ifndef __LED_H__
#define __LED_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "ennType.h"


typedef enum tag_LED_Status
{
	LED_NORMAL = 0,    //�̵�
	LED_ERROR,             //���
	LED_OFF,                   //���
}LED_Status;

ENN_S32 ENNLED_Init(ENN_VOID);
ENN_S32 ENNLED_On(CHANNEL_t CHANNELX, LED_Status led_status);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

