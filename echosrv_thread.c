#include	"unpthread.h"
#include	<time.h>

static void	*doit_echo(void *);	
static void	*doit_time(void *);	

// Function that reads the data sent by the client and echoes it back
void str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
		Writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0){
		printf("Echo Read Error: Socket read returned with a value -1, errno = %d\n", errno);
		return;
	}
	else{
		printf("Echo client termination, socket read returned with value 0\n");
		return;									
	}
		
}

void time_srv(int sockfd ){
	
	char	buff[MAXLINE], readbuff[MAXLINE];
	time_t		ticks;
	ssize_t		n;
	for(;;){
		
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(sockfd, buff, strlen(buff));
		
		// Readable_timeo is used to monitor the socket for any messages sent by the client for a duration of 5 seconds. 
		// If ^C is done on the client to close it, the socket reads a zero which prints a message that read returned with value 0
		// and the thread terminates after properly closing the sockfd. Else it waits for a duration of 5 sec and then sends the
		// value of time to the client and this repeats.
		if(Readable_timeo(sockfd, 5) > 0){
			if( (n=read(sockfd, readbuff, MAXLINE)) == 0){
				printf("Time client termination, socket read returned with value 0\n");
				return;			
			}
			if(n < 0){
				printf("Time Read Error, socket read returned with value -1, errno = %d\n", errno);
				return;
			}
				
		}				
	}	
	
}	

int main(int argc, char **argv)
{
	int				listenfd[2];
	int 			connfd_echo, connfd_time;
	pthread_t		tid_echo, tid_time;
	socklen_t		addrlen, len;
	
	struct sockaddr_in	servaddr[2];
	int port[2]={24036,24037};
	struct sockaddr_in cliaddr_echo,cliaddr_time;
	socklen_t clilen_echo, clilen_time;
	int i;
	int			maxfdp1;
	fd_set		rset;
	const int on = 1;
	int flags[2];
	
	// listenfd[0] is echo and listenfd[1] is time
	for(i=0;i<2;i++){
		
	listenfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr[i], sizeof(servaddr[i]));
	servaddr[i].sin_family      = AF_INET;
	servaddr[i].sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr[i].sin_port        = htons(port[i]);
	
	Setsockopt(listenfd[i], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	Bind(listenfd[i], (SA *) &servaddr[i], sizeof(servaddr[i]));
	
	if( (flags[i] = fcntl(listenfd[i], F_GETFL, 0)) < 0)
		err_sys("F_GETFL error");
	flags[i] |= O_NONBLOCK;
	if(fcntl(listenfd[i], F_SETFL, flags[i]) < 0)
		err_sys("F_SETFL error");

	Listen(listenfd[i], LISTENQ);
	
	}
	
	// SIGPIPE is ignored, but its effects are correctly handled in the time_srv function
	Signal(SIGPIPE,SIG_IGN);
	
	FD_ZERO(&rset);
	for ( ; ; ) {
		FD_SET(listenfd[0], &rset);
		FD_SET(listenfd[1], &rset);
		maxfdp1 = max(listenfd[0], listenfd[1]) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		// echo socket is readable 
		if (FD_ISSET(listenfd[0], &rset)) {	
			clilen_echo = sizeof(cliaddr_echo);
			connfd_echo = Accept(listenfd[0], (SA *) &cliaddr_echo, &clilen_echo);
			flags[0] &= ~O_NONBLOCK;
			if(fcntl(connfd_echo, F_SETFL, flags[0]) < 0)
				err_sys("F_SETFL error");
						
			Pthread_create(&tid_echo, NULL, &doit_echo, (void *) connfd_echo);
		
		}

		// time socket is readable 
		if (FD_ISSET(listenfd[1], &rset)) {  
			clilen_time = sizeof(cliaddr_time);
			connfd_time = Accept(listenfd[1], (SA *) &cliaddr_time, &clilen_time);
			flags[1] &= ~O_NONBLOCK;
			if(fcntl(connfd_time, F_SETFL, flags[1]) < 0)
				err_sys("F_SETFL error");
			
			Pthread_create(&tid_time, NULL, &doit_time, (void *) connfd_time);
			
		}
	}
	
}

static void * doit_echo(void *arg)
{
	Pthread_detach(pthread_self());
	str_echo((int) arg);	
	Close((int) arg);		
	return;
}

static void * doit_time(void *arg)
{
	Pthread_detach(pthread_self());
	time_srv((int) arg);	
	Close((int) arg);		
	return;
}



