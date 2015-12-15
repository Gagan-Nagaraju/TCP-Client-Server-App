# TCP-Client-Server-Application

#USER DOCUMENTATION:

There are four .c files in the directory: echosrv_thread.c, timecli_final.c, echocli_final.c and echotimecli.c
To run the code:
1) Type make
2) To run the client code: ./echotimecli <IP> or ./echotimecli <domain name>
3) To run the server code: ./echosrv_thread
4) To remove all the executable and object files, type: make clean

#SYSTEM DOCUMENTATION:

All the requirements/features mentioned in the Assignment 1 specification has been implemented, like:
1) Command line arguments processing, displaying the IP or domain name of the server with appropriate error messages
2) Client parent and the Client child with InterProcess Communication using the half duplex pipes.
3) Service request termination and program robustness with respect to appropriate display of the status messages on the parent terminal.
4) Handling EINTR during slow system calls and user interacting with the parent process window instead of the child window.
5) Multi-threading and server services making sure of thread safety.
6) Server robustness with respect to handling EPIPE error. The SIGPIPE signal is ignored but its effects are correctly handled
   using readable_timeo, where the socket is monitored for a duration of 5 sec and when ^C is done at the client, 0 is read at 
   the socket and the thread is terminated properly after closing the descriptor and appropriate message is displayed on the server terminal.
7) SO_REUSEADDR socket option and Non-blocking accept has also been done.
	

The server code is in the file echosrv_thread.c, the client parent code is in the file echotimecli.c and the client child codes for echo and time are in echocli_final.c and timecli_final.c respectively.



