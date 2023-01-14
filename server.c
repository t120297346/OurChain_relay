#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include "util.h"

#define BUFF_SIZE 1024
#define PORT 8787
#define ERR_EXIT(a){ perror(a); exit(1); }

int main(int argc, char *argv[]){
    int opt = 1; 
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 15, activity, valread, sd;
    int max_sd;

    struct sockaddr_in address;
    unsigned char magic_bytes[4] = {}, ourchain_bytes[4] = {0x8C, 0x70, 0x5A, 0xA8};
    char buffer[2 * BUFF_SIZE] = {};

    // set of socket descriptors
    fd_set readfds;

    // message
    char message[BUFF_SIZE] = "Hello World from Server";

    for (int i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }

    // create a master socket 
    if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0){
        ERR_EXIT("socket failed\n")
    }

    //set master socket to allow multiple connections ,
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 )
    {
        ERR_EXIT("setsockopt");
    }

    // Set server address information
    bzero(&address, sizeof(address)); // erase the data
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    
    // Bind the server file descriptor to the server address
    if(bind(master_socket, (struct sockaddr *)&address , sizeof(address)) < 0) {
        ERR_EXIT("bind failed\n");
    }
    printf("Listener on port %d \n", PORT);
        
    // Listen on the server file descriptor
    if(listen(master_socket , 3) < 0) {
        ERR_EXIT("listen failed\n");
    }

    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(1) {
	//clear the socket set 
        FD_ZERO(&readfds);
	
	//add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

	//add child sockets to set
        for(int i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

	activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	if((activity < 0) && (errno!=EINTR)) {
            printf("select error\n");
        }

	//If something happened on the master socket ,
        //then its an incoming connection
        if(FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                ERR_EXIT("accept failed");
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            //send new connection greeting message
            if(send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }
            

            puts("Welcome Relay Server");

            //add new socket to array of sockets
            for(int i = 0; i < max_clients; i++) {
                //if position is empty
                if( client_socket[i] == 0 ){
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

	//else its some IO operation on some other socket
        for(int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if(FD_ISSET(sd, &readfds)) {
                //Check if it was for closing , and also read the
                //incoming message
                if((valread = read(sd, magic_bytes, sizeof(magic_bytes))) == 0) {
                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                          inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }

                //Echo back the message that came in
                else {
                    //set the string terminating NULL byte on the end of the data read
		            int test = CompareMagicBytes(magic_bytes, ourchain_bytes);
                    char* ips = "";
                    if(test == 0) {
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        if(!IPIsExist(inet_ntoa(address.sin_addr))) {
                            WriteIP(inet_ntoa(address.sin_addr));
                        }
                        
                        GetIPs(inet_ntoa(address.sin_addr), &ips);
                    }

                    if(strcmp(ips, "") == 0){
                        if(send(sd, "null", 4, 0) != strlen(ips)) {
                            perror("send");
                        }
                    }
                    else{
                        if(send(sd, ips, strlen(ips), 0) != strlen(ips)) {
                            perror("send");
                        }
                    }
                }
            }
        }
    }

    return 0;
}
