#include "judge.h"
#include "stm32f4xx_dma.h"
#include "string.h"
#include "math.h"
#include "judgeStruct.h"


static ext_student_interactive_all_data_t Judge_DrawBuff;//画图显示缓存区
static int8_t CountGraph = 0;//统计缓存区里面有多少个图形，最多一次发送7个
static graphic_data_struct_t *GraphDataBuff[7];
static int8_t NowSendingGraph = 0;//是否正在画图，
static uint8_t FrameSeq	=	0;//包序号
uint16_t SelfRobot = RedHero;
uint16_t SelfOperator= RedHeroOperator;
static float VoltageLast = 1.0;
static float VoltageNow = 2.0;
static float CapicityPercentLast = 0.1;
static float CapicityPercentNow = 0.0;

static float ScreenPitch = 1.0;
static float LastScreenPitch = 2.0;
static float ScreenYaw = 0.1;
static float LastScreenYaw = 0.2;

uint8_t ATK_BUFF=0X08;
uint8_t DFS_BUFF=0X04;
uint8_t COOL_BUFF=0X02;
uint8_t HP_BUFF=0X01;
static int ATK_ON, DEF_ON, COL_ON, HPF_ON=0;
static uint8_t Hurting=0;//伤害显示标志
static int Loop_Flag=0;//超级电容数据，

static uint8_t RadarNeedUpdate = 0;
static uint8_t IsDrawEqualLine = 0;//害怕划线时，因为电容在更新数据会出现冲突

//下面是装甲板旋转
static graphic_color_tpye_e	Armor_Color_1 = Green,Armor_Color_2 = Green,Armor_Color_3 = Green,Armor_Color_4 = Green;
static float Armor_YawAngle = 0.0;

Rec_DataDoubleBuff_t Judge_DataBuff = {NULL,NULL,{0},{0} };
Judge_TxDataBuff_t	Judge_TxDataBuff = {NULL,NULL,{0} ,{0} ,0,0,0};
receive_judge_t judge_recv_mesg;//解包获得的最终数据存在这儿
Judge_UndateFlag_t	Judge_UndateFlag = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static int Temp, OnDrawing=0;
#define CHANNEL_NUMBER_STR	7
#define CHANNEL_NUMBER_MAX	9	//通道数目，最长不要超过2位数。一个字符串占1个通道，7个图形占一个通道
	
static graphic_color_tpye_e Judge_GraphChannelColor[CHANNEL_NUMBER_MAX - CHANNEL_NUMBER_STR][7];

typedef struct
{
	uint64_t LastTime;
	uint32_t TimeSpaceMS;
	uint8_t HasExisted;
	uint8_t NeedUpdate;
	char NextStr[31]; 
	char Name[4];
	graphic_data_struct_t GraphData;
}ChannelMsg_t;

ChannelMsg_t Channel[CHANNEL_NUMBER_MAX];
	
	
//形参表：Position_x线中点x，Position_y线中点y，LongestLineLen最长线长度，DistancePerLine每条线间距
void Draw_EqualHeightLine(uint8_t RobotLevel);
void Judge_SetDMASend(uint8_t* Addr,uint16_t HowMany);
void Judge_TestSpeed(frame_cmd_id_e WhichMsg);
void DefaultGraphData(graphic_data_struct_t *GraphData_t,uint8_t *GraphName);
void Judge_Delay_us(uint32_t Time_us);
void Judge_Delay_ms(uint32_t Time_ms);
//

void Judge_SelfControl(uint8_t *DataAddr)
{//自定义控制器，最长一次发30个字节，
	static JudgeFullFrame_t *Temp = NULL;
	static self_control_interactive_all_data_t *TempOfSelfInteractive_all_data = NULL;
	static uint8_t SendSEQ	=	0;//包序号
	
	++SendSEQ;
	Temp = (JudgeFullFrame_t*)Judge_TxDataBuff.WhichBuffToCPU;
	Temp->header.SOF = FrameHeaderSOF;
	Temp->header.DataLength = sizeof(DataAddr);
	Temp->header.Seq = SendSEQ;
	Temp->header.CRC8 = get_crc8_check_sum(Judge_TxDataBuff.WhichBuffToCPU,4,CRC8_INIT_NUM);
	
	Temp->frame_cmd_id = SELF_CONTROL_INTERACTIVE_ID;
	strcpy((char*)TempOfSelfInteractive_all_data->Data,(char*)DataAddr);
	*((uint16_t*)(Temp->data + Temp->header.DataLength)) = get_crc16_check_sum(Judge_TxDataBuff.WhichBuffToCPU,7 + sizeof(DataAddr),CRC16_INIT_NUM);
	if(Judge_TxDataBuff.NowDMASendIng == 0)
	{
	
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}
	}
	else
	{
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
		}

	}
}
	
	
//	

void Judge_RobotToRobot(Robots_ID_e receiver, uint8_t *DataAddr)
{//机器人间交互
	static JudgeFullFrame_t *Temp = NULL;
	static ext_student_interactive_all_data_t *TempOfinteractive_all_data = NULL;
	static uint8_t SendSEQ	=	0;//包序号
	
	++SendSEQ;
	
	Temp = (JudgeFullFrame_t*)Judge_TxDataBuff.WhichBuffToCPU;
	
	Temp->header.SOF = FrameHeaderSOF;
	Temp->header.DataLength = sizeof(DataAddr) + 6;
	Temp->header.Seq = SendSEQ;
	Temp->header.CRC8 = get_crc8_check_sum(Judge_TxDataBuff.WhichBuffToCPU,4,CRC8_INIT_NUM);
	
	Temp->frame_cmd_id = ROBOT_INTERACTIVE_ID;
	
	TempOfinteractive_all_data = (ext_student_interactive_all_data_t*)Temp->data;
	
	TempOfinteractive_all_data->interactive_header_data.sender_ID = SelfRobot;
	TempOfinteractive_all_data->interactive_header_data.receiver_ID = receiver;
	TempOfinteractive_all_data->interactive_header_data.data_cmd_id = 0x0200;
	//memcmp(TempOfinteractive_all_data->Data,DataAddr,HowManyDatas);
	strcpy((char*)TempOfinteractive_all_data->Data,(char*)DataAddr);
	
	*((uint16_t*)(Temp->data + Temp->header.DataLength)) = get_crc16_check_sum(Judge_TxDataBuff.WhichBuffToCPU,13 + sizeof(DataAddr),CRC16_INIT_NUM);
	
	if(Judge_TxDataBuff.NowDMASendIng == 0)
	{
	
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}
	}
	else if(Judge_TxDataBuff.NowDMASendIng == 1)
	{
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			//Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			//Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			//Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			//Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			//Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			//Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}

	}
}

//DrawHowMuchGraph只能是1、2、5、7
//形参表：GraphOrString=='G'表示发送图形数据，为'S'表示发送字符数据

void SendGraphBuffData(char GraphOrString)
{
	static JudgeFullFrame_t *Temp = NULL;
		
	NowSendingGraph = 1;//防止在发送的时候，有新加画图的图形把数据修改了
	++ FrameSeq;
	Temp = (JudgeFullFrame_t*)Judge_TxDataBuff.WhichBuffToCPU;
	
	Temp->header.SOF = FrameHeaderSOF;
	//Temp->header.DataLength = HowManyDatas + 6;
	Temp->header.Seq = FrameSeq;
	//Temp->header.CRC8 = get_crc8_check_sum(Judge_TxDataBuff.WhichBuffToCPU,4,CRC8_INIT);
	
	Temp->frame_cmd_id = ROBOT_INTERACTIVE_ID;
	Temp->header.DataLength = 6;//帧头加帧CMD加图形CMD加crc16帧尾 == 5 + 2 + 6 + 2== 15字节
	
	//下标0-4帧头，5-6帧CMD，7-12图形CMD，
	//13-27是第一图形，28-42第二，43-57第三，58-72第四，73-87第五，88-102第六，103-117第七，118-119为crc16
	//Judge_DrawBuff.Data[]的下标0-14是第一图形，15-29第二，30-44第三，45-59第四，60-74第五，75-89第六，90-104第七，105-106为crc16
	switch(CountGraph)
	{
		case 1:
		{
			//memcpy(&JudgeTXBuff[13],(uint8_t*)&graphic_data,15);
			Temp->header.DataLength += 15;			
			Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0101;			
			break;
		}
		
		case 2:
		{
			Temp->header.DataLength += 30;
			Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0102;
			break;
		}
		case 3:
		{//
			if(GraphOrString == 'G')
			{
				memcpy(&Judge_DrawBuff.Data[45],&Judge_DrawBuff.Data[30],15);//把第三个图形的数据复制到第四个图形
			}
			else if(GraphOrString == 'S')
			{
				Temp->header.DataLength += 45;
				Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0110;//字符型
				break;
			}
		}
		case 4:
		{//4个不能直接发，要凑成5个发
			memcpy(&Judge_DrawBuff.Data[60],&Judge_DrawBuff.Data[45],15);//把第四个图形的数据复制到第五个图形
		}
		case 5:
		{
			Temp->header.DataLength += 75;
			Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0103;
			break;
		}
		case 6:
		{//6个不能直接发，要凑成7个发
			memcpy(&Judge_DrawBuff.Data[90],&Judge_DrawBuff.Data[75],15);//把第六个图形的数据复制到第七个图形
		}
		case 7:
		{
			Temp->header.DataLength += 105;
			Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0104;
			break;
		}
	}
	
	Temp->header.CRC8 = get_crc8_check_sum(Judge_TxDataBuff.WhichBuffToCPU,4,CRC8_INIT_NUM);
	memcpy(Temp->data,(uint8_t*)&Judge_DrawBuff,Temp->header.DataLength);
	*((uint16_t*)(Temp->data + Temp->header.DataLength)) = get_crc16_check_sum(Judge_TxDataBuff.WhichBuffToCPU,7 + Temp->header.DataLength,CRC16_INIT_NUM);

	if(Judge_TxDataBuff.NowDMASendIng == 0)
	{
	
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}
	}
	else
	{
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			//Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			//Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			//Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			//Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			//Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			//Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}

	}		
	CountGraph = 0;//清空GraphDataBuff
	NowSendingGraph = 0;
}
//返回值：-1表示缓存区已满，0表示加入缓存区成功，1表示正在画图，请下一次在添加缓存区，因为没构造FIFO，所以在读的时候不能进行写入
int8_t AddGraphToBuff(graphic_data_struct_t *GraphData_t)
{
	if(NowSendingGraph == 1)
	{
		return 1;
	}
	else if(NowSendingGraph == 0 && CountGraph < 7)
	{
		GraphDataBuff[CountGraph] = (graphic_data_struct_t*)&Judge_DrawBuff.Data[CountGraph * 15];
		//把*GraphData_t映射到TXBUFF里面，就节省了存graphic_data_struct_t的空间
		//HEADER_LEN == 5，CMD_LEN == 2，也就是画第0个图形时，是从JudgeTXBuff的第13位开始存的
		GraphDataBuff[CountGraph]->graphic_name[0] = GraphData_t->graphic_name[0];
		GraphDataBuff[CountGraph]->graphic_name[1] = GraphData_t->graphic_name[1];
		GraphDataBuff[CountGraph]->graphic_name[2] = GraphData_t->graphic_name[2];
		GraphDataBuff[CountGraph]->operate_tpye = GraphData_t->operate_tpye;
		GraphDataBuff[CountGraph]->graphic_tpye= GraphData_t->graphic_tpye;
		GraphDataBuff[CountGraph]->layer = GraphData_t->layer;
		GraphDataBuff[CountGraph]->color = GraphData_t->color;
		GraphDataBuff[CountGraph]->start_angle = GraphData_t->start_angle;
		GraphDataBuff[CountGraph]->end_angle = GraphData_t->end_angle;
		GraphDataBuff[CountGraph]->width = GraphData_t->width;
		GraphDataBuff[CountGraph]->start_x = GraphData_t->start_x;
		GraphDataBuff[CountGraph]->start_y = GraphData_t->start_y;
		GraphDataBuff[CountGraph]->radius = GraphData_t->radius;
		GraphDataBuff[CountGraph]->end_x = GraphData_t->end_x;
		GraphDataBuff[CountGraph]->end_y = GraphData_t->end_y;
		++ CountGraph;
	}
	else if(NowSendingGraph == 0 && CountGraph < 7)
	{
		return -1;
	}

	return 2;
}



//下面是需要在在屏幕上打印的字符和数据
//打印装甲板旋转角度和装甲板颜色，10HZ。角度0-360，逆时针为正
int8_t PrintArmorRotate(float Yaw,graphic_color_tpye_e Armor_1,graphic_color_tpye_e Armor_2,
						graphic_color_tpye_e Armor_3,graphic_color_tpye_e Armor_4)
{
	if(Yaw < 0 || Yaw > 360)
		return 1;
	
	if(Yaw != Armor_YawAngle || Armor_1 != Armor_Color_1 || Armor_2 != Armor_Color_2 || Armor_3 != Armor_Color_3 || Armor_4 != Armor_Color_4)
	{
		Armor_YawAngle = Yaw;
		Armor_Color_1 = Armor_1;
		Armor_Color_2 = Armor_2;
		Armor_Color_3 = Armor_3;
		Armor_Color_4 = Armor_4;
		Channel[7].NeedUpdate = 1;
	}
	
	return 0;
	
}
	

//在屏幕上打印字符。输入形参：WhichChannel范围为0-7，表示8个通道。STR为通道要显示的字符串
int8_t PrintfToScreen(uint8_t WhichChannel,char *STR)
{
	uint8_t BuffTemp[31] = {0};

	if(WhichChannel > 7)
		return 1;

	if(strlen((char*)STR) <= 30)
	{
		memcpy(BuffTemp,STR,strlen((char*)STR));
	}
	else
	{
		memcpy(BuffTemp,STR,30);
	}
	memcpy(Channel[WhichChannel].NextStr,BuffTemp,30);
	
	Channel[WhichChannel].NeedUpdate = 1;
	return 0;
}
//在屏幕上打印YAW轴和PITCH轴数据。
int8_t PrintfYawPitchToScreen(float YawAngel,float PitchAngel)
{
	
	if(ScreenPitch != PitchAngel || ScreenYaw != YawAngel)
	{
		LastScreenPitch = PitchAngel;
		ScreenPitch = PitchAngel;
		LastScreenYaw = ScreenYaw;
		ScreenYaw = YawAngel;	
		Channel[7].NeedUpdate = 1;
	}
	
	return 0;
}
/*
@FunctionName:Draw_CapV
@Brief:Draw the frame of the cap symble on the screen
@Notice:输入
@LatestUpdate:
@Author:
*/
int8_t Draw_CapV(float Votlage2,float Percent)
{	
	if(OnDrawing==0 && Loop_Flag==0){
		if(Percent < 0)
			Percent = 0;
		if(Percent > 100)
			Percent = 100;
		uint32_t Color;
		if(Percent > 60)
			Color=Green;
		else if(Percent > 40)
			Color=Orange;
		else
			Color=RedBlue;
		graphic_data_struct_t GraphData;
		DefaultGraphData(&GraphData, (uint8_t*)"CP3");
		GraphData.layer = 2;
		GraphData.operate_tpye =ReviseGraph;
		GraphData.color = Color;
		GraphData.width = CapWidth ;
		GraphData.start_x = CapStartX + 2;
		GraphData.start_y = CapStartY + CapWidth / 2;
		GraphData.end_x = CapStartX + CapLength * Percent / 100-2;
		GraphData.end_y = CapStartY + CapWidth / 2;
		AddGraphToBuff(&GraphData);//修改电池初始内部电量
		
		Graph_DrawLineInit(&GraphData,(uint8_t*)"CP4",CapStartX+CapLength/5,CapStartY+CapWidth+16,0,0,WIDTH_VOLTAGE_2,RedBlue);
		GraphData.layer = 2;
		GraphData.operate_tpye =ReviseGraph;
		GraphData.start_angle = CHAR_SIZE_VOLTAGE_2;
		GraphData.graphic_tpye = GraphFloat;
		GraphData.end_angle = 2;//小数点有效位数
		memcpy((uint8_t*)&GraphData + 11,(uint8_t*)& Votlage2,4);
		AddGraphToBuff(&GraphData);
		SendGraphBuffData('G');
		OnDrawing=1;
		Loop_Flag=1;
	}
	return 0;
}
//
int8_t PrintCircleColor(uint8_t WhichChannel,graphic_color_tpye_e NextColor)
{//使用通道8的7个图形作为图形通道
	if(WhichChannel > 7)
		return 1;

	if(Judge_GraphChannelColor[1][WhichChannel] != NextColor)
	{
		Judge_GraphChannelColor[1][WhichChannel] = NextColor;
		Channel[8].NeedUpdate = 1;
	}
	return 0;
}

//返回值：-1表示不能初始化。0表示初始化成功

