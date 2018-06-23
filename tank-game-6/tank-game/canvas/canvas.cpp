
/*
 *	̹�˴�ս�����򣬱���ʱ���ܾ�����д�ɺ����ٵ��ã�������mypaint�������̫��
 */

#include "stdafx.h"
#include <stdio.h>

#include "mmsystem.h"//��������ͷ�ļ�
#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���
#define OBSTACLE_NUMBER 40  //�����ϰ���ĸ���
#define REWARDS_NUMBER 1

HINSTANCE hInst;
HBITMAP  bg,start,tankImg[4],tankImg_2[4],bullet,bullet_2,obstacle1,obstacle2,triangle,explain,scoreBackground,rewards,grass;						//ͼƬ;						//ͼƬ
HDC		hdc, mdc, bufdc;
HWND	hWnd;
DWORD	tPre, tNow;						//����ˢ��Ƶ��


bool reward_exit=true;
bool isStart = false;				//��Ϸ��ʼ
int triangelY = 90;				//��ʼ����������y����
bool showExplain = false;			//�Ƿ���ʾ˵��ͼ

//����д����̹��ͬʱ�ƶ�
bool state_top,state_left,state_bottom,state_right;			//��ⷽ���
bool state_w,state_a,state_d,state_s;			//���wsad�� 
int tank1_flag,tank2_flag;						//����̹���ƶ��ٶ�

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);		
void				StartPaint(HDC hdc);
BOOL isObstable(int tank_x,int tank_y);
BOOL isRewards_1(int tank_x,int tank_y);   //�Ե���Ʒ������ֵ

void tank1_Shot(int bx,int by,int ii);  //̹��1(��ɫ)���ӵ�����  �ӵ�������
void tank2_Shot(int bx_2,int by_2,int i_2);  //̹��2(��ɫ)���ӵ�����  �ӵ������� 
void setObstacle();  //�����ϰ���������ĸ�������  �ϰ�λ�ú�����ֵ
void setReward();  //���ý�������ĸ�������  λ��
void tank1_beatObstacle(int bx, int by,int ii);  //̹��1�ӵ� �����ϰ�
void tank2_beatObstacle(int bx_2, int by_2,int ii_2);  //̹��2���ӵ� �����ϰ�



Obstacle *obstacle=new Obstacle[OBSTACLE_NUMBER]; //�����ϰ��������� ��¼�ϰ�λ�ú�����ֵ
Obstacle *reward=new Obstacle[REWARDS_NUMBER]; //���ý����������� ��¼����λ��

//Ϊ������ʱ��Ϊȫ�ֱ���
//��ʼ����̹��1
Tank myTank(100,100,LEFT,1);				//define left 2
//��ʼ����̹��2
Tank myTank_2(30,30,RIGHT,1);				//�����Ƚ�tank������ֵ����Ϊ1  �����ӵ��򵽾���ʧ


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	setObstacle();   //  �����ϰ��ͽ�Ʒ ��ʼֵ �������ᵼ���ϰ�һ��ʼ����ʾ������ײ�����ʱ�����ʾ ���ߴ򲻵�
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
			if (tNow - tPre >= 10){				//ˢ��Ƶ��
				if(isStart)				//��Ϸ�Ѿ���ʼ
					MyPaint(hdc);
				else
					StartPaint(hdc);			//��Ϸδ��ʼ
			}
		}
	}

	return msg.wParam;
}

//ע�ᴰ���࣬����Ҫ����ԭ��
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

