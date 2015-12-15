#include	"unp.h"

void str_cli(FILE *, int);

// Function to send status message to parent when ^C is done on the xterm
void intpHandler(int sig){
	fprintf(stderr, "Child process terminated due to EINTR\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int		sockfd;
	struct  sockaddr_in	servaddr;

	if (argc != 3)
		err_quit("usage: executable <IP> pipeDescriptor");
	
	int fd = (int) strtol(argv[2], NULL, 10);
	// Write end of the pipe is dup2 to stderr to send status messages to parent
	dup2(fd, 2);
	Close(fd);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(24036);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("Error connecting to the server");
	
	
	Signal(SIGINT, intpHandler);
	
	str_cli(stdin, sockfd);		

	exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
	int			maxfdp1, stdineof;
	fd_set		rset;
	char		buf[MAXLINE];
	int		n;

	stdineof = 0;
	FD_ZERO(&rset);
	for ( ; ; ) {
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		
		// Select is used to monitor both the socket and the stdin
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {	
			if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
				if (stdineof == 1){
					fprintf(stderr, "Child process terminated due to eof\n");
					return;		
					
				}					
				else
					err_quit("Server terminated prematurely");										
			}

			Write(fileno(stdout), buf, n);
		}

		if (FD_ISSET(fileno(fp), &rset)) {  
			if ( (n = Read(fileno(fp), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);	
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			Writen(sockfd, buf, n);
		}
	}
}

