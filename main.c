/*################################################################*/
/*#HW07_131044026.c                                        #*/
/*#-----------------------                                       #*/
/*#Written by M.Cemil GUNEY on April 23, 2014                 #*/
/*#                                                              #*/
/*#Description:                                                  #*/
/*#-----------------------                                       #*/
/*#GIT checkers game to be played by 2 players,which a modified	#*/
/*#	 version of Turkish checkers		                           #*/
/*################################################################*/

/*---------------Includes-------------------*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*---------------------Defines--------------------------*/
#define COLUMN_SIZE 8
#define MAX_LINE 25
#define P_COLUMN_SIZE 2
#define MAX_NAME 25
#define MAX_COMMAND 250

typedef enum {
    LEFT,
	UP,
	RIGHT
} direction_t;
/*----------------------------Function Prototypes-----------------------------------*/
int playGame(char* initFileName);
int getInitState(FILE *initFile,int table[][COLUMN_SIZE],int *rowC);
void printGame(int table[][COLUMN_SIZE],int rowC);
int makeMove(int table[][COLUMN_SIZE],int rowC,int playerNo,int *coorY,int *coorX,direction_t direction);
void getPieces( int table[][COLUMN_SIZE],int rowC,
				int pieces1[][P_COLUMN_SIZE],int *rowCP1,
				int pieces2[][P_COLUMN_SIZE],int *rowCP2);			
void printPieces(int pieces[][P_COLUMN_SIZE],int rowC);
void getOppositeView(int tableIn[][COLUMN_SIZE],int rowC,int tableReversed[][COLUMN_SIZE]);
int isBlocked(	int table[][COLUMN_SIZE],int rowC,int playerNo,int coorY,int coorX);
int isTerminated(int table[][COLUMN_SIZE],int rowC);
void saveGame(char* fileName,int table[][COLUMN_SIZE],int rowC);
void saveLog(char* fileName,char* logs,int rowC);
int isMoveLegal(int table[][COLUMN_SIZE],int rowC,int coorY,int coorX);

