#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

#define up(r,c) (board[r-1][c]==chSort&&board[r-2][c]==chSort&&board[r-3][c]==chSort&&board[r-4][c]==chSort)
#define down(r,c) (board[r+1][c]==chSort&&board[r+2][c]==chSort&&board[r+3][c]==chSort&&board[r+4][c]==chSort)
#define left(r,c) (board[r][c-1]==chSort&&board[r][c-2]==chSort&&board[r][c-3]==chSort&&board[r][c-4]==chSort)
#define right(r,c) (board[r][c+1]==chSort&&board[r][c+2]==chSort&&board[r][c+3]==chSort&&board[r][c+4]==chSort)
#define upleft(r,c) (board[r-1][c-1]==chSort&&board[r-2][c-2]==chSort&&board[r-3][c-3]==chSort&&board[r-4][c-4]==chSort)
#define downleft(r,c) (board[r+1][c-1]==chSort&&board[r+2][c-2]==chSort&&board[r+3][c-3]==chSort&&board[r+4][c-4]==chSort)
#define upright(r,c) (board[r-1][c+1]==chSort&&board[r-2][c+2]==chSort&&board[r-3][c+3]==chSort&&board[r-4][c+4]==chSort)
#define downright(r,c) (board[r+1][c+1]==chSort&&board[r+2][c+2]==chSort&&board[r+3][c+3]==chSort&&board[r+4][c+4]==chSort)

class CGobang	//棋子类
{
	char chSort;			//棋子的类别
	int x,y;			//棋子的位置
	static char board[20][20];	//棋盘
	static int SpotsOnBoard;	//盘上棋子数
  public:
	int nWin;			//赢棋数
	int nLose;			//输棋数
	static int nDraw;		//平局数
    CGobang(char chsort);		//构造函数，决定一方棋子的类别
    void PlayTurn(void);		//走一步棋
	void PlayTurn_AI(void);		//AI走一步棋
	void IfError(int &x,int &y);	//判断位置输入是否有错
	void PrintInfo(void);		//打印对象输赢情况
	int Judge();			//判断是否5子连珠，是返回1，不是返回0
    void Win(void);			//赢棋
	void Lose(void);		//输棋
	static void Draw(void);		//平局
    static void StatisticalFunction(int qiju[][20][8][2]);			//统计函数
	static void ValueFunction(int qiju[][20][8][2],int a1[][20],int a2[][20]);//评估函数
	static void InitialAndPrintBoard(void);						//初始化并打印棋盘
	static void PrintBoard(void);	//打印棋盘
	static int GetFull(void);	//判断棋盘是否已布满棋子
};

int CGobang::nDraw=0;
int CGobang::SpotsOnBoard=0;
char CGobang::board[20][20];

//构造函数棋子类型
CGobang::CGobang(char chsort)
{
	chSort=chsort;
	nWin=0;
	nLose=0;
}

//统计函数
void CGobang::StatisticalFunction(int qiju[20][20][8][2])
{
	int i,j,k,t,count;
	int tx,ty;
	int dx[8]={0,1,1,1,0,-1,-1,-1};	//下标+4刚好是对面
	int dy[8]={-1,-1,0,1,1,1,0,-1};
	for(i=0;i<20;i++)
	  for(j=0;j<20;j++)
	    if(board[i][j]=='-')
	      for(k=0;k<8;k++)
	      {
		for(t=count=0,tx=i,ty=j;t<5;t++)
		{
			tx+=dx[k];
			ty+=dy[k];
			if(tx>19||tx<0||ty>19||ty<0) break;
			if(board[tx][ty]=='X') count++;
			else break;
		}
		qiju[i][j][k][0]=count;
		for(t=count=0,tx=i,ty=j;t<5;t++)
		{
			tx+=dx[k];
			ty+=dy[k];
			if(tx>19||tx<0||ty>19||ty<0) break;
			if(board[tx][ty]=='O') count++;
			else break;
		}
		qiju[i][j][k][1]=count;
	      }
}

//评估函数
void CGobang::ValueFunction(int qiju[20][20][8][2],int a1[][20],int a2[][20])
{
	int i,j,k;
	int win;
	for(i=0;i<20;i++)
	  for(j=0;j<20;j++)
	    if(board[i][j]=='-')
	    {
			for(k=0,win=1;k<4;k++)	//各个方向
			  if(qiju[i][j][k][0]+qiju[i][j][k+4][0]>=4) win+=100000;
			  else if(qiju[i][j][k][0]+qiju[i][j][k+4][0]==3) win+=10000;
			  else if(qiju[i][j][k][0]+qiju[i][j][k+4][0]==2) win+=100;
			  else if(qiju[i][j][k][0]+qiju[i][j][k+4][0]==1) win+=10;
			a1[i][j]=win;		//玩家X在该位置的评分
			for(k=0,win=1;k<4;k++)	//各个方向
			  if(qiju[i][j][k][1]+qiju[i][j][k+4][1]>=4) win+=1000000;
			  else if(qiju[i][j][k][1]+qiju[i][j][k+4][1]==3) win+=10000;
			  else if(qiju[i][j][k][1]+qiju[i][j][k+4][1]==2) win+=100;
			  else if(qiju[i][j][k][1]+qiju[i][j][k+4][1]==1) win+=10;
			a2[i][j]=win;		//玩家O在该位置的评分
	    }
	    else a1[i][j]=a2[i][j]=0;
}