/*
@FunctionName:DefaultGraphData
@Brief:画线初始化
@Notice:
@LatestUpdate:2021.10.30
@Author:
*/
void DefaultGraphData(graphic_data_struct_t *GraphData_t,uint8_t *GraphName)
{
	GraphData_t->graphic_name[0] = GraphName[0];
	GraphData_t->graphic_name[1] = GraphName[1];
	GraphData_t->graphic_name[2] = GraphName[2];
	GraphData_t->operate_tpye =	AddGraph;
	GraphData_t->graphic_tpye = Line;
	GraphData_t->layer = 1;
	GraphData_t->color=Orange;
	GraphData_t->width = 2;
	GraphData_t->start_x = 840;
	GraphData_t->start_y = 540;
	GraphData_t->end_x = 1080;
	GraphData_t->end_y = 540;	
}





/*
@FunctionName:Graph_DrawLineInit
@Brief:根据输入坐标，名称，颜色画直线
@Notice:用于绘制图像
@LatestUpdate:
@Author:
*/
void Graph_DrawLineInit(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
						uint32_t Start_X,uint32_t Start_Y,uint32_t End_X,uint32_t End_Y,uint32_t width,uint32_t Color)
{
	GraphData_t->graphic_name[0] = GraphName[0];
	GraphData_t->graphic_name[1] = GraphName[1];
	GraphData_t->graphic_name[2] = GraphName[2];
	GraphData_t->operate_tpye =	AddGraph;
	GraphData_t->graphic_tpye = Line;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = width;
	GraphData_t->start_x = Start_X;
	GraphData_t->start_y = Start_Y;
	GraphData_t->end_x = End_X;
	GraphData_t->end_y = End_Y;	
}

/*
@FunctionName:Drow_Logo
@Brief:Draw the logo of SCU HUOGUO TEAM
@Notice:Approximately it will take 450ms to draw the logo
@LatestUpdate:2021.11.07
@Author:
*/

void Drow_Logo(void)
{
	graphic_data_struct_t GraphData;
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U01",94+X_move,364+Y_move,99+X_move,364+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U02",92+X_move,361+Y_move,101+X_move,360+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U03",88+X_move,356+Y_move,103+X_move,355+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U04",86+X_move,351+Y_move,106+X_move,351+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U05",82+X_move,346+Y_move,109+X_move,346+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U06",79+X_move,341+Y_move,113+X_move,341+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U07",74+X_move,336+Y_move,116+X_move,336+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M01",87+X_move,325+Y_move,146+X_move,325+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M02",93+X_move,319+Y_move,141+X_move,319+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M03",97+X_move,315+Y_move,137+X_move,315+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M04",100+X_move,310+Y_move,136+X_move,310+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M05",105+X_move,304+Y_move,134+X_move,304+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M06",109+X_move,300+Y_move,141+X_move,300+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M07",112+X_move,296+Y_move,144+X_move,296+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M00",83+X_move,330+Y_move,149+X_move,330+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M08",117+X_move,292+Y_move,145+X_move,292+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M09",123+X_move,288+Y_move,147+X_move,288+Y_move,6,RedBlue);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M10",192-83+X_move,618-330+Y_move,192-149+X_move,618-330+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M11",192-117+X_move,618-292+Y_move,192-145+X_move,618-292+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M12",192-123+X_move,618-288+Y_move,192-147+X_move,618-288+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M13",192-87+X_move,618-325+Y_move,192-146+X_move,618-325+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M14",192-93+X_move,618-319+Y_move,192-141+X_move,618-319+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M15",192-97+X_move,618-315+Y_move,192-137+X_move,618-315+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M16",192-100+X_move,618-310+Y_move,192-136+X_move,618-310+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M17",192-105+X_move,618-304+Y_move,192-134+X_move,618-304+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M18",192-109+X_move,618-300+Y_move,192-141+X_move,618-300+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"M19",192-112+X_move,618-296+Y_move,192-144+X_move,618-296+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D01",94+X_move,614-364+Y_move,99+X_move,614-364+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D02",92+X_move,614-361+Y_move,101+X_move,614-360+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D03",88+X_move,614-356+Y_move,103+X_move,614-355+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D04",86+X_move,614-351+Y_move,106+X_move,614-351+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D05",82+X_move,614-346+Y_move,109+X_move,614-346+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D06",79+X_move,614-341+Y_move,111+X_move,614-341+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D07",74+X_move,614-336+Y_move,116+X_move,614-336+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"U08",39+X_move,334+Y_move,151+X_move,334+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RR1",81+X_move,309+Y_move,92+X_move,295+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RR2",74+X_move,283+Y_move,85+X_move,306+Y_move,9,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RR3",71+X_move,286+Y_move,87+X_move,286+Y_move,9,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RB1",86+X_move,331+Y_move,112+X_move,330+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RB2",112+X_move,330+Y_move,122+X_move,319+Y_move,6,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"RB3",98+X_move,315+Y_move,117+X_move,316+Y_move,4,Cyan);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"D08",39+X_move,614-332+Y_move,151+X_move,614-332+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"J01",84+X_move,331+Y_move,124+X_move,287+Y_move,5,RedBlue);
	AddGraphToBuff(&GraphData);
	Graph_DrawLineInit(&GraphData,(uint8_t*)"J02",66+X_move,330+Y_move,108+X_move,284+Y_move,5,Cyan);
	AddGraphToBuff(&GraphData);
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E01",36+X_move,278+Y_move,75+X_move,276+Y_move,5,7);
	AddGraphToBuff(&GraphData);
		
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E02",75+X_move,276+Y_move,95+X_move,248+Y_move,5,7);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E03",95+X_move,248+Y_move,116+X_move,276+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E04",116+X_move,276+Y_move,156+X_move,278+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E05",156+X_move,278+Y_move,137+X_move,309+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E06",137+X_move,309+Y_move,156+X_move,336+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E07",156+X_move,336+Y_move,117+X_move,339+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E08",117+X_move,339+Y_move,96+X_move,371+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E09",96+X_move,371+Y_move,76+X_move,339+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E10",76+X_move,339+Y_move,36+X_move,337+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E11",36+X_move,337+Y_move,57+X_move,307+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"E12",57+X_move,307+Y_move,36+X_move,278+Y_move,5,Black);
	AddGraphToBuff(&GraphData);
	

	Graph_DrawLineInit(&GraphData,(uint8_t*)"B01",78+X_move,331+Y_move,119+X_move,285+Y_move,4,Cyan);
	AddGraphToBuff(&GraphData);
	
	Graph_DrawLineInit(&GraphData,(uint8_t*)"R01",72+X_move,330+Y_move,113+X_move,284+Y_move,4,RedBlue);
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(50);

}



void Judge_DrawInit(uint16_t senderID,uint16_t receiverID)
{
	Judge_DrawBuff.interactive_header_data.data_cmd_id = 0x0101;
	Judge_DrawBuff.interactive_header_data.sender_ID = senderID;
	Judge_DrawBuff.interactive_header_data.receiver_ID = receiverID;
	
//	((ext_student_interactive_all_data_t*)JudgeTXBuff)->interactive_header_data.data_cmd_id = 0x0101;//内容ID，0101画1个图形，data长度21
//	((ext_student_interactive_all_data_t*)JudgeTXBuff)->interactive_header_data.sender_ID = senderID;
//	((ext_student_interactive_all_data_t*)JudgeTXBuff)->interactive_header_data.receiver_ID = receiverID;
	
//	JudgeTXBuff[HEADER_LEN + CMD_LEN + 2] = senderID % 256;//16进制数，先存低8位，再存高8位
//    JudgeTXBuff[HEADER_LEN + CMD_LEN + 3] = senderID / 256;
//	
//	JudgeTXBuff[HEADER_LEN + CMD_LEN + 4] = receiverID % 256;
//    JudgeTXBuff[HEADER_LEN + CMD_LEN + 5] = receiverID / 256;
}


//
void Draw_EqualHeightLine_Vice(uint8_t *Name,uint16_t Position_Y,uint16_t Length_L,uint16_t Length_S,uint16_t Length_V,uint8_t HasExi)
{
	graphic_data_struct_t GraphData;
	DefaultGraphData(&GraphData, Name);
	GraphData.width = Judge_Equ_Line_All_Width;
	GraphData.color = Judge_Equ_Line_All_Color;
	
	if(HasExi == 1)
		GraphData.operate_tpye = ReviseGraph;
	GraphData.start_x = Judge_Equ_Line_ALL_Position_X - Length_L / 2.0;
	GraphData.start_y = Position_Y;
	GraphData.end_x = Judge_Equ_Line_ALL_Position_X + Length_L / 2.0 ;
	GraphData.end_y = Position_Y;
	AddGraphToBuff(&GraphData);//长横线
	
//	GraphData.graphic_name[2] = '2';
//	GraphData.start_x = Judge_Equ_Line_ALL_Position_X - Length_L / 2.0;
//	GraphData.start_y = Position_Y + Length_V / 2.0;
//	GraphData.end_x = Judge_Equ_Line_ALL_Position_X - Length_L / 2.0 ;
//	GraphData.end_y = Position_Y - Length_V / 2.0;
//	AddGraphToBuff(&GraphData);//最左边短竖线
//	
//	GraphData.graphic_name[2] = '3';
//	GraphData.start_x = Judge_Equ_Line_ALL_Position_X + Length_L / 2.0;
//	GraphData.start_y = Position_Y - Length_V / 2.0;
//	GraphData.end_x = Judge_Equ_Line_ALL_Position_X + Length_L / 2.0 ;
//	GraphData.end_y = Position_Y + Length_V / 2.0;
//	AddGraphToBuff(&GraphData);//最右边短竖线
	
	GraphData.graphic_name[2] = '4';
	GraphData.start_x = Judge_Equ_Line_ALL_Position_X - Length_S / 2.0;
	GraphData.start_y = Position_Y + Length_V / 2.0;
	GraphData.end_x = Judge_Equ_Line_ALL_Position_X - Length_S / 2.0 ;
	GraphData.end_y = Position_Y - Length_V / 2.0;
	AddGraphToBuff(&GraphData);//短竖线
	
	GraphData.graphic_name[2] = '5';
	GraphData.start_x = Judge_Equ_Line_ALL_Position_X + Length_S / 2.0;
	GraphData.start_y = Position_Y + Length_V / 2.0;
	GraphData.end_x = Judge_Equ_Line_ALL_Position_X + Length_S / 2.0 ;
	GraphData.end_y = Position_Y - Length_V / 2.0;
	AddGraphToBuff(&GraphData);//短竖线
	
	GraphData.graphic_name[2] = '6';
	GraphData.start_x = Judge_Equ_Line_ALL_Position_X;
	GraphData.start_y = Position_Y + Length_V / 4.0;
	GraphData.end_x = Judge_Equ_Line_ALL_Position_X;
	GraphData.end_y = Position_Y - Length_V / 4.0;
	AddGraphToBuff(&GraphData);//中间短竖线
	
	SendGraphBuffData('G');
	
	Judge_Delay_ms(200);
	
	
}
//

/*
@FunctionName:Draw_HorizontalInit
@Brief:中央画大横线
@Notice:一共会画15条线，7组一发延时200ms；Judge_DontDrawHorizontal定义为1，不使用此函数
@LatestUpdate:2021.10.30
@Author:
*/
void Draw_HorizontalInit(void)
{
	graphic_data_struct_t GraphData;
	float Space = Judge_Horizontal_Line_Length / 14;
	if(Judge_DontDrawHorizontal == 1)
		return;
	DefaultGraphData(&GraphData, (uint8_t*)"HO1");
	GraphData.width = Judge_Horizontal_Line_Width;
	GraphData.color = Cyan;
	GraphData.start_x = 960- Judge_Horizontal_Line_Length / 2.0;
	GraphData.start_y = 540;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 ;
	GraphData.end_y = 540;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '2';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 ;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '3';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 ;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '4';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '5';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 2;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 2;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '6';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 3;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 3;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '7';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 4;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 4;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	SendGraphBuffData('G');
	
	Judge_Delay_ms(200);
	
	GraphData.graphic_name[2] = '8';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 5;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 5;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '9';
	GraphData.start_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 6;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 - Judge_Horizontal_Line_Length / 2.0 + Space * 6;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[1] = 'R';
	GraphData.graphic_name[2] = '1';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '2';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 2;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 2;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '3';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 3;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 3;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '4';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 4;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 4;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '5';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 5;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 5;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 ;
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(200);
	
	GraphData.graphic_name[2] = '6';
	GraphData.start_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 6;
	GraphData.start_y = 540 - Judge_Vertical_Line_Height / 2.0 - Judge_Vertical_Line_Height * 0.50;
	GraphData.end_x = 960 + Judge_Horizontal_Line_Length / 2.0 - Space * 6;
	GraphData.end_y = 540 + Judge_Vertical_Line_Height / 2.0 + Judge_Vertical_Line_Height * 0.50;
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(200);
}

/*
@FunctionName:Draw_CapicityInit
@Brief:超级电容画图初始化
@Notice:
@LatestUpdate:
@Author:
*/


void Draw_CapicityInit(void)
{

	graphic_data_struct_t GraphData;
	DefaultGraphData(&GraphData, (uint8_t*)"CP1");
	GraphData.color = CapFrameColor;
	GraphData.graphic_tpye = Rectangle;
	GraphData.width = RECTANGLE_WIDTH;
	GraphData.start_x = CapStartX;
	GraphData.start_y = CapStartY;
	GraphData.end_x = CapStartX+CapLength;
	GraphData.end_y = CapStartY+CapWidth;
	GraphData.layer = 2;
	AddGraphToBuff(&GraphData);//边框
	
	DefaultGraphData(&GraphData, (uint8_t*)"CP2") ;
	GraphData.color = CapFrameColor ;
	GraphData.width = CapHeadWidth ;
	GraphData.start_x = CapStartX + CapLength ;
	GraphData.start_y = CapStartY + CapWidth/2 ;
	GraphData.end_x = GraphData.start_x + CapHeadLength ;
	GraphData.end_y =CapStartY + CapWidth/2;
	AddGraphToBuff(&GraphData);//电池右侧头
	
	DefaultGraphData(&GraphData, (uint8_t*)"CP3");
	GraphData.layer = 2;
	GraphData.color = Green;
	GraphData.width = CapWidth-2;
	GraphData.start_x = CapStartX + 2;
	GraphData.start_y = CapStartY + CapWidth/2 ;
	GraphData.end_x = CapStartX + CapLength - 2 ;//510+80-2;
	GraphData.end_y = CapStartY + CapWidth/2 ;
	AddGraphToBuff(&GraphData);//电池内部电量显示
	
	if(VOLTAGE_DONT_USE == 0)
	{
		//float Voltage_Temp = GetSuperCap()->capvol;
		float Voltage_Temp =0;
		Graph_DrawLineInit(&GraphData,(uint8_t*)"CP4",CapStartX+CapLength/5 , CapStartY + CapWidth +16,0,0,WIDTH_VOLTAGE_2,RedBlue);
		GraphData.layer = 2;
		GraphData.start_angle = CHAR_SIZE_VOLTAGE_2;
		GraphData.graphic_tpye = GraphFloat;
		GraphData.end_angle = 2;//小数点有效位数
		memcpy((uint8_t*)&GraphData + 11,(uint8_t*)&Voltage_Temp,4);
		AddGraphToBuff(&GraphData);
	}
	
	SendGraphBuffData('G');
	Draw_BuffStr((uint8_t*)"CP5",Orange,CapStartX+4*CapLength/5,CapStartY+CapWidth+16,(uint8_t*)"V");

//	Judge_Delay_ms(1000);
//	Graph_DeleteLayer(2,(uint8_t*)"CP1");
//	Graph_DeleteLayer(1,(uint8_t*)"CP5");
//	Graph_DeleteLayer(2,(uint8_t*)"CP3");	
//	Graph_DeleteLayer(2,(uint8_t*)"CP4");
//	Graph_DeleteLayer(4,(uint8_t*)"Cir");
	SendGraphBuffData('G');
	Judge_Delay_ms(200);
	
}
/*

void Graph_DrawLineInit(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
						uint32_t Start_X,uint32_t Start_Y,uint32_t End_X,uint32_t End_Y,uint32_t width,uint32_t Color)
{
	GraphData_t->graphic_name[0] = GraphName[0];
	GraphData_t->graphic_name[1] = GraphName[1];
	GraphData_t->graphic_name[2] = GraphName[2];
	GraphData_t->operate_tpye =	AddGraph;
	GraphData_t->graphic_tpye = Line;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = width;
	GraphData_t->start_x = Start_X;
	GraphData_t->start_y = Start_Y;
	GraphData_t->end_x = End_X;
	GraphData_t->end_y = End_Y;	
}

*/

