/**
  ******************************************************************************
  * @author  Lanceli
  * @version V1.0.0
  * @date    09-May-2020
  * @brief   netif configuration
  ******************************************************************************
  * @attention
  * This project is for learning only. If it is for commercial use, please contact the author.
	*
	*Copyright (c) 2020 Lanceli All rights reserved.
  ******************************************************************************
  */
#ifndef __BSPCONFIG_H
#define __BSPCONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

#define	MAC_1       0x00
#define	MAC_2       0x08
#define	MAC_3       0xdc
#define	MAC_4       0x25
#define	MAC_5       0x26
#define	MAC_6       0x27

#define LOCALIP_1   192
#define LOCALIP_2   168
#define LOCALIP_3   1
#define LOCALIP_4   150
	
#define REMOTEIP_1	192
#define REMOTEIP_2	168
#define REMOTEIP_3	1
#define REMOTEIP_4	100
	
#define NETMARK_1   255
#define NETMARK_2   255
#define NETMARK_3   255
#define NETMARK_4   0

#define GATEWAY_1   192
#define GATEWAY_2   168
#define GATEWAY_3   1
#define GATEWAY_4   1

#define LOCALPORT   5000
#define REMOTELPORT 5000
#define REMOTELPORT 5000

typedef enum
{
	AT_MODE = 0,
	DATA_MODE = 1,
}START_MODE;

typedef enum
{
	TCP_SERVER = 1,
	TCP_CLIENT = 2,
}WORK_MODE;

typedef struct  
{
	u8 mac[6];
	u8 remoteip[4];
	u8 localip[4];
	u8 netmask[4];
	u8 gateway[4];
	u16 localport;
	u16 remoteport;
	
	vu8 dhcpstatus;			//dhcp state:
}__lwip_dev;

typedef struct  
{
	u32 BaudRate;
	u16 StopBits;
	u16 Parity;
	u16 HardwareFlowControl;
}__uart_dev;


#pragma pack(4)
typedef struct
{
	__lwip_dev ipdev;
	__uart_dev uartdev;
	
	u8 dhcp_enable;
	u8 work_mode;
	u8 start_state;
}EmbeverConfig_TypeDef;
#pragma pack()


extern EmbeverConfig_TypeDef EmbeverStruct;

extern void LwIP_Init(void);
#ifdef __cplusplus
}
#endif

#endif /* __BSPCONFIG_H */