//走一步棋,包含纠错的一步
void CGobang::PlayTurn(void)
{
	cout<<"现在由玩家"<<chSort<<"下子"<<endl;
	cout<<"行:";
	cin>>x;
	while(!cin)//当输入的数据类型与定义的变量类型不一致
	{
		char str[20];
		cin.clear();//清空输入缓冲区
		cin.getline(str,20);
		cout<<"非法输入，请输入一个整数：";
		cin>>x;//重新输入
	}
	cout<<"列:";
	cin>>y;
	while(!cin)
	{
		char str[20];
		cin.clear();
		cin.getline(str,20);
		cout<<"非法输入，请输入一个整数：";
		cin>>y;
	}
	IfError(x,y);
	board[x-1][y-1]=chSort;
	SpotsOnBoard++;
	PrintBoard();
}

//AI走一步棋
void CGobang::PlayTurn_AI(void)
{
	int i,j;
	int x1=0,x2=0,y1=0,y2=0;
	int qiju[20][20][8][2];		//用四维数组记录每个位置八个方向的信息
	int a1[20][20],a2[20][20];	//记录各位置的分数
	StatisticalFunction(qiju);	//统计函数
	ValueFunction(qiju,a1,a2);	//评估函数
/*-----------------算出分数最高的空位-----------------*/
	for(i=0;i<20;i++)
	  for(j=0;j<20;j++)
	    if(a1[x1][y1]<a1[i][j])	//X玩家的位置分数
	    {
		x1=i;
		y1=j;
	    }
	for(i=0;i<20;i++)
	  for(j=0;j<20;j++)
	    if(a2[x2][y2]<a2[i][j])	//O玩家的位置分数
	    {
		x2=i;
		y2=j;
	    }
	if(chSort=='O')
	  if(a1[x1][y1]>a2[x2][y2])
	  {
		x=x1;
		y=y1;
	  }
	  else
	  {
		x=x2;
		y=y2;
	  }
	else
	  if(a1[x1][y1]<a2[x2][y2])
	  {
		x=x2;
		y=y2;
	  }
	  else
	  {
		x=x1;
		y=y1;
	  }
	board[x][y]=chSort;
	SpotsOnBoard++;
	PrintBoard();
}

//判断有无走错
void CGobang::IfError(int &x,int &y)
{
	while(x>20||x<1||y>20||y<1||board[x-1][y-1]!='-')
	{
		cout<<"该位置无法输入，请重新输入"<<endl;
		cout<<"行:";
		cin>>x;
		cout<<"列:";
		cin>>y;
		//IfError(x,y);
	}
}

//判断是否5子连珠，是返回1，不是返回0
int CGobang::Judge(void)
{
	int i,j;
	for(i=0;i<20;i++)
	  for(j=0;j<20;j++)
	    if(board[i][j]==chSort)
	      if(i>3 && up(i,j)) return 1;
	      else if(i<20-4 && down(i,j)) return 1;
	      else if(j>3 && left(i,j)) return 1;
	      else if(j<20-4 && right(i,j)) return 1;
	      else if(i>3 && j>3 && upleft(i,j)) return 1;
	      else if(i>3 && j<20-4 && upright(i,j)) return 1;
	      else if(i<20-4 && j<3 && downleft(i,j)) return 1;
	      else if(i<20-4 && j<20-4 && downright(i,j)) return 1;
	return 0;

}

void CGobang::Win(void) { nWin++; }
void CGobang::Lose(void) { nLose++; }
void CGobang::Draw(void) { nDraw++; }

//打印对象的情况
void CGobang::PrintInfo(void)
{
	cout<<"玩家"<<chSort<<"赢了"<<nWin<<"次,输了"<<nLose<<"次"<<endl;
}

//初始化并打印棋盘
void CGobang::InitialAndPrintBoard(void)
{
	int N=20;
	int M=20;
	int k,j,i;
	system("CLS");
	for(k=0;k<=N;k++)
	  cout<<setw(3)<<setfill(' ')<<k;
	cout<<endl;
	for(i=1;i<=M;i++)
	{
		cout<<setw(3)<<setfill(' ')<<i;
		for(j=1;j<=N;j++)
		{
			board[i-1][j-1]='-';
			cout<<setw(3)<<setfill(' ')<<board[i-1][j-1];
		}
		cout<<endl;
	}
}

//打印棋盘
void CGobang::PrintBoard(void)
{
	int N=20;
	int M=20;
	int k,j,i;
	system("CLS");
	for(k=0;k<=N;k++)
	  cout<<setw(3)<<setfill(' ')<<k;
	cout<<endl;
	for(i=1;i<=M;i++)
	{
		cout<<setw(3)<<setfill(' ')<<i;
		for(j=1;j<=N;j++)
		  cout<<setw(3)<<setfill(' ')<<board[i-1][j-1];
		cout<<endl;
	}
}