/*
@FunctionName:
@Brief:
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_DynamicNum(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
					uint32_t Start_X,uint32_t Start_Y,uint32_t start_angle,uint32_t Color, uint32_t Num)
{
	GraphData_t->graphic_name[0] = GraphName[0];
	GraphData_t->graphic_name[1] = GraphName[1];
	GraphData_t->graphic_name[2] = GraphName[2];
	GraphData_t->operate_tpye =	ReviseGraph;
	GraphData_t->graphic_tpye = GraphFloat;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = 2;
	GraphData_t->start_x = Start_X;
	GraphData_t->start_y = Start_Y;
	GraphData_t->start_angle = CHAR_SIZE_VOLTAGE_2;
	GraphData_t->graphic_tpye = GraphFloat;
	GraphData_t->end_angle = 2;//小数点有效位数
	memcpy((uint8_t*)GraphData_t + 11,(uint8_t*)&Num,4);
	AddGraphToBuff(GraphData_t);
	
}

void Draw_StaticNum(graphic_data_struct_t *GraphData_t,uint8_t *GraphName,
					uint32_t Start_X,uint32_t Start_Y,uint32_t start_angle,uint32_t Color, uint32_t Num)
{
	GraphData_t->graphic_name[0] = GraphName[0];
	GraphData_t->graphic_name[1] = GraphName[1];
	GraphData_t->graphic_name[2] = GraphName[2];
	GraphData_t->operate_tpye =	AddGraph;
	GraphData_t->graphic_tpye = GraphFloat;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = 2;
	GraphData_t->start_x = Start_X;
	GraphData_t->start_y = Start_Y;
	GraphData_t->start_angle = start_angle;
	GraphData_t->graphic_tpye = GraphFloat;
	GraphData_t->end_angle = 2;//小数点有效位数
	memcpy((uint8_t*)GraphData_t + 11,(uint8_t*)&Num,4);
	AddGraphToBuff(GraphData_t);
	
}


/*
@FunctionName:Draw_EqualHeightLine
@Brief:根据不同机器人等级,显示线形
@Notice:执行一次该函数需要耗时1.4秒钟左右。并且执行时会阻塞电容、通道、YAW轴等数据
@LatestUpdate:
@Author:
*/
void Draw_EqualHeightLine(uint8_t RobotLevel)
{
	static uint8_t HadExisted_1 = 0;
	static uint8_t HadExisted_2 = 0;
	static uint8_t HadExisted_3 = 0;
	static uint8_t HadExisted_4 = 0;
	static uint8_t HadExisted_5 = 0;
	static uint8_t HadExisted_6 = 0;
	static uint8_t HadExisted_7 = 0;
	IsDrawEqualLine = 1;
	Judge_Delay_ms(200);
	if(CountGraph != 0)
	{
		SendGraphBuffData('G');
		Judge_Delay_ms(200);
	}
	switch(RobotLevel)
	{
		case 15:
		{
			if(Judge_Equ_Line_1_1_length_L != 0 || HadExisted_1 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"111",Judge_Equ_Line_1_1_Position_Y,Judge_Equ_Line_1_1_length_L,Judge_Equ_Line_1_1_length_S,Judge_Equ_Line_1_1_Length_Vertical,HadExisted_1);
				HadExisted_1 = 1;
			}
			
//			if(Judge_Equ_Line_1_2_length_L != 0 || HadExisted_2 == 1)
//			{
//				Draw_EqualHeightLine_Vice((uint8_t*)"121",Judge_Equ_Line_1_2_Position_Y,Judge_Equ_Line_1_2_length_L,Judge_Equ_Line_1_2_length_S,Judge_Equ_Line_1_2_Length_Vertical,HadExisted_2);
//				HadExisted_2 = 1;
//			}
//			
			if(Judge_Equ_Line_1_3_length_L != 0 || HadExisted_3 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"131",Judge_Equ_Line_1_3_Position_Y,Judge_Equ_Line_1_3_length_L,Judge_Equ_Line_1_3_length_S,Judge_Equ_Line_1_3_Length_Vertical,HadExisted_3);
				HadExisted_3 = 1;
			}
			
//			if(Judge_Equ_Line_1_4_length_L != 0 || HadExisted_4 == 1)
//			{
//				Draw_EqualHeightLine_Vice((uint8_t*)"141",Judge_Equ_Line_1_4_Position_Y,Judge_Equ_Line_1_4_length_L,Judge_Equ_Line_1_4_length_S,Judge_Equ_Line_1_4_Length_Vertical,HadExisted_4);
//				HadExisted_4 = 1;
//				
//			}
			
			if(Judge_Equ_Line_1_5_length_L != 0 || HadExisted_5 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"151",Judge_Equ_Line_1_5_Position_Y,Judge_Equ_Line_1_5_length_L,Judge_Equ_Line_1_5_length_S,Judge_Equ_Line_1_5_Length_Vertical,HadExisted_5);
				HadExisted_5 = 1;
			}
			
			
//			if(Judge_Equ_Line_1_6_length_L != 0 || HadExisted_6 == 1)
//			{
//				Draw_EqualHeightLine_Vice((uint8_t*)"161",Judge_Equ_Line_1_6_Position_Y,Judge_Equ_Line_1_6_length_L,Judge_Equ_Line_1_6_length_S,Judge_Equ_Line_1_6_Length_Vertical,HadExisted_6);
//				HadExisted_6 = 1;
//			}
			
			if(Judge_Equ_Line_1_7_length_L != 0 || HadExisted_7 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"171",Judge_Equ_Line_1_7_Position_Y,Judge_Equ_Line_1_7_length_L,Judge_Equ_Line_1_7_length_S,Judge_Equ_Line_1_7_Length_Vertical,HadExisted_7);
				HadExisted_7 = 1;
			}
			
			break;
			
		}
		case 18:
		{
			if(Judge_Equ_Line_2_1_length_L != 0 || HadExisted_1 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"111",Judge_Equ_Line_2_1_Position_Y,Judge_Equ_Line_2_1_length_L,Judge_Equ_Line_2_1_length_S,Judge_Equ_Line_2_1_Length_Vertical,HadExisted_1);
				HadExisted_1 = 1;
			}
			
			if(Judge_Equ_Line_2_2_length_L != 0 || HadExisted_2 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"121",Judge_Equ_Line_2_2_Position_Y,Judge_Equ_Line_2_2_length_L,Judge_Equ_Line_2_2_length_S,Judge_Equ_Line_2_2_Length_Vertical,HadExisted_2);
				HadExisted_2 = 1;
			}
			
			if(Judge_Equ_Line_2_3_length_L != 0 || HadExisted_3 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"131",Judge_Equ_Line_2_3_Position_Y,Judge_Equ_Line_2_3_length_L,Judge_Equ_Line_2_3_length_S,Judge_Equ_Line_2_3_Length_Vertical,HadExisted_3);
				HadExisted_3 = 1;
			}
			
			if(Judge_Equ_Line_2_4_length_L != 0 || HadExisted_4 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"141",Judge_Equ_Line_2_4_Position_Y,Judge_Equ_Line_2_4_length_L,Judge_Equ_Line_2_4_length_S,Judge_Equ_Line_2_4_Length_Vertical,HadExisted_4);
				HadExisted_4 = 1;
			}
			
			if(Judge_Equ_Line_2_5_length_L != 0 || HadExisted_5 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"151",Judge_Equ_Line_2_5_Position_Y,Judge_Equ_Line_2_5_length_L,Judge_Equ_Line_2_5_length_S,Judge_Equ_Line_2_5_Length_Vertical,HadExisted_5);
				HadExisted_5 = 1;
			}
			
			if(Judge_Equ_Line_2_6_length_L != 0 || HadExisted_6 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"161",Judge_Equ_Line_2_6_Position_Y,Judge_Equ_Line_2_6_length_L,Judge_Equ_Line_2_6_length_S,Judge_Equ_Line_2_6_Length_Vertical,HadExisted_6);
				HadExisted_6 = 1;
			}
			
			if(Judge_Equ_Line_2_7_length_L != 0 || HadExisted_7 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"171",Judge_Equ_Line_2_7_Position_Y,Judge_Equ_Line_2_7_length_L,Judge_Equ_Line_2_7_length_S,Judge_Equ_Line_2_7_Length_Vertical,HadExisted_7);
				HadExisted_7 = 1;
			}
			
			break;
		}
		case 30:
		{
			if(Judge_Equ_Line_3_1_length_L != 0 || HadExisted_1 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"111",Judge_Equ_Line_3_1_Position_Y,Judge_Equ_Line_3_1_length_L,Judge_Equ_Line_3_1_length_S,Judge_Equ_Line_3_1_Length_Vertical,HadExisted_1);
				HadExisted_1 = 1;
			}
			
			if(Judge_Equ_Line_3_2_length_L != 0 || HadExisted_2 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"121",Judge_Equ_Line_3_2_Position_Y,Judge_Equ_Line_3_2_length_L,Judge_Equ_Line_3_2_length_S,Judge_Equ_Line_3_2_Length_Vertical,HadExisted_2);
				HadExisted_2 = 1;
			}
			
			if(Judge_Equ_Line_3_3_length_L != 0 || HadExisted_3 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"131",Judge_Equ_Line_3_3_Position_Y,Judge_Equ_Line_3_3_length_L,Judge_Equ_Line_3_3_length_S,Judge_Equ_Line_3_3_Length_Vertical,HadExisted_3);
				HadExisted_3 = 1;
			}
			
			if(Judge_Equ_Line_3_4_length_L != 0 || HadExisted_4 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"141",Judge_Equ_Line_3_4_Position_Y,Judge_Equ_Line_3_4_length_L,Judge_Equ_Line_3_4_length_S,Judge_Equ_Line_3_4_Length_Vertical,HadExisted_4);
				HadExisted_4 = 1;
			}
			
			if(Judge_Equ_Line_3_5_length_L != 0 || HadExisted_5 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"151",Judge_Equ_Line_3_5_Position_Y,Judge_Equ_Line_3_5_length_L,Judge_Equ_Line_3_5_length_S,Judge_Equ_Line_3_5_Length_Vertical,HadExisted_5);
				HadExisted_5 = 1;
			}
			
			if(Judge_Equ_Line_3_6_length_L != 0 || HadExisted_6 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"161",Judge_Equ_Line_3_6_Position_Y,Judge_Equ_Line_3_6_length_L,Judge_Equ_Line_3_6_length_S,Judge_Equ_Line_3_6_Length_Vertical,HadExisted_6);
				HadExisted_6 = 1;
			}
			
			if(Judge_Equ_Line_3_7_length_L != 0 || HadExisted_7 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"171",Judge_Equ_Line_3_7_Position_Y,Judge_Equ_Line_3_7_length_L,Judge_Equ_Line_3_7_length_S,Judge_Equ_Line_3_7_Length_Vertical,HadExisted_7);
				HadExisted_7 = 1;
			}
			
			break;
		}
		default:
		{
			if(Judge_Equ_Line_2_1_length_L != 0 || HadExisted_1 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"111",Judge_Equ_Line_2_1_Position_Y,Judge_Equ_Line_2_1_length_L,Judge_Equ_Line_2_1_length_S,Judge_Equ_Line_2_1_Length_Vertical,HadExisted_1);
				HadExisted_1 = 1;
			}
			
			if(Judge_Equ_Line_2_2_length_L != 0 || HadExisted_2 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"121",Judge_Equ_Line_2_2_Position_Y,Judge_Equ_Line_2_2_length_L,Judge_Equ_Line_2_2_length_S,Judge_Equ_Line_2_2_Length_Vertical,HadExisted_2);
				HadExisted_2 = 1;
			}
			
			if(Judge_Equ_Line_2_3_length_L != 0 || HadExisted_3 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"131",Judge_Equ_Line_2_3_Position_Y,Judge_Equ_Line_2_3_length_L,Judge_Equ_Line_2_3_length_S,Judge_Equ_Line_2_3_Length_Vertical,HadExisted_3);
				HadExisted_3 = 1;
			}
			
			if(Judge_Equ_Line_2_4_length_L != 0 || HadExisted_4 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"141",Judge_Equ_Line_2_4_Position_Y,Judge_Equ_Line_2_4_length_L,Judge_Equ_Line_2_4_length_S,Judge_Equ_Line_2_4_Length_Vertical,HadExisted_4);
				HadExisted_4 = 1;
			}
			
			if(Judge_Equ_Line_2_5_length_L != 0 || HadExisted_5 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"151",Judge_Equ_Line_2_5_Position_Y,Judge_Equ_Line_2_5_length_L,Judge_Equ_Line_2_5_length_S,Judge_Equ_Line_2_5_Length_Vertical,HadExisted_5);
				HadExisted_5 = 1;
			}
			
			if(Judge_Equ_Line_2_6_length_L != 0 || HadExisted_6 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"161",Judge_Equ_Line_2_6_Position_Y,Judge_Equ_Line_2_6_length_L,Judge_Equ_Line_2_6_length_S,Judge_Equ_Line_2_6_Length_Vertical,HadExisted_6);
				HadExisted_6 = 1;
			}
			
			if(Judge_Equ_Line_2_7_length_L != 0 || HadExisted_7 == 1)
			{
				Draw_EqualHeightLine_Vice((uint8_t*)"171",Judge_Equ_Line_2_7_Position_Y,Judge_Equ_Line_2_7_length_L,Judge_Equ_Line_2_7_length_S,Judge_Equ_Line_2_7_Length_Vertical,HadExisted_7);
				HadExisted_7 = 1;
			}
			break;
		}
	}
	
	IsDrawEqualLine = 0;
}


void Draw_GraphChannel_StrInit_Vice(uint8_t *Name,uint16_t Position_X,uint16_t Position_Y,uint8_t* DisplayStr)
{//图层1
	graphic_data_struct_t GraphData;
	//字符暂存区
	uint8_t StrTemp[30] = {0};
	
	DefaultGraphData(&GraphData, Name);
	GraphData.graphic_tpye = GraphChar;
	GraphData.color = GRAPH_CHANNEL_STR_COLOR_ALL;
	GraphData.width = GRAPH_CHANNEL_STR_WIDTH_ALL;
	GraphData.start_angle = GRAPH_CHANNEL_STR_LARGE_ALL;
	GraphData.end_angle = strlen((char*)DisplayStr);
	GraphData.start_x = Position_X;
	GraphData.start_y = Position_Y;
	
	AddGraphToBuff(&GraphData);
	memcpy(StrTemp,DisplayStr,strlen((char*)DisplayStr));
	memcpy(&Judge_DrawBuff.Data[15],StrTemp,30);
	CountGraph = 3;
	SendGraphBuffData('S');
	
	Judge_Delay_ms(200);
}
//
void Draw_GraphChannel_StrInit(void)
{//初始化色块说明字符串
	static uint16_t SendTime_ms = 0;
	
	while(Judge_TxDataBuff.NowDMASendIng != 0 || NowSendingGraph != 0)
	{
		++ SendTime_ms;
		if(SendTime_ms >= 200)
		{
			SendTime_ms = 0;
			Judge_TxDataBuff.NowDMASendIng = 0;
			NowSendingGraph = 0;
		}
		else
		{
			Judge_Delay_ms(1);
		}
	}
	Draw_BuffStr((uint8_t*)"001",RedBlue,70,730,(uint8_t*)"TEST");
	Judge_Delay_ms(200);
	Draw_BuffStr((uint8_t*)"002",RedBlue,70,760,(uint8_t*)"BUFF");
	if(GRAPH_USE_THIS_GRAPH_8_1)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P81",GRAPH_CHANNEL_STR_8_1_POSITION_X,GRAPH_CHANNEL_STR_8_1_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_1);
	
	if(GRAPH_USE_THIS_GRAPH_8_2)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P82",GRAPH_CHANNEL_STR_8_2_POSITION_X,GRAPH_CHANNEL_STR_8_2_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_2);
	
	if(GRAPH_USE_THIS_GRAPH_8_3)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P83",GRAPH_CHANNEL_STR_8_3_POSITION_X,GRAPH_CHANNEL_STR_8_3_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_3);
	
	if(GRAPH_USE_THIS_GRAPH_8_4)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P84",GRAPH_CHANNEL_STR_8_4_POSITION_X,GRAPH_CHANNEL_STR_8_4_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_4);
	
	if(GRAPH_USE_THIS_GRAPH_8_5)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P85",GRAPH_CHANNEL_STR_8_5_POSITION_X,GRAPH_CHANNEL_STR_8_5_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_5);
	
	if(GRAPH_USE_THIS_GRAPH_8_6)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P86",GRAPH_CHANNEL_STR_8_6_POSITION_X,GRAPH_CHANNEL_STR_8_6_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_6);
	
	if(GRAPH_USE_THIS_GRAPH_8_7)
		Draw_GraphChannel_StrInit_Vice((uint8_t*)"P87",GRAPH_CHANNEL_STR_8_7_POSITION_X,GRAPH_CHANNEL_STR_8_7_POSITION_Y,(uint8_t*)GRAPH_CHANNEL_STR_8_7);
	
	
}


