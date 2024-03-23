/* 
 Title : Command-Line-Based Chat Room Application 
(TCP socket) 
 Description : This file is a client side program which sends message based on Authentication
               and stores the client side chat history in a file
 Author : Akshaya Baalaji Senthilraj ,Ankitha Anil Devanalli,
          Kamireddy Maheswar Reddy,Murali Dharan S, Yalagiri Swethavani
 version : 1.0
 Created : 18-03-2024
 Last Modified : 22-03-2024
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define PORT 1034

// File reader
char history[100];

//This creating server and client
struct sockaddr_in client, server;

//Function declaration
void displayfile(FILE *);

int main()
{
    int n, server_descriptor, bytes_read;
    //For reading the message from the client
    char message[40];

    char user_passwd[10]; //array for entering password
    char user_name[10];   //array for entering username
    char status[15];      //status from the server

    // file from which data need to be extracted
    FILE *file1;
    file1 = fopen("history.txt", "r");

    //This if for creation of socket on IPv4 and TCP based socket connection
    if ((server_descriptor = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    //This is for identifying the server
    server.sin_family = PF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //This is for connecting with server
    if (connect(server_descriptor, (struct sockaddr *)&server, sizeof(client)) == -1)
    {
        perror("connect");
        exit(1);
    }

    //For getting and sending the username to server for authentication
    fputs("\nEnter the username : ", stdout);
    fgets(user_name, 10, stdin);
    send(server_descriptor, user_name, 10, 0);

    //For getting and sending the password to server for authentication
    fputs("\nEnter the room password : ", stdout);
    fgets(user_passwd, 10, stdin);
    send(server_descriptor, user_passwd, 10, 0);

    //for receiving the status from the server
    if ((bytes_read = recv(server_descriptor, status, 15, 0)) == -1)
    {
        perror("recv at status ");
        exit(1);
    }
    //make the last index to '\0'
    status[bytes_read] = '\0';

    int choice;
    int a = 1;

    printf("\n");
    //validates the status of client credentials
    if (strcmp(status, "Connected") == 0)
    {
        printf("Status received %s \nWelcome %s \n ", status, user_name);
        printf("------------------------ Connection Successful -------------------------\n\n");

        //For sending and displaying the message
        while (a)
        {
            printf("Enter your choice 1 for history 0 for chat : ");
            scanf("%d", &choice);
            if (choice)
            {
                displayfile(file1); //function call
            }
            else
            {
                fputs("Enter the message : ", stdout);
                while (getchar() != '\n')
                    ; // this clears the input buffer
                //get message input from the user and send to server
                fgets(message, 40, stdin);
                fputs("Sending.......\n", stdout);
                send(server_descriptor, message, 40, 0);
            }
        }
    }
    //if the client status of failure breaks the connection
    if (strcmp(status, "Failed") == 0)
    {
        printf("Invalid credentials \n");
    }

    return 0;
}
/*
Function name   : displayfile()
Description     : This function will print the content
                  of the file by file-reader
Parameters      : file1
Return          : void
*/
void displayfile(FILE *file1)
{
    printf("------------------------Displaying chat history -------------------------\n");
    rewind(file1); // back to beginning of file
    // this helps to take the data and store on array
    while (fgets(history, sizeof(history), file1) != NULL)
    {
        // printf("**********************************\n");
        fprintf(stdout, "%s", history);
        // printf("**********************************\n");
    }
    printf("\n");
}