//判断是否布满棋盘，是返回1，不是返回0
int CGobang::GetFull(void)
{
	return SpotsOnBoard==400;
}

/*-------main-------*/
int main(void)
{
	loop:
	int selection;
	cout<<"************************************************************\n";
	cout<<"*祝您五子棋游戏愉快!                                       *"<<endl;
	cout<<"*第一个玩家的棋子用X来表示，第二个玩家的棋子用O来表示      *"<<endl;
	cout<<"*请选择游戏模式，输入模式前的序号进行选择                  *"<<endl;
	cout<<"*1.人机模式                                                *"<<endl;
	cout<<"*2.双人模式                                                *"<<endl;
	cout<<"*3.退出游戏                                                *"<<endl;
	cout<<"************************************************************\n";
	cin>>selection;

/*-------人机模式-------*/
	if(selection==1)
	{
		char typeselection;
		cout<<"请你选择你的玩家类型，玩家X先下子"<<endl;
		cout<<"1.玩家Ｘ"<<endl;
		cout<<"2.玩家Ｏ"<<endl;
		cin>>typeselection;

		//玩家先行
		if(typeselection=='1'||typeselection=='x'||typeselection=='X')
		{
			CGobang playerX('X');
			CGobang playerO('O');
			char choice='Y';//初始选择，使循环能够进行下去
			while(choice=='Y'||choice=='y')
			{//主循环开始
				CGobang::InitialAndPrintBoard();//初始化并打印键盘
				while(CGobang::GetFull()==0)
				{//一局游戏的循环
					playerX.PlayTurn();
					if(playerX.Judge()==1)
					{
						cout<<"玩家X获得胜利"<<endl;
						playerX.nWin++;
						playerO.nLose++;
						break;
 					}
					playerO.PlayTurn_AI();//玩家O是AI
					if(playerO.Judge()==1)
					{
						cout<<"玩家O获得胜利"<<endl;
						playerO.nWin++;
						playerX.nLose++;
						break;
 					}
				}//一局游戏的循环结束
				if(CGobang::GetFull()==1)
				{
					cout<<"游戏平局！"<<endl;
					CGobang::nDraw++;
				}
				cout<<"游戏结束！"<<endl;
				playerX.PrintInfo();
			    playerO.PrintInfo();
				cout<<"是否继续下一局游戏，是输入Y，否则输入任意键退出"<<endl;
				cin>>choice;
			}//主循环结束
			goto loop;
 		}

		//AI先行
		if(typeselection=='2'||typeselection=='O'||typeselection=='o'||typeselection=='0')
		{
			CGobang playerX('X');
			CGobang playerO('O');
			char choice='Y';//初始选择，使循环能够进行下去
			while(choice=='Y'||choice=='y')
			{//主循环开始
				CGobang::InitialAndPrintBoard();//初始化并打印键盘
				while(CGobang::GetFull()==0)
				{//一局游戏的循环
					playerX.PlayTurn_AI();//玩家Ｘ是AI
					if(playerX.Judge()==1)
					{
						cout<<"玩家X获得胜利"<<endl;
						playerX.nWin++;
						playerO.nLose++;
						break;
					}
					playerO.PlayTurn();
					if(playerO.Judge()==1)
					{
						cout<<"玩家O获得胜利"<<endl;
						playerO.nWin++;
						playerX.nLose++;
						break;
					}
				}//一局游戏的循环结束
				if(CGobang::GetFull()==1)
				{
					cout<<"游戏平局！"<<endl;
					CGobang::nDraw++;
				}
				cout<<"游戏结束！"<<endl;
				cout<<"是否继续下一局游戏，是输入Y，不是输入N"<<endl;
				cin>>choice;
			}//主循环结束
			goto loop;
		}
	}

/*-------双人模式-------*/
	if(selection==2)
	{
		CGobang playerX('X');
		CGobang playerO('O');
		char choice='Y';//初始选择，使循环能够进行下去
		while(choice=='Y')
		{//主循环开始
			CGobang::InitialAndPrintBoard();
			while(CGobang::GetFull()==0)
			{//一局游戏的循环
				playerX.PlayTurn();
				if(playerX.Judge()==1)
				{
					cout<<"玩家X获得胜利"<<endl;
					playerX.nWin++;
					playerO.nLose++;
					break;
				}
				playerO.PlayTurn();
				if(playerO.Judge()==1)
				{
					cout<<"玩家O获得胜利"<<endl;
					playerO.nWin++;
					playerX.nLose++;
					break;
 				}
			}//一局游戏的循环结束
			if(CGobang::GetFull()==1)
			{
				cout<<"游戏平局！"<<endl;
				CGobang::nDraw++;
			}
			cout<<"游戏结束！"<<endl;
			playerX.PrintInfo();
			playerO.PrintInfo();
			cout<<"是否继续下一局游戏？是输入Y，否则输入任意键退出"<<endl;
			cin>>choice;
		}//主循环结束
		goto loop;
	}

	if(selection==3) exit(0);
	return 0;
}