/*
@FunctionName:Draw_BuffStr
@Brief:
@Notice:
@LatestUpdate:
@Author:
*/
void Draw_BuffStr(uint8_t *Name, uint16_t Buf_Color,uint16_t Position_X,uint16_t Position_Y, uint8_t* DisplayStr)
{//图层1
	graphic_data_struct_t GraphData;
	//字符暂存区
	uint8_t StrTemp[30] = {0};
	
	
	DefaultGraphData(&GraphData, Name);
	GraphData.graphic_tpye = GraphChar;
	GraphData.color = Buf_Color;
	GraphData.layer = 1;
	GraphData.width = GRAPH_CHANNEL_STR_WIDTH_ALL;
	GraphData.start_angle = 18;
	GraphData.end_angle = strlen((char*)DisplayStr);
	GraphData.start_x = Position_X;
	GraphData.start_y = Position_Y;
	
	AddGraphToBuff(&GraphData);
	memcpy(StrTemp,DisplayStr,strlen((char*)DisplayStr));
	memcpy(&Judge_DrawBuff.Data[15],StrTemp,30);
	CountGraph = 3;
	SendGraphBuffData('S');
	//Judge_Delay_ms(50);
	
}



void Draw_StaticStr(graphic_data_struct_t *GraphData_t, uint8_t *Name, uint16_t Color,
			  uint16_t Position_X,uint16_t Position_Y, uint16_t StrSize , uint8_t* DisplayStr)
{
	uint8_t StrTemp[30] = {0};
	GraphData_t->graphic_name[0] = Name[0];
	GraphData_t->graphic_name[1] = Name[1];
	GraphData_t->graphic_name[2] = Name[2];
	GraphData_t->operate_tpye =	AddGraph;
	GraphData_t->graphic_tpye = GraphChar;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = 2;
	GraphData_t->start_x = Position_X;
	GraphData_t->start_y = Position_Y;
	GraphData_t->start_angle = StrSize;
	GraphData_t->end_angle = strlen((char*)DisplayStr);	
	
	AddGraphToBuff(GraphData_t);
	memcpy(StrTemp,DisplayStr,strlen((char*)DisplayStr));
	memcpy(&Judge_DrawBuff.Data[15],StrTemp,30);
	CountGraph = 3;
	SendGraphBuffData('S');
	Judge_Delay_ms(50);

}

void Draw_DynamicStr(graphic_data_struct_t *GraphData_t, uint8_t *Name, uint16_t Color,
			uint16_t Position_X,uint16_t Position_Y, uint16_t StrSize , uint8_t* DisplayStr)
{
	uint8_t StrTemp[30] = {0};
	GraphData_t->graphic_name[0] = Name[0];
	GraphData_t->graphic_name[1] = Name[1];
	GraphData_t->graphic_name[2] = Name[2];
	GraphData_t->operate_tpye =	ReviseGraph;
	GraphData_t->graphic_tpye = GraphChar;
	GraphData_t->layer = 3;
	GraphData_t->color=Color;
	GraphData_t->width = 2 ;
	GraphData_t->start_x = Position_X;
	GraphData_t->start_y = Position_Y;
	GraphData_t->start_angle = StrSize;
	GraphData_t->end_angle = strlen((char*)DisplayStr);	
	
	AddGraphToBuff(GraphData_t);
	memcpy(StrTemp,DisplayStr,strlen((char*)DisplayStr));
	memcpy(&Judge_DrawBuff.Data[15],StrTemp,30);
	CountGraph = 3;
	SendGraphBuffData('S');
}


void Draw_RobotState(void)
{	graphic_data_struct_t GraphData;
	Graph_DeleteLayer(3,(uint8_t*)"STA");
	Graph_DeleteLayer(3,(uint8_t*)"ST1");
	Graph_DeleteLayer(3,(uint8_t*)"ST2");
	Graph_DeleteLayer(3,(uint8_t*)"ST3");

	SendGraphBuffData('G');
	Judge_Delay_ms(50);
	
	Draw_StaticStr(&GraphData, (uint8_t*)"STA", RedBlue ,30,740, 16 , (uint8_t*)"Robot State");
	Draw_StaticStr(&GraphData, (uint8_t*)"ST1", White ,30,700, 12 , (uint8_t*)"Robot LEVEL:");
	Draw_StaticStr(&GraphData, (uint8_t*)"ST2", White ,30,660, 12 , (uint8_t*)"HURT TYPE: ");
	Draw_StaticStr(&GraphData, (uint8_t*)"ST3", White ,30,620, 12 , (uint8_t*)"Yaw&Pitch:");
	Draw_StaticStr(&GraphData, (uint8_t*)"ST4", White ,30,580, 12 , (uint8_t*)"Fric&rammer:");
	Draw_StaticStr(&GraphData, (uint8_t*)"ST5", White ,30,540, 12 , (uint8_t*)"Chassis:");

	Draw_StaticStr(&GraphData, (uint8_t*)"st1", Green ,180,700, 12 , (uint8_t*)"I");
	Draw_StaticStr(&GraphData, (uint8_t*)"st2", Green ,140,660, 12 , (uint8_t*)"None");
	
	Draw_StaticStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"Off Off");
	Draw_StaticStr(&GraphData, (uint8_t*)"st5", Green ,140,540, 12 , (uint8_t*)"Follow");
	
	Draw_StaticNum(&GraphData,(uint8_t*)"st3",170,620, 10,Green, 0);
	Draw_StaticNum(&GraphData,(uint8_t*)"sT3",230,620, 10,Green, 0);
	SendGraphBuffData('G');
	Judge_Delay_ms(50);

	

}




void Draw_RobotStatus(void)
{
	graphic_data_struct_t GraphData;
	static uint8_t last_hurt_type=6;
	static uint8_t last_state_level=1;
	static uint32_t temp=0;
	static uint32_t DelayTemp=1;
	temp++;
	//机器人等级
	if ( OnDrawing==0 && last_state_level!= Judge_robot_status()->robot_level){
		if(Judge_robot_status()->robot_level==2)
			Draw_DynamicStr(&GraphData, (uint8_t*)"st1", Orange , 200 ,700, 18 , (uint8_t*)"II");
		if(Judge_robot_status()->robot_level==3)
			Draw_DynamicStr(&GraphData, (uint8_t*)"st1", Orange , 200 ,700, 18 , (uint8_t*)"III");
		last_state_level=Judge_robot_status()->robot_level;
		OnDrawing=1;
	}
	
	//伤害类型：装甲板扣血，裁判系统离线扣血，超射速扣血，超枪管热量扣血，底盘超功率扣血，撞击伤害扣血
	if ( OnDrawing==0 && Judge_GetFlag(ROBOT_HURT_ID)!=0){
		if(Judge_RobotHurt()->hurt_type==0)
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)"Armor Hurt");
		if(Judge_RobotHurt()->hurt_type==1)                  
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)" Judge OffLine");
		if(Judge_RobotHurt()->hurt_type==2)                  
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)" Over ShootingSpeed");
		if(Judge_RobotHurt()->hurt_type==3)                  
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)" Over BarrelHeat");
		if(Judge_RobotHurt()->hurt_type==4)                  
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)" Over ChasisPower");
		if(Judge_RobotHurt()->hurt_type==5)                 
			Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Orange ,150,660, 12 , (uint8_t*)" Crushing");
		OnDrawing=1;
		Hurting=1;
		DelayTemp=20;
		Judge_ClearFlagOf(ROBOT_HURT_ID);//清除伤害更新标志
	}
	else if(OnDrawing==0 && Hurting==1&&Temp==DelayTemp){//DelayTemp延长伤害类型的存在时间，因为此数据更新较快
		Draw_DynamicStr(&GraphData, (uint8_t*)"st2", Green ,180,660, 12 , (uint8_t*)"None");
		OnDrawing=1;
		Hurting=0;
		DelayTemp=1;
	}
	
	
	
//	if(OnDrawing==0 && (last_state.fric != GetGimbalState()->fric_state || last_state.rammer != GetGimbalState()->rammer_state))
//	{
//		if(GetGimbalState()->fric_state==0 && GetGimbalState()->rammer_state==0)
//			Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"Off Off");
//		if(GetGimbalState()->fric_state==0 && GetGimbalState()->rammer_state==1)
//			Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"Off On");
//		if(GetGimbalState()->fric_state==1 && GetGimbalState()->rammer_state==0)
//			Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"On Off");
//		if(GetGimbalState()->fric_state==1 && GetGimbalState()->rammer_state==1)
//			Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"On On");
//		OnDrawing=1;
//		last_state.fric = GetGimbalState()->fric_state;
//		last_state.rammer = GetGimbalState()->rammer_state;
//	
//	}
//	
	
	
}
static uint8_t last_fricstate=0;
static uint8_t last_rammerstate=0;
//画摩擦轮和拨弹轮状态
void Draw_FricRammer(uint8_t fric_state,uint8_t rammer_state){
	graphic_data_struct_t GraphData;
	if(OnDrawing==0 && (last_fricstate != fric_state || last_rammerstate != rammer_state))
		{
			if(fric_state==0 && rammer_state==0)
				Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"Off Off");
			if(fric_state==0 && rammer_state==1)
				Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"Off On");
			if(fric_state==1 && rammer_state==0)
				Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"On Off");
			if(fric_state==1 && rammer_state==1)
				Draw_DynamicStr(&GraphData, (uint8_t*)"st4", Green ,170,580, 12 , (uint8_t*)"On On");
			OnDrawing=1;
			last_fricstate = fric_state;
			last_rammerstate = rammer_state;
		
		}

}

void Draw_YawPitch(float yaw,float pitch)
{
	graphic_data_struct_t GraphData;
	if(OnDrawing==0 && Loop_Flag==1 )
	{
		Draw_DynamicNum(&GraphData,(uint8_t*)"st3",160,620, 10 , Green, yaw);
		Draw_DynamicNum(&GraphData,(uint8_t*)"sT3",230,620, 10 , Green, pitch) ;
		SendGraphBuffData('G');
		OnDrawing=1;
		Loop_Flag=0;
		
	}
	
	

}



void Drawing_StateUpdate(void)
{
	if(OnDrawing==1){
		Temp++;
		if (Temp==10){
			Temp=0;
			OnDrawing=0;
		}
	}

}


void Draw_Buff(void)
{
	static uint8_t Buff_Now;
	graphic_data_struct_t GraphData;
	Buff_Now=Judge_BuffState()->power_rune_buff;
	if(Buff_Now!=0)
	{	//Draw_BuffStr((uint8_t*)"BF1",RedBlue,BUFF_TitleX,BUFF_TitleY,(uint8_t*)"BUFF");
		//Judge_Delay_ms(200);
		if((Buff_Now & ATK_BUFF) == ATK_BUFF && ATK_ON==0 && OnDrawing==0){
			Draw_BuffStr( (uint8_t*)"ABF",Purplish , BUFF_ALLX , BUFF_StartY ,(uint8_t*)"ATK BUFF");
			ATK_ON=1;
			OnDrawing=1;
			}
		if((Buff_Now & DFS_BUFF) == DFS_BUFF && DEF_ON==0 && OnDrawing==0){
			Draw_BuffStr((uint8_t*)"DBF",Purplish, BUFF_ALLX ,BUFF_StartY-BUFF_LineH ,(uint8_t*)"DBF BUFF");
			DEF_ON=1;
			OnDrawing=1;
			}
		if((Buff_Now & COOL_BUFF) == COOL_BUFF && COL_ON==0 && OnDrawing==0){     
			Draw_BuffStr((uint8_t*)"CBF",Purplish, BUFF_ALLX ,BUFF_StartY-2*BUFF_LineH ,(uint8_t*)"COOL BUFF");
			COL_ON=1;
			OnDrawing=1;
			}
		if((Buff_Now&HP_BUFF) == HP_BUFF && HPF_ON == 0 && OnDrawing == 0){          
			Draw_BuffStr((uint8_t*)"HBF",Purplish, BUFF_ALLX ,BUFF_StartY-3*BUFF_LineH ,(uint8_t*)"HP BUFF");
			HPF_ON=1;
			OnDrawing=1;
			}
		
	}
	
	else if(OnDrawing==0)
	{

		Graph_DeleteLayer(1,(uint8_t*)"ABF");
		Graph_DeleteLayer(1,(uint8_t*)"DBF");
		Graph_DeleteLayer(1,(uint8_t*)"CBF");
		Graph_DeleteLayer(1,(uint8_t*)"HPF");
		SendGraphBuffData('G');
		ATK_ON=0;
		DEF_ON=0;
		COL_ON=0;
		HPF_ON=0;
		Judge_Delay_ms(50);
		OnDrawing=1;
	}
	
	
}

/*
@FunctionName: Graph_DeleteLayer
@Brief: Delete  
@Notice:
@LatestUpdate:
@Author:
*/
void Graph_DeleteLayer(int32_t Layer,uint8_t *Name)
{
	graphic_data_struct_t GraphData;
	DefaultGraphData(&GraphData, Name);
	GraphData.operate_tpye =DelGraph;
	GraphData.layer =Layer;
	AddGraphToBuff(&GraphData);
	
	
}

//在屏幕上画车身尺寸线Delete
void Draw_SizeLine(void)
{
	graphic_data_struct_t GraphData;
	
	DefaultGraphData(&GraphData, (uint8_t*)"SZ1");
	GraphData.color = Judge_Car_Size_Line_Color;
	GraphData.width = Judge_Car_Size_Line_Width;
	GraphData.start_x = 960 - Judge_Car_Size_BottomWidthMax / 2.0;
	GraphData.start_y = 0;
	GraphData.end_x = 960 - Judge_Car_Size_TopWidthMax / 2.0 ;
	GraphData.end_y = Judge_Car_Size_LineHign;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '2';
	GraphData.start_x = 960 - Judge_Car_Size_TopWidthMax / 2.0;
	GraphData.start_y = Judge_Car_Size_LineHign;
	GraphData.end_x = 960 + Judge_Car_Size_TopWidthMax / 2.0 ;
	GraphData.end_y = Judge_Car_Size_LineHign;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '3';
	GraphData.start_x = 960 + Judge_Car_Size_TopWidthMax / 2.0;
	GraphData.start_y = Judge_Car_Size_LineHign;
	GraphData.end_x = 960 + Judge_Car_Size_BottomWidthMax / 2.0 ;
	GraphData.end_y = 0;
	AddGraphToBuff(&GraphData);
	
	GraphData.color = Yellow;
	
	GraphData.graphic_name[2] = '4';
	GraphData.start_x = 960 - Judge_Car_Size_BottomWidthMin / 2.0;
	GraphData.start_y = 0;
	GraphData.end_x = 960 - Judge_Car_Size_TopWidthMin / 2.0 ;
	GraphData.end_y = Judge_Car_Size_LineHign;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '5';
	GraphData.start_x = 960 + Judge_Car_Size_TopWidthMin / 2.0;
	GraphData.start_y = Judge_Car_Size_LineHign;
	GraphData.end_x = 960 + Judge_Car_Size_BottomWidthMin / 2.0 ;
	GraphData.end_y = 0;
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	Judge_Delay_ms(200);
	
}



void Draw_Int32InScreen(uint16_t Position_x,uint16_t Position_y, int32_t UserNum,uint8_t *Name)
{
	graphic_data_struct_t GraphData;
	
	DefaultGraphData(&GraphData, Name);
	GraphData.color = Green;
	GraphData.layer = 1;
	GraphData.graphic_tpye = GraphInt;
	GraphData.start_angle = 10;//字体大小
	GraphData.width = 1;
	GraphData.start_x = Position_x ;
	GraphData.start_y = Position_y;
	memcpy((uint8_t*)&GraphData + 11,(uint8_t*)&UserNum,4);

	AddGraphToBuff(&GraphData);
	GraphData.operate_tpye = ReviseGraph;
	AddGraphToBuff(&GraphData);
	
	SendGraphBuffData('G');
	
}

