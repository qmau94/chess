#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#define LEN 1024 

int board[11][11];

FILE *fo[11][11];

typedef struct{
	int i,j;
} point;


typedef struct{
	char account[LEN];
	char pass[LEN];
	char nick_name[LEN];
	int status; //1 on, 0 off
	int sockfd;
} User;


typedef struct{
	char nick_name[LEN];
	int sockfd;
} User_t;

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

int check_user(char account[],char pass[],User user[],int max_user){
	int i;

	for(i=0;i<max_user;i++){
			if(strcmp(account,user[i].account)==0){
				if(strcmp(pass,user[i].pass)==0) return 1;
			}
	}
	return 0;
}

int check_mark(char mesg[]){
	int mark;
	char mesg2[LEN];

	strcpy(mesg2,mesg);
	mark = atoi(strtok(mesg2,"|"));
	return mark;
}

int take_user(User user[]){ //Lay danh sach user vao mang user
	int max_user;
	int i=0;
	FILE *f;
	char ch[80];
	char *pch;

	f=fopen("user_list.txt","r");
	fgets(ch,79,f);
	while(!feof(f)){
		pch = strtok(ch,"|");
		strcpy(user[i].account,pch);
		while(1){
			pch = strtok(NULL,"|");
			strcpy(user[i].pass,pch);
			pch = strtok(NULL,"|");
			strcpy(user[i].nick_name,pch);
			pch = strtok (NULL,"|");
			pch = strtok (NULL,"|");	
			if (pch == NULL) break;
		}
		user[i].status = 0;
		user[i].sockfd = -1;
		i++;
		fgets(ch,79,f);
	}
	max_user = i;
	fclose(f);
	return max_user;

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

int sign_up(int connSock,char mesg[], User user[],int max_user){
	int max_u;
	char account[LEN];
	char pass[LEN];
	char nick_name[LEN];
	char mesg2[LEN];
	char *pch;
	int t,i;
	FILE *f;
	char buff_ok[LEN]="1|0|0|0|0|";
	char buff_faile[LEN]="0|0|0|0|0|";

	strcpy(mesg2,mesg);
	pch = strtok(mesg2,"|");
	pch = strtok(NULL,"|");
	pch = strtok(NULL,"|");
	strcpy(account,pch);
	pch = strtok(NULL,"|");
	strcpy(pass,pch);
	pch = strtok(NULL,"|");
	strcpy(nick_name,pch);
	for ( i = 0; i < max_user; i++)	{
		t=0;
		if(strcmp(account,user[i].account)==0) {t=1;break;}
		
	}
	if(t == 0){
		f = fopen("user_list.txt","a");
		fprintf(f,"%s|%s|%s|\n",account,pass,nick_name);
		fclose(f);

		/*them user vua moi tao vao danh sach*/
		strcpy(user[max_user].account,account);
		strcpy(user[max_user].pass,pass);
		strcpy(user[max_user].nick_name,nick_name);
		user[max_user].status=1;
		user[max_user].sockfd = connSock;
		max_u = max_user + 1;
		printf("\nUser @%s da dang ki va dang dang nhap!!",account );
		send(connSock, buff_ok, strlen(buff_ok), 0);//gui lai cho client da dang ki thanh cong
		return max_u;
	}else{
		send(connSock, buff_faile, strlen(buff_faile), 0);//dang ki that bai
		return max_user;
	}

}


void login(int connSock,char mesg[], User user[], int max_user){
	char account[LEN];
	char pass[LEN];
	char mesg2[LEN];
	char *pch;
	int t,i;
	char buff_ok[LEN]="1|0|0|0|0|";
	char buff_faile[LEN]="0|0|0|0|0|";

	strcpy(mesg2,mesg);
	pch = strtok(mesg2,"|");
	pch = strtok(NULL,"|");
	pch = strtok(NULL,"|");
	strcpy(account,"");
	strcpy(account,pch);
	pch = strtok(NULL,"|");
	strcpy(pass,"");
	strcpy(pass,pch);

	t = check_user(account,pass,user,max_user);
	
	if(t == 1){
		//thay doi trang thai cua user thanh online
		for(i=0;i<max_user;i++){
			if(strcmp(account,user[i].account) == 0){
				if(user[i].status == 1){
					printf("Gui tin dang nhap that bai\n");
					send(connSock, buff_faile, strlen(buff_faile), 0);//dang nhap that bai
					return;
				}
				else{
					user[i].status = 1;
					user[i].sockfd = connSock;
				}
				break;
			}
		}
		printf("\nDoi trang thai thanh cong. Gui cho user\n");
		send(connSock, buff_ok, strlen(buff_ok), 0);//gui lai cho client da dang nhap thanh cong
		printf("Done\n");
	}else{
		printf("\nDang nhap that bai!!");
		send(connSock, buff_faile, strlen(buff_faile), 0);//dang nhap that bai
	}
}

void show_user_list(int sockfd,User user[], int max_user){
	int i,x,y,t;
	char buff[LEN]="2|0|";

	for ( i = 0; i < max_user; i++){
		if(user[i].status==1){
			if(user[i].sockfd!=sockfd){
				strcat(buff,user[i].nick_name);
				strcat(buff,"|");
			}
		}
	//strcat(buff,"|");
	}
	send(sockfd,buff,strlen(buff),0);
}

void play_request(int sockfd,char mesg[],User user[],int max_user){
	int i,t,x,y;
	char mesg2[LEN],buff[LEN],buff2[]="0";
	char *pch;
	char nick_name[LEN];

	strcpy(mesg2,mesg);
	pch= strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	//x: nguoi nha
	//y: nguoi gui
	for(x=0; x<max_user;x++){
		if(user[x].status == 1){
			if(strcmp(nick_name,user[x].nick_name)==0) {
				i=1;
				break;
			}
		}
		i=0;
	}

	if(i==0){
		strcpy(buff,"5|0|0|0|0|");
		send(sockfd,buff,strlen(buff),0);
		return;
	}else{
		for(y=0;y<max_user;y++){
			if(user[y].sockfd == sockfd) break;
		}
	}
	creat_mesg(3,user[y].nick_name,buff2,buff2,buff2,buff);
	i=send(user[x].sockfd,buff,strlen(buff),0);
}

void accept_request(int sockfd, char mesg[],User user[], int max_user){
	int i,x,y,t,j;
	char mesg2[LEN],buff[LEN],buff2[LEN]="0",nick_name[LEN];
	char filename[100];
	char *pch;

	strcpy(mesg2, mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	//x: nguoi gui mesg chap nhan
	//y: nguoi nhan

	for (x = 0; x < max_user; x++)	{
		if(user[x].sockfd==sockfd) break;
	}
	for (y = 0; y < max_user; y++){
		if(user[y].status==1){
			if(strcmp(user[y].nick_name,nick_name)==0) {
				i=1;
				break;
			}
		}
		i=0;
	}
	if(i==0){
		creat_mesg(7,user[y].nick_name,buff2,buff2,buff2,buff);
		send(sockfd,buff,strlen(buff),0);
	}else{
		creat_mesg(4,user[x].nick_name,buff2,buff2,buff2,buff);
		send(user[y].sockfd,buff,strlen(buff),0);

		//Tao file log
		i=min(x,y);
		j=max(x,y);

		strcpy(filename,"");
		strcat(filename,user[i].nick_name);
		strcat(filename," vs ");
		strcat(filename,user[j].nick_name);
		strcat(filename," in ");
		time_t rawtime;
	    struct tm *timeinfo;

	    time(&rawtime);
	    timeinfo = localtime(&rawtime);
	    strcat(filename,asctime(timeinfo));

	    strcat(filename,".txt");

	    fo[i][j]=fopen(filename,"w");
	    fprintf(fo[i][j], "%s choi voi %s\n",user[i].nick_name,user[j].nick_name );
	    fprintf(fo[i][j], "Thoi gian choi %s\n",asctime(timeinfo));
	}

}
void not_accept_request(int sockfd, char mesg[],User user[], int max_user){
	int i,x,y,t;
	char mesg2[LEN],buff[LEN],buff2[LEN]="0",nick_name[LEN];
	char *pch;

	strcpy(mesg2, mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	//x: nguoi gui
	//y: nguoi nhan
	for (x = 0; x < max_user; x++)	{
		if(user[x].sockfd==sockfd) break;
	}
	for (y = 0; y < max_user; y++){
		if(user[y].status==1){
			if(strcmp(user[y].nick_name,nick_name)==0) break;
		}
	}
	creat_mesg(5,user[x].nick_name,buff2,buff2,buff2,buff);
	send(user[y].sockfd,buff,strlen(buff),0);

}

void playing(int sockfd, char mesg[],User user[], int max_user){
	int i,x,y,t,j;
	char mesg2[LEN],buff[LEN],buff2[LEN]="0",buff3[LEN],nick_name[LEN];

	char *pch;

	strcpy(mesg2, mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	pch=strtok(NULL,"|");
	strcpy(buff3,pch);
	//x: nguoi gui
	//y: nguoi nhan
	for (x = 0; x < max_user; x++)	{
		if(user[x].sockfd==sockfd) break;
	}
	for (y = 0; y < max_user; y++){
		if(strcmp(user[y].nick_name,nick_name)==0){
			if(user[y].status==1){
				i=1;
				break;
			}
		}
		i=0;
	}
	if(i==0){
		creat_mesg(7,nick_name,buff2,buff2,buff2,buff);
		send(sockfd,buff,strlen(buff),0);
	}else{ //-1 Quan den 1 Quan trang (buff3)
		creat_mesg(6,user[x].nick_name,buff3,buff2,buff2,buff);
		// printf("\n%s Gui cho %s xau %s\n",user[y].nick_name,user[x].nick_name,buff);
		i=send(user[y].sockfd,buff,strlen(buff),0);
	}


	i=min(x,y);
	j=max(x,y);
	printf("%s\n",buff3);
	fprintf(fo[i][j], "%s di: %s\n",user[x].nick_name,buff3);
}
void stop_play(int sockfd,char mesg[],User user[], int max_user){
	int i,x,y,t,j;
	char mesg2[LEN],buff[LEN],buff2[LEN]="0",nick_name[LEN];
	char *pch;

	strcpy(mesg2, mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	//x: nguoi gui
	//y: nguoi nhan
	for (x = 0; x < max_user; x++)	{
		if(user[x].sockfd==sockfd) break;
	}
	for (y = 0; y < max_user; y++){
		if(user[y].status==1){
			if(strcmp(user[y].nick_name,nick_name)==0) break;
		}
	}
	strcpy(buff,"");
	creat_mesg(7,user[x].nick_name,buff2,buff2,buff2,buff);
	send(user[y].sockfd,buff,strlen(buff),0);

	strcpy(mesg2,mesg);
	pch=strtok(mesg2,"|");
	pch=strtok(NULL,"|");
	strcpy(nick_name,pch);
	pch=strtok(NULL,"|");

	i=min(x,y);
	j=max(x,y);
	
	if (strcmp(pch,"0")==0)
	{
		fprintf(fo[i][j],"%s da thoat va thua cuoc\n", user[x].nick_name);
	}	
	fprintf(fo[i][j], "%s da thang %s\n", user[y].nick_name,user[x].nick_name);

	fclose(fo[i][j]);
}

void log_out(int sockfd,User user[],int max_user){
	int i;
	for(i=0;i<max_user;i++){
		if(user[i].sockfd==sockfd){
			user[i].status=0;
			break;
		}
	}
}


void main(){
	
	int listenSock, connSock;
    struct sockaddr_in serv_addr, client_addr;
    int clientAddrLenght;
    User user[100];
    char tmp[100];
    int max_user;
    char mesg[LEN];
    char mesg2[LEN];
    char buff[LEN]="11|";
    int t,n;
    int recvBytes,sentBytes;
    fd_set readfds,allset;
    int i, maxi,sockfd;
    int rv;
    struct pollfd client[30];

    serv_addr.sin_family = AF_INET;       
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    serv_addr.sin_port = htons(5556); 

    
    if((listenSock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("\nTao socket loi!!\n");
        return;
    }
    if(bind(listenSock, (struct sockaddr *) &serv_addr,sizeof(serv_addr))<0) {
        printf("\nBind loi!!\n");
        return;
        }
    if(listen(listenSock, 10) == -1) {
        printf("\nListen loi!!\n");
        return;
    }
    for ( i = 1; i < 30; i++){
        client[i].fd=-1;
    }
    client[0].fd = listenSock;
    client[0].events = POLLRDNORM;	
    max_user= take_user(user);
        
    while(1){
     	rv = poll(client, 30,3); //Number of client = 30, time out=3
    
     	if(client[0].revents & POLLRDNORM){
            clientAddrLenght=sizeof(client_addr);
            connSock = accept(listenSock, (struct sockaddr *) &client_addr, (socklen_t*)&clientAddrLenght);
            

            for(i=1;i < 30;i++){
                if(client[i].fd < 0){
                    client[i].fd = connSock;
                    break;
                }
            }//end for
            client[i].events = POLLRDNORM;
            if(--rv <= 0) //no more readable descriptor. Decrement before return
                continue;
        }//end if
        for(i=1;i<30;i++){
            if((sockfd = client[i].fd) <0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR)){
                strcpy(mesg,"");
                n=recv(sockfd, mesg, LEN, 0);

                printf("\nXau nhan tu Client: %s",mesg );
                
                mesg[n]='\0';
                if((n)==0){
                    close(sockfd);
                    client[i].fd = -1;
                    for ( i = 0; i < max_user; i++){
                    	if(user[i].sockfd==sockfd){
                    		user[i].status=0;
                    		break;
                    	}
                    }
                }else{
                    // printf("\nXau nhan tu Client: %s",mesg );
                    t=check_mark(mesg);
                   	if(t == 0)	max_user = sign_up(sockfd,mesg,user,max_user);
                   	if(t == 1)	{
                   		login(sockfd,mesg,user,max_user);
                   		
                   	}
                   	if(t == 2)	show_user_list(sockfd,user,max_user);
                   	if(t == 3)	play_request(sockfd,mesg,user,max_user);
                   	if(t == 4)	accept_request(sockfd,mesg,user,max_user);
                   	if(t == 5)	not_accept_request(sockfd,mesg,user,max_user);
                   	if(t == 6)	playing(sockfd,mesg,user,max_user);
                   	if(t == 7)	stop_play(sockfd,mesg,user,max_user);
                   	if(t == 8)	log_out(sockfd,user,max_user);
                }
                
            if(--rv <= 0)
                break;
            }
        }
    	
     	
    }//end while
}

