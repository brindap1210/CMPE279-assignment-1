// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct passwd* pwd_ptr;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
	&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    //privilege separation starts here
    pid_t sub_process = fork();
    int status = 0; 
    if(sub_process == -1) 
    {
        perror("Fork failed"); 
        exit(EXIT_FAILURE); 
    }   

    if(sub_process == 0) // child process 
    { 
        pwd_ptr = getpwnam("nobody");
        // if(setuid(pwd_ptr->pw_uid) == -1){
        printf("Log: Child Process: UID of nobody=%ld\n",(long) pwd_ptr->pw_uid);
        if(setuid(pwd_ptr->pw_uid) != 0){
            perror("failed to set id of child process");
            // printf("The user ID is %d not 99 \n", getuid());
            exit(EXIT_FAILURE);
        } else {
            valread = read(new_socket, buffer, 1024);
            printf("Read %d bytes: %s\n", valread, buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
            printf("*** Child process is done ***\n");
        }
    } else {
        while ((sub_process = wait(&status)) > 0);
        printf("*** Parent process is done ***\n");
    }

    // valread = read(new_socket, buffer, 1024);
    // printf("Read %d bytes: %s\n", valread, buffer);
    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

    return 0;

}
