
#ifndef _MODBU_SLAVE_TABLE_H_
#define _MODBU_SLAVE_TABLE_H_


#include "ennType.h"
#include "bacenum.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define MODBUS_STRING_MAX_LEN	20

#define UART_CHANNEL_MAX_NUM		4

#define MODBUS_REG_MAX_NAME		20
#define MODBUS_FUNCODE_MAX_NUM		4
#define MODBUS_READ_FILE_BUF_LEN 	4096
#define MODBUS_REG_ATTR_MAX_LEN 	512

#define MODBUS_PRITY_NONE "NONE"
#define MODBUS_PRITY_ODD "ODD"
#define MODBUS_PRITY_EVEN "EVEN"
#define MODBUS_PRITY_MARK "MARK"
#define MODBUS_PRITY_SPACE "SPACE"

#define PRO_645_MAX_CHANNEL_NUM 	14
/***********************************************************/

typedef enum tag_CHANNEL_STATUS
{
	CHANNEL_IDEL,
	CHANNEL_BUSY,
}CHANNEL_STATUS;

typedef enum tag_MODBUS_CHANNEL
{
	MODBUS_CHANNEL_0 = 0,
	MODBUS_CHANNEL_1,
	MODBUS_CHANNEL_2,
	MODBUS_CHANNEL_3,
	MODBUS_CHANNEL_4,
	MODBUS_CHANNEL_5,
	MODBUS_CHANNEL_6,
	MODBUS_CHANNEL_7,
}MODBUS_CHANNEL;


/*add by shanjianchao 2015-01-22*/
typedef enum tag_Protocol_Type
{
	PROTOCOL_MODBUS = 0,
	PROTOCOL_645_1997,
	PROTOCOL_645_2007,
}Protocol_Type;

typedef enum tag_Data_Type
{
	Data_DECIMAL = 0,
	Data_INT,
}Data_Type;

typedef struct tag_Bacnet_Register_List{
	BACNET_OBJECT_TYPE Object_Type;
	uint32_t Object_Instance;
	BACNET_PROPERTY_ID Object_Property;
	float Present_Value;
	BACNET_ENGINEERING_UNITS Units;
}Bacnet_Register_List;


typedef struct tag_Register_List
{
	ENN_U8		u8Interval;    // ��ǰ�Ĵ�������һ���Ĵ����ĵ�ַ���
	ENN_U16 	u16RegAddr;   //�Ĵ��� ��ʼ��ַ
	ENN_U16 	u16RegNum;   //�Ĵ�������
	ENN_CHAR		*pName;  //�Ĵ���������
	ENN_CHAR		*pAttr;    //�Ĵ���������
	ENN_FLOAT		fRegK;   //K����ֵy=kx+d
	ENN_FLOAT		fRegD;    //D����ֵ
	ENN_U16		u16SvInterval;        //10s,20s   //���ݲɼ�����
	//ENN_U8			u8ByteOrder;//0->'AB'  1->'BA'  2->''
	ENN_CHAR			*pBtOrder;//'AB'  'BA'  '1234'
	ENN_U16		u16RegID;      //�Ĵ���(������)ID
	//added for bacnet
	BACNET_OBJECT_TYPE   object_type;  //bacnet��������
	ENN_U32	object_instance;                 //bacnet����ʵ��
	BACNET_ENGINEERING_UNITS   units;   //��Ӧ�Ĵ������ݵĵ�λ
	
	struct tag_Register_List *next;
}Register_List;

typedef struct tag_Slave_FunCode_List
{
	ENN_U8 	u8MBFunCode;
	ENN_U16 u16Count;     //��ǰ�豸������������� �Ĵ����ܳ���(�����������)(�Ĵ�������)
	ENN_U16 u16TotalRegNum;   //��ǰ�豸������������Ĵ������ܳ���
	Register_List *pRegister_List;
	struct tag_Slave_FunCode_List *next;
}Slave_FunCode_List;

typedef struct tag_Slave_List
{
	ENN_U8 	u8SlaveAddr;  //�豸��ַ
	ENN_CHAR	*pSlaveName;  //�豸����
	ENN_U8	u8DataFormat;   //���ݸ�ʽ
	ENN_U16	u16Status;    //�豸�Ĺ���״̬ 1  ����������0 ��������
	Slave_FunCode_List *pFunCode_List;  //����������
	struct tag_Slave_List *next;     //ָ����һ���豸
}Slave_List;

