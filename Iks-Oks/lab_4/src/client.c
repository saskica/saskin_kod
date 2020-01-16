/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2019/2020
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

void ispis(char a[3][3]) {
    int u=0;
    printf("  %d %d %d\n", u, u+1, u+2);
    for(int l=0; l<3; l++) {
            printf("%d ", u++);
        for(int k=0; k<3; k++) {
            if(k==2) {
                printf("%c", a[l][k]);
                printf("\n");
            } else {
                printf("%c|", a[l][k]);
            }
        }
     }
}

int main(int argc , char *argv[])
{

    int sock;
    struct sockaddr_in server;
    char *message[DEFAULT_BUFLEN];
    char server_message1[DEFAULT_BUFLEN];
    char server_message2[3][3];
    char server_message3[DEFAULT_BUFLEN];
    char result[DEFAULT_BUFLEN];
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
    if(recv(sock , server_message1 , 16, 0) < 0)
    {
                        puts("Receive failed");
                        return 1;
    }

    puts(server_message1);

    char str[]="game_over";
    while(1) {
   	 if(recv(sock , server_message2 , 9 , 0) < 0)
	 {
                       puts("Receive failed");
                        return 1;
         }

	 if(!strcmp(server_message2, str)) {
		 break;
	 }
   	 ispis(server_message2);
    	 if(recv(sock , server_message3 , 14, 0) < 0)
	 {
                        puts("Receive failed");
                        return 1;
         }
    	 puts(server_message3);

   	 scanf("%s", message);
   	 if( send(sock , message , strlen(message), 0) < 0)
   	 {
       		 puts("Send failed");
       		 return 1;
   	 }
          if(recv(sock , server_message2 , 9 , 0) < 0)
         {
                       puts("Receive failed");
                        return 1;
         }
	 ispis(server_message2);

    }

    if(recv(sock , server_message2 , 9 , 0) < 0)
    {
                        puts("Receive failed");
                        return 1;
    }
    ispis(server_message2);

    if(recv(sock , result , 9 , 0) < 0) 
    {
                        puts("Receive failed");
                        return 1;
    }
    puts(result);

    close(sock);

    return 0;
}

