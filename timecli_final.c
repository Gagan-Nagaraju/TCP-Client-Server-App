#include	"unp.h"

// Function to send status message to parent when ^C is done on the xterm
void interuptHandler(int signo){
	fprintf(stderr, "Child process terminated due to EINTR\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int			sockfd, n;
	char		recvline[MAXLINE + 1], buff[MAXLINE];
	struct      sockaddr_in	servaddr;
	int			maxfdp1;
	fd_set		rset;

	if (argc != 3)
		err_quit("format: executable <IP> pipeDescriptor");
	
	int fd = (int) strtol(argv[2], NULL, 10);	
	dup2(fd, 2);
	Close(fd);

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(24037);	
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);	

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("Error connecting to the server");
	
	Signal(SIGINT, interuptHandler);
		
	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");
	else
		fprintf(stderr, "Server terminated prematurely\n");	
	
	exit(0);
}
