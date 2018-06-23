
/*
 *	坦克大战主程序，编码时功能尽量编写成函数再调用，以免在mypaint里面代码太多
 */

#include "stdafx.h"
#include <stdio.h>

#include "mmsystem.h"//导入声音头文件
#pragma comment(lib,"winmm.lib")//导入声音头文件库
#define OBSTACLE_NUMBER 40  //定义障碍物的个数
#define REWARDS_NUMBER 1

HINSTANCE hInst;
HBITMAP  bg,start,tankImg[4],tankImg_2[4],bullet,bullet_2,obstacle1,obstacle2,triangle,explain,scoreBackground,rewards,grass;						//图片;						//图片
HDC		hdc, mdc, bufdc;
HWND	hWnd;
DWORD	tPre, tNow;						//控制刷新频率


bool reward_exit=true;
bool isStart = false;				//游戏开始
int triangelY = 90;				//开始界面三角形y坐标
bool showExplain = false;			//是否显示说明图

//尝试写两个坦克同时移动
bool state_top,state_left,state_bottom,state_right;			//检测方向键
bool state_w,state_a,state_d,state_s;			//检测wsad键 
int tank1_flag,tank2_flag;						//控制坦克移动速度

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);		
void				StartPaint(HDC hdc);
BOOL isObstable(int tank_x,int tank_y);
BOOL isRewards_1(int tank_x,int tank_y);   //吃到奖品加生命值

void tank1_Shot(int bx,int by,int ii);  //坦克1(绿色)被子弹击中  子弹的坐标
void tank2_Shot(int bx_2,int by_2,int i_2);  //坦克2(绿色)被子弹击中  子弹的坐标 
void setObstacle();  //设置障碍对象数组的各个参数  障碍位置和生命值
void setReward();  //设置奖励数组的各个参数  位置
void tank1_beatObstacle(int bx, int by,int ii);  //坦克1子弹 击中障碍
void tank2_beatObstacle(int bx_2, int by_2,int ii_2);  //坦克2的子弹 击中障碍



Obstacle *obstacle=new Obstacle[OBSTACLE_NUMBER]; //设置障碍对象数组 记录障碍位置和生命值
Obstacle *reward=new Obstacle[REWARDS_NUMBER]; //设置奖励对象数组 记录奖励位置

//为方便暂时作为全局变量
//初始化的坦克1
Tank myTank(100,100,LEFT,1);				//define left 2
//初始化的坦克2
Tank myTank_2(30,30,RIGHT,1);				//测试先将tank的生命值设置为1  当被子弹打到就消失


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	setObstacle();   //  设置障碍和奖品 初始值 这样不会导致障碍一开始不显示或者碰撞处理的时候才显示 或者打不掉
	setReward();

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	GetMessage(&msg, NULL, 0, 0);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tNow = GetTickCount();
			if (tNow - tPre >= 10){				//刷新频率
				if(isStart)				//游戏已经开始
					MyPaint(hdc);
				else
					StartPaint(hdc);			//游戏未开始
			}
		}
	}

	return msg.wParam;
}

//注册窗口类，不需要明白原理
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "canvas";
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

