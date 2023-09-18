#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__

#include "config.h"

/* for ano_af v6 high btye in front */
/* for ano_af v7 low  byte in front */
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

#define JudgeOSD_DataLength			19
#define JudgeOSD_GraphLength		20
#define Graph_config_length 		15
#define GRAPH_1_LENGTH					30

#define JudgeOSD_CmdId					0x0301
#define JudgeOSD_DataId_DeletG	0x0100
#define JudgeOSD_DataId_1Graph	0x0101
#define JudgeOSD_DataId_2Graph	0x0102
#define JudgeOSD_DataId_5Graph	0x0103
#define JudgeOSD_DataId_7Graph	0x0104
#define JudgeOSD_DataId_String	0x0110
#define RED_DRONE_ID						6
#define BLUE_DRONE_ID						106
#define RED_DRONE_OSD_ID				0x0106
#define BLUE_DRONE_OSD_ID				0x016A

#define ENERGY_POINT						0x0001

/* osd graph operate type */
#define OPERATE_ADD			1
#define OPERATE_CHANGE	2
#define OPERATE_DELETE	3

/* osd graph gragh type */
#define TYPE_CIRCLE			2
#define TYPE_FLOAT			5
#define TYPE_INTEGER		6
#define TYPE_CHAR				7	

/* osd graph color */
#define COLOR_BLACK			7


/* RC structure */
typedef __packed struct{
	unsigned short ch0;
	unsigned short ch1;
	unsigned short ch2;
	unsigned short ch3;
	unsigned short s1;
	unsigned short s2;
}Remote;

typedef __packed 	struct{
	short x;
	short y;
	short z;
	unsigned char press_l;
	unsigned char press_r;
}Mouse;

typedef __packed struct
{
	unsigned short w,s,a,d,q,e,r,f,g,z,x,c,v,b,shift,ctrl;
}Key;

typedef __packed struct{
	Remote rc;
	Mouse mouse;
	Key key;
  uint32_t updateTime;
	short DisConnect;
	uint8_t Ever_Lost;
}RC_Ctl_t;

/* PC Receive structure */
typedef struct{
	float PCTargetPitch;
	float PCTargetYaw;
	short DisConnect;
	uint32_t PC_Receive_Cnt;
	uint32_t updateTime;
}PC_Recv_t;

typedef struct{
	/* status data */
	uint8_t robot_id;
	
	/* position data */
	float position_x;
	float position_y;
	float position_z;
	float position_yaw;
	
	/* drone energy data */
	uint16_t energy_point;
	uint8_t attack_time;
	
	/* realtime shot data */
	uint8_t bullet_type;
	uint8_t bullet_freq;
	float bullet_speed;
	
	/* bullet remain number */
	uint16_t bullet_remaining_num;
}tGameInfo;

typedef __packed struct{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t integer_data:32;
}tGraphConfig_integer;

typedef __packed struct{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t float_data:32;
}tGraphConfig_float;

typedef __packed struct{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t radius:10;
	uint32_t end_x:11;
	uint32_t end_y:11;
}tGraphConfig_string;

typedef __packed struct{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t radius:10;
	uint32_t end_x:11;
	uint32_t end_y:11;
}tGraphConfig_picture;

void Can1Receive0(CanRxMsg rx_message0);
void Can1Receive1(CanRxMsg rx_message1);

void Can2Receive0(CanRxMsg rx_message0);
void Can2Receive1(CanRxMsg rx_message1);

void RemoteReceive(volatile unsigned char rx_buffer[]);

void PCReceive(unsigned char PCReceivebuffer[]);
void PCSend(void);

void ANOSend(void);

void JudgeReceive(unsigned char ReceiveBuffer[]);
void JudgeOSD_NumberSend(uint8_t datatype);

#endif


/* end */










