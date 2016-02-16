#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <sys/select.h>
#define LEN 1024 

const int pawn = 100;
const int knight = 200;
const int bishop = 300;
const int rook = 400;
const int queen = 500;
const int king = 600;

int board[11][11];
char me[100],you[100];
int blwh=0;

char you_won[]=
"__   __           __        __          _ \n"
"\\ \\ / /__  _   _  \\ \\      / /__  _ __ | |\n"
" \\ V / _ \\| | | |  \\ \\ /\\ / / _ \\| '_ \\| |\n"
"  | | (_) | |_| |   \\ V  V / (_) | | | |_|\n"
"  |_|\\___/ \\__,_|    \\_/\\_/ \\___/|_| |_(_)\n";
                                          
char you_lost[]=
"__   __            _              _   _ \n"
"\\ \\ / /__  _   _  | |    ___  ___| |_| |\n"
" \\ V / _ \\| | | | | |   / _ \\/ __| __| |\n"
"  | | (_) | |_| | | |__| (_) \\__ \\ |_|_|\n"
"  |_|\\___/ \\__,_| |_____\\___/|___/\\__(_)\n";


typedef struct{
	int i,j;
} point;

typedef struct{
	char nick_name[LEN];
} User_List;

///////////Check co
int min(int xx,int yy)
{
	if (xx>yy) return yy;
	else return xx;
}

int max(int xx,int yy)
{
	if (xx<yy) return yy;
	else return xx;
}

void khoitaoboard()
{
	int i,j;
	for (i = 0; i < 12; ++i)
		for (j = 0; j < 11; ++j)
			board[i][j]=0;

	for (i = 0; i < 2; ++i)
		for (j = 0; j < 11; ++j)
			board[i][j]=-1;
	
	for (i = 10; i < 12; ++i)
		for (j = 0; j < 11; ++j)
			board[i][j]=-1;

	for (i = 0; i < 12; ++i)
		for (j = 0; j < 2; ++j)
			board[i][j]=-1;

	for (i = 0; i < 12; ++i)
		for (j = 10; j < 12; ++j)
			board[i][j]=-1;

	board[2][2]=rook;
	board[2][3]=knight;
	board[2][4]=bishop;
	board[2][5]=queen;
	board[2][6]=king;
	board[2][7]=bishop;
	board[2][8]=knight;
	board[2][9]=rook;
	
	for (i = 2; i < 10; ++i)
	{
		board[3][i]=pawn;
		board[8][i]=-pawn;

		board[9][i]=-board[2][i];
	}

	i=board[9][5];
	board[9][5]=board[9][6];
	board[9][6]=i;
}

void inboard()
{
	system("clear");
	printf("You vs %s\n",you);
	int i,j;
	char c;
	for (i = 2; i < 10; ++i)
	{
		printf("%4d ",10-i);
		for (j = 2; j < 10; ++j)
		{	
			if ((i+j)%2==0)
			{
				printf("\033[47m\033[30m ");
			}
			else
				printf("\033[40m\033[5m ");
			switch (board[i][j])
			{
				case 100:
					printf("♙");
					break;
				case -100:
					printf("♟");
					break;
				case 200:
					printf("♘");
					break;
				case -200:
					printf("♞");
					break;
				case 300:
					printf("♗");
					break;
				case -300:
					printf("♝");
					break;
				case 400:
					printf("♖");
					break;
				case -400:
					printf("♜");
					break;					
				case 500:
					printf("♕");
					break;					
				case -500:
					printf("♛");
					break;									
				case 600:
					printf("♔");
					break;													
				case -600:
					printf("♚");
					break;
				default:
					printf(" ");
			}
			if ((i+j)%2==0)
				printf(" \033[0m");
			else printf(" \033[0m");
			
		}
		printf("\n");
	}

	printf("%4c",' ');
	for (c='A'; c < 'I'; ++c)
	{
		printf("%3c",c);
	}
	printf("\n");
}


