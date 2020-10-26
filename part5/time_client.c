/* time_client.c - main */

#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>                                                                            
#include <netinet/in.h>
#include <arpa/inet.h>
                                                                                
#include <netdb.h>

#define BUFLEN	100	/* buffer length */

#define	MSG		"Any Message \n"

struct pdu {
	char type;
	char data[100];
};


/*------------------------------------------------------------------------
 * main - UDP client for TIME service that prints the resulting time
 *------------------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	char	*host = "localhost";
	int	port = 3000;
	char	now[100];		/* 32-bit integer to hold time	*/ 
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int i, s, n, type;	/* socket descriptor and socket type	*/
	char *bp, rbuf[BUFLEN],filename[BUFLEN];
	FILE *outptr;

	switch (argc) {
	case 1:
		break;
	case 2:
		host = argv[1];
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "usage: UDPtime [host [port]]\n");
		exit(1);
	}

	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                                                                
    sin.sin_port = htons(port);
                                                                                        
    /* Map host name to IP address, allowing for dotted decimal */
    if ( phe = gethostbyname(host) ){
    	memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    } else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
		fprintf(stderr, "Can't get host entry \n");
                                                                                
    /* Allocate a socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
		fprintf(stderr, "Can't create socket \n");
	
                                                                                
    /* Connect the socket */
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "Can't connect to %s %s \n", host, "Time");
	
	/* pdu setup */
	struct pdu spud;
	
	while(1) {
		write(1,"Enter Filename (q to exit): ", sizeof("Enter Filename (q to exit): "));
		spud.type = 'C';
		n = read(0, spud.data, 100); 	// Read filename requested
		if (spud.data[0] == 'q'){
			fprintf(stderr, "bye\n");
			exit(0);
		} else if (spud.data[0] == '?'){
			fprintf(stderr, "[INFO] Simulating Bad Packet Type 'R'\n");
			fprintf(stderr, "Unknown Packet Error\n");
			spud.type = 'E';
			strcpy(spud.data,"WEIRD PACKET!!");
			write(s, &spud, n+1);
			continue;
		}


		spud.data[n-1] = '\0';			// Charstring cast
		strcpy(filename, spud.data);
		write(s, &spud, n+1);			// Send the PDU to the server
		n = read(s, &spud, 101);
		switch(spud.type){
			case 'E':
				fprintf(stderr, "[ERROR] Error from server: %s\n", spud.data);
				break;
			case 'D':
				outptr = fopen(filename, "w+");
				fprintf(outptr, spud.data, BUFLEN);
				while(i = read(s, &spud, BUFLEN+1) > 0) {
					fprintf(outptr, spud.data, BUFLEN);
					if (spud.type == 'F'){
						fprintf(stderr, "[INFO] File Download Complete: %s\n", filename);
						break;
					} 	
				}
				fclose(outptr);
				break;
			case 'F': 
				outptr = fopen(filename, "w+");
				i = read(s, &spud, BUFLEN+1);
				fprintf(outptr, spud.data,n-1);
				fclose(outptr);
				break;
		}
		
	}
}
