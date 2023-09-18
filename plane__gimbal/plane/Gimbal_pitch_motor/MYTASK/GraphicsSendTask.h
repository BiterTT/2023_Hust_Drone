
#ifndef __GRAPHICS_SEND_TASK_H
#define __GRAPHICS_SEND_TASK_H

#include "stdint.h"


#define SEND_MAX_SIZE    128    //�ϴ��������ĳ���
#define frameheader_len  5       //֡ͷ����
#define cmd_len          2       //�����볤��
#define crc_len          2       //CRC16У��
/* ����ϵͳ���ݰ����� */
#define HEADER_LEN 			 5       //֡ͷ����
#define CMD_LEN          2       //�����볤��
#define CRC_LEN          2       //β��CRC16У��
#define DRAWING_PACK     15      //��1��ͼ���ݰ�����
/*��Ļ���*/
#define SCREEN_WIDTH 1080
#define SCREEN_LENGTH 1920			//��Ļ�ֱ���
/* �������� */
#define Game_Type_RMUC 1	 //�����Կ���
#define Game_Type_RMUT 2	 //������
#define Game_Type_RMUA 3	 //�˹�������ս��
#define Game_Type_RMUL_3V3 4 //��У������3V3
#define Game_Type_RMUL_1V1 5 //��У������1V1

/* �����׶� */
#define Game_Progress_Unstart 0	  //δ��ʼ����
#define Game_Progress_Prepare 1	  //׼���׶�
#define Game_Progress_SelfCheck 2 //�Լ�׶�
#define Game_Progress_5sCount 3	  // 5s����ʱ
#define Game_Progress_Battle 4	  //��ս��
#define Game_Progress_Calculate 5 //����������

/* ������� */
#define Game_Result_Draw 0	  //ƽ��
#define Game_Result_RedWin 1  //�췽ʤ��
#define Game_Result_BlueWin 2 //����ʤ��

/* ������Ϣ */
#define Warning_Yellow 1  //���ƾ���
#define Warning_Red 2	  //���ƾ���
#define Warning_Failure 3 //�и�

/* ������ID */
#define Robot_ID_Red_Hero 1			//�췽Ӣ��
#define Robot_ID_Red_Engineer 2		//�췽����
#define Robot_ID_Red_Infantry3 3	//�췽����3
#define Robot_ID_Red_Infantry4 4	//�췽����4
#define Robot_ID_Red_Infantry5 5	//�췽����5
#define Robot_ID_Red_Aerial 6		//�췽���˻�
#define Robot_ID_Red_Sentry 7		//�췽�ڱ�
#define Robot_ID_Red_Darts 8		//�췽����
#define Robot_ID_Red_Radar 9		//�췽�״�
#define Robot_ID_Blue_Hero 101		//����Ӣ��
#define Robot_ID_Blue_Engineer 102	//��������
#define Robot_ID_Blue_Infantry3 103 //��������3
#define Robot_ID_Blue_Infantry4 104 //��������4
#define Robot_ID_Blue_Infantry5 105 //��������5
#define Robot_ID_Blue_Aerial 106	//�������˻�
#define Robot_ID_Blue_Sentry 107	//�����ڱ�
#define Robot_ID_Blue_Darts 108		//��������
#define Robot_ID_Blue_Radar 109		//�����״�

/* ������ID */
#define Cilent_ID_Red_Hero 0x0101		//�췽Ӣ�۲�����
#define Cilent_ID_Red_Engineer 0x0102	//�췽���̲�����
#define Cilent_ID_Red_Infantry3 0x0103	//�췽����3������
#define Cilent_ID_Red_Infantry4 0x0104	//�췽����4������
#define Cilent_ID_Red_Infantry5 0x0105	//�췽����5������
#define Cilent_ID_Red_Aerial 0x0106		//�췽����
#define Cilent_ID_Blue_Hero 0x0165		//����Ӣ�۲�����
#define Cilent_ID_Blue_Engineer 0x0166	//�������̲�����
#define Cilent_ID_Blue_Infantry3 0x0167 //��������3������
#define Cilent_ID_Blue_Infantry4 0x0168 //��������4������
#define Cilent_ID_Blue_Infantry5 0x0169 //��������5������
#define Cilent_ID_Blue_Aerial 0x016A	//��������

/* ����ϵͳ���ݶ�����ID */
#define Drawing_Delete_ID				0x0100
#define Drawing_Graphic1_ID     		0x0101
#define Drawing_Graphic2_ID 			0x0102
#define Drawing_Graphic5_ID     		0x0103
#define Drawing_Graphic7_ID     		0x0104
#define Drawing_Char_ID     			0x0110

/*UI��������*/
#define Op_None 0			//�ղ���
#define Op_Add 1			//����ͼ��
#define Op_Change 2		//�޸�ͼ��
#define Op_Delete 3		//ɾ��ͼ��
#define Op_Init		1		//��ʼ����Ҳ��������ͼ��

/*UIͼ����ɫ*/
#define Red_Blue 0
#define Yellow   1
#define Green    2
#define Orange   3
#define Purple	 4
#define Pink     5
#define Cyan		 6
#define Black    7
#define White    8


