/********************************************
* �汾:	TheMonster_1.1_2017.08.09
*********************************************/
#include "init.h"
#include "function.h"		//����ҵ����ϸ�ӳ������ϸģ����򶼷���function.hͷ�ļ���main.c�ļ���Ҫд�ܵ�ģ�����
#include "ysy.h"				//2017.10
#include "edge.h"

#define AI 							analog
#define DI							digital
#define ClearScreen 		clear_screen
#define ShowStr 				display_GB2312_string		//�ַ�������
#define ShowAI					display_GB2312_u16      //��λ����
#define ShowDI					display_GB2312_u1		    //һλ����
#define ServoSetMode		UP_CDS_SetMode					
#define ServoSetAngle		UP_CDS_SetAngle	

void RobotInit(void);
void AD_Detection(void);
void GoodMoto(int lspeed,int rspeed);
void set_speed(void);
int worker(void);
int judge_stage(void);
int off_stage(void);

extern int Rs,Ls;
extern int attack_speed;
extern int normal_speed;
extern int back_speed;

u8 Key = 0;
u8 Key_Temp = 0;
int attack_speed=700,         //�ٶȳ�ʼֵ
		normal_speed=600,
		back_speed=-800,	
		max_speed=900,            //ȫ�ٹ����ٶ�
		attack_dis=1000,           //̨��Ѱ�еľ���
		judge_stage_dis=1200,     //�ж�̨���´���������
		temp,
		t=0,
		location_flag,            //�ж�̨���µı���
		front_flag=0,
		left_flag= 0,
		right_flag=0,
		back_flag= 0,
		except_left_flag = 0,
		except_right_flag = 0,
		except_front_flag = 0,
		except_back_flag = 0,
		ltxback=0,                //̨����̨�ı���
		up_stage_delay=0,
		left=0,
		right=0,
		scr_n=0,
		zj=0,
		defend_flag=0,              //��̨��־
		attack_flag=0,              //������־
		max_speed_flag=0;           //ȫ�ٹ�����־

int main(void)
{
//	int i,n=0;
	MarineRobot_init();
	start_feedback();//��������

	while(1)
	{
		Key = KEY_Scan(0);
		Key_Temp = Key;
		ShowStr(0,1,"1�����ģʽ");
		ShowStr(2,1,"2��������");
		ShowStr(4,1,"3�����Գ���");
		ShowStr(6,1,"4���������ת����");
		Key = KEY_Scan(0);
		if(Key != 0) Key_Temp = Key;
		if(Key_Temp==1)//����1�����
		{
			test();
		}
		else if(Key_Temp==2)//����2�����
		{
			//������
			set_speed();
			ClearScreen();
			ShowStr(2,1,"gogogo!!!");
			while(1)
			{
				if(AI(10)>=2000) break;
			}
	  	up_stage();                          //���ε�̨
			while(1)
			{
				//get_edge();
				//GoodMoto(0,0);	
				//select_high();
				//find_single();
				worker();
			}
		}
		else if(Key_Temp==3)//����3�����
		{
			ClearScreen();
		}
		else if(Key_Temp==4)//����4�����
		{
			Key=0;
			while(1)
			{
				ClearScreen();
				ShowStr(6,1,"GoodMoto(50,-50)");
				GoodMoto(50,-50);
				delay_ms(50);
				Key=KEY_Scan(0);
				if(Key != 0)
				{
					GoodMoto(0,0);
					break;
				}
			}
			//touch_sensor();
		}
	}
}

int judge_stage()
{
		//�ж�̨����
		front_flag=0;
		left_flag= 0;
		right_flag=0;
		back_flag= 0;
		except_left_flag = 0;
		except_right_flag = 0;
		except_front_flag = 0;
		except_back_flag = 0;
	
		if( AI(2)>judge_stage_dis || AI(3) >judge_stage_dis )
			front_flag=1;
		if(AI(11)>judge_stage_dis || AI(10) > judge_stage_dis ||AI(9)>judge_stage_dis)
			left_flag=1;
		if(AI(5)>judge_stage_dis || AI(6) > judge_stage_dis ||AI(7)>judge_stage_dis)
			right_flag=1;
		if(AI (8)>judge_stage_dis ||AI(14)>judge_stage_dis)
			back_flag=1;
		if(front_flag == 1 && (back_flag == 1 || AI(10)>judge_stage_dis || AI(6)>judge_stage_dis))													//ǰ2 && ����||��||�ң�
			except_front_flag = 1;
		if(back_flag == 1 && (front_flag == 1 || AI(10)>judge_stage_dis||AI(11)>judge_stage_dis || AI(6)>judge_stage_dis||AI(5)>judge_stage_dis))	//�� && ��ǰ��һ || ��|| �ң�
				except_back_flag = 1;
		if(left_flag == 1 && (right_flag == 1 || AI(3)>judge_stage_dis || AI(8)>judge_stage_dis))														//����һ && ������һ || ǰ || ��
				except_left_flag = 1;
		if(right_flag == 1 && (left_flag == 1 || AI(2)>judge_stage_dis || AI(8)>judge_stage_dis))														//����һ && ������һ || ǰ || ��
				except_right_flag = 1;
		
		ClearScreen();
		if(except_front_flag == 1 ||  except_back_flag == 1 || except_left_flag == 1 || except_right_flag == 1)
		{
			location_flag 	 = 1;				//��̨��
			ShowStr(2,2,"t a i   x i a------------------");
		}
			
		else
		{
			location_flag 	 = 0;				//��̨��
			ShowStr(2,2,"t a i  s h a n g-------------------");
		}

		return location_flag;
}