//初始化
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;

	hWnd = CreateWindow("canvas", "Window", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd, 50, 50, 650, 480, true);  //111111
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	bmp = CreateCompatibleBitmap(hdc, 850, 480);
	SelectObject(mdc, bmp);
		

	//加载图片
	start = (HBITMAP)LoadImage(NULL, "start.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);			//开始界面
	//bg = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);				//背景图
	//奖品
	rewards= (HBITMAP)LoadImage(NULL, "rewards.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);	
	//上下左右对应 0 1 2 3 
	tankImg[0] = (HBITMAP)LoadImage(NULL, "top.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				//坦克1
	tankImg[2] = (HBITMAP)LoadImage(NULL, "left.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[3] = (HBITMAP)LoadImage(NULL, "right.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[1] = (HBITMAP)LoadImage(NULL, "bottom.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	bullet = (HBITMAP)LoadImage(NULL, "bullet.bmp", IMAGE_BITMAP, 10, 20, LR_LOADFROMFILE);		

	tankImg_2[0] = (HBITMAP)LoadImage(NULL, "top_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				//坦克2
	tankImg_2[2] = (HBITMAP)LoadImage(NULL, "left_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg_2[3] = (HBITMAP)LoadImage(NULL, "right_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg_2[1] = (HBITMAP)LoadImage(NULL, "bottom_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	bullet_2= (HBITMAP)LoadImage(NULL, "bullet_2.bmp", IMAGE_BITMAP, 10, 20, LR_LOADFROMFILE);	

	obstacle1= (HBITMAP)LoadImage(NULL, "obstacle1.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);	//障碍
	obstacle2 = (HBITMAP)LoadImage(NULL, "obstacle2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);				//背景		
	triangle = (HBITMAP)LoadImage(NULL, "triangle.bmp", IMAGE_BITMAP, 20, 40, LR_LOADFROMFILE);				//三角形		
	explain = (HBITMAP)LoadImage(NULL, "explain.bmp", IMAGE_BITMAP, 300, 400, LR_LOADFROMFILE);				//三角形		
	scoreBackground = (HBITMAP)LoadImage(NULL, "scoreBackground.bmp", IMAGE_BITMAP, 200, 450, LR_LOADFROMFILE);		
	grass = (HBITMAP)LoadImage(NULL, "grass.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);		
	MyPaint(hdc);					//调用绘图函数

	return TRUE;
}

void tank1_Shot(int bx,int by,int ii)    //坦克1(绿色)被子弹击中  子弹的坐标
{
	Bullet *b_2 = myTank_2.GetBullet();	
	int tank_x=myTank.GetX();
	int tank_y=myTank.GetY();
		Bullet temp = b_2[ii];	
		if(temp.exist == false)
			return;
		if(temp.GetX()>=tank_x && temp.GetX()<=(tank_x+30) && temp.GetY()>=tank_y && temp.GetY()<=(tank_y+30))
	  {
		myTank.ReduceTankValue();
	    b_2[ii].exist=false;
		return;
	  }
		else if((temp.GetX()+30)>=tank_x && (temp.GetX()+30)<=(tank_x+30) && temp.GetY()>=tank_y && temp.GetY()<=(tank_y+30))
		{
			myTank.ReduceTankValue();
	        b_2[ii].exist=false;
		  return;

		}
		else if((temp.GetX()+30)>=tank_x && (temp.GetX()+30)<=(tank_x+30) && (temp.GetY()+30)>=tank_y && (temp.GetY()+30)<=(tank_y+30))
		{
		
			myTank.ReduceTankValue();
	        b_2[ii].exist=false;
		   return;
		}
	    else if(temp.GetX()>=tank_x && temp.GetX()<=(tank_x+30) && (temp.GetY()+30)>=tank_y && (temp.GetY()+30)<=(tank_y+30))
	   {
		  myTank.ReduceTankValue();
	      b_2[ii].exist=false;
		 return;
	   }


}

void tank2_Shot(int bx_2,int by_2,int i_2)   //坦克2 被坦克1 击中   子弹坐标 和子弹数组下标
{
	Bullet *b = myTank.GetBullet();	
	int tank_x=myTank_2.GetX();
	int tank_y=myTank_2.GetY();
		Bullet temp = b[i_2];	
		if(temp.exist == false)
			return;
		if(temp.GetX()>=tank_x && temp.GetX()<=(tank_x+30) && temp.GetY()>=tank_y && temp.GetY()<=(tank_y+30))
	  {
		myTank_2.ReduceTankValue();
	    b[i_2].exist=false;
		return;
	  }
		else if((temp.GetX()+30)>=tank_x && (temp.GetX()+30)<=(tank_x+30) && temp.GetY()>=tank_y && temp.GetY()<=(tank_y+30))
		{
			myTank_2.ReduceTankValue();
	         b[i_2].exist=false;
		  return;

		}
		else if((temp.GetX()+30)>=tank_x && (temp.GetX()+30)<=(tank_x+30) && (temp.GetY()+30)>=tank_y && (temp.GetY()+30)<=(tank_y+30))
		{
		
			myTank_2.ReduceTankValue();
	        b[i_2].exist=false;
		   return;
		}
	    else if(temp.GetX()>=tank_x && temp.GetX()<=(tank_x+30) && (temp.GetY()+30)>=tank_y && (temp.GetY()+30)<=(tank_y+30))
	   {
		  myTank_2.ReduceTankValue();
	       b[i_2].exist=false;
		 return;
	   }

}

void tank1_beatObstacle(int bx, int by,int ii)  //坦克1的子弹 击中障碍
{
	Bullet *b = myTank.GetBullet();	
	Bullet temp = b[ii];	
	for(int i=0;i<OBSTACLE_NUMBER;i++)
	{
		if(temp.exist == false)
			continue;
		if(temp.GetX()>=obstacle[i].GetOX() && temp.GetX()<=(obstacle[i].GetOX()+30) && temp.GetY()>=obstacle[i].GetOY() && temp.GetY()<=(obstacle[i].GetOY()+30))
	  {
		  obstacle[i].ReduceObstacleValue();
	      b[ii].exist=false;
		  continue;
	  }
		else if((temp.GetX()+30)>=obstacle[i].GetOX() && (temp.GetX()+30)<=(obstacle[i].GetOX()+30) && temp.GetY()>=obstacle[i].GetOY() && temp.GetY()<=(obstacle[i].GetOY()+30))
		{
			obstacle[i].ReduceObstacleValue();
	         b[ii].exist=false;
		    continue;

		}
		else if((temp.GetX()+30)>=obstacle[i].GetOX() && (temp.GetX()+30)<=(obstacle[i].GetOX()+30) && (temp.GetY()+30)>=obstacle[i].GetOY() && (temp.GetY()+30)<=(obstacle[i].GetOY()+30))
		{
		
			obstacle[i].ReduceObstacleValue();
	        b[ii].exist=false;
		   continue;
		}
	    else if(temp.GetX()>=obstacle[i].GetOX() && temp.GetX()<=(obstacle[i].GetOX()+30) && (temp.GetY()+30)>=obstacle[i].GetOY() && (temp.GetY()+30)<=(obstacle[i].GetOY()+30))
	   {
		  obstacle[i].ReduceObstacleValue();
	       b[ii].exist=false;
		 continue;
	   }
	}


}

void tank2_beatObstacle(int bx_2, int by_2,int ii_2)  //坦克2的子弹 击中障碍
{
	Bullet *b = myTank_2.GetBullet();	
	Bullet temp = b[ii_2];	
	for(int i=0;i<OBSTACLE_NUMBER;i++)
	{
		if(temp.exist == false)
			continue;
		if(temp.GetX()>=obstacle[i].GetOX() && temp.GetX()<=(obstacle[i].GetOX()+30) && temp.GetY()>=obstacle[i].GetOY() && temp.GetY()<=(obstacle[i].GetOY()+30))
	  {
		  obstacle[i].ReduceObstacleValue();
	      b[ii_2].exist=false;
		  continue;
	  }
		else if((temp.GetX()+30)>=obstacle[i].GetOX() && (temp.GetX()+30)<=(obstacle[i].GetOX()+30) && temp.GetY()>=obstacle[i].GetOY() && temp.GetY()<=(obstacle[i].GetOY()+30))
		{
			obstacle[i].ReduceObstacleValue();
	         b[ii_2].exist=false;
		    continue;

		}
		else if((temp.GetX()+30)>=obstacle[i].GetOX() && (temp.GetX()+30)<=(obstacle[i].GetOX()+30) && (temp.GetY()+30)>=obstacle[i].GetOY() && (temp.GetY()+30)<=(obstacle[i].GetOY()+30))
		{
		
			obstacle[i].ReduceObstacleValue();
	        b[ii_2].exist=false;
		   continue;
		}
	    else if(temp.GetX()>=obstacle[i].GetOX() && temp.GetX()<=(obstacle[i].GetOX()+30) && (temp.GetY()+30)>=obstacle[i].GetOY() && (temp.GetY()+30)<=(obstacle[i].GetOY()+30))
	   {
		  obstacle[i].ReduceObstacleValue();
	       b[ii_2].exist=false;
		 continue;
	   }
	}


}

void setObstacle()
{
	obstacle[0].Set(130, 150);	//1
	obstacle[1].Set(160,150);
	obstacle[2].Set(190,150);
	obstacle[3].Set(280,150);////////40 个障碍  设置i%5==0 是 可以被打掉的障碍-> obstacle2
	obstacle[4].Set(310,150);
	obstacle[5].Set(340,150);
	obstacle[6].Set(430,150);
	obstacle[7].Set(520,150);
	obstacle[8].Set(130,180);	//2
	obstacle[9].Set(280,180);
	obstacle[10].Set(430,180);
	obstacle[11].Set(520,180);
								//2'
	obstacle[31].Set(130,210);	
	obstacle[32].Set(280,210);
	obstacle[33].Set(430,210);
	obstacle[34].Set(520,210);

	obstacle[12].Set(130,240); //3
	obstacle[13].Set(160,240);
	obstacle[14].Set(190,240);
	obstacle[15].Set(280,240);
	obstacle[16].Set(430,240);
	obstacle[17].Set(520,240); 

	obstacle[18].Set(190,270);//4
	obstacle[19].Set(280,270);
	obstacle[20].Set(430,270);
	obstacle[21].Set(520,270);

	obstacle[35].Set(190,300);//4'
	obstacle[36].Set(280,300);
	obstacle[37].Set(430,300);
	obstacle[38].Set(520,300);

	obstacle[22].Set(130,330);
	obstacle[23].Set(160,330);
	obstacle[24].Set(190,330);
	obstacle[25].Set(280,330);
	obstacle[26].Set(310,330);
	obstacle[27].Set(340,330);
	obstacle[28].Set(430,330);
	obstacle[29].Set(460,330);
	obstacle[30].Set(490,330);
	obstacle[39].Set(520,330);

	for(int i=0;i<OBSTACLE_NUMBER;i++)   //先设置障碍的生命值都为1
	{
		obstacle[i].SetValue(1);
	}

}
void setReward()
{
	reward[0].Set(340,150);
}
BOOL isObstable(int tank_x,int tank_y)
{

	for(int i=0;i<OBSTACLE_NUMBER;i++)
	{
		if(tank_x>=obstacle[i].GetOX() && tank_x<=(obstacle[i].GetOX()+30) && tank_y>=obstacle[i].GetOY() && tank_y<=(obstacle[i].GetOY()+30))
		{
			return true;
		}
		else if((tank_x+30)>=obstacle[i].GetOX() && (tank_x+30)<=(obstacle[i].GetOX()+30) && tank_y>=obstacle[i].GetOY() && tank_y<=(obstacle[i].GetOY()+30))
		{
			return true;
		}
		else if((tank_x+30)>=obstacle[i].GetOX() && (tank_x+30)<=(obstacle[i].GetOX()+30) && (tank_y+30)>=obstacle[i].GetOY() && (tank_y+30)<=(obstacle[i].GetOY()+30))
		{
			return true;
		}
	    else if(tank_x>=obstacle[i].GetOX() && tank_x<=(obstacle[i].GetOX()+30) && (tank_y+30)>=obstacle[i].GetOY() && (tank_y+30)<=(obstacle[i].GetOY()+30))
		{
			return true;
		}
		
     
	}
	return false;
}
void tank1_Move();
void tank2_Move();
BOOL isRewards_1(int tank_x,int tank_y)  //1 吃到奖品加生命值
{

	for(int i=0;i<REWARDS_NUMBER;i++)
	{
		if(tank_x>=reward[i].GetOX() && tank_x<=(reward[i].GetOX()+30) && tank_y>=reward[i].GetOY() && tank_y<=(reward[i].GetOY()+30))
		{
			myTank.AddTankValue();
			reward_exit=false;
			return true;
		}
		else if((tank_x+30)>=reward[i].GetOX() && (tank_x+30)<=(reward[i].GetOX()+30) && tank_y>=reward[i].GetOY() && tank_y<=(reward[i].GetOY()+30))
		{
			myTank.AddTankValue();
			reward_exit=false;
			return true;
		}
		else if((tank_x+30)>=reward[i].GetOX() && (tank_x+30)<=(reward[i].GetOX()+30) && (tank_y+30)>=reward[i].GetOY() && (tank_y+30)<=(reward[i].GetOY()+30))
		{
			myTank.AddTankValue();
			reward_exit=false;
			return true;
		}
	    else if(tank_x>=reward[i].GetOX() && tank_x<=(reward[i].GetOX()+30) && (tank_y+30)>=reward[i].GetOY() && (tank_y+30)<=(reward[i].GetOY()+30))
		{
			myTank.AddTankValue();
			reward_exit=false;
			return true;
		}
		
	}
	return false;
}
void StartPaint(HDC hdc)
{	
	
	if(isStart == false){
		//贴开始界面
		SelectObject(bufdc, start);
		BitBlt(mdc, 0, 0, 650, 450, bufdc, 0, 0, SRCCOPY);		
		//BitBlt(hdc, 0, 0, 650, 450, mdc, 0, 0, SRCCOPY);

		//贴三角形(245,90) (245,130)
		SelectObject(bufdc, triangle);
		BitBlt(mdc,245, triangelY, 20, 20, bufdc, 0, 20, SRCAND);
		BitBlt(mdc, 245, triangelY, 20, 20, bufdc, 0, 0, SRCPAINT);
		//贴游戏说明
		if(showExplain == true){				//贴说明图
			SelectObject(bufdc, explain);
			BitBlt(mdc,120, 160, 300, 200, bufdc, 0, 200, SRCAND);
			BitBlt(mdc, 120, 160, 300, 200, bufdc, 0, 0, SRCPAINT);
		}


		BitBlt(hdc, 0, 0, 650, 450, mdc, 0, 0, SRCCOPY);
		tPre = GetTickCount();
		
	}	
}

void DrawGrass(){
	SelectObject(bufdc,grass);
	BitBlt(mdc, 30, 30, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 60, 30, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 90, 30, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 120, 30, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 160, 30, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 30, 60, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 60, 60, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 90, 60, 30, 30, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 120, 60, 30, 30, bufdc, 0, 0, SRCCOPY);
}
void MyPaint(HDC hdc)
{				
	if(isStart == false)
		return;
	MoveWindow(hWnd, 150, 100, 850, 480, true);
	//绘图
	SelectObject(bufdc, bg);
	BitBlt(mdc, 0, 0, 650, 450, bufdc, 0, 0, SRCCOPY);					

	/*
	 * 坦克1（若后期修改背景图，需要修改坦克图）
	 */
	int myDirection = myTank.GetDirection();			//坦克方向
	int myPositionX = myTank.GetX();			//坦克位置
	int myPositionY = myTank.GetY();			//坦克位置

	if(myTank.GetTankValue()>0)     //贴坦克之前判断
	{
	  SelectObject(bufdc, tankImg[myDirection]);
	  BitBlt(mdc, myPositionX, myPositionY, 30, 30, bufdc, 0, 0, SRCCOPY);

	}

	//判断坦克移动状态
	if(tank1_flag >= 1)
		tank1_Move();
	tank1_flag = (tank1_flag+1) % 2;

	if(tank2_flag >= 1)
		tank2_Move();
	tank2_flag = (tank2_flag+1) % 2;

	/*
	 * 坦克2（若后期修改背景图，需要修改坦克图）
	 */
	int myDirection_2= myTank_2.GetDirection();			//坦克方向
	int myPositionX_2= myTank_2.GetX();			//坦克位置
	int myPositionY_2= myTank_2.GetY();			//坦克位置

	if(myTank_2.GetTankValue()>0) {
	SelectObject(bufdc, tankImg_2[myDirection_2]);
	BitBlt(mdc, myPositionX_2, myPositionY_2, 30, 30, bufdc, 0, 0, SRCCOPY);		
	}
	else
	{
		; //贴图 玩家1胜利 按esc重新开始游戏 或者别的按键
	}
	/*
	 * 画障碍物  SCU 形状
	 */
     
	DrawGrass();			//贴草图
	
	SelectObject(bufdc,obstacle1);                               //贴第一行障碍 8个
	BitBlt(mdc,130, 150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,160,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,190,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,310,150, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[5].GetValue()>0)           //能被打掉的障碍  
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,340,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	}
	else
	{
		obstacle[5].Set(130, 150);
		if(reward_exit) 
		{
			SelectObject(bufdc,rewards);  //贴奖品
			BitBlt(mdc,340,150, 30, 30, bufdc, 0, 0, SRCCOPY);
		}
	}
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//贴第二行障碍 4个
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,130,180, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,180, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[10].GetValue()>0)
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,430,180, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[10].Set(130, 150);
	}

	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,180, 30, 30, bufdc, 0, 0, SRCCOPY);	

																	//贴第二'行障碍 4个
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,130,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,210, 30, 30, bufdc, 0, 0, SRCCOPY);	

																	//贴第三行障碍 6个
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,130,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,160,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,190,240, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[15].GetValue()>0)
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,280,240, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[15].Set(130, 150);
	}

	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//贴第四行障碍 4个

	if(obstacle[18].GetValue()>0)
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,190,270, 30, 30, bufdc, 0, 0, SRCCOPY);}
	else
	{
		obstacle[18].Set(130, 150);
	}
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,270, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[20].GetValue()>0)
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,430,270, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[20].Set(130, 150);
	}

	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,270, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[35].GetValue()>0)
	{																	//贴第四'行障碍 4个
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,190,300, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[35].Set(130, 150);
	}

	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,300, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle);
	BitBlt(mdc,430,300, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,300, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//贴第五行障碍9个
	SelectObject(bufdc,obstacle1);                               
	BitBlt(mdc,130, 330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,160,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,190,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	if(obstacle[25].GetValue()>0)
	{	
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,280,330, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[25].Set(130, 150);
	}

	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,310,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,340,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,460,330, 30, 30, bufdc, 0, 0, SRCCOPY);

	if(obstacle[30].GetValue()>0)
	{	
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,490,330, 30, 30, bufdc, 0, 0, SRCCOPY);	}
	else
	{
		obstacle[30].Set(130, 150);
	}
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,330, 30, 30, bufdc, 0, 0, SRCCOPY);	

	//画坦克1子弹
	//int dir = myTank.GetDirection();
	Bullet *b = myTank.GetBullet();			//子弹数组	
	int x= myTank.GetX();
	int y = myTank.GetY();	
	int speed = 4;				//子弹速度
	for(int i = 0;i < 20;i++){				//遍历子弹		
		Bullet temp = b[i];			
		if(temp.exist == false)
			continue;
		int bx = temp.GetX();				//子弹横坐标
		int by = temp.GetY();				//子弹纵坐标
		int dir = temp.GetDir();			//子弹方向
		if(dir == 0){			//top
			by -= speed;
			if(by<=0||by>=450){				//子弹越界
				b[i].exist = false;
				continue;
			}
			b[i].SetY(by);
		}
		if(dir == 1){			//下
			by += speed;
			if(by<=0||by>=450){				//子弹越界
				b[i].exist = false;
				continue;
			}
			b[i].SetY(by);
		}
		if(dir == 2){			//左
			bx -= speed;
			if(bx<=0||bx>=650){				//子弹越界
				b[i].exist = false;
				continue;
			}
			b[i].SetX(bx);
		}
		if(dir == 3){			//右
			bx += speed;
			if(bx<=0||bx>=650){				//子弹越界
				b[i].exist = false;
				continue;
			}
			b[i].SetX(bx);
		}

		//贴子弹

		SelectObject(bufdc, bullet);
		BitBlt(mdc, bx, by, 10, 10, bufdc, 0, 10, SRCAND);
		BitBlt(mdc, bx, by, 10, 10, bufdc, 0, 0, SRCPAINT);
		tank2_Shot(bx,by,i);
		tank1_beatObstacle(bx,by,i);		
	}

	//画坦克2子弹
	//int dir = myTank.GetDirection();
	Bullet *b_2 = myTank_2.GetBullet();			//子弹数组	
	int x_2= myTank_2.GetX();
	int y_2 = myTank_2.GetY();	
	int speed_2 = 4;				//子弹速度
	for(int i = 0;i < 20;i++){				//遍历子弹		
		Bullet temp = b_2[i];			
		if(temp.exist == false)
			continue;
		int bx_2 = temp.GetX();				//子弹横坐标
		int by_2 = temp.GetY();				//子弹纵坐标
		int dir = temp.GetDir();			//子弹方向
		if(dir == 0){			//top
			by_2 -= speed_2;
			if(by_2<=0||by_2>=450){				//子弹越界
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetY(by_2);
		}
		if(dir == 1){			//下
			by_2 += speed_2;
			if(by_2<=0||by_2>=450){				//子弹越界
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetY(by_2);
		}
		if(dir == 2){			//左
			bx_2 -= speed_2;
			if(bx_2<=0||bx_2>=650){				//子弹越界
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetX(bx_2);
		}
		if(dir == 3){			//右
			bx_2 += speed_2;
			if(bx_2<=0||bx_2>=650){				//子弹越界
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetX(bx_2);
		}

		//贴子弹
		SelectObject(bufdc, bullet_2);
		BitBlt(mdc, bx_2, by_2, 10, 10, bufdc, 0, 10, SRCAND);   //去掉子弹的黑色背景
		BitBlt(mdc, bx_2, by_2, 10, 10, bufdc, 0, 0, SRCPAINT);
		tank1_Shot(bx_2,by_2,i);
		tank2_beatObstacle(bx_2,by_2,i);
	
	}

	//贴评分图
	SelectObject(bufdc, scoreBackground);
	BitBlt(mdc, 650, 0, 200, 450, bufdc, 0, 0, SRCCOPY);

	BitBlt(hdc, 0, 0, 850, 450, mdc, 0, 0, SRCCOPY);
	tPre = GetTickCount();


}
int tempSpeed = 3;			//临时使用
void tank1_Move(){
	if(state_top){
					myTank.SetDirection(TOP);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y -= tempSpeed;
					if(isObstable(x,y)||isRewards_1(x,y) )
					{
						y+=tempSpeed;				//是障碍就不移动
					}
					
					if(y <= 0)	y = 0;
					if(y >= 450) y = 450;
					myTank.SetY(y);
					
	}else if(state_bottom){
					myTank.SetDirection(BOTTOM);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y += tempSpeed;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						y-=tempSpeed;				//是障碍就不移动
					}
					if(y <= 0)	y = 0;
					if(y >= 420) y = 420;
					myTank.SetY(y);
					
	}else if(state_left){
					myTank.SetDirection(LEFT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x -= tempSpeed;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x+=tempSpeed;				//是障碍就不移动
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);
	}else if(state_right){
					myTank.SetDirection(RIGHT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x += tempSpeed;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x-=tempSpeed;				//是障碍就不移动
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);
	}
}

void tank2_Move(){
	if(state_w){
					myTank_2.SetDirection(TOP);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 -= tempSpeed;
					if(isObstable(x_2,y_2))
					{
						y_2+=tempSpeed;				//是障碍就不移动
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 450) y_2 = 450;
					myTank_2.SetY(y_2);
	}else if(state_s){
					myTank_2.SetDirection(BOTTOM);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 += tempSpeed;				//移动速度
					if(isObstable(x_2,y_2))
					{
						y_2-=tempSpeed;				//是障碍就不移动
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 420) y_2 = 420;
					myTank_2.SetY(y_2);
	}else if(state_a){
					myTank_2.SetDirection(LEFT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 -= tempSpeed;				//移动速度
					if(isObstable(x_2,y_2))
					{
						x_2+=tempSpeed;				//是障碍就不移动
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);
	}else if(state_d){
					myTank_2.SetDirection(RIGHT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 += tempSpeed;				//移动速度
					if(isObstable(x_2,y_2))
					{
						x_2-=tempSpeed;				//是障碍就不移动
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);
	}
}
//事件处理函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//按下键盘开始游戏
		case WM_LBUTTONDOWN:  		
			//if(isStart == true)
			//	break;								
			//isStart = true;	
			//PlaySound("music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);	    //播放背景音乐		
			break;
		case WM_KEYUP:	
			switch (wParam)
			{
				case VK_UP:
					state_top = 0;
					break;
				case VK_DOWN:
					state_bottom = 0;
					break;
				case VK_LEFT:
					state_left = 0;
					break;
				case VK_RIGHT:
					state_right = 0;
					break;
				case 'W':
					state_w = 0;
					break;
				case 'S':
					state_s = 0;
					break;
				case 'A':
					state_a = 0;
					break;
				case 'D':
					state_d = 0;
					break;
				default:
					break;
			}

			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_RETURN:		//按下回车
					if(isStart == false){					
						if(triangelY == 90){			//开始游戏
							isStart = true;
							showExplain = false;
						}else{
							showExplain = true;
						}
					}
					break;

				case VK_ESCAPE:{
					PostQuitMessage(0);
				}
				break;
			
				//坦克1移动    向上
				case VK_UP:		{
						if(isStart == false){
						triangelY = 90;
						showExplain = false;
						return 0;
					}
					state_top = 1;
					/*myTank.SetDirection(TOP);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y -= 5;
					if(isObstable(x,y)||isRewards_1(x,y) )
					{
						y+=5;				//是障碍就不移动
					}
					
					if(y <= 0)	y = 0;
					if(y >= 450) y = 450;
					myTank.SetY(y);*/
					}
					break;
			     //坦克2移动   向上
				case 'W':		{	
					state_w = 1;
					/*myTank_2.SetDirection(TOP);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 -= 5;
					if(isObstable(x_2,y_2))
					{
						y_2+=5;				//是障碍就不移动
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 450) y_2 = 450;
					myTank_2.SetY(y_2);*/
					}
					break;
				case VK_DOWN:
					{	
						if(isStart == false){   //显示提示菜单
						triangelY = 130;
						return 0;
						}
					state_bottom = true;
					/*myTank.SetDirection(BOTTOM);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y += 5;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						y-=5;				//是障碍就不移动
					}
					if(y <= 0)	y = 0;
					if(y >= 420) y = 420;
					myTank.SetY(y);*/
					}
					break;

			 //坦克2移动   向下
					case 'S':
					{	
					state_s = 1;
					/*myTank_2.SetDirection(BOTTOM);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 += 5;				//移动速度
					if(isObstable(x_2,y_2))
					{
						y_2-=5;				//是障碍就不移动
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 420) y_2 = 420;
					myTank_2.SetY(y_2);*/
					}
					break;

				case VK_LEFT:
					{	
					state_left = 1;
					/*myTank.SetDirection(LEFT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x -= 5;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x+=5;				//是障碍就不移动
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);*/
					}
					break;
					//坦克2移动   向左
					case 'A':
					{	
					state_a = 1;
					/*myTank_2.SetDirection(LEFT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 -= 5;				//移动速度
					if(isObstable(x_2,y_2))
					{
						x_2+=5;				//是障碍就不移动
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);*/
					}
					break;


				case VK_RIGHT:
					{	
					state_right = 1;
					/*myTank.SetDirection(RIGHT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x += 5;				//移动速度
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x-=5;				//是障碍就不移动
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);*/
			
			
				}
				break;

				//坦克2移动   向右
				case 'D':
					{	
					state_d = 1;
					/*myTank_2.SetDirection(RIGHT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 += 5;				//移动速度
					if(isObstable(x_2,y_2))
					{
						x_2-=5;				//是障碍就不移动
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);*/
			
			
				}
				break;

				case VK_SPACE:{
					int dir = myTank.GetDirection();
					Bullet *b = myTank.GetBullet();			//子弹数组		
					int x= myTank.GetX();
					int y = myTank.GetY();		
					int index = -1;			//新增子弹下标
					for(int i = 0;i < 20;i++){
						if(b[i].exist == false){
							index = i;
							b[i].exist = true;
							break;
						}
					}
					if(index == -1)
						return 0;			//同时最多发射20个子弹
					if(dir == 0){				//方向为上
						//设置子弹位置
						b[index].SetX(x+10);
						b[index].SetY(y+2);
						//设置子弹方向
						b[index].SetDir(dir);
					}
					if(dir == 1){				//方向为下
						//设置子弹位置
						b[index].SetX(x+10);
						b[index].SetY(y+33);
						//设置子弹方向
						b[index].SetDir(dir);
					}
					if(dir == 2){				//方向为左
						//设置子弹位置
						b[index].SetX(x - 13);
						b[index].SetY(y+10);
						//设置子弹方向
						b[index].SetDir(dir);
					}
					if(dir == 3){				//方向为右
						//设置子弹位置
						b[index].SetX(x+33);
						b[index].SetY(y+10);
						//设置子弹方向
						b[index].SetDir(dir);
					}
					break;
				 }
				break;

				   //坦克2发射子弹
					case VK_TAB:{
					int dir_2 = myTank_2.GetDirection();
					Bullet *b_2 = myTank_2.GetBullet();			//子弹数组		
					int x_2= myTank_2.GetX();
					int y_2 = myTank_2.GetY();		
					int index_2 = -1;			//新增子弹下标
					for(int i = 0;i < 20;i++){
						//tank1_Shot(x_2,y_2);
						if(b_2[i].exist == false)
						{
							index_2 = i;
							b_2[i].exist = true;
							break;
						}
					}
					if(index_2 == -1)
						return 0;			//同时最多发射20个子弹
					if(dir_2 == 0){				//方向为上
						//设置子弹位置
						b_2[index_2].SetX(x_2+10);
						b_2[index_2].SetY(y_2+2);
						//设置子弹方向
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 1){				//方向为下
						//设置子弹位置
						b_2[index_2].SetX(x_2+10);
						b_2[index_2].SetY(y_2+33);
						//设置子弹方向
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 2){				//方向为左
						//设置子弹位置
						b_2[index_2].SetX(x_2 - 13);
						b_2[index_2].SetY(y_2+10);
						//设置子弹方向
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 3){				//方向为右
						//设置子弹位置
						b_2[index_2].SetX(x_2+33);
						b_2[index_2].SetY(y_2+10);
						//设置子弹方向
						b_2[index_2].SetDir(dir_2);
					}
					break;
				 }
				break;



			}
			break;





	case WM_DESTROY:{
		//int i;

		DeleteDC(mdc);
		DeleteDC(bufdc);
		DeleteObject(bg);
		ReleaseDC(hWnd, hdc);

		PostQuitMessage(0);
		}
		break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


