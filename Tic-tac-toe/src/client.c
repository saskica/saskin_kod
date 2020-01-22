/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2019/2020
    Semestar:       Zimski (V)
    Autori:         Aleksandra Pjevalica
		    Jovana Milanovic

    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

void Grid(char matrix[3][3]);

int main(int argc , char *argv[])
{

    int sock;
    struct sockaddr_in server;
    char position[DEFAULT_BUFLEN];
    char player[DEFAULT_BUFLEN];
    char ticTacToeMatrix[3][3];
    char turn[DEFAULT_BUFLEN];
    char gameResults[DEFAULT_BUFLEN];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    puts("Connected\n");

    //X or O player
    if(recv(sock , player , 16 , 0) < 0)
    {
	puts("Receive failed");
        return 1;
    }

    puts(player);
    puts("***************************************\n");

    while(1)
    {
   	if(recv(sock , ticTacToeMatrix , 9 , 0) < 0)
	{
		puts("Receive failed");
                return 1;
        }

	if(!strcmp(ticTacToeMatrix , "game_over"))
	{
		break;
	}
   	Grid(ticTacToeMatrix);
	puts("\n");

    	if(recv(sock , turn , 15 , 0) < 0)
	{
                puts("Receive failed");
                return 1;
        }
    	puts(turn);

	//Enter the position in the matrix
   	scanf("%s", position);
   	if(send(sock , position , strlen(position) , 0) < 0)
   	{
       		puts("Send failed");
       		return 1;
   	}

        if(recv(sock , ticTacToeMatrix , 9 , 0) < 0)
        {
                puts("Receive failed");
		return 1;
        }
	puts("\n");
	Grid(ticTacToeMatrix);
	puts("\n");
    }

    //Final results
    puts("***************************************\n");
    if(recv(sock , ticTacToeMatrix , 9 , 0) < 0)
    {
	puts("Receive failed");
        return 1;
    }
    Grid(ticTacToeMatrix);
    puts("\n");

    if(recv(sock , gameResults , 9 , 0) < 0) 
    {
        puts("Receive failed");
	return 1;
    }
    puts(gameResults);
    puts("***************************************\n");

    close(sock);

    return 0;
}

void Grid(char matrix[3][3])
{
    int rowColumnNum = 0;
    printf("  %d %d %d\n" , rowColumnNum , rowColumnNum + 1 , rowColumnNum + 2);
    for(int row = 0 ; row < 3 ; row++)
    {
	printf("%d " , rowColumnNum++);
        for(int column = 0 ; column < 3 ; column++)
	{
                if(column == 2)
		{
                	printf("%c", matrix[row][column]);
                	printf("\n");
            	} else
		{
                	printf("%c|", matrix[row][column]);
            	}
        }
     }
}

