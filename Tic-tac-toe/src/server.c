/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2019/2020
    Semestar:       Zimski (V)
    Autori:	    Aleksandra Pjevalica
		    Jovana Milanovic
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int rows[3] = {0,0,0};
int columns[3] = {0,0,0};
int mainDiagonal = 0;
int antiDiagonal = 0;
int counter = 0;
int winnerX = 0;
int winnerO = 0;
int draw = 0;

void TicTacToeInit(char matrix[3][3]);
int GameOver(int row , int column , int move , int counter);

int main(int argc , char *argv[])
{
    int socketDesc , clientSock[2] , c;
    struct sockaddr_in server , client[2];
    char clientMessage[DEFAULT_BUFLEN];
    char *playerX = "You are player X";
    char *playerY = "You are player O";
    char *yourMove = "It's your turn:";
    char *winMessage = "You won! ";
    char *loseMessage = "You lost ";
    char *drawMessage = "It's draw";
    char ticTacToeMatrix[3][3];

    //Create socket
    socketDesc = socket(AF_INET , SOCK_STREAM , 0);
    if (socketDesc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if(bind(socketDesc ,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");

    //Listen
    listen(socketDesc , 2);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept connection from an incoming client
    clientSock[0] = accept(socketDesc , (struct sockaddr *)&client[0] , (socklen_t*)&c);
    if (clientSock[0] < 0)
    {
        perror("Accept failed");
        return 1;
    }
    puts("Connection accepted. Player X is connected.");

    //Accept connection from an incoming client
    clientSock[1] = accept(socketDesc , (struct sockaddr *)&client[1] , (socklen_t*)&c);
    if (clientSock[1] < 0)
    {
        perror("Accept failed");
        return 1;
    }
    puts("Connection accepted. Player O is connected.");

    if(send(clientSock[0] , playerX , strlen(playerX) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    if(send(clientSock[1] , playerY , strlen(playerY) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    TicTacToeInit(ticTacToeMatrix);
    //Player X plays first
    int turn = 1;
    int go = 1;
    int row , column , position;

    puts("Game is starting...");
    while(go)
    {
	if(turn == 1)
	{
        	if(send(clientSock[0] , ticTacToeMatrix , strlen(*ticTacToeMatrix) , 0) < 0)
    	        {
        		puts("Send failed");
        		return 1;
   	        }
	        if(send(clientSock[0] , yourMove , strlen(yourMove) , 0) < 0)
    	        {
        		puts("Send failed");
        		return 1;
   	        }

            	if(recv(clientSock[0] , clientMessage , DEFAULT_BUFLEN , 0) < 0)
	    	{
			puts("Receive failed");
			return 1;
	    	}
		//Updating tic-tac-toe matrix
	    	position = atoi(clientMessage);
	    	row = position/10;
	    	column = position%10;
	    	ticTacToeMatrix[row][column] = 'X';

	    	if(send(clientSock[0] , ticTacToeMatrix , strlen(*ticTacToeMatrix) , 0) < 0)
             	{
                        puts("Send failed");
                        return 1;
             	}
		//Player O is next to move
	    	turn--;
        }
	else
	{
            	if(send(clientSock[1] , ticTacToeMatrix , strlen(*ticTacToeMatrix) , 0) < 0)
    	     	{
        		puts("Send failed");
        		return 1;
   	     	}
	     	if(send(clientSock[1] , yourMove , strlen(yourMove) , 0) < 0)
    	     	{
        		puts("Send failed");
        		return 1;
   	     	}
	     	if(recv(clientSock[1] , clientMessage , DEFAULT_BUFLEN , 0) < 0)
             	{
			puts("Receive failed");
			return 1;
	     	}
		//Updating tic-tac-toe matrix
	     	position = atoi(clientMessage);
	     	row = position/10;
	     	column = position%10;
	     	ticTacToeMatrix[row][column]='O';

	     	if(send(clientSock[1] , ticTacToeMatrix , strlen(*ticTacToeMatrix) , 0) < 0)
             	{
                        puts("Send failed");
                        return 1;
             	}
		//Player X is next to move
             	turn++;
        }
	counter++;
	go = !GameOver(row , column , turn , counter);

	if(go == 0)
	{
		char *signal = "game_over";
		for(int i = 0; i < 2; i++)
		{
			if(send(clientSock[i] , signal , strlen(signal) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
		}
	}
    }

    puts("Game over. Sending results.");
    for(int i = 0; i < 2; i++)
    {
    	if(send(clientSock[i] , ticTacToeMatrix , strlen(*ticTacToeMatrix) , 0) < 0)
   	{
       		puts("Send failed");
                return 1;
    	}
    }

    if(winnerX)
    {
	if(send(clientSock[0] , winMessage  , strlen(winMessage) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	if(send(clientSock[1] , loseMessage  , strlen(loseMessage) , 0) < 0)
	{
                puts("Send failed");
		return 1;
        }
    }
    else if(winnerO)
    {
	if(send(clientSock[1] , winMessage  , strlen(winMessage) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
        if(send(clientSock[0] , loseMessage  , strlen(loseMessage) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
    }
    else if(draw)
    {
	if(send(clientSock[0] , drawMessage  , strlen(drawMessage) , 0) < 0)
	{
                puts("Send failed");
                return 1;
        }
        if(send(clientSock[1] , drawMessage  , strlen(drawMessage) , 0) < 0)
	{
                puts("Send failed");
                return 1;
        }
    }

    puts("Closing sockets...");
    close(clientSock[0]);
    close(clientSock[1]);
    close(socketDesc);

    return 0;
}

void TicTacToeInit(char matrix[3][3])
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
	{
            	matrix[i][j]='_';
        }
    }
}

int GameOver(int row , int column , int turn , int counter)
{
    int add;
    if(turn == 0)
    {
        add = 1;
    }
    else
    {
        add = -1;
    }
    rows[row] += add;
    if(rows[row] == 3)
    {
        winnerX = 1;
        return 1;
    }
    if(rows[row] == -3)
    {
        winnerO = 1;
        return 1;
    }
    columns[column] += add;
    if(columns[column] == 3)
    {
        winnerX = 1;
        return 1;
    }
    if(columns[column] == -3)
    {
        winnerO = 1;
        return 1;
    }
    if(row == column)
    {
        mainDiagonal += add;
    }
    if(mainDiagonal == 3)
    {
        winnerX = 1;
        return 1;
    }
    if(mainDiagonal == -3)
    {
        winnerO = 1;
        return 1;
    }
    if(row + column == 2)
    {
        antiDiagonal += add;
    }
    if(antiDiagonal == 3)
    {
        winnerX = 1;
        return 1;
    }
    if(antiDiagonal == -3)
    {
        winnerO = 1;
        return 1;
    }
    if(counter == 9)
    {
        draw = 1;
        return 1;
    }
    return 0;
}