int off_stage()
{
	ltxback=0;
	//̨�³���
	if( (AI(2) > 2000  && (AI(8)>1100 || AI(14)>1100)  &&  AI(12)<600 &&  AI(6) <=600 && AI (10) <=600))
	{
		up_stage_delay+=1;
		if(up_stage_delay>3)
		{
			GoodMoto(400,400);
			delay_ms(400);
			GoodMoto(0,0);
			delay_ms(40);
			GoodMoto(back_speed,back_speed);
			delay_ms(1000);
			up_stage_delay=0;
			GoodMoto(0,0);
			delay_ms(40);
			GoodMoto(back_speed,0);
			delay_ms(200);
		}
	}
	else if(  (AI(2)>2000 || AI(3)>2000) && AI(5)>1200 && AI(6)>550 )
	{
		GoodMoto(200,200);
		ltxback=1;
	}
	else if( (AI(2)>2000 || AI(3)>2000) && AI(11)>1200 && AI(10)>550)
	{
		GoodMoto(200,200);
		ltxback=2;
	}
	else
	{
		left=AI(2);
		right=AI(3);
		if( left>2100 || right>2100)
		{
			GoodMoto(-200,-200);
			delay_ms(100);
		}
		else 
		{
			scr_n+=1;
			GoodMoto(200,-200);
			if(scr_n>200)
			{
				scr_n=0;
				GoodMoto(300,300);
				delay_ms(1000);
			}
		}
	}
	
	if( ltxback==1 || ltxback==2 ) zj++;
	else zj=0;
	
	if(zj>100)
	{
		if(ltxback==2)
		{
			GoodMoto(-300,-300);
			delay_ms(200);
			while(AI(2)>500 && AI(3)>500) 
			{
				GoodMoto(200,-200);
			}
			GoodMoto(400,400);
			delay_ms(1000);
			GoodMoto(0,0);
			delay_ms(40);
			GoodMoto(-500,500);
			delay_ms(200);
			zj=0;
		}
		else if(ltxback==1)
		{
			GoodMoto(-300,-300);
			delay_ms(200);
			while(AI(2)>500 && AI(3)>500) 
			{
				GoodMoto(-200,200);
			}
			GoodMoto(400,400);
			delay_ms(1000);
			GoodMoto(0,0);
			delay_ms(40);
			GoodMoto(500,-500);
			delay_ms(200);
			zj=0;
		}
	}
	return 0;
}

int turn_to_attack()
{/* 
		���з���1��δ���з���0
	*/
			if(AI(2)>=attack_dis || AI(3)>=attack_dis) return 1;
			else if(AI(1)>= attack_dis || AI(11)>=attack_dis || AI(10)>= attack_dis || AI(9)>=attack_dis || AI(14)>=attack_dis)
			{
				GoodMoto(-700,700);
				//delay_ms(100);
				return 1;
			}
			else if(AI(13) >= attack_dis || AI(5) >=attack_dis || AI(6) >=attack_dis || AI(7)>= attack_dis || AI(8) >= attack_dis)
			{
				GoodMoto(700,-700);
				//delay_ms(100);
				return 1;
			}
			else 
			{
				return 0;
			}
}

int check_edge()
{
		/* 
		���з���1��δ���з���0
		*/
		left=DI(1);
		right=DI(2);
		if(left || right)//ǰ����̨
		{
			defend_flag=1;                 //��̨��־
			GoodMoto(-200,-200);					 //���ݵ���
			delay_ms(100);
			GoodMoto(0,0);
			delay_ms(40);
			if(AI(2)<700 || AI(3)<700 ||AI(2) >2200 ||AI(3) >2200) defend_flag=0;
			else  {GoodMoto(0,0);return 1;} //�ص�
			//����ת��
			if(turn_to_attack()==1) return 1;
			
			GoodMoto(-600,-600);
			delay_ms(250);
			
			if(left)
			{
				GoodMoto(0,-800);
			}
			else if(right)
			{	
				GoodMoto(-800,0);
			}
			if(left && right)
				delay_ms(400);
			else
				delay_ms(200);
			GoodMoto(0,0);
			delay_ms(40);
		}

		return 0;
}

int on_stage()
{
	check_edge();//����Ե
	if(defend_flag==1)
	{
		//��̨
		if(AI(2)<700 || AI(3)<700 ||AI(2) >2200 ||AI(3) >2200) defend_flag=0;
		else  {GoodMoto(0,0);return 0;}
	}
	
	if(attack_flag==0)
	{
		GoodMoto(normal_speed,normal_speed);
		max_speed_flag=0;
	}
	else
	{
		if(max_speed_flag>=100)   
		{
			GoodMoto(900,900);                                   //ȫ�ٹ����ٶ�
		}
		else  
		{
			GoodMoto(attack_speed,attack_speed);max_speed_flag+=1;
		}
	}
	
	attack_flag=0;
	if(AI(2)>=attack_dis || AI(3)>=attack_dis) 
	{
		attack_flag=1;
		check_edge();
	}
	else
	{
		//����ת��
		attack_flag=0;
		turn_to_attack();
	}
	return 0;
}

int worker(void)
{
	while(1)
	{
		//�ж�̨����
		location_flag=judge_stage();
		if(location_flag)
		{
			//��̨��
			off_stage();
		}
		else
		{
			//��̨��
			on_stage();
			//GoodMoto(0,0);
		}
	}
}
