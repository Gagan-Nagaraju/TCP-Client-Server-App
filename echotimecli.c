#include	"unp.h"
#include <ctype.h>
#include <string.h>

// Utility function to check if IP or domain name of the server is entered
int ifIP(char array[])
{
    int i;
   
    for (i=0; array[i] != '\0'; i++)
    {       
        if (!isdigit(array[i])){
			if(array[i]!='.')
				return 0;
		}
           
    }
    return 1;
}

// Signal Handler for SIGCHLD
void sig_child(int signo)
{
	pid_t	pid;
	int		status;

	while ( (pid = waitpid(-1, &status, WNOHANG)) > 0)
		printf("Child with pid : %d terminated\n", pid);
	return;
}

int main(int argc, char **argv)
{
	char			*ptr, **pptr;
	char			str[INET_ADDRSTRLEN];
	struct hostent	*hptr;
	struct in_addr  ipaddr;
	int 			isIP, status1, status2;
	char 			service[MAXLINE];
	pid_t 			pid1, pid2, childpid1, childpid2;
	int 			pfd[2];
	char 			temp[MAXLINE], readbuff1[MAXLINE], readbuff2[MAXLINE], buff[MAXLINE];
	int 			ch, maxfdp1, n, m, r1, r2, ready;
	fd_set 			rset;
	void			sig_child(int);
	int 			flag = 0;
	const char		*argument;
	
	// Displaying the domain name or the IP address of the server on the terminal	
	if((isIP=ifIP(argv[1])) == 0){
		
		ptr = *++argv;
		if ( (hptr = gethostbyname(ptr)) == NULL) {
			err_msg("gethostbyname error for host: %s: %s",
					ptr, hstrerror(h_errno));
			exit(1);
		}

		switch (hptr->h_addrtype) {
		case AF_INET:
			for ( pptr = hptr->h_addr_list; *pptr != NULL; pptr++){
				if(flag==0){
					argument = Inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
					printf("The IP address of the server is: %s \n", argument);
					flag = 1;					
				}
				else
					printf("The IP address of the server is: %s \n", Inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
				
			}			
			
			break;

		default:
			err_ret("unknown address type");
			break;
		}
		
		
	}	
	else{
		
		Inet_pton(AF_INET, argv[1], &ipaddr);
		if ( (hptr = gethostbyaddr(&ipaddr, sizeof(ipaddr), AF_INET)) == NULL) {
			err_msg("gethostbyname error for host: %s: %s",
					ptr, hstrerror(h_errno));
			exit(2);
		}
		printf("The domain name of the server is: %s \n", hptr->h_name);
		argument = argv[1];
		
	}
	
	Signal(SIGCHLD, sig_child);
	
	for(;;){	
			
		bzero(service, sizeof(service));
		bzero(buff, sizeof(buff));
		printf("What service do you request? echo or time?(enter quit to exit)\n");
		if(Fgets(service, MAXLINE, stdin) != NULL){
			if(strcmp(service,"quit\n")==0)
				break;
			
			// Launch the echo service on xterm, if echo is entered on the terminal
			else if(strcmp(service,"echo\n")==0){
				
				if(pipe(pfd) == -1){
					err_sys("cli: Pipe cannot be created");
					exit(3);
				}
				
				if( (childpid1 = Fork()) == 0){
					// child process
					Close(pfd[0]);
					snprintf(temp, sizeof(temp), "%d", pfd[1]);
					if( (execlp("xterm","xterm","-e","./echocli_final",argument,temp,(char *)0)) < 0 ){
						err_sys("xterm not created");
						exit(4);
						
					}
				}
				// parent process
				Close(pfd[1]);
				FD_ZERO(&rset);
				for(;;){
					FD_SET(pfd[0], &rset);
					FD_SET(fileno(stdin), &rset);
					maxfdp1 = max(pfd[0], fileno(stdin)) + 1;
					
					// select is used to monitor both the stdin and the read end of the pipe
					if( (ready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0){
						if(errno == EINTR)
							continue;
						else
							err_sys("Select Error");
					}
					
					if(FD_ISSET(pfd[0], &rset)){
						bzero(readbuff1, sizeof(readbuff1));
						if((r1 = read(pfd[0], readbuff1, MAXLINE)) != 0){
							if(r1 < 0 && errno == EINTR)
								continue;
							else if(r1 < 0)
								err_sys("Read Error");
							else
								printf("%s", readbuff1);							
						}
							
						if(r1 == 0){
							Close(pfd[0]);							
							break;
						}						
					}
					
					if(FD_ISSET(fileno(stdin), &rset)){
						if((n = Read(fileno(stdin), buff, MAXLINE)) > 0)
							printf("You are interacting with the wrong window\n");
					}
									
				}
				
			}

			// Launch the time service on xterm, if time is entered on the terminal
			else if(strcmp(service, "time\n") == 0){
				
				if(pipe(pfd) == -1){
					err_sys("cli: Pipe cannot be created");
					exit(3);
				}
				
				if( (childpid2 = Fork()) == 0){
					// child process
					Close(pfd[0]);
					snprintf(temp, sizeof(temp), "%d", pfd[1]);
					if( (execlp("xterm","xterm","-e","./timecli_final",argument, temp, (char *)0)) < 0 ){
						err_sys("xterm not created");
						exit(5);
						
					}
				}
				Close(pfd[1]);
				FD_ZERO(&rset);
				for(;;){
					FD_SET(pfd[0], &rset);
					FD_SET(fileno(stdin), &rset);
					maxfdp1 = max(pfd[0], fileno(stdin)) + 1;
					
					// select is used to monitor both the stdin and the read end of the pipe
					if( (ready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0){
						if(errno == EINTR)
							continue;
						else
							err_sys("Select Error");
					}
					
					if(FD_ISSET(pfd[0], &rset)){
						bzero(readbuff2, sizeof(readbuff2));
						if( (r2 = read(pfd[0], readbuff2, MAXLINE)) != 0){
							if(r2 < 0 && errno == EINTR)
								continue;
							else if(r2 < 0)
								err_sys("Read Error");
							else
								printf("%s", readbuff2);	
							
						}
						if(r2 == 0){
							Close(pfd[0]);							
							break;
						}						
					}
					
					if(FD_ISSET(fileno(stdin), &rset)){
						if((m = Read(fileno(stdin), buff, MAXLINE)) > 0)
							printf("You are interacting with the wrong window\n");						
					}
									
				}
				
			}
			
			// Check for a valid service entered by the user
			else{
				printf("Enter a valid service\n");
				continue;				
			}
		}	
		
	}
	
	printf("Exiting the parent client\n");
	exit(0);
}
