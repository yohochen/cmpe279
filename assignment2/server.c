// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // printf("Argc: %d\n", argc);

    if (argc > 1)
    {
        // re-exec
        new_socket = atoi(argv[1]);
        valread = read(new_socket, buffer, 1024);
        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

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
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Dropping privilege

    pid_t pid = fork();

    if (pid > 0)
    {               // Positive value: if it is the parent process.
        wait(NULL); // block parent process until child finishs
    }
    else if (pid < 0)
    { // Negative value: if an error occurred.
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    else
    { // zero: if it is the child process (the process created).
        uid_t nobody_uid = getpwnam("nobody")->pw_uid;
        setuid(nobody_uid);

        char args_sock[50];
        char old_args[50];
        sprintf(args_sock, "%d", new_socket);
        sprintf(old_args, "%s", argv[0]);
        char *args[] = {old_args, args_sock, NULL};
        execvp(old_args, args);
    }
    return 0;
}
