/*
 Title : Command-Line-Based Chat Room Application
(TCP socket)
 Description : This file is a server side program which accept clients based on
Authentication and stores the client side chat history in a file.On conclusion,
this file mainly focuss on IPv4 TCP socket based communication. Author : Akshaya
Baalaji Senthilraj ,Ankitha Anil Devanalli, Kamireddy Maheswar Reddy,Murali
Dharan S, Yalagiri Swethavani version : 1.0 Created : 18-03-2024 Last Modified :
22-03-2024
 */

#include <arpa/inet.h>  // inet_ntoa
#include <netinet/in.h> // struct sockaddr_in
#include <pthread.h>    // ptherad_create
#include <stdio.h>      //printf, sprintf, fprintf, perror
#include <stdlib.h>     //exit, malloc, free
#include <string.h>     // strcpy , strcmp, strcat , strelen
#include <sys/socket.h> //bind , listen , accept , recv , socket
#include <sys/wait.h>   // wait
#include <unistd.h>     // fork , getpid , close

#define port_number 1034

// Default username and password for chat-room
char passwd[] = "12345";
char user1[] = "Akshay";
char user2[] = "Mahesh";
char user3[] = "Murali";
char user4[] = "Dharan";

// File reader
char history[100];

int server_descriptor;

// Function declaration
void displayfile(FILE *);
void *handle_client(void *);

/*
Function name   : int main()
Description     : This function will generate the process
                  2 by fork() and 2 by thread
Parameters      : NULL
Return          : 0
*/
int main()
{
    int yes = 1;
    socklen_t length = 0;

    // array for storing the client message
    char message[256];

    // This creating server and client
    struct sockaddr_in server, client;

    // this is for storing the thread id
    pthread_t thread_id, thread_id_1;

    // This if for creation of socket on IPv4 and TCP based socket connection
    if ((server_descriptor = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // This is assigning the sockaddr_in to server
    server.sin_port = htons(port_number);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // This for setting the option of the socket
    if (setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    // This is binding the server to socket
    if (bind(server_descriptor, (struct sockaddr *)&server, sizeof(server)) ==
        -1)
    {
        perror("bind");
        exit(1);
    }

    // This waits for the connection from client
    if (listen(server_descriptor, 5) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Waiting for connection.............\n");

    // This is for storing process id
    pid_t process_id;

    // For creating 2 fork and we use the child process for the client connection
    for (int i = 0; i < 2; i++)
    {
        process_id = fork();
        if (process_id < 0)
        {
            perror("fork");
            exit(1);
        }
        if (process_id == 0)
        {
            printf("Child process created by fork.\n");
            handle_client(NULL);
            exit(0);
        }
    }

    // For creating 2 thread and we use for client connection
    pthread_create(&thread_id, NULL, &handle_client, NULL);
    printf("Thread created by pthread_create.\n");
    pthread_create(&thread_id_1, NULL, &handle_client, NULL);
    printf("Thread created by pthread_create.\n");

    // This is for making the parent process wait till the child process completes
    for (int i = 0; i < 2; i++)
    {
        wait(NULL);
    }
    close(server_descriptor);
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
    printf("FROM FILE FUNC\n");
    rewind(file1); // make the file pointer points to beginning of file
    // this helps to take the bytes_reada and store on array
    while (fgets(history, sizeof(history), file1) != NULL)
    {
        printf("**********************************\n");
        fprintf(stderr, "%s", history);
        printf("**********************************\n");
    }
}

/*
Function name   : handle_client()
Description     : This function make the process to effectively
                  handle the client side operations and file handling
Parameters      : void
Return          : NULL
*/
void *handle_client(void *arg)
{
    int receiver_descriptor;
    char message[256];
    char username[20];
    char userpasswd[20];
    char status[15];
    int bytes_read;

    // This is for storing socket length
    socklen_t length = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    // This is the FILE pointer for file operations
    FILE *file1;
    file1 = fopen("history.txt", "a");

    // This is for establishing client connection with server
    while (1)
    {
        if ((receiver_descriptor = accept(
                 server_descriptor, (struct sockaddr *)&client, &length)) == -1)
        {
            perror("accept");
            exit(1);
        }

        // For reading the username from the client side
        if ((bytes_read = recv(receiver_descriptor, username, 40, 0)) == -1)
        {
            perror("recv at username");
            exit(1);
        }
        // Removing newline character from username
        for (int i = 0; i < bytes_read; i++)
        {
            if (username[i] == '\n')
            {
                username[i] = '\0';
                break;
            }
        }
        printf("Username received %s* of length  %ld \n ", username,
               strlen(username));

        // For reading the password from the client side
        if ((bytes_read = recv(receiver_descriptor, userpasswd, 40, 0)) == -1)
        {
            perror("recv at password ");
            exit(1);
        }

        for (int i = 0; i < bytes_read; i++)
        {
            if (userpasswd[i] == '\n')
            {
                userpasswd[i] = '\0';
                break;
            }
        }
        printf("Password received %s of length  %ld \n ", userpasswd,
               strlen(userpasswd));

        // This if condition will check the client credentials
        if (strcmp(passwd, userpasswd) == 0 &&
            (strcmp(username, user1) == 0 || strcmp(username, user2) == 0 ||
             strcmp(username, user3) == 0 || strcmp(username, user4) == 0))
        {
            // we are sending the status of connection toc lient
            strcpy(status, "Connected");
            printf("The status is %s", status);
            // for client socket we use receiver_descriptor
            send(receiver_descriptor, status, strlen(status), 0);
            printf("Got connection from client: %s\n", inet_ntoa(client.sin_addr));

            // After the successful authentication , Clients logged in
            while (1)
            {
                if ((bytes_read = recv(receiver_descriptor, message, 40, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }
                message[bytes_read] = '\0';
                printf("Mess received %s of length  %d \n ", message, bytes_read);

                // this is for the file appending
                char st[100] = "Client : ";
                pid_t pid = getpid();
                char cpid[6];
                // convert the integer to string an append to file
                sprintf(cpid, "%d", pid);
                strcat(st, cpid);
                strcat(st, "  : ");
                strcat(st, message); // here we are concating the client and the message

                // This if is for appending the client message to the file
                if (fputs(st, file1) == EOF)
                {
                    perror("Error append to file");
                    fclose(file1);
                    exit(0);
                }
                // This is a function call for printing content of the file
                displayfile(file1);
            }
            close(receiver_descriptor);
        }
        // this else will make end the client connection
        else
        {
            printf("Error : invalid credentials");
            strcpy(status, "Failed");
            printf("The status is %s", status);

            // send Failed for the client
            send(receiver_descriptor, status, strlen(status), 0);
            close(receiver_descriptor);
        }
    }
    return NULL;
}