void Judge_UseUART_Function_Name(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef	DMA_InitStructure;
//	uint8_t DMA_PreStart = 0;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8,ENABLE);
//	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA1, ENABLE);
	
	RCC_AHB1PeriphClockCmd(Judge_UseGPIO_GPIO_PeriphClock,ENABLE); 
	RCC_APB1PeriphClockCmd(Judge_UseUART_PeriphClock,ENABLE);
	RCC_AHB1PeriphClockCmd(Judge_UseDMA_PeriphClock_RX, ENABLE);
	RCC_AHB1PeriphClockCmd(Judge_UseDMA_PeriphClock_TX, ENABLE);	
	
//	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_UART8);
//	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_UART8);
	
	GPIO_PinAFConfig(Judge_UseGPIO_GPIO_Typedef_Name,Judge_UseGPIO_GPIO_PINSource_TX,Judge_UseUART_GPIOAF_Name);
	GPIO_PinAFConfig(Judge_UseGPIO_GPIO_Typedef_Name,Judge_UseGPIO_GPIO_PINSource_RX,Judge_UseUART_GPIOAF_Name);
	
	GPIO_InitStructure.GPIO_Pin = Judge_UseGPIO_GPIO_GPIO_Pin_TX | Judge_UseGPIO_GPIO_GPIO_Pin_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(Judge_UseGPIO_GPIO_Typedef_Name,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
//	USART_Init(UART8, &USART_InitStructure);	
//	USART_Cmd(UART8, ENABLE); 
//	USART_ClearFlag(UART8, USART_FLAG_TC);
	
	USART_Init(Judge_UseUART_Typedef_Name, &USART_InitStructure);	
	USART_Cmd(Judge_UseUART_Typedef_Name, ENABLE); 
	USART_ClearFlag(Judge_UseUART_Typedef_Name, USART_FLAG_TC);
	
//	USART_ITConfig(UART8, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ITConfig(Judge_UseUART_Typedef_Name, USART_IT_IDLE, ENABLE);//开启空闲中断
//  NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = Judge_UseUART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_DMACmd(Judge_UseUART_Typedef_Name,USART_DMAReq_Rx,ENABLE);//开启串口DMA
	
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = Judge_UseDMA_IRQChannel_RX;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	DMA_DeInit(DMA1_Stream6);
	DMA_DeInit(Judge_UseDMA_Stream_Typedef_Name_RX);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_5;//通道选择
	DMA_InitStructure.DMA_Channel = Judge_UseDMA_Channel_RX;//通道选择
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART8->DR);//DMA外设地址，对DR进行读操作相当于接收，对DR进行写操作相当于发送
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(Judge_UseUART_Typedef_Name->DR);//DMA外设地址，对DR进行读操作相当于接收，对DR进行写操作相当于发送
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Judge_DataBuff.WhichBuffToDMA;//DMA存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储模式
	DMA_InitStructure.DMA_BufferSize = Judge_RecDataBuffDepth;//数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储区增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度：8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度：8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//使用循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//最高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//设置FIFO阈值无效
	DMA_InitStructure.DMA_MemoryBurst = DMA_Mode_Normal;//设置DMA存储器突发模式为正常模式
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
//	DMA_Init(DMA1_Stream6,&DMA_InitStructure);
	DMA_Init(Judge_UseDMA_Stream_Typedef_Name_RX,&DMA_InitStructure);
		
//	DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);
//	DMA_Cmd(DMA1_Stream6,ENABLE);
	DMA_ITConfig(Judge_UseDMA_Stream_Typedef_Name_RX,DMA_IT_TC,ENABLE);
	DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_RX,ENABLE);
	
	/////////////////////下面开始配置DMA的发送，数据流0，通道5
//	USART_DMACmd(UART8,USART_DMAReq_Tx,ENABLE);//开启串口DMA
	USART_DMACmd(Judge_UseUART_Typedef_Name,USART_DMAReq_Tx,ENABLE);//开启串口DMA
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = Judge_UseDMA_IRQChannel_TX;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	DMA_DeInit(DMA1_Stream0);
	DMA_DeInit(Judge_UseDMA_Stream_Typedef_Name_TX);	
//	DMA_InitStructure.DMA_Channel = DMA_Channel_5;//通道选择
	DMA_InitStructure.DMA_Channel = Judge_UseDMA_Channel_TX;//通道选择
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART8->DR); //DMA外设地址，对DR进行读操作相当于接收，对DR进行写操作相当于发送
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(Judge_UseUART_Typedef_Name->DR); //DMA外设地址，对DR进行读操作相当于接收，对DR进行写操作相当于发送
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Judge_TxDataBuff.WhichBuffToDMA;//DMA存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//外设到存储模式
	DMA_InitStructure.DMA_BufferSize = 200;//数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储区增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度：8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度：8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//不使用循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//最高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//设置FIFO阈值无效
	DMA_InitStructure.DMA_MemoryBurst = DMA_Mode_Normal;//设置DMA存储器突发模式为正常模式
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
//	DMA_Init(DMA1_Stream0,&DMA_InitStructure);
	DMA_Init(Judge_UseDMA_Stream_Typedef_Name_TX,&DMA_InitStructure);
		
//	DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
//	DMA_Cmd(DMA1_Stream0,DISABLE);
	DMA_ITConfig(Judge_UseDMA_Stream_Typedef_Name_TX,DMA_IT_TC,ENABLE);
	DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_TX,DISABLE);
}

void Judge_SetDMASend(uint8_t* Addr,uint16_t HowMany)
{
//	DMA_Cmd(DMA1_Stream0,DISABLE);
	DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_TX,DISABLE);
//	while(DMA_GetCmdStatus(DMA1_Stream0) != DISABLE)
	while(DMA_GetCmdStatus(Judge_UseDMA_Stream_Typedef_Name_TX) != DISABLE)
	{
		//确保DMA可以被设置  
	}
//	DMA1_Stream0->M0AR = (uint32_t)Addr;
//	DMA_SetCurrDataCounter(DMA1_Stream0,HowMany);
//	DMA_Cmd(DMA1_Stream0, ENABLE);                      //开启DMA传输 
	Judge_UseDMA_Stream_Typedef_Name_TX->M0AR = (uint32_t)Addr;
	DMA_SetCurrDataCounter(Judge_UseDMA_Stream_Typedef_Name_TX,HowMany);
	DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_TX, ENABLE);                      //开启DMA传输 
	Judge_TxDataBuff.NowDMASendIng = 1;
}




/*
@FunctionName:Judge_Delay_us
@Brief:裁判系统1000us延时，一个机器周期=6个状态周期，1/84Mhz*6*14000=1000us 
@LatestUpdate:
@Author:
*/
void Judge_Delay_us(uint32_t Time_us)
{
	volatile uint32_t Temp = 0;
	volatile uint32_t Temp_1 = 0;
	volatile uint32_t Temp_2 = 0;
	for(Temp = 0; Temp < Time_us ; ++ Temp)
	{
		for(Temp_1 = 0; Temp_1 < 14000; ++ Temp_1)
		{
			Temp_2 = Temp_2 + 1;
		}		
	}
	
}

/*
@FunctionName:Judge_Delay_ms
@Brief:裁判系统ms延时
@Notice:
@LatestUpdate:
@Author:
*/
void Judge_Delay_ms(uint32_t Time_ms)
{
	volatile uint32_t HasDelayed_ms = 0;
	for(HasDelayed_ms = 0; HasDelayed_ms < Time_ms; ++ HasDelayed_ms)
	{
		Judge_Delay_us(1);
	}	
}
/*
@FunctionName:Judge_Check
@Brief:用于裁判系统收发数据缓冲区初始化，裁判系统自检和兵种识别
@Notice:原JudgeInit
@LatestUpdate:2021.10.30
@Author:
*/
void Judge_Check(void) 
{
	volatile ext_game_robot_status_t State;
	uint32_t TimeTemp = 1;
	Judge_DataBuff.WhichBuffToDMA = Judge_DataBuff.Buff_0;
	Judge_DataBuff.WhichBuffToCPU = Judge_DataBuff.Buff_0;
	
	Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
	Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;	
	Judge_TxDataBuff.HowLongWillSend_Buff0 = 0;
	Judge_TxDataBuff.HowLongWillSend_Buff0 = 1;
	Judge_UseUART_Function_Name();//裁判系统串口初始化配置
	
	while(Judge_GetFlag(ROBOT_STATE_ID) < 10)
	{
		++ TimeTemp;
		Judge_Delay_ms(10);
		if(TimeTemp % 500 == 0)
		{//裁判系统接线错误
			if(Judge_GetFlag((frame_cmd_id_e)JUDGE_SPEED_TEST_ID) == 0)
			{
				printf("The line of Judege system is mistakes\n");
				break;
			}
			else 
			{
				//printf("裁判系统解析错误");
				if(TimeTemp == 3000)
					break;
			}
		}
	}
	printf("Judge System init successfully\n");
	State = *Judge_GetRobotState();
	SelfRobot = State.robot_id;
	switch(State.robot_id)
	{
		case RedHero:
		{
			Judge_DrawInit(RedHero,RedHeroOperator);
			SelfOperator = RedHeroOperator;
		}
		break;
		case RedEngineer:
		{
			Judge_DrawInit(RedEngineer,RedEngineerOperator);
			SelfOperator = RedEngineerOperator;
		}
		break;
		case RedInfantry1:
		{
			Judge_DrawInit(RedInfantry1,RedInfantry1Operator);
			SelfOperator = RedInfantry1Operator;
		}
		break;
		case RedInfantry2:
		{
			Judge_DrawInit(RedInfantry2,RedInfantry2Operator);
			SelfOperator = RedInfantry2Operator;
		}
		break;
		case RedInfantry3:
		{
			Judge_DrawInit(RedInfantry3,RedInfantry3Operator);
			SelfOperator = RedInfantry3Operator;
		}
		break;
		case RedAirplane:
		{
			Judge_DrawInit(RedAirplane,RedAirplaneOperator);
			SelfOperator = RedAirplaneOperator;
		}
		break;
		case RedSentry:
		{//哨兵
			//Judge_DrawInit(RedHero,RedHeroOperator);
			SelfOperator = 0;
		}
		break;
		case RedRadar:
		{//雷达
			//Judge_DrawInit(RedHero,RedHeroOperator);
			SelfOperator = 0;
		}
		break;
		case BlueHero:
		{
			Judge_DrawInit(BlueHero,BlueHeroOperator);
			SelfOperator = BlueHeroOperator;
		}
		break;
		case BlueEngineer:
		{
			Judge_DrawInit(BlueEngineer,BlueEngineerOperator);
			SelfOperator = BlueEngineerOperator;
		}
		break;
		case BlueInfantry1:
		{
			Judge_DrawInit(BlueInfantry1,BlueInfantry1Operator);
			SelfOperator = BlueInfantry1Operator;
		}
		break;
		case BlueInfantry2:
		{
			Judge_DrawInit(BlueInfantry2,BlueInfantry2Operator);
			SelfOperator = BlueInfantry2Operator;
		}
		break;
		case BlueInfantry3:
		{
			Judge_DrawInit(BlueInfantry3,BlueInfantry3Operator);
			SelfOperator = BlueInfantry3Operator;
		}
		break;
		case BlueAirplane:
		{
			Judge_DrawInit(BlueAirplane,BlueAirplaneOperator);
			SelfOperator = BlueAirplaneOperator;
		}
		break;
		case BlueSentry:
		{//哨兵
			//Judge_DrawInit(RedHero,RedHeroOperator);
			SelfOperator = 0;
		}
		break;
		case BlueRadar:
		{//雷达
			//Judge_DrawInit(RedHero,RedHeroOperator);
			SelfOperator = 0;
		}
		break;	
	}	
}


uint8_t* Judge_FindTureFrame(uint8_t *WhereStart)
{//从开始地址起，找到一个符合协议的正确帧，把帧首地址返回
	static uint8_t *LinShiAddr = NULL;
	static uint8_t CRC8_Result = 0;
	static uint16_t CRC16_Result = 0;
	LinShiAddr = (uint8_t*)strchr((const char*)WhereStart,0XA5);//找0XA5
	if(LinShiAddr == NULL)
	{//说明没有0XA5了，返回NULL
		return NULL;
	}
	else
	{
		CRC8_Result = get_crc8_check_sum(LinShiAddr,4,CRC8_INIT_NUM);
		if(CRC8_Result == LinShiAddr[4] )
		{//帧头校验正确
			CRC16_Result = get_crc16_check_sum(LinShiAddr,7 + (uint16_t)*(uint16_t*)&LinShiAddr[1],CRC16_INIT_NUM);
			if(CRC16_Result == (uint16_t)*(uint16_t*)&LinShiAddr[ 7 + (uint16_t)*(uint16_t*)&LinShiAddr[1] ] )
			{//全帧校验正确
				return LinShiAddr;
			}
		}
	}
	return NULL;
}

void Judge_UnpackRecData(uint16_t HowManyDataRec)
{//将收到的数据解析到judge_recv_mesg
	//static uint16_t WhereFirst_A5	= 0;//找出在这个帧里面，第几个字母是0xA5
	static uint8_t *HereStartUnpack = NULL;//存储第一次出现A5的地址
	//static JudgeFullFrame_t *Temp;
	static uint32_t tmp;
	HereStartUnpack = Judge_FindTureFrame(Judge_DataBuff.WhichBuffToCPU);
	
	while(HereStartUnpack != NULL)
	{
		//Temp = (JudgeFullFrame_t*)HereStartUnpack;
		switch( ( ( JudgeFullFrame_t* )HereStartUnpack)->frame_cmd_id )
		{
			case GAME_STATE_ID:
			{//比赛状态（1Hz）
				memcpy((char*)&judge_recv_mesg.gameState,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_game_state_t));
				++ Judge_UndateFlag.gameState;
			}
			break;
			case GAME_RESUIT_ID:
			{//比赛结果
				memcpy((char*)&judge_recv_mesg.gameResult,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_game_result_t));
				++ Judge_UndateFlag.gameResult;
			}
			break;
			case ROBOT_HP_ID:
			{//机器人血量数据（1Hz）
				memcpy((char*)&judge_recv_mesg.robotHP,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_game_robot_HP_t));
				++ Judge_UndateFlag.robotHP;
			}
			break;
//			case DART_FIRE_STATE:
//			{//飞镖发射状态
//				memcpy((char*)&judge_recv_mesg.DartStatus,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_dart_status_t));
//				++ Judge_UndateFlag.DartStatus;
//			}
			break;
			case GAIN_AND_PUNISHMENT_STATE:
			{//人工智能挑战赛加成与惩罚区状态，1HZ
				memcpy((char*)&judge_recv_mesg.BuffDebuffStatus,
						(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,
						sizeof(ext_ICRA_buff_debuff_zone_status_t));
				++ Judge_UndateFlag.BuffDebuffStatus;
			}
			break;
			case FIELD_EVENT_ID:
			{//场地事件，1HZ
				memcpy((char*)&judge_recv_mesg.fieldEvent,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_event_data_t));
				++ Judge_UndateFlag.fieldEvent;
			}
			break;
			case SUPPLY_ACTION_ID:
			{//补给站动作标识
				memcpy((char*)&judge_recv_mesg.supplyAction,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_supply_projectile_action_t));
				++ Judge_UndateFlag.supplyAction;
			}
			break;
			case JUDGE_WARNING_ID:
			{//裁判警告数据
				memcpy((char*)&judge_recv_mesg.warning,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(referee_warning_t));
				++ Judge_UndateFlag.warning;
			}
			break;
			case DART_FIRE_COUNTDOWN:
			{//飞镖发射口倒计时，1HZ发送
				memcpy((char*)&judge_recv_mesg.DartRemainTime,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_dart_remaining_time_t));
				++ Judge_UndateFlag.DartRemainTime;
			}
			break;
			case ROBOT_STATE_ID:
			{//机器人状态（10Hz）
				memcpy((char*)&judge_recv_mesg.robotState,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_game_robot_status_t));
				++ Judge_UndateFlag.robotState;
			}
			break;
			case REALTIME_POWER_ID:
			{//实时功率热量（50Hz）
				memcpy((char*)&judge_recv_mesg.powerHeat,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_power_heat_data_t));
				++ Judge_UndateFlag.powerHeat;
			}
			break;
			case ROBOT_POSITION_ID:
			{//机器人位置（10Hz）
				memcpy((char*)&judge_recv_mesg.robotPosition,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_game_robot_pos_t));
				++ Judge_UndateFlag.robotPosition;
			}
			break;
			case ROBOT_BUFF_ID:
			{//机器人增益
				memcpy((char*)&judge_recv_mesg.robotBUFF,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_buff_t));
				++ Judge_UndateFlag.robotBUFF;
			}
			break;
			case DRONE_POWER_ID:
			{//空中机器人能量状态数据，10HZ，只有空中机器人主控发送
				memcpy((char*)&judge_recv_mesg.droneEnergy,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_aerial_robot_energy_t));
				++ Judge_UndateFlag.droneEnergy;
			}
			break;
			case ROBOT_HURT_ID:
			{//伤害数据
				memcpy((char*)&judge_recv_mesg.robotHurt,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_robot_hurt_t));
				++ Judge_UndateFlag.robotHurt;
			}
			break;
			case ROBOT_SHOOT_ID:
			{//射击数据
				memcpy((char*)&judge_recv_mesg.robotShoot,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_shoot_data_t));
				++ Judge_UndateFlag.robotShoot;
			}
			break;
			case SURPLUS_BULLET_ID:
			{//剩余子弹数（哨兵和空中机器人）
				memcpy((char*)&judge_recv_mesg.bulletRemaining,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_bullet_remaining_t));
				++ Judge_UndateFlag.bulletRemaining;
			}
			break;
			case ROBOT_RFID_STATE:
			{//机器人RFID状态，1HZ周期发送
				memcpy((char*)&judge_recv_mesg.rfid_status,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_rfid_status_t));
				++ Judge_UndateFlag.rfid_status;
			}
			break;
			case ROBOT_OPERATOR_CMD:
			{//飞镖机器人客户端指令数据，10HZ
				memcpy((char*)&judge_recv_mesg.DartClientCmd,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_dart_client_cmd_t));
				++ Judge_UndateFlag.DartClientCmd;
			}
			break;
			case ROBOT_INTERACTIVE_ID:
			{//机器人间交互，发送触发
				memset((char*)&judge_recv_mesg.student_interactive_all_data,0,sizeof(ext_student_interactive_all_data_t));
				memcpy((char*)&judge_recv_mesg.student_interactive_all_data,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_student_interactive_all_data_t));
				++ Judge_UndateFlag.student_interactive_all_data;
			}
			break;
			case SELF_CONTROL_INTERACTIVE_ID:
			{///自定义控制器交互数据
				memset((char*)&judge_recv_mesg.self_control_interactive_all_data,0,sizeof(self_control_interactive_all_data_t));
				memcpy((char*)&judge_recv_mesg.self_control_interactive_all_data,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(self_control_interactive_all_data_t));
				++ Judge_UndateFlag.self_control_interactive_all_data;
			}
			break;
			case ROBOT_COMMAND_ID:
			{//小地图交互数据，cmd_id == 0x0303
				memcpy((char*)&judge_recv_mesg.ext_robot_command,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(ext_robot_command_t));
				++ Judge_UndateFlag.ext_robot_command;
			}
			break;
			case KEY_AND_MOUSE_ID:
			{//客户端下发鼠标键盘信息
				memcpy((char*)&judge_recv_mesg.key_and_mouse,(char*)&( ( JudgeFullFrame_t* )HereStartUnpack)->data,sizeof(key_and_mouse_t));
				++ Judge_UndateFlag.key_and_mouse;
			}
			break;
			default: break;
		}
		
		if(( ( JudgeFullFrame_t* )HereStartUnpack)->header.DataLength +  9 < HowManyDataRec)
		{//说明缓存区内数据比一帧数据要长
			//HereStartUnpack = (uint8_t*)( ((uint8_t*)&(( JudgeFullFrame_t* )HereStartUnpack)->data) + 2 + (( JudgeFullFrame_t* )HereStartUnpack)->header.DataLength);
			*HereStartUnpack = 0xFF;//把处理过的帧破坏掉，一定要加
			(( JudgeFullFrame_t* )HereStartUnpack)->frame_cmd_id = BREAK_FRAME_Num;
			HereStartUnpack += 9 + (( JudgeFullFrame_t* )HereStartUnpack)->header.DataLength;
			//第二句比第一句效率高
			HereStartUnpack = Judge_FindTureFrame(HereStartUnpack);
		}
		else
		{
			HereStartUnpack = NULL;
		}
	}
}





