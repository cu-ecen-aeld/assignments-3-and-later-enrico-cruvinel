#include <sys/socket.h> 
#include <syslog.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>

static volatile sig_atomic_t accept_connections = 1; 
const char* socketdata_filepath = "/var/tmp/aesdsocketdata";

static void sighandler(int signal_number){
    (void)signal_number;
    accept_connections = 0;
    syslog(LOG_INFO, "Caught signal, exiting");
}

int handle_error(const char* msg){
    perror(msg);
    return -1;
}

int find_newline(const char* buf, int size){
    int i = -1;
    for(int j = 0; j < size; j++){
        if (buf[j] == '\n'){
            i = j; 
            break;
        }
    }
    return i;
}

int main(int argc, char **argv){

    int daemon = 0;
    if(argc == 2 && strcmp(argv[1], "-d") == 0) daemon = 1;

    int rc = 0;

    // open file for writing
    int fd = open(socketdata_filepath, O_RDWR | O_CREAT | O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd < 0) return handle_error("open");

    // setup syslog logging using the LOG_USER facility   
    openlog(NULL, 0, LOG_USER);

    // setup SIGINT and SIGTERM handling
    struct sigaction act;
    memset(&act,0,sizeof(struct sigaction));
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);

    rc = sigaction(SIGINT, &act, NULL); 
    if(rc!=0) return handle_error("sigaction SIGINT");
    rc = sigaction(SIGTERM, &act, NULL); 
    if(rc!=0) return handle_error("sigaction SIGTERM");

    // (1) create stream socket 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) return handle_error("socket");

    // set SO_REUSEADDR
    int optval = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) return handle_error("setsockopt");
    
    // (2) bind socket to addr
    struct sockaddr_in addr;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    rc = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc == -1) return handle_error("bind");

    if(daemon){
        pid_t pid = fork();
        if(pid == -1) return handle_error("fork");
        if(pid != 0) return 0; // parent
    }
    
    // (3) listen for connections
    rc = listen(sockfd, SOMAXCONN);
    if (rc == -1) return handle_error("listen");

    // (4) accept connections
    while(accept_connections){
        struct sockaddr_in peer_addr; 
        socklen_t peer_addr_size;
        peer_addr_size = sizeof(peer_addr);

        int clientfd = accept(sockfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
        if(clientfd == -1 && errno == EINTR && !accept_connections) break;
        if(clientfd == -1) return handle_error("accept");
        
        // log connection
        char client_ip[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET, &peer_addr.sin_addr, client_ip, INET_ADDRSTRLEN) == NULL) strcpy(client_ip, "unknown");
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        // (4.a) read packets from connection
        char buf[256];
        ssize_t bytes_read = 0;
        int newline_idx = -1; 
        while(newline_idx == -1){
            bytes_read = recv(clientfd, buf, sizeof(buf), 0);
            if(bytes_read < 0) return handle_error("recv");
            if(bytes_read == 0) break;

            newline_idx = find_newline(buf, bytes_read);

            int write_len = (newline_idx == -1) ? bytes_read : (newline_idx + 1);
            if(write(fd, buf, write_len) < 0) return handle_error("write");

            if(newline_idx != -1) break;
        }

        // (4.c) return full content to client
        if(lseek(fd, 0, SEEK_SET) < 0) return handle_error("lseek");

        while((bytes_read = read(fd, buf, sizeof(buf))) > 0){
            int total_sent = 0;
            while(total_sent < bytes_read){
                ssize_t sent = send(clientfd, buf + total_sent, bytes_read - total_sent, 0);
                if(sent < 0) return handle_error("send");
                total_sent += sent;
            }
        }
        if(bytes_read < 0) return handle_error("read");
        
        // (4.d) close connection
        close(clientfd);
        syslog(LOG_INFO, "Closed connection from %s", client_ip);
    }

    // (5) caught signal, exiting
    // completing any open connection operations, 
    // closing any open sockets, 
    close(sockfd);
    close(fd);

    // deleting the file /var/tmp/aesdsocketdata
    remove(socketdata_filepath);

    closelog();

    return 0;
}
