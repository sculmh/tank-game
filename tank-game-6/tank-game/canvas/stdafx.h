// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

//������
#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
//Ϊ��Լʱ�䣬����ʱд������

//������
class Object
{
public:
	Object(){};
	virtual ~Object(){};
private:

};

//�ӵ���
class Bullet: public Object
{
public:	
	bool exist;
	Bullet(){exist = false;}
	void Set(int xx,int yy){
		x = xx,y = yy;				//�����ӵ�����
	}
	void SetX(int xx){x = xx;}
	void SetY(int yy){y = yy;}
	int GetX(){return x;}
	int GetY(){return y;}	
	int GetDir(){return dir;}
	void SetDir(int d){dir = d;}
private:
	int x,y;			//�ӵ�����
	int dir;			//�ӵ�����	
};



//̹����
class Tank: public Object
{
public:		
	Tank(int xx=20,int yy= 20,int d = LEFT,int tank_v=10){x = xx,y = yy,dir = d,tank_value=tank_v;}
	void Set(int xx,int yy){x = xx,y = yy;}
	void SetX(int xx){x = xx;}
	void SetY(int yy){y = yy;}
	void SetDirection(int dirr){dir = dirr;}			//����
	void SetTankValue(int tank_v) 
	{
		this->tank_value=tank_v;
	}
	void ReduceTankValue() 
	{
		this->tank_value--;
	}
	void AddTankValue() 
	{
		this->tank_value++;
	}
	Bullet* GetBullet(){return bullet;}
	int GetDirection(){return dir;}
	int GetX(){return x;}
	int GetY(){return y;}
	int GetTankValue(){return tank_value;}
	
private:
	int x,y;			//����
	int dir;			//����
	Bullet bullet[20];			//�ӵ�����	
	int tank_value; //���Ա�����ʧ��һ������ֵ
};
class Obstacle:public Object
{
public:
	Obstacle() 
	{
	}
	Obstacle(int o_xx,int o_yy) 
	{
		o_x=o_xx;
		o_y=o_yy;
	}
	int GetOX(){return o_x;}
	int GetOY(){return o_y;}
	int GetValue(){return obstacle_value;}
	void Set(int o_x,int o_y)
	{
		this->o_x = o_x;
		this->o_y= o_y;
	}
	void SetValue(int o_v)
	{
		this->obstacle_value=o_v;

	}
	void ReduceObstacleValue() 
	{
		obstacle_value--;
	}
private:
	int o_x,o_y;
	int obstacle_value; //���Ա�����ʧ��һ������ֵ

};








