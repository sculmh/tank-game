1.SCU 坦克障碍形状实现 目前有两种障碍  40块  
0~39  5倍数(i=5,10,15,20,25,30,35)是能打穿的obstacle2.bmp  剩余是不能的  //obstacle[i]
6倍数 某些被打到一定值可以出一些rewards
增加说明 空格和 tab 按键发射 完成
2.
子弹威力的问题---- 暂不考虑   
增加了一个坦克
3.子弹打坦克  1/2 玩家   
坦克的生命值暂时设置为1  打到她就把 它 的生命值设成0 
刷新之前判断一下
 两个连着的障碍会被同时击中。
tank1_Shot(x_2,y_2); 已经实现 
两个玩家  第二个玩家控制myTank_2
   W
A  S  D                  tab    VK_TAB


还需要
if(myTank_2.GetTankValue()>0) {
	SelectObject(bufdc, tankImg_2[myDirection_2]);
	BitBlt(mdc, myPositionX_2, myPositionY_2, 30, 30, bufdc, 0, 0, SRCCOPY);		
	}
	else
	{
		; //贴图 玩家1胜利 按esc重新开始游戏 或者别的按键
	}




新增加两个 rewards  在obstacle类里  obstacle[i]    //  i=