//void UART8_IRQHandler(void) 
//Judge_CheckGraphChannel
void Judge_UseUART_IRQHandler(void) 
{
	static uint8_t rec = 0;
	static uint32_t HowManyDataRec = 0;//因为这个变量每次进中断都要用，所以声明为static，不需要每次进来都创建变量，节省一个机器周期
	
//    if (USART_GetITStatus(UART8, USART_IT_IDLE) != RESET) 
    if (USART_GetITStatus(Judge_UseUART_Typedef_Name, USART_IT_IDLE) != RESET)
	{//空闲中断
//		DMA_Cmd(DMA1_Stream6, DISABLE); //关闭DMA,防止处理其间有数据
//		rec = UART8->SR;
//		rec = UART8->DR;//清楚空闲中断，必须先读SR，再读DR
		DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_RX, DISABLE); //关闭DMA,防止处理其间有数据
		rec = Judge_UseUART_Typedef_Name->SR;
		rec = Judge_UseUART_Typedef_Name->DR;//清除空闲中断，必须先读SR，再读DR
		rec = 0;
		
//		HowManyDataRec = Judge_RecDataBuffDepth - DMA_GetCurrDataCounter(DMA1_Stream6);//得到当前DMA收到了多少字节
//		DMA_SetCurrDataCounter(DMA1_Stream6, Judge_RecDataBuffDepth);
		HowManyDataRec = Judge_RecDataBuffDepth - DMA_GetCurrDataCounter(Judge_UseDMA_Stream_Typedef_Name_RX);//得到当前DMA收到了多少字节
		DMA_SetCurrDataCounter(Judge_UseDMA_Stream_Typedef_Name_RX, Judge_RecDataBuffDepth);
		//////////////////此时，如果又来了一帧新的数据，也不影响数据处理，也不会耽误数据接收

		DMA_ClearFlag(Judge_UseDMA_Stream_Typedef_Name_RX,Judge_UseDMA_ClearFlag);
		DMA_ClearITPendingBit(Judge_UseDMA_Stream_Typedef_Name_RX, Judge_UseDMA_ClearIT_RX);//不进DMA中断
		
		Judge_UnpackRecData(HowManyDataRec);
		
		DMA_Cmd(Judge_UseDMA_Stream_Typedef_Name_RX, ENABLE);     //打开DMA,		
    }
}

//void DMA1_Stream6_IRQHandler(void)
void Judge_UseDMA_IRQHandler_RX(void) 
{//只有串口的空闲中断失效才会进DMA中断
//	if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6)) 
//	{
//        DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
//        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);        
//    }
	if (DMA_GetITStatus(Judge_UseDMA_Stream_Typedef_Name_RX, Judge_UseDMA_ClearIT_RX)) 
	{
        DMA_ClearFlag(Judge_UseDMA_Stream_Typedef_Name_RX, Judge_UseDMA_ClearIT_RX);
        DMA_ClearITPendingBit(Judge_UseDMA_Stream_Typedef_Name_RX, Judge_UseDMA_ClearIT_RX);        
    }
}

//void DMA1_Stream0_IRQHandler(void)
void Judge_UseDMA_IRQHandler_TX(void) 
{//串口8的发送中断
//	if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
	if (DMA_GetITStatus(Judge_UseDMA_Stream_Typedef_Name_TX, Judge_UseDMA_ClearIT_TX)) 	
	{
		
//        DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
//        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
		  DMA_ClearFlag(Judge_UseDMA_Stream_Typedef_Name_TX, Judge_UseDMA_ClearIT_TX);
          DMA_ClearITPendingBit(Judge_UseDMA_Stream_Typedef_Name_TX, Judge_UseDMA_ClearIT_TX);
		
//		if(NowSendingGraph == 1)
//		{
//			NowSendingGraph = 0;
//		}
		
		Judge_TxDataBuff.NowDMASendIng = 0;
		
		if(Judge_TxDataBuff.WhichBuffToDMA == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 0;
			if(Judge_TxDataBuff.HowLongWillSend_Buff1 != 0)
			{//说明在DMA发送期间，又向DMA缓存区里加了一帧数据，在BUFF1里面
				Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
				Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
				Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
				Judge_TxDataBuff.NowDMASendIng = 1;
			}
		}
		else if(Judge_TxDataBuff.WhichBuffToDMA == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 0;
			if(Judge_TxDataBuff.HowLongWillSend_Buff0 != 0)
			{//说明在DMA发送期间，又向DMA缓存区里加了一帧数据，在BUFF0里面
				Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
				Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
				Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;
				Judge_TxDataBuff.NowDMASendIng = 1;
			}
		}
    }
}

//void UART8_SendChar(uint8_t b)
void Judge_UseUART_SendChar(uint8_t b) 
{
//    while (USART_GetFlagStatus(UART8, USART_FLAG_TXE) == RESET);
//    USART_SendData(UART8, b);
    while (USART_GetFlagStatus(Judge_UseUART_Typedef_Name, USART_FLAG_TXE) == RESET);
    USART_SendData(Judge_UseUART_Typedef_Name, b);
}

void Draw_DmaDoubleBuffTest(void)
{//dma双缓存测试
	Judge_Init();
	//Judge_TestSpeed(ROBOT_POSITION_ID);
	Judge_TestSpeed(JUDGE_SPEED_TEST_ID);
	while(1)
		;
}


void Judge_DefaultChannel(ChannelMsg_t *ppp,uint16_t Channel_X,char GphOrStr)
{
	ppp->HasExisted = 0;
	
	ppp->GraphData.color = COLOR_CH1;
	ppp->GraphData.layer = 1;
	ppp->GraphData.graphic_tpye = GraphChar;
	ppp->GraphData.start_angle = CHAR_SIZE_CH1;//15;//字体大小
	ppp->GraphData.width = WIDTH_CH1;
	ppp->GraphData.start_x = POSITION_X_CH1;
	ppp->GraphData.start_y = POSITION_Y_CH1 - Channel_X * SPACE_Y_CHANNEL;
	
	ppp->LastTime = 0;
	ppp->NeedUpdate = 0;
	ppp->NextStr[0] = 0;
	ppp->TimeSpaceMS = 150;
	ppp->Name[0] = '0' + Channel_X / 100;
	ppp->Name[1] = '0' + (Channel_X % 100) / 10;
	ppp->Name[2] = '0' + Channel_X % 10;
	
	if(GphOrStr == 'G')
	{
		ppp->Name[0] = 'G';
		ppp->GraphData.graphic_tpye = Circle;
	}
	
	ppp->GraphData.graphic_name[0] = ppp->Name[0];
	ppp->GraphData.graphic_name[1] = ppp->Name[1];
	ppp->GraphData.graphic_name[2] = ppp->Name[2];
	
}


/*
@FunctionName:Judge_Init
@Brief:裁判系统通信和UI画图初始化
@Notice:该函数耗时较长。如果裁判系统接线正确，执行完成此函数大概需要1秒钟的时间，
		如果裁判系统接线出错且初始化了printf函数，
		完全执行需要大概需要6秒钟的时间。如果没有初始化printf函数，则该函数会卡死。
@LatestUpdate:
@Author:
*/
void Judge_Init(void)
{	uint16_t Temp = 0;
	Judge_Check();
//	Draw_HorizontalInit();
	Judge_Delay_ms(200);
	Drow_Logo();
	Judge_Delay_ms(200);
	Draw_EqualHeightLine(1);
	Draw_CapicityInit();
	Judge_Delay_ms(200);
//	Draw_GraphChannel_StrInit();//F D C
//	Draw_BuffArea();
	Draw_RobotState();
	Judge_Delay_ms(200);
	Draw_SizeLine();
	for(Temp = 0; Temp < CHANNEL_NUMBER_MAX; ++ Temp)
	{
		if(Temp < CHANNEL_NUMBER_STR - 1)
		{
			Judge_DefaultChannel(&Channel[Temp],Temp,'S');
		}
		else
		{
			Judge_DefaultChannel(&Channel[Temp],Temp,'G');
		}
	}
}


