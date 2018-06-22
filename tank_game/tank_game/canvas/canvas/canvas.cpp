
/*
 *	̹�˴�ս�����򣬱���ʱ���ܾ�����д�ɺ����ٵ��ã�������mypaint�������̫��
 */

#include "stdafx.h"
#include <stdio.h>

#include "mmsystem.h"//��������ͷ�ļ�
#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���
#define OBSTACLE_NUMBER 40  //�����ϰ���ĸ���

HINSTANCE hInst;
HBITMAP  bg,start,tankImg[4],bullet,obstacle1,obstacle2,triangle,explain,scoreBackground;						//ͼƬ;						//ͼƬ
HDC		hdc, mdc, bufdc;
HWND	hWnd;
DWORD	tPre, tNow;						//����ˢ��Ƶ��
bool isStart = false;				//��Ϸ��ʼ
int triangelY = 90;				//��ʼ����������y����
bool showExplain = false;			//�Ƿ���ʾ˵��ͼ

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);		
void				StartPaint(HDC hdc);
BOOL isObstable(int tank_x,int tank_y);

//Ϊ������ʱ��Ϊȫ�ֱ���
//��ʼ����̹��
Tank myTank(100,100,LEFT);				//define left 2



int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;

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

	MoveWindow(hWnd, 50, 50, 650, 480, true);
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

	//�������Ҷ�Ӧ 0 1 2 3 
	tankImg[0] = (HBITMAP)LoadImage(NULL, "top.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				//̹��
	tankImg[2] = (HBITMAP)LoadImage(NULL, "left.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[3] = (HBITMAP)LoadImage(NULL, "right.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	tankImg[1] = (HBITMAP)LoadImage(NULL, "bottom.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);				
	bullet = (HBITMAP)LoadImage(NULL, "bullet.bmp", IMAGE_BITMAP, 10, 20, LR_LOADFROMFILE);		

	obstacle1= (HBITMAP)LoadImage(NULL, "obstacle1.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);	//�ϰ�
	obstacle2 = (HBITMAP)LoadImage(NULL, "obstacle2.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL, "bg.bmp", IMAGE_BITMAP, 650, 450, LR_LOADFROMFILE);				//����		
	triangle = (HBITMAP)LoadImage(NULL, "triangle.bmp", IMAGE_BITMAP, 20, 40, LR_LOADFROMFILE);				//������		
	explain = (HBITMAP)LoadImage(NULL, "explain.bmp", IMAGE_BITMAP, 300, 400, LR_LOADFROMFILE);				//������		
	scoreBackground = (HBITMAP)LoadImage(NULL, "scoreBackground.bmp", IMAGE_BITMAP, 200, 450, LR_LOADFROMFILE);				//����ϵͳ		

	MyPaint(hdc);					//���û�ͼ����

	return TRUE;
}
BOOL isObstable(int tank_x,int tank_y)
{
	Obstacle *obstacle=new Obstacle[OBSTACLE_NUMBER];
	obstacle[0].Set(130, 150);	//1
	obstacle[1].Set(160,150);
	obstacle[2].Set(190,150);
	obstacle[3].Set(280,150);////////40 ���ϰ�  ����i%5==0 �� ���Ա�������ϰ�-> obstacle1
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
void MyPaint(HDC hdc)
{				
	if(isStart == false)
		return;
	
	MoveWindow(hWnd, 150, 100, 850, 480, true);

	//�汳��ͼ
	SelectObject(bufdc, bg);
	BitBlt(mdc, 0, 0, 650, 450, bufdc, 0, 0, SRCCOPY);					
					

	/*
	 * ����̹�ˣ��������޸ı���ͼ����Ҫ�޸�̹��ͼ��
	 */
	int myDirection = myTank.GetDirection();			//̹�˷���
	int myPositionX = myTank.GetX();			//̹��λ��
	int myPositionY = myTank.GetY();			//̹��λ��

	SelectObject(bufdc, tankImg[myDirection]);
	BitBlt(mdc, myPositionX, myPositionY, 30, 30, bufdc, 0, 0, SRCCOPY);		
	
	/*
	 * ���ϰ���  SCU ��״
	 */
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
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,340,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,150, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//���ڶ����ϰ� 4��
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,130,180, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,180, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,430,180, 30, 30, bufdc, 0, 0, SRCCOPY);	
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
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,280,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,240, 30, 30, bufdc, 0, 0, SRCCOPY);	
																	//���������ϰ� 4��
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,190,270, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,270, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,430,270, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,270, 30, 30, bufdc, 0, 0, SRCCOPY);	

																		//������'���ϰ� 4��
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,190,300, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,280,300, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
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
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,280,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,310,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,340,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,430,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,460,330, 30, 30, bufdc, 0, 0, SRCCOPY);
	SelectObject(bufdc,obstacle2);
	BitBlt(mdc,490,330, 30, 30, bufdc, 0, 0, SRCCOPY);	
	SelectObject(bufdc,obstacle1);
	BitBlt(mdc,520,330, 30, 30, bufdc, 0, 0, SRCCOPY);	


	//������̹���ӵ�
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
	}
		
	//������ͼ
	SelectObject(bufdc, scoreBackground);
	BitBlt(mdc, 650, 0, 200, 450, bufdc, 0, 0, SRCCOPY);

	BitBlt(hdc, 0, 0, 850, 450, mdc, 0, 0, SRCCOPY);
	tPre = GetTickCount();

}


//�¼�������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//���¼��̿�ʼ��Ϸ
		case WM_LBUTTONDOWN:  		
			//if(isStart == true)
				//break;								
			//isStart = true;	
			//PlaySound("music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);	    //���ű�������				
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
			
				//̹���ƶ�
				case VK_UP:		{	
					if(isStart == false){
						triangelY = 90;
						showExplain = false;
						return 0;
					}
					myTank.SetDirection(TOP);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y -= 5;
					if(isObstable(x,y))
					{
						y+=5;				//���ϰ��Ͳ��ƶ�
					}
					
					if(y <= 0)	y = 0;
					if(y >= 450) y = 450;
					myTank.SetY(y);
					}
					break;
				case VK_DOWN:
					{	
					if(isStart == false){
						triangelY = 130;
						return 0;
					}
					myTank.SetDirection(BOTTOM);
					int x = myTank.GetX();
					int y = myTank.GetY();
					y += 5;				//�ƶ��ٶ�
					if(isObstable(x,y))
					{
						y-=5;				//���ϰ��Ͳ��ƶ�
					}
					if(y <= 0)	y = 0;
					if(y >= 420) y = 420;
					myTank.SetY(y);
					}
					break;
				case VK_LEFT:
					{	
					myTank.SetDirection(LEFT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x -= 5;				//�ƶ��ٶ�
					if(isObstable(x,y))
					{
						x+=5;				//���ϰ��Ͳ��ƶ�
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);
					}
					break;
				case VK_RIGHT:
					{	
					myTank.SetDirection(RIGHT);
					int x = myTank.GetX();
					int y = myTank.GetY();
					x += 5;				//�ƶ��ٶ�
					if(isObstable(x,y))
					{
						x-=5;				//���ϰ��Ͳ��ƶ�
					}
					if(x <= 0)	x = 0;
					if(x >= 590) x = 590;
					myTank.SetX(x);
			
			
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
			}
			break;
			  
	case WM_DESTROY:{
		int i;

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
