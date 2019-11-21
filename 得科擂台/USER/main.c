/********************************************
* 版本:	TheMonster_1.1_2017.08.09
*********************************************/
#include "init.h"
#include "function.h"		//所有业务详细子程序和详细模块程序都放在function.h头文件，main.c文件主要写跑的模块程序。
#include "ysy.h"				//2017.10
#include "edge.h"

#define AI 							analog
#define DI							digital
#define ClearScreen 		clear_screen
#define ShowStr 				display_GB2312_string		//字符串数显
#define ShowAI					display_GB2312_u16      //四位数显
#define ShowDI					display_GB2312_u1		    //一位数显
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
int attack_speed=700,         //速度初始值
		normal_speed=600,
		back_speed=-800,	
		max_speed=900,            //全速攻击速度
		attack_dis=1000,           //台上寻敌的距离
		judge_stage_dis=1200,     //判断台上下传感器距离
		temp,
		t=0,
		location_flag,            //判断台上下的变量
		front_flag=0,
		left_flag= 0,
		right_flag=0,
		back_flag= 0,
		except_left_flag = 0,
		except_right_flag = 0,
		except_front_flag = 0,
		except_back_flag = 0,
		ltxback=0,                //台下上台的变量
		up_stage_delay=0,
		left=0,
		right=0,
		scr_n=0,
		zj=0,
		defend_flag=0,              //守台标志
		attack_flag=0,              //攻击标志
		max_speed_flag=0;           //全速攻击标志

int main(void)
{
//	int i,n=0;
	MarineRobot_init();
	start_feedback();//启动反馈

	while(1)
	{
		Key = KEY_Scan(0);
		Key_Temp = Key;
		ShowStr(0,1,"1、检测模式");
		ShowStr(2,1,"2、主程序");
		ShowStr(4,1,"3、测试程序");
		ShowStr(6,1,"4、电机正反转测试");
		Key = KEY_Scan(0);
		if(Key != 0) Key_Temp = Key;
		if(Key_Temp==1)//按键1程序段
		{
			test();
		}
		else if(Key_Temp==2)//按键2程序段
		{
			//主程序
			set_speed();
			ClearScreen();
			ShowStr(2,1,"gogogo!!!");
			while(1)
			{
				if(AI(10)>=2000) break;
			}
	  	up_stage();                          //单次登台
			while(1)
			{
				//get_edge();
				//GoodMoto(0,0);	
				//select_high();
				//find_single();
				worker();
			}
		}
		else if(Key_Temp==3)//按键3程序段
		{
			ClearScreen();
		}
		else if(Key_Temp==4)//按键4程序段
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
		//判断台上下
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
		if(front_flag == 1 && (back_flag == 1 || AI(10)>judge_stage_dis || AI(6)>judge_stage_dis))													//前2 && （后||左||右）
			except_front_flag = 1;
		if(back_flag == 1 && (front_flag == 1 || AI(10)>judge_stage_dis||AI(11)>judge_stage_dis || AI(6)>judge_stage_dis||AI(5)>judge_stage_dis))	//后 && （前任一 || 左|| 右）
				except_back_flag = 1;
		if(left_flag == 1 && (right_flag == 1 || AI(3)>judge_stage_dis || AI(8)>judge_stage_dis))														//左任一 && （右任一 || 前 || 后）
				except_left_flag = 1;
		if(right_flag == 1 && (left_flag == 1 || AI(2)>judge_stage_dis || AI(8)>judge_stage_dis))														//右任一 && （左任一 || 前 || 后）
				except_right_flag = 1;
		
		ClearScreen();
		if(except_front_flag == 1 ||  except_back_flag == 1 || except_left_flag == 1 || except_right_flag == 1)
		{
			location_flag 	 = 1;				//擂台下
			ShowStr(2,2,"t a i   x i a------------------");
		}
			
		else
		{
			location_flag 	 = 0;				//擂台上
			ShowStr(2,2,"t a i  s h a n g-------------------");
		}

		return location_flag;
}

int off_stage()
{
	ltxback=0;
	//台下程序
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
		遇敌返回1，未遇敌返回0
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
		遇敌返回1，未遇敌返回0
		*/
		left=DI(1);
		right=DI(2);
		if(left || right)//前铲出台
		{
			defend_flag=1;                 //守台标志
			GoodMoto(-200,-200);					 //短暂倒车
			delay_ms(100);
			GoodMoto(0,0);
			delay_ms(40);
			if(AI(2)<700 || AI(3)<700 ||AI(2) >2200 ||AI(3) >2200) defend_flag=0;
			else  {GoodMoto(0,0);return 1;} //守敌
			//遇敌转向
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
	check_edge();//检查边缘
	if(defend_flag==1)
	{
		//守台
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
			GoodMoto(900,900);                                   //全速攻击速度
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
		//遇敌转向
		attack_flag=0;
		turn_to_attack();
	}
	return 0;
}

int worker(void)
{
	while(1)
	{
		//判断台上下
		location_flag=judge_stage();
		if(location_flag)
		{
			//擂台下
			off_stage();
		}
		else
		{
			//擂台上
			on_stage();
			//GoodMoto(0,0);
		}
	}
}
