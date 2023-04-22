#include <string>
#include <cstring>
#include <cstdlib>

#include <errno.h>
#include <signal.h>
#include <stdio.h>				// NULL 
#include <unistd.h>				// gethostname 
#include <sys/types.h>		// 
#include <sys/socket.h>		// socket bind listen
#include <sys/wait.h>
#include <netinet/in.h>		// struct sockaddr_in
#include <netdb.h>				// gethostbyname struct hostent


#define PORTNUM 50000 /* random port number */


/* as child processes end we need to catch their returns to avoid zombies*/
void fireman(void) {
	while(waitpid(-1, NULL, WNOHANG) > 0)
		;
}

/* function that uses the socket to send something */
void do_something(int s) {
	(void) s;
}

/* establish a socket/connection */
int establish(unsigned short portnum) {
	char myname[10];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;

	memset(&sa, 0, sizeof(struct sockaddr_in));						/* clear address */
	gethostname(myname, 10);															/* identification */
	hp = gethostbyname(myname);														/* get address info */
	if (hp == NULL)		
		return (-1);
	sa.sin_family = hp->h_addrtype;												/* host address */
	sa.sin_port = htons(portnum);													/* port number */

	/** Listen to connections
	 * 	@socket: Allocate a socket descriptor.
	 * 	@prototypes: int socket(int domain, int type, int protocol);
	 * 
	 * */ 
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {			/* create socket */
		close(s);
		return (-1);
	}

	// Binds a socket to an address
	if (bind(s, (struct sockaddr*)&sa, sizeof(sa)) < 0) {	/* bind address to socket */
		close(s);
		return(-1);
	}

	// Set max # of requests before start denying them
	listen(s, 3);																					/* max # of queued connects */
	return(s);
}

/* wait for a connection to occur on a socket created with establish() */
int get_connection(int s) {
	int t;																								/* socket of connection */

	if ((t = accept(s,NULL,NULL)) < 0) 										/* accept connection if there is one */
		return(-1);
	return(t);
}

/* call a particular port number on a particular host */
int call_socket(char *hostname, unsigned short portnum) {
	struct sockaddr_in sa;
	struct hostent *hp;
}

int main(int argc, char* argv[]) {

	(void) argc;
	(void) argv;

	int s, t;

	if ((s = establish(PORTNUM)) < 0) { 	/* "plug in the phone" */
		perror("establish");
		exit(1);
	}

	//signal(SIGCHLD, fireman); 						/* eliminate zombies */
	
	for(;;) {															/* loop for "phone calls" */
		if ((t = get_connection(s)) < 0) {	/* get a connection	*/
			if (errno == EINTR)								/* EINTR (Error code) might happen on accept() */
				continue;												/* new try */
			perror("accept");									/* failure */
			exit(1);
		}
		switch(fork()) {										/* try to handle the connection */
			case -1 :													/* failure */
				perror("fork");
				close(s);
				close(t);
				exit(1);
			case 0 :													/* child process */
				close(s);
				do_something(t);
				exit(0);
			default :													/* parent process */
				close(t);	
				continue;
		}
	}
}

/** @resources: 
 * 
 *		@sockets:
 *		cis,temple.edu/~ingargio/old/cis307s96/readings/docs/sockets.html
 * 
 * 		@bind:
 * 		gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
 * 	
*/