int checkpawn(point t1,point t2)
{
	if( (t1.j==t2.j) & (board[t2.i][t2.j]==0)) //Di thang
	{
		if (board[t1.i][t1.j]==pawn)
		{
			if (t2.i-t1.i==1)
				return 1;
			if ((t2.i-t1.i==2) & (t1.i==3))
				return 1;
		}
		else{
			if (t1.i-t2.i==1)
				return 1;
			if ((t1.i-t2.i==2) & (t1.i==8))
				return 1;
		}
	}

	if(abs(board[t2.i][t2.j])>10) //An cheo
	{
		if ((abs(t2.i-t1.i)==1)&(abs(t2.j-t1.j)==1))
		{
			if (board[t1.i][t1.j]==pawn)
			{
				if ((t2.i-t1.i==1) &&(abs(t2.j-t1.j)==1) && (board[t2.i][t2.j]<90) )
					return 1;
			}
			else
			{
				if ((t1.i-t2.i==1) &&(abs(t2.j-t1.j)==1) &&(board[t2.i][t2.j]>90) )
					return 1;	
			}			
		}
	}

	return 0;
}

int checkknight(point t1, point t2)
{
	if (board[t2.i][t2.j]!=-1)
	{	
		if ( ((abs(t1.i-t2.i)==1) & (abs(t1.j-t2.j)==2)) | ((abs(t1.i-t2.i)==2) & (abs(t1.j-t2.j)==1)))
		{
			if (board[t2.i][t2.j]==0)
				return 1;
			if (board[t1.i][t1.j]>0)
			{
				if (board[t2.i][t2.j]<90)
					return 1;
			}
			else if (board[t2.i][t2.j]>90)
			{
				return 1;
			}
		}
	}

	return 0;
}

int checkbishop(point t1,point t2)
{
	int k,i,j;

	if (board[t2.i][t2.j]!=-1)
	{	
		if (abs(t1.i-t2.i)==abs(t1.j-t2.j))
		{
			if ((t1.i<t2.i)&(t1.j<t2.j))
				for (k = 1; k < t2.i-t1.i; ++k)
					if (board[t1.i+k][t1.j+k]!=0)
						return 0;
			if ((t1.i>t2.i)&(t1.j>t2.j))
				for (k = 1; k < t1.i-t2.i; ++k)
					if (board[t2.i+k][t2.j+k]!=0)
						return 0;								
			if ((t1.i>t2.i)&(t1.j<t2.j))
				for ( k = 1; k < t1.i-t2.i; ++k)
					if (board[t2.i+k][t2.j-k]!=0)
						return 0;					
			if ((t1.i<t2.i)&(t1.j>t2.j))
				for ( k = 1; k < t2.i-t1.i; ++k)
					if (board[t2.i-k][t2.j+k]!=0)
						return 0;			
			if (board[t2.i][t2.j]==0)
				return 1;
			if (board[t1.i][t1.j]>0)
			{
				if (board[t2.i][t2.j]<90)
					return 1;
			}
			else if (board[t2.i][t2.j]>90)
			{
				return 1;
			}
		}
	}

	return 0;
}

int checkrook(point t1,point t2)
{
	int i;
	if (board[t2.i][t2.j]!=-1)
	{	
		if (t1.i==t2.i)
		{
			for (i = 1; i < abs(t2.j-t1.j); ++i)
				if (board[t1.i][min(t1.j,t2.j)+i]!=0)
					return 0;
		}
		if (t1.j==t2.j)
		{
			for ( i = 1; i < abs(t2.i-t1.i); ++i)
				if (board[min(t1.i,t2.i)+i][t1.j]!=0)
					return 0;
		}

		if (board[t2.i][t2.j]==0)
			return 1;

		if (board[t1.i][t1.j]>0)
			{
				if (board[t2.i][t2.j]<90)
					return 1;
			}
			else if (board[t2.i][t2.j]>90)
			{
				return 1;
			}
	}

	return 0;

}

int checkqueen(point t1,point t2)
{
	 	return ((checkbishop(t1,t2)==1) || (checkrook(t1,t2)==1));
}

int checkking(point t1,point t2)
{
	if ( (abs(t1.i-t2.i)<=1) & (abs(t1.j-t2.j)<=1) )
	{
		return checkqueen(t1,t2);
	}

	return 0;
}

