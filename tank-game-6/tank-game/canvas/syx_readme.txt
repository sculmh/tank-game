1.SCU ̹���ϰ���״ʵ�� Ŀǰ�������ϰ�  40��  
0~39  5����(i=5,10,15,20,25,30,35)���ܴ򴩵�obstacle2.bmp  ʣ���ǲ��ܵ�  //obstacle[i]
6���� ĳЩ����һ��ֵ���Գ�һЩrewards
����˵�� �ո�� tab �������� ���
2.
�ӵ�����������---- �ݲ�����   
������һ��̹��
3.�ӵ���̹��  1/2 ���   
̹�˵�����ֵ��ʱ����Ϊ1  �����Ͱ� �� ������ֵ���0 
ˢ��֮ǰ�ж�һ��
 �������ŵ��ϰ��ᱻͬʱ���С�
tank1_Shot(x_2,y_2); �Ѿ�ʵ�� 
�������  �ڶ�����ҿ���myTank_2
   W
A  S  D                  tab    VK_TAB


����Ҫ
if(myTank_2.GetTankValue()>0) {
	SelectObject(bufdc, tankImg_2[myDirection_2]);
	BitBlt(mdc, myPositionX_2, myPositionY_2, 30, 30, bufdc, 0, 0, SRCCOPY);		
	}
	else
	{
		; //��ͼ ���1ʤ�� ��esc���¿�ʼ��Ϸ ���߱�İ���
	}




���������� rewards  ��obstacle����  obstacle[i]    //  i=