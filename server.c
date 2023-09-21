#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define LISTEN_PORT 8080

int main(int argc, char *argv[])
{
//socket file desciptors
int sock_listen, sock_recv;
int i, addr_size, bytes_received;
int incoming_len;
int recv_msg_size;
struct sockaddr_in my_addr, recv_addr;
int select_ret;
fd_set readfds;
struct timeval timeout={0,0};
struct sockaddr remote_addr;
char buf[BUF_SIZE];
    
// create socket for listening
sock_listen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
if (sock_listen < 0)
{
printf("socket() failed\n");
exit(0);
}
// build the local address structure
memset(&my_addr, 0, sizeof(my_addr));
my_addr.sin_family = AF_INET;
my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
my_addr.sin_port = htons((unsigned short)LISTEN_PORT);

// bind the socket to the local ip address
i = bind(sock_listen, (struct sockaddr *) &my_addr,
sizeof(my_addr));
if (i < 0)
{
printf("bind() failed\n");
exit(0);
}
// assign listener to variable to perform error handling
i = listen(sock_listen, 5);
if (i < 0)
{
printf("listen() failed\n");
exit(0);
}

int pid; // declare process identifier
while (1)
{
    // set up a new socket to receieve data on
      addr_size = sizeof(recv_addr);
    
    // accept incoming connections, create a new socket for each one
      sock_recv = accept(sock_listen, (struct sockaddr*)&recv_addr,&addr_size);
    
    if (sock_recv == -1)
            return 1;
    
    // create a new child process to handle the incoming connection, close the listener if creating child process fails
    if ((pid = fork()) == 0) {
           close(sock_listen);

      while (1) {
	    // receive data on the new socket
          bytes_received = recv(sock_recv, buf, BUF_SIZE, 0);
          
          // if there are no bytes received, client has disconnected
          if (bytes_received == 0) {
                  printf("The server has lost connection with a client\n");
                  close(sock_recv);
                  break;
          }
        
          //Close the socket if the condition below true
          else if (strcmp(buf, "shutdown") == 10) {
                    printf("The connection between the server and a client has been lost\n");
                    close(sock_recv);
                    break;
                }
         else {
              buf[bytes_received] = 0;
		   // displays message on server
              printf("Message received: %s\n", buf); 
            }
        }
    }
}
    
close(sock_recv);
close(sock_listen);

}