void Judge_SendStrChannel(ChannelMsg_t *WhichP)
{//字符串通道
	static uint16_t SendTime_ms = 0;
	if(Judge_TxDataBuff.NowDMASendIng != 0 || NowSendingGraph != 0)
	{
		++ SendTime_ms;
		if(SendTime_ms >= 200)
		{
			SendTime_ms = 0;
			Judge_TxDataBuff.NowDMASendIng = 0;
			NowSendingGraph = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		SendTime_ms = 0;
	}
	
	if(WhichP->HasExisted == 0)
	{
		WhichP->GraphData.operate_tpye = AddGraph;
		WhichP->HasExisted = 1;
	}
	else
	{
		WhichP->GraphData.operate_tpye = ReviseGraph;
	}
	
	AddGraphToBuff(&WhichP->GraphData);
	memcpy(&Judge_DrawBuff.Data[15],WhichP->NextStr,30);
	CountGraph = 3;
	SendGraphBuffData('S');
	WhichP->NeedUpdate = 0;
}


void Judge_CheckGraphChannel_7_1(void)
{//通道7_1和7_2，是超级电容通道。包括一个图形和一个浮点数
	graphic_data_struct_t GraphData;
	static uint16_t SendTime_ms = 0;
	static int32_t FloatTemp_V = 0;
	
	//更新超级电容内部图形颜色和长短
	DefaultGraphData(&GraphData, (uint8_t*)"CP3");
	GraphData.operate_tpye = ReviseGraph;
	GraphData.layer = 2;
	if(CapicityPercentNow <= 20)
	{
		GraphData.color = Purplish;
	}
	else if(CapicityPercentNow >= 80)
	{
		GraphData.color = Green;
	}
	else
	{
		GraphData.color = Yellow;
	}
	GraphData.width = RECTANGLE_HEIGHT;
	GraphData.start_x = POSITION_X_CAP;
	GraphData.start_y = POSITION_Y_CAP + RECTANGLE_HEIGHT / 2.0f;
	GraphData.end_x = (POSITION_X_CAP + (CapicityPercentNow * RECTANGLE_LENGTH / 100.0f));
	GraphData.end_y = POSITION_Y_CAP + RECTANGLE_HEIGHT / 2.0f;
	AddGraphToBuff(&GraphData);//电池内部电量			
		
	if(VOLTAGE_DONT_USE == 0)
	{	
		DefaultGraphData(&GraphData, (uint8_t*)"CP4");
		GraphData.operate_tpye = ReviseGraph;
		GraphData.layer = 2;
		GraphData.start_angle = CHAR_SIZE_VOLTAGE_2;
		GraphData.graphic_tpye = GraphFloat;
		GraphData.color = COLOR_VOLTAGE_2;
		GraphData.end_angle = 2;//小数点有效位数
		GraphData.start_x = POSITION_X_CAP + RECTANGLE_LENGTH + RECTANGLE_WIDTH/2.0;
		GraphData.start_y = POSITION_Y_CAP + RECTANGLE_HEIGHT / 2.0f;
		FloatTemp_V = VoltageNow * 1000.0f;
		memcpy((uint8_t*)&GraphData + 11,(uint8_t*)&FloatTemp_V,4);
		AddGraphToBuff(&GraphData);
	}
}

void Judge_CheckGraphChannel_7_3(void)
{//通道7_3和通道7_4，是PITCH轴和YAW轴通道
	static uint8_t HasExist3_4 = 0;
	static int32_t FloatTemp_P = 0;
	static int32_t FloatTemp_Y = 0;
	
	graphic_data_struct_t GraphData;
	
	DefaultGraphData(&GraphData, (uint8_t*)"G73");
	if(HasExist3_4 == 0)
	{
		GraphData.operate_tpye = AddGraph;
		HasExist3_4 = 1;
	}
	else
	{
		GraphData.operate_tpye = ReviseGraph;
	}
	GraphData.graphic_tpye = GraphFloat;
	GraphData.layer = 2;
	GraphData.color = COLOR_YAW_PIT;
	GraphData.start_angle = CHAR_SIZE_YAW_PIT;//15;//字体大小
	GraphData.end_angle = 2;//小数点有效位数
	GraphData.width = WIDTH_YAW_PIT;
	GraphData.start_x = POSITION_X__YAW_PIT;
	GraphData.start_y = POSITION_Y__YAW_PIT + CHAR_SIZE_YAW_PIT;
	FloatTemp_P = ScreenPitch * 1000.0f;
	memcpy((uint8_t*)&GraphData + 11,(uint8_t*)&FloatTemp_P,4);
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '4';
	GraphData.start_y = POSITION_Y__YAW_PIT - CHAR_SIZE_YAW_PIT;
	FloatTemp_Y = ScreenYaw * 1000.0f;
	memcpy((uint8_t*)&GraphData + 11,(uint8_t*)&FloatTemp_Y,4);
	if(GRAPH_USE_YAW_OR_ARMOR == 0)
		AddGraphToBuff(&GraphData);
}



void Judge_CheckGraphChannel_7_5(void)
{//通道7_5/6/7，还有通道4，是图形装甲板旋转通道
	
	static uint8_t HasExist7_5 = 0;
	static float Temp_1_X = 0.0,Temp_1_Y = 0.0;
	static float Temp_2_X = 0.0,Temp_2_Y = 0.0;
	static float Temp_3_X = 0.0,Temp_3_Y = 0.0;
	static float Temp_4_X = 0.0,Temp_4_Y = 0.0;
	static float Temp_Angle = 0.0,Temp_Sin = 0.0,Temp_Cos = 0.0;
	static uint16_t PositionStart_X_1 = 0,PositionStart_Y_1 = 0,PositionEnd_X_1 = 0,PositionEnd_Y_1 = 0;
	static uint16_t PositionStart_X_2 = 0,PositionStart_Y_2 = 0,PositionEnd_X_2 = 0,PositionEnd_Y_2 = 0;
	static uint16_t PositionStart_X_3 = 0,PositionStart_Y_3 = 0,PositionEnd_X_3 = 0,PositionEnd_Y_3 = 0;
	static uint16_t PositionStart_X_4 = 0,PositionStart_Y_4 = 0,PositionEnd_X_4 = 0,PositionEnd_Y_4 = 0;
	
	graphic_data_struct_t GraphData;
	
	if(GRAPH_USE_YAW_OR_ARMOR != 1)
		return;
	
	//下面算每根线的起始坐标和终止坐标
	Temp_Angle = Armor_YawAngle / 180.0f * 3.1415926f;
	Temp_Sin = sinf(Temp_Angle);
	Temp_Cos = cosf(Temp_Angle);
	Temp_1_X = JUDGE_GRAPH_ROTATE_X - JUDGE_GRAPH_DISTANCE * Temp_Sin;
	Temp_1_Y = JUDGE_GRAPH_ROTATE_Y + JUDGE_GRAPH_DISTANCE * Temp_Cos;
	Temp_2_X = JUDGE_GRAPH_ROTATE_X - JUDGE_GRAPH_DISTANCE * Temp_Cos;
	Temp_2_Y = JUDGE_GRAPH_ROTATE_Y - JUDGE_GRAPH_DISTANCE * Temp_Sin;
	Temp_3_X = JUDGE_GRAPH_ROTATE_X + JUDGE_GRAPH_DISTANCE * Temp_Sin;
	Temp_3_Y = JUDGE_GRAPH_ROTATE_Y - JUDGE_GRAPH_DISTANCE * Temp_Cos;
	Temp_4_X = JUDGE_GRAPH_ROTATE_X + JUDGE_GRAPH_DISTANCE * Temp_Cos;
	Temp_4_Y = JUDGE_GRAPH_ROTATE_Y + JUDGE_GRAPH_DISTANCE * Temp_Sin;
	PositionStart_X_1 = Temp_1_X - JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionStart_Y_1 = Temp_1_Y - JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionEnd_X_1 = Temp_1_X + JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionEnd_Y_1 = Temp_1_Y + JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	
	PositionStart_X_2 = Temp_2_X + JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionStart_Y_2 = Temp_2_Y - JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionEnd_X_2 = Temp_2_X - JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionEnd_Y_2 = Temp_2_Y + JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	
	PositionStart_X_3 = Temp_3_X - JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionStart_Y_3 = Temp_3_Y - JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionEnd_X_3 = Temp_3_X + JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionEnd_Y_3 = Temp_3_Y + JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	
	PositionStart_X_4 = Temp_4_X + JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionStart_Y_4 = Temp_4_Y - JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	PositionEnd_X_4 = Temp_4_X - JUDGE_GRAPH_BOARD_LENGTH * Temp_Sin / 2.0f;
	PositionEnd_Y_4 = Temp_4_Y + JUDGE_GRAPH_BOARD_LENGTH * Temp_Cos / 2.0f;
	
	DefaultGraphData(&GraphData, (uint8_t*)"G74");
	if(HasExist7_5 == 0)
	{
		GraphData.operate_tpye = AddGraph;
		HasExist7_5 = 1;
	}
	else
	{
		GraphData.operate_tpye = ReviseGraph;
	}
	GraphData.graphic_tpye = Line;
	GraphData.layer = 2;
	GraphData.color = Armor_Color_1;
	GraphData.width = JUDGE_GRAPH_BOARD_WIDTH;
	GraphData.start_x = PositionStart_X_1;
	GraphData.start_y = PositionStart_Y_1;
	GraphData.end_x = PositionEnd_X_1;
	GraphData.end_y = PositionEnd_Y_1;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '5';
	GraphData.color = Armor_Color_2;
	GraphData.start_x = PositionStart_X_2;
	GraphData.start_y = PositionStart_Y_2;
	GraphData.end_x = PositionEnd_X_2;
	GraphData.end_y = PositionEnd_Y_2;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '6';
	GraphData.color = Armor_Color_3;
	GraphData.start_x = PositionStart_X_3;
	GraphData.start_y = PositionStart_Y_3;
	GraphData.end_x = PositionEnd_X_3;
	GraphData.end_y = PositionEnd_Y_3;
	AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '7';
	GraphData.color = Armor_Color_4;
	GraphData.start_x = PositionStart_X_4;
	GraphData.start_y = PositionStart_Y_4;
	GraphData.end_x = PositionEnd_X_4;
	GraphData.end_y = PositionEnd_Y_4;
	AddGraphToBuff(&GraphData);
	
	
}

void Judge_CheckGraphChannel_7_6(void)
{//通道7_6，是图形通道
}

void Judge_CheckGraphChannel_7_7(void)
{//通道7_7，是图形通道
	
}

void Judge_CheckGraphChannel_8_7(void)
{//通道8_7，是图形通道，
	static uint8_t HasExist8_7 = 0;
	graphic_data_struct_t GraphData;
	
	DefaultGraphData(&GraphData, (uint8_t*)"G81");
	if(HasExist8_7 == 0)
	{
		GraphData.operate_tpye = AddGraph;
		HasExist8_7 = 1;
	}
	else
	{
		GraphData.operate_tpye = ReviseGraph;
	}
	GraphData.graphic_tpye = Rectangle;
	GraphData.layer = 2;
	GraphData.radius = GRAPH_CHANNEL_ALL_RADIUS;
	GraphData.color = Judge_GraphChannelColor[1][0];
	GraphData.width = GRAPH_CHANNEL_8_1_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_1_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_1_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_1_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_1_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_1 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '2';
	GraphData.color = Judge_GraphChannelColor[1][1];
	GraphData.width = GRAPH_CHANNEL_8_2_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_2_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_2_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_2_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_2_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_2 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '3';
	GraphData.color = Judge_GraphChannelColor[1][2];
	GraphData.width = GRAPH_CHANNEL_8_3_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_3_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_3_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_3_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_3_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_3 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '4';
	GraphData.color = Judge_GraphChannelColor[1][3];
	GraphData.width = GRAPH_CHANNEL_8_4_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_4_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_4_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_4_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_4_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_4 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '5';
	GraphData.color = Judge_GraphChannelColor[1][4];
	GraphData.width = GRAPH_CHANNEL_8_5_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_5_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_5_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_5_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_5_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_5 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '6';
	GraphData.color = Judge_GraphChannelColor[1][5];
	GraphData.width = GRAPH_CHANNEL_8_6_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_6_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_6_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_6_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_6_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_6 == 1)
		AddGraphToBuff(&GraphData);
	
	GraphData.graphic_name[2] = '7';
	GraphData.color = Judge_GraphChannelColor[1][6];
	GraphData.width = GRAPH_CHANNEL_8_7_WIDTH;
	GraphData.start_x = GRAPH_CHANNEL_8_7_POSITION_X;
	GraphData.start_y = GRAPH_CHANNEL_8_7_POSITION_Y;
	GraphData.end_x = GRAPH_CHANNEL_8_7_POSITION_X + GRAPH_CHANNEL_ALL_LENGTH;
	GraphData.end_y = GRAPH_CHANNEL_8_7_POSITION_Y;
	if(GRAPH_USE_THIS_GRAPH_8_7 == 1)
		AddGraphToBuff(&GraphData);
	
}

void Judge_SendGraphChannel(uint8_t WhichG)
{//图形通道,检测图形通道，图形通道有2个量，一个是所属的字符串量，一个是字符串通道内的图像通道
	static uint16_t SendTime_ms = 0;
	
	if(Judge_TxDataBuff.NowDMASendIng != 0 || NowSendingGraph != 0)
	{
		++ SendTime_ms;
		if(SendTime_ms >= 200)
		{
			SendTime_ms = 0;
			Judge_TxDataBuff.NowDMASendIng = 0;
			NowSendingGraph = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		SendTime_ms = 0;
	}//安全
	
	switch(WhichG)
	{
		case 7:
		{
			Judge_CheckGraphChannel_7_1();
			Judge_CheckGraphChannel_7_3();
			Judge_CheckGraphChannel_7_5();
			Judge_CheckGraphChannel_7_6();
			Judge_CheckGraphChannel_7_7();
			SendGraphBuffData('G');
			break;
		}
		case 8:
		{
			Judge_CheckGraphChannel_8_7();
			SendGraphBuffData('G');
			break;
		}
	}
	Channel[WhichG].NeedUpdate = 0;
}

int8_t Judge_RadarSendStr(Robots_ID_e WhichRobot,float Position_M_X,float Position_M_Y)
{
	static uint16_t SendTime_ms = 0;
	static JudgeFullFrame_t *Temp = NULL;
	
	if(Judge_TxDataBuff.NowDMASendIng != 0 || NowSendingGraph != 0)
	{
		++ SendTime_ms;
		if(SendTime_ms >= 200)
		{
			SendTime_ms = 0;
			Judge_TxDataBuff.NowDMASendIng = 0;
			NowSendingGraph = 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		SendTime_ms = 0;
	}//安全
	
	NowSendingGraph = 1;//防止在发送的时候，有新加画图的图形把数据修改了
	
	++ FrameSeq;
	Temp = (JudgeFullFrame_t*)Judge_TxDataBuff.WhichBuffToCPU;
	Temp->header.SOF = FrameHeaderSOF;
	Temp->header.Seq = FrameSeq;
	Temp->frame_cmd_id = RADAR_SEND_MAP_ID;
	Temp->header.DataLength = 14;//小地图共10个字节
	
	*((uint16_t*)(Temp->data)) = (uint16_t)WhichRobot;
	memcpy((uint8_t*)(Temp->data + 2),&Position_M_X,4);
	memcpy((uint8_t*)(Temp->data + 6),&Position_M_Y,4);
	memcpy((uint8_t*)(Temp->data + 10),&Position_M_Y,4);
	Temp->header.CRC8 = get_crc8_check_sum(Judge_TxDataBuff.WhichBuffToCPU,4,CRC8_INIT_NUM);
	*((uint16_t*)(Temp->data + Temp->header.DataLength)) = get_crc16_check_sum(Judge_TxDataBuff.WhichBuffToCPU,7 + Temp->header.DataLength,CRC16_INIT_NUM);
	
	if(Judge_TxDataBuff.NowDMASendIng == 0)
	{
	
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_0,Judge_TxDataBuff.HowLongWillSend_Buff0);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_1;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_0;	
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
			Judge_SetDMASend(Judge_TxDataBuff.Buff_1,Judge_TxDataBuff.HowLongWillSend_Buff1);
			Judge_TxDataBuff.WhichBuffToCPU = Judge_TxDataBuff.Buff_0;
			Judge_TxDataBuff.WhichBuffToDMA = Judge_TxDataBuff.Buff_1;
		}
	}
	else
	{
		if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_0)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff0 = 9 + Temp->header.DataLength;
		}
		else if(Judge_TxDataBuff.WhichBuffToCPU == Judge_TxDataBuff.Buff_1)
		{
			Judge_TxDataBuff.HowLongWillSend_Buff1 = 9 + Temp->header.DataLength;
		}
	}
	
	NowSendingGraph = 0;

	return 0;	
	
	
}

void Judge_RadarLoop_1MS(void)
{//必须每1ms都要执行一次这个函数，尽量精准的每1ms执行一次。此函数内无延时，正常情况下每0.1ms内完全可以执行完成此函数
	static uint64_t JudgeSendTime_ms = 151;
	static uint64_t JudgeSendLastTime_ms = 0;
	static uint8_t LastHadUndated = 1;
	static uint8_t LastUndateErr = 0;
	
	
	if(LastUndateErr != 0)
	{
		
	}
	
	if(JudgeSendTime_ms - JudgeSendLastTime_ms < 150)
	{
		return;
	}
	
}
/*
@FunctionName:Judge_LoopPer1MS
@Brief:
@Notice:必须每1ms都要执行一次这个函数，尽量精准的每1ms执行一次。此函数内无延时，正常情况下每0.1ms内完全可以执行完成此函数
@LatestUpdate:
@Author:汪晨晨
*/
void Judge_LoopPer1MS(void)
{
	static uint64_t JudgeSendTime_ms = 151;
	static uint64_t JudgeSendLastTime_ms = 0;
	static uint16_t ChannelCount = 0;
	static uint16_t LastUpdateChannel = 0;
	static uint8_t LastHadUndated = 1;
	static uint8_t SendGr_Temp = 0;

	JudgeSendTime_ms += 1;
	
	if(IsDrawEqualLine != 0)
		return;
	
	if(Channel[LastUpdateChannel].NeedUpdate != 0 && LastHadUndated == 0)
	{
		JudgeSendLastTime_ms = JudgeSendTime_ms;
		Channel[LastUpdateChannel].LastTime = JudgeSendTime_ms;
		if(LastUpdateChannel < CHANNEL_NUMBER_STR)
		{
			Judge_SendStrChannel(&Channel[LastUpdateChannel]);
		}
		else
		{
			Judge_SendGraphChannel(LastUpdateChannel);
		}
		
		if(Channel[LastUpdateChannel].NeedUpdate == 0)
		{
			LastHadUndated = 1;
			++ LastUpdateChannel;
			if(LastUpdateChannel >= CHANNEL_NUMBER_MAX)
				LastUpdateChannel = 0;
		}
		return;
	}
	
	if(JudgeSendTime_ms - JudgeSendLastTime_ms < Channel[LastUpdateChannel].TimeSpaceMS)
	{
		return;
	}
	
	for(ChannelCount = 0; ChannelCount < CHANNEL_NUMBER_MAX ; ++ ChannelCount)
	{
		
		if(Channel[LastUpdateChannel].NeedUpdate != 0)
		{
			JudgeSendLastTime_ms = JudgeSendTime_ms;
			Channel[LastUpdateChannel].LastTime = JudgeSendTime_ms;
			
			if(LastUpdateChannel < CHANNEL_NUMBER_STR)
			{
				Judge_SendStrChannel(&Channel[LastUpdateChannel]);
			}
			else
			{
				Judge_SendGraphChannel(LastUpdateChannel);
			}
			
			if(Channel[LastUpdateChannel].NeedUpdate == 0)
			{
				LastHadUndated = 1;
				++ LastUpdateChannel;
				if(LastUpdateChannel >= CHANNEL_NUMBER_MAX)
					LastUpdateChannel = 0;
			}
			else
			{
				LastHadUndated = 0; 
			}
			break;
		}
		else
		{
			++ LastUpdateChannel;
			if(LastUpdateChannel >= CHANNEL_NUMBER_MAX)
				LastUpdateChannel = 0;
		}
	}
}

//////////////////////////////////////////
//下面是测试函数：

void Judge_ClearFlag(void)
{
	Judge_UndateFlag.BuffDebuffStatus = 0;
	Judge_UndateFlag.bulletRemaining = 0;
	Judge_UndateFlag.DartClientCmd = 0;
	Judge_UndateFlag.DartRemainTime = 0;
	Judge_UndateFlag.DartStatus = 0;
	Judge_UndateFlag.droneEnergy = 0;
	Judge_UndateFlag.fieldEvent = 0;
	Judge_UndateFlag.gameResult = 0;
	Judge_UndateFlag.gameState = 0;
	Judge_UndateFlag.powerHeat = 0;
	Judge_UndateFlag.rfid_status = 0;
	Judge_UndateFlag.robotBUFF = 0;
	Judge_UndateFlag.robotHP = 0;
	Judge_UndateFlag.robotHurt = 0;
	Judge_UndateFlag.robotPosition = 0;
	Judge_UndateFlag.robotShoot = 0;
	Judge_UndateFlag.robotState = 0;
	Judge_UndateFlag.student_interactive_all_data = 0;
	Judge_UndateFlag.supplyAction = 0;
	Judge_UndateFlag.warning = 0;
	Judge_UndateFlag.self_control_interactive_all_data = 0;
	Judge_UndateFlag.ext_robot_command = 0;
	Judge_UndateFlag.key_and_mouse = 0;
}