//ͼ������
typedef __packed struct		//����UI UI�ַ�������
{
	uint8_t graphic_name[3];	//ͼ������Ϊ�ͻ���ָ��
	uint32_t operate_tpye:3;	//UIͼ�β���
	uint32_t graphic_tpye:3;	//UIͼ������
	uint32_t layer:4;					//UIͼ��0-9
	uint32_t color:4;					//UIͼ����ɫ0-8
	uint32_t start_angle:9;
	uint32_t end_angle:9;	
	uint32_t width:10;		//�߿�
	uint32_t start_x:11;	//��ʼ����X
	uint32_t start_y:11;	//��ʼ����Y
	uint32_t radius:10;	
	uint32_t end_x:11;	//��ֹ����X
	uint32_t end_y:11;	//��ֹ����Y
}graphic_data_struct_t;	

/* ͼ�λ������� */
typedef enum {
	TYPE_LINE = 0U,
	TYPE_RECTANGLE = 1U,
	TYPE_CIRCLE = 2U,
	TYPE_OVAL = 3U,
	TYPE_ARC = 4U,
	TYPE_FLOAT = 5U,
	TYPE_INT = 6U,
	TYPE_CHAR = 7U,
} graphic_tpye;


/*����ϵͳ������Ϣ��*���������ϵͳ����Э��*/
typedef __packed struct
{
	uint16_t data_cmd_id;
	uint16_t send_ID;
	uint16_t receiver_ID;
}student_interactive_header_data_t;//��������

typedef __packed struct
{
	uint8_t data[113];
}robot_interactive_data_t;//��������

typedef __packed struct		//����UI UIɾ��ͼ������ 
{
	uint8_t operate_tpye;		//0�ղ���  1ɾ������ͼ��  2ɾ������ͼ��
	uint8_t layer;					//ͼ���  0~9
}ext_client_custom_graphic_delete_t;//�ͻ���ɾ��ͼ��

typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
}ext_client_custom_graphic_single_t;//�ͻ��˻���һ��ͼ��

typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[2];
}ext_client_custom_graphic_double_t;//�ͻ��˻�������ͼ��

typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[5];
}ext_client_custom_graphic_five_t;//�ͻ��˻������ͼ��

typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct[7];
}ext_client_custom_graphic_seven_t;//�ͻ��˻����߸�ͼ��

typedef __packed struct
{
	graphic_data_struct_t grapic_data_struct;
	char data[30];
}ext_client_custom_character_t;//�ͻ��˻����ַ�

//����������Ϣ
typedef __packed struct
{
	uint16_t data_cmd_id;	//���ݶ�����ID
	uint16_t sender_ID;	//������ID
	uint16_t receiver_ID;	//������ID
	ext_client_custom_graphic_seven_t graphic_custom;//�Զ���ͼ������: �ͻ��˻����߸�ͼ��
}ext_student_interactive_header_data_t;	

typedef __packed struct
{
	uint16_t data_cmd_id;	//���ݶ�����ID
	uint16_t sender_ID;	//������ID
	uint16_t receiver_ID;	//������ID
	ext_client_custom_character_t char_custom;//�Զ����ַ�������
}ext_student_interactive_char_header_data_t;



void JudgementDataSend(void);
void JudgementCustomizeGraphics(int Op_type);//ͼ��
void JudgementCustomizeChar(int Op_type);//�ַ�
void referee_data_pack_handle(uint8_t sof,uint16_t cmd_id, uint8_t *p_data, uint16_t len);
void referee_data_load_Graphic(int Op_type);
void referee_data_load_String(int Op_type);
void GraphicSendtask(void *pvParameters);


void referee_data_load_shootUI(uint8_t operate_type,uint8_t robot_level);
void referee_data_load_NumberUI(void);
void GraphicSendtask(void *pvParameters);

void Send_UIPack(uint16_t data_cmd_id, uint16_t SendID,uint16_t receiverID, uint8_t* data, uint16_t pack_len);
void Send_toReferee(uint16_t cmd_id, uint16_t data_len);//
//ֱ��ͼ�����ݽṹ��
graphic_data_struct_t* Line_Draw(uint8_t layer,int Op_Type,uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy, uint16_t line_width, int color,uint8_t name[]);
//����ͼ�����ݽṹ��
graphic_data_struct_t* Rectangle_Draw(uint8_t layer,int Op_Type,uint16_t startx,uint16_t starty,uint16_t endx,uint16_t endy, uint16_t line_width, int color,uint8_t name[]);
//�õ����Ƹ���ͼ�νṹ��
graphic_data_struct_t* FloatData_Draw(uint8_t layer,int Op_Type,uint16_t startx,uint16_t starty, float data_f, uint8_t size ,uint8_t valid_bit, uint16_t line_width, int color,uint8_t name[]);
//�õ��ַ�ͼ�����ݽṹ��
graphic_data_struct_t* CharGraphic_Draw(uint8_t layer,int Op_Type,uint16_t startx,uint16_t starty, uint8_t size, uint8_t len, uint16_t line_width, int color,uint8_t name[]);
//Բ��ͼ�����ݽṹ��
graphic_data_struct_t* Circle_Draw(uint8_t layer,int Op_Type,uint16_t startx,uint16_t starty,uint32_t radius, uint16_t line_width, int color,uint8_t name[]);

void UI_Distance_Init(void);
void UI_Distance_Change(void);
void ShootLines_Init(void);
void Char_Init(void);
void PitchUI_Change(short Pitch_100 ,uint8_t Init_Cnt);
void YawUI_Change(short Yaw_100 ,uint8_t Init_Cnt);
void GimbalMode_Change(char Gimbal_Flag,uint8_t Init_Cnt);
void FrictionMode_Change(char Fric_Flag,uint8_t Init_Cnt);


#endif