typedef struct tag_Code_645_List
{
	ENN_U32			u32ChannelCode;
	ENN_U16			u16RegNum;
	Data_Type		eData_Type;
	//ENN_S16			s16DCoe;
	//float 			fKCoe;
	ENN_CHAR		*pName;
	ENN_CHAR		*pAttr;

	ENN_FLOAT	fRegK;
	ENN_FLOAT	fRegD;
	ENN_U16		u16SvInterval;
	ENN_U8			u8ByteOrder;//0->'AB'  1->'BA'  2->''
	ENN_U16		u16RegID;
	struct tag_Code_645_List *next;
}Code_645_List;


typedef struct tag_Dev645_List
{
	ENN_CHAR 	ucDevAddr[16];
	ENN_CHAR	*pSlaveName;
	ENN_U16		u16Status;
	ENN_U8		u8ChannelNum;
	Code_645_List *pCode_645_List;
	//ENN_U32		*pChannelCode;
	struct tag_Dev645_List *next;
}Dev645_List;

typedef union tag_Device_Type
{
	Slave_List *pModBus_List;
	Dev645_List *pDev645_List;
}Device_Type;

typedef struct tag_Channel_List
{
	ENN_U8	u8Channel;      //ͨ���Ŵ�0 ��ʼ
	ENN_U8 	u8Protocol;     //�����豸��ͨ��Э���
	ENN_U8 	u8Status;
	ENN_U8	u8SlaveNum;   //��Ӧͨ���µĴ��豸����
	ENN_U8	u8DataBit;        //���ڲ���������λ
	ENN_U8	u8StoptBit;       //���ڲ�����ֹͣλ
	ENN_U8	u8Parity;            //���ڲ�������żУ��λ
	ENN_U32	u32BaudRate;     //���ڲ����Ĳ�����bit/s
	ENN_U32	u32Idletime;        //ͨ����ʱ�ȴ�ʱ��
	//Slave_List 	*pSlaveList;
	Device_Type unDeviceType;
	struct tag_Channel_List *next;
}Channel_List;

typedef struct tag_Addr_Map_List
{
	ENN_U16 	u16DevRegAddr;
	ENN_U16 	u16SlaveRegNum;
	ENN_U8		u8UARTChannel;
	ENN_U8 		u8SlaveAddr;
	ENN_U16 	u16SlaveRegAddr;
	struct tag_Addr_Map_List *next;
}Addr_Map_List;

typedef struct tag_FunCode_Write_List
{
	ENN_U8 		u8MBFunCode;
	ENN_U16 	u16StartAddr;
	ENN_U16 	u16EndAddr;
	Addr_Map_List 	*pAddr_Map_List;
	struct tag_FunCode_Write_List *next;
}FunCode_Write_List;


typedef struct tag_FunCode_List
{
	ENN_U8 	u8MBFunCode;
	ENN_U16 u16StartAddr;   //ӳ�书�������ʼ�ļĴ�����ַ
	ENN_U16 u16EndAddr;    //ӳ�书����Ľ����ļĴ�����ַʵ���ϱ�����ǼĴ�����������
	ENN_U16 Offset[8];           //��¼ÿ��һ��ͨ������ͬ�������������ʼλ�õ�ƫ����
	ENN_U16 DataLen;           //�����ܳ���:ע�����ݳ����ǼĴ������ȵ�����һ�����ȵļĴ������������ֽ�
	ENN_U8 	*pData;                //���ݴ洢�ռ���ENNModBus_Add_FunCode�������Ѿ�����ռ������
	ENN_U8 	*pPos;                 //��ʼ����ֵΪpData���׵�ַ
	Addr_Map_List 	*pAddr_Map_List;
	struct tag_FunCode_List *next;
}FunCode_List;

typedef struct tag_rlSectionName
{
	ENN_CHAR            *name;
	ENN_CHAR            *param;
	struct tag_rlSectionName   *next;
}rlSectionName;

typedef struct tag_rlSection
{
	ENN_CHAR		*name;
	rlSectionName *firstName;
	struct tag_rlSection   *next;
}rlSection;

typedef struct tag_Dev645_Map
{
	ENN_U16		u16code;
	ENN_CHAR 	strName[50];
}Dev645_Map;

ENN_ErrorCode_t ENNModBus_Read_Configure(void);

#ifdef __cplusplus
}

#endif /* __cplusplus */


#endif     /* _CYAPI_H_ */