//��ʼ��
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
		

	//����ͼƬ
	start = (HBITMAP)LoadImage(NULL, "start.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);			//��ʼ����
	//bg = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);				//����ͼ
	//��Ʒ
	rewards= (HBITMAP)LoadImage(NULL, "rewards.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);	
	//�������Ҷ�Ӧ 0 1 2 3 
	tankImg[0] = (HBITMAP)LoadImage(NULL, "top.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				//̹��1
	tankImg[2] = (HBITMAP)LoadImage(NULL, "left.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[3] = (HBITMAP)LoadImage(NULL, "right.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[1] = (HBITMAP)LoadImage(NULL, "bottom.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	bullet = (HBITMAP)LoadImage(NULL, "bullet.bmp", IMAGE_BITMAP, 10, 20, LR_LOADFROMFILE);		

	tankImg_2[0] = (HBITMAP)LoadImage(NULL, "top_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				//̹��2
	tankImg_2[2] = (HBITMAP)LoadImage(NULL, "left_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg_2[3] = (HBITMAP)LoadImage(NULL, "right_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg_2[1] = (HBITMAP)LoadImage(NULL, "bottom_2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	bullet_2= (HBITMAP)LoadImage(NULL, "bullet_2.bmp", IMAGE_BITMAP, 10, 20, LR_LOADFROMFILE);	

	obstacle1= (HBITMAP)LoadImage(NULL, "obstacle1.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);	//�ϰ�
	obstacle2 = (HBITMAP)LoadImage(NULL, "obstacle2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);				//����		
	triangle = (HBITMAP)LoadImage(NULL, "triangle.bmp", IMAGE_BITMAP, 20, 40, LR_LOADFROMFILE);				//������		
	explain = (HBITMAP)LoadImage(NULL, "explain.bmp", IMAGE_BITMAP, 300, 400, LR_LOADFROMFILE);				//������		
	scoreBackground = (HBITMAP)LoadImage(NULL, "scoreBackground.bmp", IMAGE_BITMAP, 200, 450, LR_LOADFROMFILE);		
	grass = (HBITMAP)LoadImage(NULL, "grass.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);		
	MyPaint(hdc);					//���û�ͼ����

	return TRUE;
}

void tank1_Shot(int bx,int by,int ii)    //̹��1(��ɫ)���ӵ�����  �ӵ�������
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

void tank2_Shot(int bx_2,int by_2,int i_2)   //̹��2 ��̹��1 ����   �ӵ����� ���ӵ������±�
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

void tank1_beatObstacle(int bx, int by,int ii)  //̹��1���ӵ� �����ϰ�
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

void tank2_beatObstacle(int bx_2, int by_2,int ii_2)  //̹��2���ӵ� �����ϰ�
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
	obstacle[3].Set(280,150);////////40 ���ϰ�  ����i%5==0 �� ���Ա�������ϰ�-> obstacle2
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

	for(int i=0;i<OBSTACLE_NUMBER;i++)   //�������ϰ�������ֵ��Ϊ1
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
BOOL isRewards_1(int tank_x,int tank_y)  //1 �Ե���Ʒ������ֵ
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
		//����ʼ����
		SelectObject(bufdc, start);
		BitBlt(mdc, 0, 0, 650, 450, bufdc, 0, 0, SRCCOPY);		
		//BitBlt(hdc, 0, 0, 650, 450, mdc, 0, 0, SRCCOPY);

		//��������(245,90) (245,130)
		SelectObject(bufdc, triangle);
		BitBlt(mdc,245, triangelY, 20, 20, bufdc, 0, 20, SRCAND);
		BitBlt(mdc, 245, triangelY, 20, 20, bufdc, 0, 0, SRCPAINT);
		//����Ϸ˵��
		if(showExplain == true){				//��˵��ͼ
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
	//��ͼ
	SelectObject(bufdc, bg);
	BitBlt(mdc, 0, 0, 650, 450, bufdc, 0, 0, SRCCOPY);					

	/*
	 * ̹��1���������޸ı���ͼ����Ҫ�޸�̹��ͼ��
	 */
	int myDirection = myTank.GetDirection();			//̹�˷���
	int myPositionX = myTank.GetX();			//̹��λ��
	int myPositionY = myTank.GetY();			//̹��λ��

	if(myTank.GetTankValue()>0)     //��̹��֮ǰ�ж�
	{
	  SelectObject(bufdc, tankImg[myDirection]);
	  BitBlt(mdc, myPositionX, myPositionY, 30, 30, bufdc, 0, 0, SRCCOPY);

	}

	//�ж�̹���ƶ�״̬
	if(tank1_flag >= 1)
		tank1_Move();
	tank1_flag = (tank1_flag+1) % 2;

	if(tank2_flag >= 1)
		tank2_Move();
	tank2_flag = (tank2_flag+1) % 2;

	/*
	 * ̹��2���������޸ı���ͼ����Ҫ�޸�̹��ͼ��
	 */
	int myDirection_2= myTank_2.GetDirection();			//̹�˷���
	int myPositionX_2= myTank_2.GetX();			//̹��λ��
	int myPositionY_2= myTank_2.GetY();			//̹��λ��

	if(myTank_2.GetTankValue()>0) {
	SelectObject(bufdc, tankImg_2[myDirection_2]);
	BitBlt(mdc, myPositionX_2, myPositionY_2, 30, 30, bufdc, 0, 0, SRCCOPY);		
	}
	else
	{
		; //��ͼ ���1ʤ�� ��esc���¿�ʼ��Ϸ ���߱�İ���
	}
	/*
	 * ���ϰ���  SCU ��״
	 */
     
	DrawGrass();			//����ͼ
	
	SelectObject(bufdc,obstacle1);                               //����һ���ϰ� 8��
	BitBlt(mdc,130, 150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,160,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,190,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,310,150, 30, 30, bufdc, 0, 0, SRCCOPY);	

	if(obstacle[5].GetValue()>0)           //�ܱ�������ϰ�  
	{
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,340,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	}
	else
	{
		obstacle[5].Set(130, 150);
		if(reward_exit) 
		{
			SelectObject(bufdc,rewards);  //����Ʒ
			BitBlt(mdc,340,150, 30, 30, bufdc, 0, 0, SRCCOPY);
		}
	}
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//���ڶ����ϰ� 4��
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

																	//���ڶ�'���ϰ� 4��
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,130,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,210, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,210, 30, 30, bufdc, 0, 0, SRCCOPY);	

																	//���������ϰ� 6��
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
																	//���������ϰ� 4��

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
	{																	//������'���ϰ� 4��
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
																	//���������ϰ�9��
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

	//��̹��1�ӵ�
	//int dir = myTank.GetDirection();
	Bullet *b = myTank.GetBullet();			//�ӵ�����	
	int x= myTank.GetX();
	int y = myTank.GetY();	
	int speed = 4;				//�ӵ��ٶ�
	for(int i = 0;i < 20;i++){				//�����ӵ�		
		Bullet temp = b[i];			
		if(temp.exist == false)
			continue;
		int bx = temp.GetX();				//�ӵ�������
		int by = temp.GetY();				//�ӵ�������
		int dir = temp.GetDir();			//�ӵ�����
		if(dir == 0){			//top
			by -= speed;
			if(by<=0||by>=450){				//�ӵ�Խ��
				b[i].exist = false;
				continue;
			}
			b[i].SetY(by);
		}
		if(dir == 1){			//��
			by += speed;
			if(by<=0||by>=450){				//�ӵ�Խ��
				b[i].exist = false;
				continue;
			}
			b[i].SetY(by);
		}
		if(dir == 2){			//��
			bx -= speed;
			if(bx<=0||bx>=650){				//�ӵ�Խ��
				b[i].exist = false;
				continue;
			}
			b[i].SetX(bx);
		}
		if(dir == 3){			//��
			bx += speed;
			if(bx<=0||bx>=650){				//�ӵ�Խ��
				b[i].exist = false;
				continue;
			}
			b[i].SetX(bx);
		}

		//���ӵ�

		SelectObject(bufdc, bullet);
		BitBlt(mdc, bx, by, 10, 10, bufdc, 0, 10, SRCAND);
		BitBlt(mdc, bx, by, 10, 10, bufdc, 0, 0, SRCPAINT);
		tank2_Shot(bx,by,i);
		tank1_beatObstacle(bx,by,i);		
	}

	//��̹��2�ӵ�
	//int dir = myTank.GetDirection();
	Bullet *b_2 = myTank_2.GetBullet();			//�ӵ�����	
	int x_2= myTank_2.GetX();
	int y_2 = myTank_2.GetY();	
	int speed_2 = 4;				//�ӵ��ٶ�
	for(int i = 0;i < 20;i++){				//�����ӵ�		
		Bullet temp = b_2[i];			
		if(temp.exist == false)
			continue;
		int bx_2 = temp.GetX();				//�ӵ�������
		int by_2 = temp.GetY();				//�ӵ�������
		int dir = temp.GetDir();			//�ӵ�����
		if(dir == 0){			//top
			by_2 -= speed_2;
			if(by_2<=0||by_2>=450){				//�ӵ�Խ��
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetY(by_2);
		}
		if(dir == 1){			//��
			by_2 += speed_2;
			if(by_2<=0||by_2>=450){				//�ӵ�Խ��
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetY(by_2);
		}
		if(dir == 2){			//��
			bx_2 -= speed_2;
			if(bx_2<=0||bx_2>=650){				//�ӵ�Խ��
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetX(bx_2);
		}
		if(dir == 3){			//��
			bx_2 += speed_2;
			if(bx_2<=0||bx_2>=650){				//�ӵ�Խ��
				b_2[i].exist = false;
				continue;
			}
			b_2[i].SetX(bx_2);
		}

		//���ӵ�
		SelectObject(bufdc, bullet_2);
		BitBlt(mdc, bx_2, by_2, 10, 10, bufdc, 0, 10, SRCAND);   //ȥ���ӵ��ĺ�ɫ����
		BitBlt(mdc, bx_2, by_2, 10, 10, bufdc, 0, 0, SRCPAINT);
		tank1_Shot(bx_2,by_2,i);
		tank2_beatObstacle(bx_2,by_2,i);
	
	}

	//������ͼ
	SelectObject(bufdc, scoreBackground);
	BitBlt(mdc, 650, 0, 200, 450, bufdc, 0, 0, SRCCOPY);

	BitBlt(hdc, 0, 0, 850, 450, mdc, 0, 0, SRCCOPY);
	tPre = GetTickCount();


}
int tempSpeed = 3;			//��ʱʹ��
void tank1_Move(){
	if(state_top){
					myTank.SetDirection(TOP);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y -= tempSpeed;
					if(isObstable(x,y)||isRewards_1(x,y) )
					{
						y+=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					
					if(y <= 0)	y = 0;
					if(y >= 450) y = 450;
					myTank.SetY(y);
					
	}else if(state_bottom){
					myTank.SetDirection(BOTTOM);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y += tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						y-=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(y <= 0)	y = 0;
					if(y >= 420) y = 420;
					myTank.SetY(y);
					
	}else if(state_left){
					myTank.SetDirection(LEFT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x -= tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x+=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);
	}else if(state_right){
					myTank.SetDirection(RIGHT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x += tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x-=tempSpeed;				//���ϰ��Ͳ��ƶ�
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
						y_2+=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 450) y_2 = 450;
					myTank_2.SetY(y_2);
	}else if(state_s){
					myTank_2.SetDirection(BOTTOM);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 += tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						y_2-=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 420) y_2 = 420;
					myTank_2.SetY(y_2);
	}else if(state_a){
					myTank_2.SetDirection(LEFT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 -= tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						x_2+=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);
	}else if(state_d){
					myTank_2.SetDirection(RIGHT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 += tempSpeed;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						x_2-=tempSpeed;				//���ϰ��Ͳ��ƶ�
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);
	}
}
//�¼�������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//���¼��̿�ʼ��Ϸ
		case WM_LBUTTONDOWN:  		
			//if(isStart == true)
			//	break;								
			//isStart = true;	
			//PlaySound("music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);	    //���ű�������		
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
				case VK_RETURN:		//���»س�
					if(isStart == false){					
						if(triangelY == 90){			//��ʼ��Ϸ
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
			
				//̹��1�ƶ�    ����
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
						y+=5;				//���ϰ��Ͳ��ƶ�
					}
					
					if(y <= 0)	y = 0;
					if(y >= 450) y = 450;
					myTank.SetY(y);*/
					}
					break;
			     //̹��2�ƶ�   ����
				case 'W':		{	
					state_w = 1;
					/*myTank_2.SetDirection(TOP);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 -= 5;
					if(isObstable(x_2,y_2))
					{
						y_2+=5;				//���ϰ��Ͳ��ƶ�
					}
					if(y_2 <= 0)	y_2 = 0;
					if(y_2 >= 450) y_2 = 450;
					myTank_2.SetY(y_2);*/
					}
					break;
				case VK_DOWN:
					{	
						if(isStart == false){   //��ʾ��ʾ�˵�
						triangelY = 130;
						return 0;
						}
					state_bottom = true;
					/*myTank.SetDirection(BOTTOM);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y += 5;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						y-=5;				//���ϰ��Ͳ��ƶ�
					}
					if(y <= 0)	y = 0;
					if(y >= 420) y = 420;
					myTank.SetY(y);*/
					}
					break;

			 //̹��2�ƶ�   ����
					case 'S':
					{	
					state_s = 1;
					/*myTank_2.SetDirection(BOTTOM);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					y_2 += 5;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						y_2-=5;				//���ϰ��Ͳ��ƶ�
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
					x -= 5;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x+=5;				//���ϰ��Ͳ��ƶ�
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);*/
					}
					break;
					//̹��2�ƶ�   ����
					case 'A':
					{	
					state_a = 1;
					/*myTank_2.SetDirection(LEFT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 -= 5;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						x_2+=5;				//���ϰ��Ͳ��ƶ�
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
					x += 5;				//�ƶ��ٶ�
					if(isObstable(x,y)||isRewards_1(x,y))
					{
						x-=5;				//���ϰ��Ͳ��ƶ�
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);*/
			
			
				}
				break;

				//̹��2�ƶ�   ����
				case 'D':
					{	
					state_d = 1;
					/*myTank_2.SetDirection(RIGHT);
					int x_2 = myTank_2.GetX();
					int y_2 = myTank_2.GetY();
					x_2 += 5;				//�ƶ��ٶ�
					if(isObstable(x_2,y_2))
					{
						x_2-=5;				//���ϰ��Ͳ��ƶ�
					}
					if(x_2 <= 0)	x_2 = 0;
					if(x_2 >= 590) x_2 = 590;
					myTank_2.SetX(x_2);*/
			
			
				}
				break;

				case VK_SPACE:{
					int dir = myTank.GetDirection();
					Bullet *b = myTank.GetBullet();			//�ӵ�����		
					int x= myTank.GetX();
					int y = myTank.GetY();		
					int index = -1;			//�����ӵ��±�
					for(int i = 0;i < 20;i++){
						if(b[i].exist == false){
							index = i;
							b[i].exist = true;
							break;
						}
					}
					if(index == -1)
						return 0;			//ͬʱ��෢��20���ӵ�
					if(dir == 0){				//����Ϊ��
						//�����ӵ�λ��
						b[index].SetX(x+10);
						b[index].SetY(y+2);
						//�����ӵ�����
						b[index].SetDir(dir);
					}
					if(dir == 1){				//����Ϊ��
						//�����ӵ�λ��
						b[index].SetX(x+10);
						b[index].SetY(y+33);
						//�����ӵ�����
						b[index].SetDir(dir);
					}
					if(dir == 2){				//����Ϊ��
						//�����ӵ�λ��
						b[index].SetX(x - 13);
						b[index].SetY(y+10);
						//�����ӵ�����
						b[index].SetDir(dir);
					}
					if(dir == 3){				//����Ϊ��
						//�����ӵ�λ��
						b[index].SetX(x+33);
						b[index].SetY(y+10);
						//�����ӵ�����
						b[index].SetDir(dir);
					}
					break;
				 }
				break;

				   //̹��2�����ӵ�
					case VK_TAB:{
					int dir_2 = myTank_2.GetDirection();
					Bullet *b_2 = myTank_2.GetBullet();			//�ӵ�����		
					int x_2= myTank_2.GetX();
					int y_2 = myTank_2.GetY();		
					int index_2 = -1;			//�����ӵ��±�
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
						return 0;			//ͬʱ��෢��20���ӵ�
					if(dir_2 == 0){				//����Ϊ��
						//�����ӵ�λ��
						b_2[index_2].SetX(x_2+10);
						b_2[index_2].SetY(y_2+2);
						//�����ӵ�����
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 1){				//����Ϊ��
						//�����ӵ�λ��
						b_2[index_2].SetX(x_2+10);
						b_2[index_2].SetY(y_2+33);
						//�����ӵ�����
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 2){				//����Ϊ��
						//�����ӵ�λ��
						b_2[index_2].SetX(x_2 - 13);
						b_2[index_2].SetY(y_2+10);
						//�����ӵ�����
						b_2[index_2].SetDir(dir_2);
					}
					if(dir_2 == 3){				//����Ϊ��
						//�����ӵ�λ��
						b_2[index_2].SetX(x_2+33);
						b_2[index_2].SetY(y_2+10);
						//�����ӵ�����
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