int move(point t1,point t2)
{
	if ((t1.i==t2.i)&(t1.j==t2.j))
		return 0;

	if (abs(board[t1.i][t1.j])==pawn)
		if (checkpawn(t1,t2)) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	if (abs(board[t1.i][t1.j])==knight)
		if (checkknight(t1,t2)) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	if (abs(board[t1.i][t1.j])==bishop)
		if (checkbishop(t1,t2)) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	if (abs(board[t1.i][t1.j])==rook)
		if (checkrook(t1,t2)==1) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	if (abs(board[t1.i][t1.j])==queen)
		if (checkqueen(t1,t2)==1) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	if (abs(board[t1.i][t1.j])==king)
		if (checkking(t1,t2)==1) 
		{
			board[t2.i][t2.j]=board[t1.i][t1.j];
			board[t1.i][t1.j]=0;
			return 1;
		}

	return 0;
}
////////////////////
int changech(char c)
{
	switch (c){
		case 'A':
			return 2;
			break;
		case 'a':
			return 2;
			break;
		case 'B':
			return 3;
			break;
		case 'b':
			return 3;
			break;
		case 'C':
			return 4;
			break;
		case 'c':
			return 4;
			break;
		case 'D':
			return 5;
			break;
		case 'd':
			return 5;
			break;
		case 'E':
			return 6;
			break;
		case 'e':
			return 6;
			break;
		case 'F':
			return 7;
			break;
		case 'f':
			return 7;
			break;
		case 'G':
			return 8;
			break;
		case 'g':
			return 8;
			break;
		case 'H':
			return 9;
			break;
		case 'h':
			return 9;
			break;
		default:
			return 0;
	}
}
int changenu(char c)
{
	return ':' - c;
}



int check_xau(char buff[]){

	int i;

	for(i=0;i<strlen(buff);i++){
		if(buff[i] == '|') {
			printf("\nXau chua ki tu khong duoc cho phep!!"); 
			return 0;
		}
	}
	return 1;
}
int check_input(char buff[]){
	int i;

	if(strlen(buff)<=3 || strlen(buff)>=15) return 0;
	else return 1;

}
void creat_mesg(int mark, char partner[],char buff1[],char buff2[],char buff3[],char mesg[]){

	char buff[LEN];
	char str_mark[10];
	char str_partner[10];
	snprintf(str_mark, 9, "%d", mark);
	

	strcpy(buff,"");
	strcat(buff,str_mark);
	strcat(buff,"|");
	strcat(buff,partner);
	strcat(buff,"|");
	strcat(buff,buff1);
	strcat(buff,"|");
	strcat(buff,buff2);
	strcat(buff,"|");
	strcat(buff,buff3);
	strcat(buff,"|");
	strcpy(mesg,"");
	strcpy(mesg,buff);

}
int login(int sockfd){
	char mesg[LEN];
	char partner[]="0";
	char pass[LEN];
	char account[LEN];
	char nick_name[]="0";
	int mark,n;
	char str_mark;

	
	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(account,"");
		fgets(account,LEN,stdin);
		account[strlen(account)-1]='\0';
		if(check_xau(account) == 0 || check_input(account) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}

	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_xau(pass) == 0 || check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	strcpy(mesg,"");
	creat_mesg(1,partner,account,pass,nick_name,mesg);
	send(sockfd, mesg,strlen(mesg), 0);
	strcpy(mesg,"");
	n=recv(sockfd, mesg, LEN, 0);

	mark= check_mark(mesg);
	if(mark == 1) {
		printf("\nDang nhap thanh cong");
		return 1;}
	else {
		printf("\nTai khoan hoac mat khau khong dung!!");
		return 0;}
}