/*
@FunctionName:Judge_TestSpeed
@Brief:
@Notice:
@LatestUpdate:
@Author:
*/
void Judge_TestSpeed(frame_cmd_id_e WhichMsg)
{//测试更新频率
	
	while(1)
	{
		Judge_ClearFlag();
		Judge_Delay_ms(1000);
		
		if((uint32_t)WhichMsg == JUDGE_SPEED_TEST_ID)
		{//打印所有数据
			printf("V:%d,%d,%d,%d,%d#%d,%d,%d,%d,%d#%d,%d,%d,%d,%d#%d,%d,%d,%d,%d#%d,%d,%d\n\r",
					Judge_UndateFlag.gameState,\
					Judge_UndateFlag.gameResult,\
					Judge_UndateFlag.robotHP,\
					Judge_UndateFlag.DartStatus,\
					Judge_UndateFlag.BuffDebuffStatus,\
					Judge_UndateFlag.fieldEvent,\
					Judge_UndateFlag.supplyAction,\
					Judge_UndateFlag.warning,\
					Judge_UndateFlag.DartRemainTime,\
					Judge_UndateFlag.robotState,\
					Judge_UndateFlag.powerHeat,\
					Judge_UndateFlag.robotPosition,\
					Judge_UndateFlag.robotBUFF,\
					Judge_UndateFlag.droneEnergy,\
					Judge_UndateFlag.robotHurt,\
					Judge_UndateFlag.robotShoot,\
					Judge_UndateFlag.bulletRemaining,\
					Judge_UndateFlag.rfid_status,\
					Judge_UndateFlag.DartClientCmd,\
					Judge_UndateFlag.student_interactive_all_data,\
					Judge_UndateFlag.self_control_interactive_all_data,\
					Judge_UndateFlag.ext_robot_command,\
					Judge_UndateFlag.key_and_mouse);
		}
		else
		{
			switch(WhichMsg)
			{
				case GAME_STATE_ID:
				{//比赛状态（1Hz）
					printf("V:%d\n\r",Judge_UndateFlag.gameState);
				}
				break;
				case GAME_RESUIT_ID:
				{//比赛结果
					printf("V:%d\n\r",Judge_UndateFlag.gameResult);
				}
				break;
				case ROBOT_HP_ID:
				{//机器人血量数据（1Hz）
					printf("HP:%d\n\r",Judge_UndateFlag.robotHP);
				}
				break;
//				case DART_FIRE_STATE:
//				{//飞镖发射状态
//					printf("V:%d\n\r",Judge_UndateFlag.DartStatus);
//				}
//				break;
				case GAIN_AND_PUNISHMENT_STATE:
				{//人工智能挑战赛加成与惩罚区状态，1HZ
					printf("V:%d\n\r",Judge_UndateFlag.BuffDebuffStatus);
				}
				break;
				case FIELD_EVENT_ID:
				{//场地事件，1HZ
					printf("V:%d\n\r",Judge_UndateFlag.fieldEvent);
				}
				break;
				case SUPPLY_ACTION_ID:
				{//补给站动作标识
					printf("V:%d\n\r",Judge_UndateFlag.supplyAction);
				}
				break;
				case JUDGE_WARNING_ID:
				{//裁判警告数据
					printf("V:%d\n\r",Judge_UndateFlag.warning);
				}
				break;
				case DART_FIRE_COUNTDOWN:
				{//飞镖发射口倒计时，1HZ发送
					printf("V:%d\n\r",Judge_UndateFlag.DartRemainTime);
				}
				break;
				case ROBOT_STATE_ID:
				{//机器人状态（10Hz）
					printf("V:%d\n\r",Judge_UndateFlag.robotState);
				}
				break;
				case REALTIME_POWER_ID:
				{//实时功率热量（50Hz）
					printf("V:%d\n\r",Judge_UndateFlag.powerHeat);
				}
				break;
				case ROBOT_POSITION_ID:
				{//机器人位置（10Hz）
					printf("V:%d\n\r",Judge_UndateFlag.robotPosition);
				}
				break;
				case ROBOT_BUFF_ID:
				{//机器人增益
					printf("V:%d\n\r",Judge_UndateFlag.robotBUFF);
				}
				break;
				case DRONE_POWER_ID:
				{//空中机器人能量状态数据，10HZ，只有空中机器人主控发送
					printf("V:%d\n\r",Judge_UndateFlag.droneEnergy);
				}
				break;
				case ROBOT_HURT_ID:
				{//伤害数据
					printf("V:%d\n\r",Judge_UndateFlag.robotHurt);
				}
				break;
				case ROBOT_SHOOT_ID:
				{//射击数据
					printf("V:%d\n\r",Judge_UndateFlag.robotShoot);
				}
				break;
				case SURPLUS_BULLET_ID:
				{//剩余子弹数（哨兵和空中机器人）
					printf("V:%d\n\r",Judge_UndateFlag.bulletRemaining);
				}
				break;
				case ROBOT_RFID_STATE:
				{//机器人RFID状态，1HZ周期发送
					printf("V:%d\n\r",Judge_UndateFlag.rfid_status);
				}
				break;
				case ROBOT_OPERATOR_CMD:
				{//飞镖机器人客户端指令数据，10HZ
					printf("V:%d\n\r",Judge_UndateFlag.DartClientCmd);
				}
				break;
				case ROBOT_INTERACTIVE_ID:
				{//机器人间交互，发送触发
					printf("V:%d\n\r",Judge_UndateFlag.student_interactive_all_data);
				}
				break;
				case SELF_CONTROL_INTERACTIVE_ID:
				{//机器人RFID状态，1HZ周期发送
					printf("V:%d\n\r",Judge_UndateFlag.self_control_interactive_all_data);
				}
				break;
				case ROBOT_COMMAND_ID:
				{//飞镖机器人客户端指令数据，10HZ
					printf("V:%d\n\r",Judge_UndateFlag.ext_robot_command);
				}
				break;
				case KEY_AND_MOUSE_ID:
				{//机器人间交互，发送触发
					printf("V:%d\n\r",Judge_UndateFlag.key_and_mouse);
				}
				break;
				default: break;
			}
		}
		
		
		
		//下面的字数太多了，可能一秒都发不完，所以直接单个测试
//		printf("裁判系统更新频率测试：\n\r \
//		比赛状态帧率:%d,\r\n	\
//		比赛结果帧率:%d,\r\n	\
//		血量帧率:%d,\r\n	\
//		飞镖发射状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛状态帧率:%d,\r\n	\
//		比赛结果信息:%d,\r\n	");
		
	}
}



/*
@FunctionName:Judge_GetFlag
@Brief:返回数据更新状态
@Notice:飞镖裁判系统的更新有问题，需要通过其他地方触发读取
@LatestUpdate:
@Author:
*/
uint16_t Judge_GetFlag(frame_cmd_id_e WhichMsg)
{		
		if((uint32_t)WhichMsg == 0x4000)
		{
			return (Judge_UndateFlag.gameState+\
					Judge_UndateFlag.gameResult+\
					Judge_UndateFlag.robotHP+\
					Judge_UndateFlag.DartStatus+\
					Judge_UndateFlag.BuffDebuffStatus+\
					Judge_UndateFlag.fieldEvent+\
					Judge_UndateFlag.supplyAction+\
					Judge_UndateFlag.warning+\
					Judge_UndateFlag.DartRemainTime+\
					Judge_UndateFlag.robotState+\
					Judge_UndateFlag.powerHeat+\
					Judge_UndateFlag.robotPosition+\
					Judge_UndateFlag.robotBUFF+\
					Judge_UndateFlag.droneEnergy+\
					Judge_UndateFlag.robotHurt+\
					Judge_UndateFlag.robotShoot+\
					Judge_UndateFlag.bulletRemaining+\
					Judge_UndateFlag.rfid_status+\
					Judge_UndateFlag.DartClientCmd+\
					Judge_UndateFlag.student_interactive_all_data+\
					Judge_UndateFlag.self_control_interactive_all_data+\
					Judge_UndateFlag.ext_robot_command+\
					Judge_UndateFlag.key_and_mouse);
		}
		else
		{
			switch(WhichMsg)
			{
				case GAME_STATE_ID:
				{//比赛状态（1Hz）
					return Judge_UndateFlag.gameState;
				}
				case GAME_RESUIT_ID:
				{//比赛结果
					return Judge_UndateFlag.gameResult;
				}
				case ROBOT_HP_ID:
				{//机器人血量数据（1Hz）
					return Judge_UndateFlag.robotHP;
				}
//				case DART_FIRE_STATE:
//				{//飞镖发射状态
//					return Judge_UndateFlag.DartStatus;
//				}
				case GAIN_AND_PUNISHMENT_STATE:
				{//人工智能挑战赛加成与惩罚区状态，1HZ
					return Judge_UndateFlag.BuffDebuffStatus;
				}
				case FIELD_EVENT_ID:
				{//场地事件，1HZ
					return Judge_UndateFlag.fieldEvent;
				}
				case SUPPLY_ACTION_ID:
				{//补给站动作标识
					return Judge_UndateFlag.supplyAction;
				}
				case JUDGE_WARNING_ID:
				{//裁判警告数据
					return Judge_UndateFlag.warning;
				}
				case DART_FIRE_COUNTDOWN:
				{//飞镖发射口倒计时，1HZ发送
					return Judge_UndateFlag.DartRemainTime;
				}
				case ROBOT_STATE_ID:
				{//机器人状态（10Hz）
					return Judge_UndateFlag.robotState;
				}
				case REALTIME_POWER_ID:
				{//实时功率热量（50Hz）
					return Judge_UndateFlag.powerHeat;
				}
				case ROBOT_POSITION_ID:
				{//机器人位置（10Hz）
					return Judge_UndateFlag.robotPosition;
				}
				case ROBOT_BUFF_ID:
				{//机器人增益
					return Judge_UndateFlag.robotBUFF;
				}
				case DRONE_POWER_ID:
				{//空中机器人能量状态数据，10HZ，只有空中机器人主控发送
					return Judge_UndateFlag.droneEnergy;
				}
				case ROBOT_HURT_ID:
				{//伤害数据
					return Judge_UndateFlag.robotHurt;
				}
				case ROBOT_SHOOT_ID:
				{//射击数据
					return Judge_UndateFlag.robotShoot;
				}
				case SURPLUS_BULLET_ID:
				{//剩余子弹数（哨兵和空中机器人）
					return Judge_UndateFlag.bulletRemaining;
				}
				case ROBOT_RFID_STATE:
				{//机器人RFID状态，1HZ周期发送
					return Judge_UndateFlag.rfid_status;
				}
				case ROBOT_OPERATOR_CMD:
				{//飞镖机器人客户端指令数据，10HZ
					return Judge_UndateFlag.DartClientCmd;
				}
				case ROBOT_INTERACTIVE_ID:
				{//机器人间交互，发送触发
					return Judge_UndateFlag.student_interactive_all_data;
				}
				case SELF_CONTROL_INTERACTIVE_ID:
				{//机器人RFID状态，1HZ周期发送
					return Judge_UndateFlag.self_control_interactive_all_data;
				}
				case ROBOT_COMMAND_ID:
				{//飞镖机器人客户端指令数据，10HZ
					return Judge_UndateFlag.ext_robot_command;
				}
				case KEY_AND_MOUSE_ID:
				{//机器人间交互，发送触发
					return Judge_UndateFlag.key_and_mouse;
				}
				default : break;
			}
		}
	return 0;		
}


/*
@FunctionName:Judge_ClearFlagOf
@Brief:清除更新标志
@Notice:
@LatestUpdate:
@Author:
*/
void Judge_ClearFlagOf(frame_cmd_id_e WhichMsg)
{
		if((uint32_t)WhichMsg == 0x4000)
		{
					Judge_UndateFlag.gameState = 0;
					Judge_UndateFlag.gameResult= 0;
					Judge_UndateFlag.robotHP= 0;
					Judge_UndateFlag.DartStatus= 0;
					Judge_UndateFlag.BuffDebuffStatus= 0;
					Judge_UndateFlag.fieldEvent= 0;
					Judge_UndateFlag.supplyAction= 0;
					Judge_UndateFlag.warning= 0;
					Judge_UndateFlag.DartRemainTime= 0;
					Judge_UndateFlag.robotState= 0;
					Judge_UndateFlag.powerHeat= 0;
					Judge_UndateFlag.robotPosition= 0;
					Judge_UndateFlag.robotBUFF= 0;
					Judge_UndateFlag.droneEnergy= 0;
					Judge_UndateFlag.robotHurt= 0;
					Judge_UndateFlag.robotShoot= 0;
					Judge_UndateFlag.bulletRemaining= 0;
					Judge_UndateFlag.rfid_status= 0;
					Judge_UndateFlag.DartClientCmd= 0;
					Judge_UndateFlag.student_interactive_all_data = 0;
		}
		else
		{
			switch(WhichMsg)
			{
				case GAME_STATE_ID:
				{//比赛状态（1Hz）
					Judge_UndateFlag.gameState = 0;
				}
				case GAME_RESUIT_ID:
				{//比赛结果
					Judge_UndateFlag.gameResult = 0;
				}
				case ROBOT_HP_ID:
				{//机器人血量数据（1Hz）
					 Judge_UndateFlag.robotHP = 0;
				}
//				case DART_FIRE_STATE:
//				{//飞镖发射状态
//					 Judge_UndateFlag.DartStatus = 0;
//				}
				case GAIN_AND_PUNISHMENT_STATE:
				{//人工智能挑战赛加成与惩罚区状态，1HZ
					 Judge_UndateFlag.BuffDebuffStatus = 0;
				}
				case FIELD_EVENT_ID:
				{//场地事件，1HZ
					 Judge_UndateFlag.fieldEvent = 0;
				}
				case SUPPLY_ACTION_ID:
				{//补给站动作标识
					 Judge_UndateFlag.supplyAction = 0;
				}
				case JUDGE_WARNING_ID:
				{//裁判警告数据
					 Judge_UndateFlag.warning = 0;
				}
				case DART_FIRE_COUNTDOWN:
				{//飞镖发射口倒计时，1HZ发送
					 Judge_UndateFlag.DartRemainTime = 0;
				}
				case ROBOT_STATE_ID:
				{//机器人状态（10Hz）
					 Judge_UndateFlag.robotState = 0;
				}
				case REALTIME_POWER_ID:
				{//实时功率热量（50Hz）
					 Judge_UndateFlag.powerHeat = 0;
				}
				case ROBOT_POSITION_ID:
				{//机器人位置（10Hz）
					 Judge_UndateFlag.robotPosition = 0;
				}
				case ROBOT_BUFF_ID:
				{//机器人增益
					 Judge_UndateFlag.robotBUFF = 0;
				}
				case DRONE_POWER_ID:
				{//空中机器人能量状态数据，10HZ，只有空中机器人主控发送
					 Judge_UndateFlag.droneEnergy = 0;
				}
				case ROBOT_HURT_ID:
				{//伤害数据
					 Judge_UndateFlag.robotHurt = 0;
				}
				case ROBOT_SHOOT_ID:
				{//射击数据
					 Judge_UndateFlag.robotShoot = 0;
				}
				case SURPLUS_BULLET_ID:
				{//剩余子弹数（哨兵和空中机器人）
					 Judge_UndateFlag.bulletRemaining = 0;
				}
				case ROBOT_RFID_STATE:
				{//机器人RFID状态，1HZ周期发送
					 Judge_UndateFlag.rfid_status = 0;
				}
				case ROBOT_OPERATOR_CMD:
				{//飞镖机器人客户端指令数据，10HZ
					 Judge_UndateFlag.DartClientCmd = 0;
				}
				case ROBOT_INTERACTIVE_ID:
				{//机器人间交互，发送触发
					 Judge_UndateFlag.student_interactive_all_data = 0;
				}
				default :break;
			}
		}	
}



//////////////////////////////////////////////////////////////////////////////////////////
//****************************************************************************************
//########################################################################################
//下面是接口函数区，需要什么数据就写函数返回什么数据
//如下：



ext_game_robot_status_t* Judge_GetRobotState(void)
{
	return &judge_recv_mesg.robotState;
}

ext_game_robot_pos_t*	Judge_GetRobotPosition(void)
{
	return &judge_recv_mesg.robotPosition;
}

ext_power_heat_data_t* Judge_GetRobot_power_heat(void)
{
	return &judge_recv_mesg.powerHeat;
}

ext_shoot_data_t* Judge_shoot_data(void)
{
	return &judge_recv_mesg.robotShoot;
}

ext_game_robot_status_t* Judge_robot_status(void)
{
	return &judge_recv_mesg.robotState;
}

ext_power_heat_data_t* Judge_heat(void)
{
	return &judge_recv_mesg.powerHeat;
}
ext_robot_hurt_t* Judge_RobotHurt(void)
{
	return &judge_recv_mesg.robotHurt;
}
ext_game_state_t* Judge_Gametime(void)
{
	return &judge_recv_mesg.gameState;
}

ext_buff_t* Judge_BuffState(void)
{
	return &judge_recv_mesg.robotBUFF;
}
//上面是接口函数区。下面是接口函数区。下面是接口函数区。下面是接口函数区。下面是接口函数区。
//########################################################################################
//****************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////
ext_game_state_t Judge_Game_State(void){
	return judge_recv_mesg.gameState;
}

ext_bullet_remaining_t* JudgeRemainBulletNum(void){
	return &judge_recv_mesg.bulletRemaining;
}

ext_game_robot_HP_t Judge_Robot_HP(){
	return judge_recv_mesg.robotHP;
}
ext_game_robot_status_t    Judge_Robot_State(){
	return judge_recv_mesg.robotState;
}