int main(){
	/*Start of Main*/
	char initFileName[MAX_NAME]="Initials.txt";		
	int error;

	error=playGame(initFileName);		

	return 0;
	/*ENd of Main*/
}
/*-----------------FUNCTIONS-------------------------------*/
/*-------------------Play GAME--------------------------------*/
int playGame(char* initFileName){
	/*STRT of play game*/
	FILE *initFile;										/*FILE* initial state of file*/
	FILE *comFile;					/*FILE* commands state of file*/

	int rowC;							/*Line counter*/
	int table[MAX_LINE][COLUMN_SIZE];	/*Main table*/
	int coorY,coorX;					/*Coordinates of piece*/
	int pieces1[MAX_LINE][P_COLUMN_SIZE],pieces2[MAX_LINE][P_COLUMN_SIZE];	/*Pieces1 and pieces 2 's coordinates*/
	int rowCP1,rowCP2;								/*pieces1 aand pieces2's counter*/
	int tableReversed[MAX_LINE][COLUMN_SIZE];		/*Main table's opposite*/
	char saveFileName[MAX_NAME]="saveInitials.txt";			/*Save file name*/
	char player1Name[MAX_NAME];						
	char player2Name[MAX_NAME];	
	char * pch;							/*Unsized string for strtok*/
	char commands[MAX_NAME];				/*Constant command string*/
	char tempCommands[MAX_NAME];			/*temporary commands string*/
	char logs[MAX_COMMAND]={0};				
	int playerNo=1;						/*Player no first player is player one*/
	int moveC=0;						/*Commands counter */
	int draw;							/*Checking draw*/			
	char playerName[MAX_NAME];			/*String of player name*/
	int finish=1;
	int checkCommands=0;				/*Checking commands*/
	int i,j;
	int resign=0;						/*Checking resign*/
	direction_t direction;				
	
	/*Get initials*/
	
	initFile=fopen(initFileName,"r-w");		
	if(initFile==NULL)						/*error code*/
		return (-1);
	getInitState(initFile,table,&rowC);			
	/*Get Player Names*/
	printf("Enter the Player 1's name :");
	scanf("%s",player1Name);
	printf("Enter the Player 2's name :");
	scanf("%s",player2Name);

	/*Select Mode*/
	
	comFile=fopen("Commands.txt","r");
	if(comFile==NULL){			/*Sequential Mode*/
		while(finish){		
			/*Print table*/
			if(playerNo==1){
				strcpy(playerName,player1Name);
				printf("\t Player %s's Turn \n",playerName);		
			}
			else if(playerNo==2){
				strcpy(playerName,player2Name);			
				printf("\t Player %s's Turn \n",playerName);
		
			}
			else
				return -1;

			printGame(table,rowC);
			
			/*Get the commands*/
			do{
				checkCommands=0;
				printf("Enter your command:");
				scanf("%s",commands);
				strcat(logs,commands);
				strcat(logs,"\n");
				strcpy(tempCommands,commands);				/*tempCommands will change*/
				++moveC;
				pch = strtok(tempCommands,"_");
				/*Switch Case for string*/

				if (strcmp(pch, "M") == 0){				/*MOVE*/
					pch=strtok(NULL,"_");				
					coorY=atoi(pch);
					pch=strtok(NULL,"_");
					coorX=atoi(pch);
					pch=strtok(NULL,"_");
					if(strcmp(pch,"LEFT")==0)
						direction=LEFT;
					else if(strcmp(pch,"UP")==0)
						direction=UP;
					else if(strcmp(pch,"RIGHT")==0)
						direction=RIGHT;
					else
						return -1;
			
					checkCommands=makeMove(table,rowC,playerNo,&coorY,&coorX,direction);

				}
				else if(strcmp(pch, "P") == 0){		/*PASS*/
					break;
				}
				else if(strcmp(pch, "SG") == 0){		/*SAVE GAME*/
					pch=strtok(NULL,"_");
					
					saveGame(pch,table,rowC);
					printf("GAME SAVED\n");
				}
				else if(strcmp(pch, "SL") == 0){		/*SAVE LOG*/
					pch=strtok(NULL,"_");
					saveLog(pch,logs,moveC);
					printf("Logs Saved\n");
			
				}
				else if(strcmp(commands, "SP") == 0){			/*SHOW PIECES*/
					getPieces(table,rowC,pieces1,&rowCP1,pieces2,&rowCP2);
					if(playerNo==1)
						printPieces(pieces1,rowCP1);
					else if(playerNo==2)
						printPieces(pieces2,rowCP2);
					else
						return -1;
					checkCommands=2;
				}
				else if(strcmp(commands, "R") == 0){			/*RESIGN*/
					if(playerNo==1)
						printf("\a\aPlayer %s win !!!!\n",player2Name);
					else if(playerNo==2)
						printf("\a\aPlayer %s win !!!!\n",player1Name);
					resign=1;				
				}
				else if(strcmp(commands, "OD") == 0){			/*OFFER DRAW*/
					printf(">>>Player %s want draw.Do you want?\n",playerName);
				}
				else if(strcmp(commands, "AD") == 0){			/*ACCEPT DRAW*/
					draw=1;
				}
				else if(strcmp(commands, "RD") == 0){			/*REJECT DRAW*/
					draw=0;	
					printf(">>>DRAW REJECTED!!!");			
				}
				else {			/*DEFULT*/
					checkCommands=1;		
				}
				if(checkCommands!=0)
					printf(">>>UNKNOWN COMMANDS!!!!\n");
			}while(checkCommands!=0);
						
			/*Check treminated*/
			if(resign==1)
				break;
			if(draw==1){
				printf(">>>The game is draw\n");
				break;
			}
			
			/*Get opposite*/
			getOppositeView(table,rowC,tableReversed);
			for(i=0;i<rowC;++i){
				for(j=0;j<COLUMN_SIZE;++j){
					table[i][j]=tableReversed[i][j];
				}
			}
			/*change the player*/
			if(playerNo==1)
				playerNo=2;
			else if(playerNo==2)
				playerNo=1;
		}
	}
	else{						/*Batch mode*/
		while(finish){		
			/*Print table*/
			if(playerNo==1){
				strcpy(playerName,player1Name);
				printf("\t Player %s's Turn \n",playerName);		
			}
			else if(playerNo==2){
				strcpy(playerName,player2Name);			
				printf("\t Player %s's Turn \n",playerName);
		
			}
			printGame(table,rowC);
			
			/*Get the commands*/
			do{
				checkCommands=0;
				
				fscanf(comFile,"%s",commands);
				printf("Commands>>>%s\n",commands);
				strcat(logs,commands);
				strcat(logs,"\n");
				strcpy(tempCommands,commands);
				++moveC;
				pch = strtok(tempCommands,"_");

				if (strcmp(pch, "M") == 0){				/*MOVE*/
					pch=strtok(NULL,"_");				
					coorY=atoi(pch);
					pch=strtok(NULL,"_");
					coorX=atoi(pch);
					pch=strtok(NULL,"_");
					if(strcmp(pch,"LEFT")==0)
						direction=LEFT;
					else if(strcmp(pch,"UP")==0)
						direction=UP;
					else if(strcmp(pch,"RIGHT")==0)
						direction=RIGHT;
					else
						return -1;
			
					checkCommands=makeMove(table,rowC,playerNo,&coorY,&coorX,direction);

				}
				else if(strcmp(pch, "P") == 0){		/*PASS*/
					break;
				}
				else if(strcmp(pch, "SG") == 0){		/*SAVE GAME*/
					pch=strtok(NULL,"_");
					saveGame(pch,table,rowC);
					printf("GAME SAVED\n");
				}
				else if(strcmp(pch, "SL") == 0){		/*SAVE LOG*/
					pch=strtok(NULL,"_");
					saveLog(pch,logs,moveC);
					printf("Logs Saved\n");
			
				}
				else if(strcmp(commands, "SP") == 0){			/*SHOW PIECES*/
					getPieces(table,rowC,pieces1,&rowCP1,pieces2,&rowCP2);
					if(playerNo==1)
						printPieces(pieces1,rowCP1);
					else if(playerNo==2)
						printPieces(pieces2,rowCP2);
					else
						return -1;
					checkCommands=2;
				}
				else if(strcmp(commands, "R") == 0){			/*RESIGN*/
					if(playerNo==1)
						printf("\a\aPlayer %s win !!!!\n",player2Name);
					else if(playerNo==2)
						printf("\a\aPlayer %s win !!!!\n",player1Name);
					resign=1;				
				}
				else if(strcmp(commands, "OD") == 0){			/*OFFER DRAW*/
					printf(">>>Player %s want draw.Do you want?\n",playerName);
				}
				else if(strcmp(commands, "AD") == 0){			/*ACCEPT DRAW*/
					draw=1;
				}
				else if(strcmp(commands, "RD") == 0){			/*REJECT DRAW*/
					draw=0;	
					printf(">>>DRAW REJECTED!!!");			
				}
				else {			/*DEFULT*/
					checkCommands=1;		
				}
				if(checkCommands!=0)
					printf(">>>UNKNOWN COMMANDS!!!!\n");
			}while(checkCommands!=0);
						
			/*Check treminated*/
			if(resign==1)
				break;
			if(draw==1){
				printf(">>>The game is draw\n");
				break;
			}
			
			/*Get opposite*/
			getOppositeView(table,rowC,tableReversed);
			for(i=0;i<rowC;++i){
				for(j=0;j<COLUMN_SIZE;++j){
					table[i][j]=tableReversed[i][j];
				}
			}
			/*change the player*/
			if(playerNo==1)
				playerNo=2;
			else if(playerNo==2)
				playerNo=1;
		}
	}
	
	

	return 0;
	/*End of playGame*/
}
/*----------------Get Initials State-----------------------------------*/
int getInitState(FILE *initFile,int table[][COLUMN_SIZE],int *rowC){
	int j;	
	int error;

	*rowC=0;
	do{
		for(j=0;j<COLUMN_SIZE;++j){
			error=fscanf(initFile,"%d",&table[(*rowC)][j]);
			if(error==0)
				return -1;
			if(error==EOF)
				break;			
		}
		++(*rowC);
	}while(error!=EOF);

	--(*rowC);

	return *rowC;
}
int makeMove(int table[][COLUMN_SIZE],int rowC,int playerNo,int *coorY,int *coorX,direction_t direction){
	int tempX,tempY;
	int otherPlayer;
	int error;

	if(playerNo==1)
		otherPlayer=2;
	else if(playerNo=2)
		otherPlayer=1;		

	--(*coorX);
	--(*coorY);
	if(table[*coorY][*coorX]==playerNo){
		if(isBlocked(table,rowC,playerNo,*coorY,*coorX)==1){		/*is blocked*/
			return -1;
		}


		if(direction==LEFT){
			if(table[*coorY][*coorX-1]==otherPlayer){
				if(isMoveLegal(table,rowC,*coorY,*coorX-2)==1)
					error=-1;
				else{
					table[*coorY][*coorX]=0;
					table[*coorY][*coorX-1]=0;
					*coorX-=2;
					error=0;
					}
			}
			else{
				if(isMoveLegal(table,rowC,*coorY,*coorX-1)==1)
					error=-1;
				else{
					table[*coorY][*coorX]=0;
					--(*coorX);
					error=0;
				}
			}

		}
		else if(direction==UP){
			if(table[*coorY-1][*coorX]==otherPlayer){
				if(isMoveLegal(table,rowC,*coorY-2,*coorX)==1)
					error=-1;

				else{
					table[*coorY][*coorX]=0;
					table[*coorY-1][*coorX]=0;
					*coorY-=2;
					error=0;
				}
			}
			else{
				if(isMoveLegal(table,rowC,*coorY-1,*coorX)==1)
					error=-1;
				else{		
					table[*coorY][*coorX]=0;	
					--(*coorY);
					error=0;
				}
			}
		}
		else if(direction==RIGHT){
			if(table[*coorY][*coorX+1]==otherPlayer){	
				if(isMoveLegal(table,rowC,*coorY,*coorX+2)==1)
					error=-1;
				else{
					table[*coorY][*coorX]=0;
					table[*coorY][*coorX+1]=0;
					*coorX+=2;
					error=0;
				}
			}
			else{
				if(isMoveLegal(table,rowC,*coorY,*coorX+1)==1)
					error=1;
				else{		
					table[*coorY][*coorX]=0;	
					++(*coorX);
					error=0;
				}
			}
		}
		if(error==0)
			table[*coorY][*coorX]=playerNo;	
	}
	else 
		error=-1;
	return error;
}
/*----------------------Print Game-----------------------------*/
void printGame(int table[][COLUMN_SIZE],int rowC){
	int i,j;
	int x;	
	printf("########1  2  3  4  5  6  7  8###########\n");
	for(i=0;i<rowC;++i){
		printf("#%d\t",i+1);
		for(j=0;j<COLUMN_SIZE;++j){
			x=table[i][j];

			if(x==0)
				printf("-");
			else if(x==1)
				printf("1");
			else if(x==-1)
				printf("X");
			else if(x==2)
				printf("2");
			
			printf("  ");
		}
		printf("\t#");
		printf("\n");
	}
	printf("#########################################\n");
}
/*----------------------Get Pieces-----------------------------*/
void getPieces( int table[][COLUMN_SIZE],int rowC,
				int pieces1[][P_COLUMN_SIZE],int *rowCP1,
				int pieces2[][P_COLUMN_SIZE],int *rowCP2){
	int i,j;
	int temp;
	int x1=0;
	int x2=0;
	
	*rowCP1=0;
	*rowCP2=0;

	for(i=0;i<rowC;++i){
		for(j=0;j<COLUMN_SIZE;++j){
			temp=table[i][j];
			if(temp==1){
				pieces1[*rowCP1][x1]=i+1;			/*y coordinates*/
				pieces1[*rowCP1][x1+1]=j+1;		/*x coordinates*/
				++(*rowCP1);
				x1=0;
			}
			else if(temp==2){
				pieces2[*rowCP2][x2]=i+1;			/*y coordinates*/
				pieces2[*rowCP2][x2+1]=j+1;		/*x coordinates*/
				++(*rowCP2);
				x1=0;
			}
		}
	}
}
/*-------------------Print PÄ°eces--------------------------------*/
void printPieces(int pieces[][P_COLUMN_SIZE],int rowC){
	int i,j=0;

	for(i=0;i<rowC;++i){
			printf("x :%d y :%d\n",pieces[i][j],pieces[i][j+1]);
			
		}
}
/*----------------------Get opposite view-----------------------------*/
void getOppositeView(int tableIn[][COLUMN_SIZE],int rowC,int tableReversed[][COLUMN_SIZE]){
	int i,j;
	int k=0,l=0;
	for(i=rowC-1;i>=0;--i){
		for(j=COLUMN_SIZE-1;j>=0;--j){
			tableReversed[k][l]=tableIn[i][j];
			++l;
			
		}
		++k;
		l=0;
	}
	



}
/*------------------------IS BLOCKED---------------------------*/
int isBlocked(	int table[][COLUMN_SIZE],int rowC,
				int playerNo,int coorY,int coorX){
	int up,left,right;
	int blocked;
	/*UP*/
	if(table[coorY-1][coorX]==playerNo || table[coorY-1][coorX]==-1  )
		up=1;
	else
		up=0;
	/*LEFT*/
	if(table[coorY][coorX-1]==playerNo || table[coorY][coorX-1]==-1  )
		left=1;
	else
		left=0;
	/*RIGHT*/
	if(table[coorY][coorX+1]==playerNo || table[coorY][coorX+1]==-1  )
		right=1;
	else
		right=0;
	/*Is Blocked*/	
	if(up==1 && left==1 && right==1)
		blocked=1;
	else
		blocked=0;

	return blocked;
}
/*--------------------Is Terminated-------------------------------*/
int isTerminated(int table[][COLUMN_SIZE],int rowC){

	int pieces1[MAX_LINE][P_COLUMN_SIZE],pieces2[MAX_LINE][P_COLUMN_SIZE];
	int rowCP1,rowCP2;
	int i,j;
	int blockedP1=0,blockedP2=0;
		

	getPieces(table,rowC,pieces1,&rowCP1,pieces2,&rowCP2);

	/*Anyone captures all pieces of his opponent*/
	
	if(rowCP1==0){
		printf("Player 2 wins\n");
		return 2;
	}
	else if(rowCP2==0){
		printf("Player 1 win!!\n");
		return 1;
	}
	for(i=0;i<COLUMN_SIZE;++i){
		if(table[0][i]==1){
			printf("PLAYER 1 WINN!!!!!");
			return 1;
		}
		else if(table[0][i]==2){
			printf("PLayer 2 WIN!!!!");
			return 2;		
		}
	}
	for(i=0;i<COLUMN_SIZE;++i){
		for(j=0;j<rowC;++j){
			if(table[i][j]==1){
				if(isBlocked(table,rowC,1,i,j)==1)
					++blockedP1;
			}
			else if(table[i][j]==2){
				if(isBlocked(table,rowC,2,i,j)==1)
					++blockedP2;
			}
			
		}
	}

	if(blockedP1==rowCP1)
		return 2;
	else if(blockedP2==rowCP2)
		return 1;

	return 0;
}
/*------------------------Save game---------------------------*/
void saveGame(char* fileName,int table[][COLUMN_SIZE],int rowC){
	FILE *saveFile;
	int i,j;
	saveFile=fopen(fileName,"w");
	
	for(i=0;i<rowC;++i){

		for(j=0;j<COLUMN_SIZE;++j){
			fprintf(saveFile,"%d	",table[i][j]);

		}
		fprintf(saveFile,"\n");
	}

	}
/*-----------------Save log----------------------------------*/
void saveLog(char* fileName,char* logs,int rowC){
		FILE *logFile;
	int i;
	char* pch;


	logFile=fopen(fileName,"w+");

	pch=strtok(logs,"\n");

	for(i=0;i<rowC;++i){

		fputs(pch,logFile);
		fputs("\n",logFile);
		pch=strtok(NULL,"\n");
	}
	fclose(logFile);
}
/*-----------------Is Move Legal----------------------------------*/
int isMoveLegal(int table[][COLUMN_SIZE],int rowC,int coorY,int coorX){
	int legal;
		
	if(table[coorY][coorX]!=0)
		legal=1;					/*Is not legal*/
	else
		legal=0;					/*Is legal*/


	return legal;
}