int sign_up(int sockfd){
	char mesg[LEN];
	char partner[]="0";
	char account[LEN];
	char pass[LEN];
	char nick_name[LEN];
	int mark;
	char str_mark;

	while(1){
		printf("\nNhap ten tai khoan: ");
		strcpy(account,"");
		fgets(account,LEN,stdin);
		account[strlen(account)-1]='\0';
		if(check_xau(account) == 0 || check_input(account) == 0){
			printf("\nMoi nhap lai!!");
		}else break;
	}
	while(1){
		printf("Nhap mat khau: ");
		strcpy(pass,"");
		fgets(pass,LEN,stdin);
		pass[strlen(pass)-1]='\0';
		if(check_xau(pass) == 0 || check_input(pass) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
	}
	while(1){
		printf("Nhap nick name: ");
		strcpy(nick_name,"");
		fgets(nick_name,LEN,stdin);
		nick_name[strlen(nick_name)-1]='\0';
		if(check_xau(nick_name) == 0 || check_input(nick_name) == 0){
			printf("\nMoi nhap lai!!\n");
		}else break;
		
	}
	creat_mesg(0,partner,account,pass,nick_name,mesg);
	send(sockfd, mesg,strlen(mesg), 0);
	strcpy(mesg,"");
	recv(sockfd, mesg, LEN, 0);
	mark= check_mark(mesg);
	if(mark == 1) {
		printf("\nDang ki thanh cong");
		return 1;}
	else {
		printf("\nTai khoan da ton tai!!");
		return 0;}
}

int check_mark(char mesg[]){
	int mark;
	char mesg2[LEN];

	strcpy(mesg2,mesg);
	mark = atoi(strtok(mesg2,"|"));
	return mark;
}
int take_user_list(char mesg[],User_List user_list[]){
	char mesg2[LEN];
	int i=0;
	char *pch;

	strcpy(mesg2,mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	while(1){
		pch=strtok(NULL,"|");
		if(pch==NULL) break;
		strcpy(user_list[i].nick_name,pch);
		i++;
	}
	return i;
}

int answer_request(int sockfd,char mesg[],User_List play_list[]){
	int i;
	char nick_name[LEN];
	char mesg2[LEN],buff[LEN],buff2[LEN];
	char *pch;
	
	char yes[2];

	strcpy(mesg2,mesg);

	pch= strtok(mesg2,"|");
	pch= strtok(NULL,"|");
	strcpy(nick_name,pch);
	printf("\n@%s muon play voi ban!!",nick_name);
	printf("\nBan co dong y(y/n): ");
	//scanf("%s",yes);
	fgets(yes,3,stdin);
	if(yes[0] == 'y' || yes[0] == 'Y') {
		add_partner(nick_name,play_list);
		strcpy(buff,"4|");
		strcat(buff,nick_name);
		strcat(buff,"|0|0|0|");
		send(sockfd,buff,strlen(buff),0);
		return 1;
	}
	else {
		strcpy(buff,"5|");
		strcat(buff,nick_name);
		strcat(buff,"|0|0|0|");
		send(sockfd,buff,strlen(buff),0);
		return 0;
	}
}
int add_partner(char nick_name[],User_List play_list[]){
	int i=0;
	while(strcmp(play_list[i].nick_name,"") != 0 ){
		i++;
	}
	strcpy(play_list[i].nick_name,nick_name);
	return i+1;
}
int del_partner(char nick_name[],User_List play_list[]){
	int i=0;
	while(1){
		if(strcmp(nick_name,play_list[i].nick_name)==0)
			break;
		i++;
	}
	while(1){
		if(strcmp(play_list[i+1].nick_name,"")==0)
			break;
		strcpy(play_list[i].nick_name,play_list[i+1].nick_name);
		i++;
	}
	strcpy(play_list[i].nick_name,"");
	return i;
}

void playing(int sockfd,User_List play_list[]){
	point t1,t2;
	int i,j,t,a=0,x,y,rv;
	int luot,moved,predone;
	char *pch;
	char nick_name[LEN];
	char mesg[LEN],mesg2[LEN];
	char buff[LEN],buff1[LEN]="0",buff2[LEN]="0",buff3[LEN]="0";
	fd_set readSet;
	struct timeval tv;

	if (blwh==1)
		luot=0;
	else luot=1;

	a=1;
	strcpy(you,play_list[0].nick_name);
	inboard();
	while(1){
		predone=0;
		tv.tv_sec = 0;
		tv.tv_usec =10000;
		FD_SET(sockfd, &readSet);
		FD_SET(fileno(stdin), &readSet);
		select(sockfd +1, &readSet, NULL, NULL, &tv);

		if(a>0){
			// if (FD_ISSET(fileno(stdin), &readSet)) 
			if (luot==0)
			{//Gui nuoc di
				if (blwh==1)
				{
					printf("Quan trang.\n");
				}
				else printf("Quan den.\n");
				printf("Hay nhap nuoc di (dang A3:B3): ");
				if(a==1){
					while(1){
						fgets(buff,LEN,stdin);
						if(buff[0]!='\n') break;
					}
					buff[strlen(buff)-1]='\0';
					if(strlen(buff)==1 && ((buff[0]=='q') || (buff[0]=='Q'))) {
						strcpy(mesg,"");
						strcpy(mesg,"7|");
						strcat(mesg,play_list[0].nick_name);
						strcpy(nick_name,play_list[0].nick_name);
						strcat(mesg,"|0|0|0|");
						send(sockfd,mesg,strlen(mesg),0);
					
						printf("%s\n",you_lost);
						a = del_partner(nick_name,play_list);
						
					}else{
						//Xu ly nuoc co trong buff
						moved=0;
						do{
							t1.i=changenu(buff[1]);
							t1.j=changech(buff[0]);

							t2.i=changenu(buff[4]);
							t2.j=changech(buff[3]);

							// if (abs(board[t2.i][t2.j])==king)
							// {
							// 	predone=1;
							// }
							if ((blwh*board[t1.i][t1.j]>0)&(move(t1,t2)==1)) moved=1;
							else
							{	
								printf("Nuoc di khong hop le.\n");
								fgets(buff,LEN,stdin);
								if((buff[0]=='q') || (buff[0]=='Q')) {
									strcpy(mesg,"");
									
									strcpy(mesg,"7|");
									strcat(mesg,play_list[0].nick_name);
									strcpy(nick_name,play_list[0].nick_name);
									strcat(mesg,"|0|0|0|");
									send(sockfd,mesg,strlen(mesg),0);
								
									a = del_partner(nick_name,play_list);
									break;		
								}
							}

						}while(moved==0);

						if (blwh==-1) strcpy(buff1,"-1");
						else strcpy(buff1,"1");

						inboard();
						creat_mesg(6,play_list[0].nick_name,buff,buff1,buff2,mesg);
						send(sockfd,mesg,strlen(mesg),0);

						// //Thong bao ket thuc van co
						// if (predone && moved==1)
						// {
						// 	strcpy(mesg,"");
						// 	strcpy(mesg,"7|");
						// 	strcat(mesg,play_list[0].nick_name);
						// 	strcpy(nick_name,play_list[0].nick_name);
						// 	strcat(mesg,"|1|0|0|");
						// 	send(sockfd,mesg,strlen(mesg),0);
							
						// 	printf("%s\n",you_won);
						// 	a = del_partner(nick_name,play_list);
						// }
					}
				}
				FD_CLR(fileno(stdin), &readSet);
			}
			if (luot==1)
			// if (FD_ISSET(sockfd, &readSet)) 
			{//Nhan duoc nuoc di
				predone=0;
				printf("Xin cho doi phuong di....\n");
				strcpy(mesg,"");
				recv(sockfd,mesg,LEN,0);
				t=check_mark(mesg);
				if(t == 6){
					strcpy(mesg2,mesg);
					pch=strtok(mesg2,"|");
					pch=strtok(NULL,"|");
					strcpy(nick_name,pch);
					pch=strtok(NULL,"|");

					t1.i=changenu(pch[1]);
					t1.j=changech(pch[0]);
					t2.i=changenu(pch[4]);
					t2.j=changech(pch[3]);

					if (abs(board[t2.i][t2.j])==king)
							{
								predone=1;
							}

					if ((predone & move(t1,t2))==1)
					{
						inboard();
						strcpy(mesg,"");
						strcpy(mesg,"7|");
						strcat(mesg,play_list[0].nick_name);
						strcpy(nick_name,play_list[0].nick_name);
						strcat(mesg,"|1|0|0|");
						send(sockfd,mesg,strlen(mesg),0);
						
						printf("%s\n",you_lost);
						a = del_partner(nick_name,play_list);
					}
					else inboard();
				}

				if(t == 7)	{
					
					strcpy(mesg2,mesg);
					pch=strtok(mesg2,"|");
					pch=strtok(NULL,"|");
					strcpy(nick_name,pch);
					a=del_partner(nick_name,play_list);

					strcpy(mesg2,mesg);
					pch=strtok(mesg2,"|");
					pch=strtok(NULL,"|");
					strcpy(nick_name,pch);
					pch=strtok(NULL,"|");

					printf("%s\n",you_won);
					if (strcmp(pch,"0")==0)
					{
						printf("\n@%s da dung choi voi ban!!",nick_name);
					}
					
	
				}
				// if(t == 3){
				// 	answer_request(sockfd,mesg,play_list);
				// 	a=0;
				// 	while(strcmp(play_list[a].nick_name,"") != 0 ){
				// 		a++;
				// 	}
					
				// }
				FD_CLR(sockfd, &readSet);
			}
			luot=1 - luot;
		}else {
			printf("\nKet thuc!");
			return;
		}
	}
}



void choose_user(int sockfd,User_List play_list[]){
	int i,t,a,x,y;
	User_List user_list[100];
	char mesg[LEN],buff[LEN],buff2[LEN]="0",buff3[LEN];


	strcpy(buff,"2|0|0|0|0");
	send(sockfd,buff,strlen(buff),0);
	strcpy(mesg,"");
	recv(sockfd,mesg,LEN,0);
	t=check_mark(mesg);
	if(t == 2) a = take_user_list(mesg,user_list); //Lay danh sach user tu server 
	if(a==0) {
		printf("\nKhong co user khac online!!");
		return;}
	for ( i = 0; i < a; i++){
		printf("\n%d.@%s",i+1,user_list[i].nick_name);
	}
	while(1){
		printf("\nBan dung quan trang. Chon user de choi. ");
		scanf("%d",&y);
		fgets(buff3,LEN,stdin);
		if(0<y&&y<=a) break;
	}
	strcpy(mesg,"");
	creat_mesg(3,user_list[y-1].nick_name,buff2,buff2,buff2,mesg);
	send(sockfd,mesg,strlen(mesg),0);
	strcpy(mesg,"");
	recv(sockfd,mesg,LEN,0);
	t = check_mark(mesg);
	if(t == 4){
		printf("\n@%s da dong y play\n",user_list[y-1].nick_name);
		add_partner(user_list[y-1].nick_name,play_list);
		// fgets(buff,LEN,stdin);
		playing(sockfd,play_list);
		return;
	}else if(t == 5){
		printf("\nUser khong dong y play!!\n");
		return;
	}
}
	
void log_out(int sockfd){
	char mesg[LEN]="8|0|0|0|0|";
	send(sockfd,mesg,strlen(mesg),0);
}

int wait(int sockfd,User_List play_list[]){

	printf("\nDoi nguoi choi khac...(hoac an 1 phim de vao MENU)");
	printf("\n");
	int rv,t,x,n;
	char mesg[LEN],nick_name[LEN],buff[LEN];
	fd_set readSet;
	struct timeval tv;

	while(1){
		tv.tv_sec = 5;
		tv.tv_usec =0;
		FD_SET(sockfd, &readSet);
		FD_SET(fileno(stdin), &readSet);
		select(sockfd +1, &readSet, NULL, NULL, &tv);

		if (FD_ISSET(fileno(stdin), &readSet)){
			fgets(buff,LEN,stdin);
			return 0;
		}else if(FD_ISSET(sockfd, &readSet)){
			strcpy(mesg,"");
			n=recv(sockfd,mesg,LEN,0);
			if(n==0) return 3;
			t=check_mark(mesg);
			if(t == 3) x = answer_request(sockfd,mesg,play_list);
			return 1;
		}
	}
}

/////////////////////////////////////////////////////////////

int menu(int sockfd){

	char choose[2];
	char aaa[1];
	int max_user_list;
	struct pollfd server[2];
	int rv,i,t;
	char mesg[LEN];
	User_List user_list[100];
	User_List play_list[1];

	while(1){
		t = wait(sockfd,play_list);
		if (t == 1){
			blwh=-1;//Quan den
			khoitaoboard();
			playing(sockfd,play_list);
		}else if(t ==0){
			printf("\n***MENU CHINH***\n");
			printf("\n1.Chon 1 user dang online de choi cung");
			printf("\n2.Thoat");
			printf("\nBan chon: ");
	    	fgets(choose,3,stdin);
	    	switch(choose[0]){
	    		case '1' :
	    			blwh=1; //Quan trang
	    			khoitaoboard();
	    			choose_user(sockfd,play_list);
	    			break;
	    		case '2' :
	    			log_out(sockfd);
	    			return 1;
	    		default :
	    			break;	    			
	    	}

		}
	}
}
void main(){
	//Khai bao cho co 
	//Quan trang hay den
	
	
	//Khai bao cho mang
	int sockfd,sentBytes, recvBytes,n=0,port;
	struct sockaddr_in serv_addr;
	char mesg[LEN];
	char recv_b[LEN];
	char choose[2];
	//int choose;
	char aaa[1];
	int t;

    serv_addr.sin_family = AF_INET;       
    serv_addr.sin_port = htons(5556);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    size_t size =100;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\nLoi tao socket!!");
    }
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\nConnect loi!!\n");
       return ;
    }else
    while(1){
    	printf("\n*****MENU*****\n");
    	printf("\n1.Dang nhap\n2.Dang ki\n3.Thoat" );
    	printf("\nBan chon: ");
    	//scanf("%d",&choose);
    	fgets(choose,3,stdin);
    	/*fgets(aaa,2,stdin);*/
    	switch(choose[0]){

    		case '1': 
    				printf("\nBan chon Dang Nhap");
    				t = login(sockfd);
    			    if(t == 1) menu(sockfd);
    			    break;
    		case '2': 	
    				printf("\nBan chon Dang Ki");
    				t = sign_up(sockfd);
    			    if(t == 1) menu(sockfd);
    			    break;
    		case '3':
    			return;
    		default : break;
    			

    	}
    }
}