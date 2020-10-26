/* time_server.c - main */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <time.h>

#define BUFLEN		100			/* buffer length */

struct pdu {
	char type;
	char data[100];
};

/*------------------------------------------------------------------------
 * main - Iterative UDP server for TIME service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	struct  sockaddr_in fsin;	/* the from address of a client	*/
	char	filename[BUFLEN],buf[BUFLEN], filebuf[BUFLEN];		/* "input" buffer; any size > 0	*/
	char    *pts;
	int		sock;			/* server socket		*/
	time_t	now;			/* current time			*/
	int		alen;			/* from-address length		*/
	struct  sockaddr_in sin; /* an Internet endpoint address         */
    int     s,sz, type;        /* socket descriptor and socket type    */
	int 	port=3000;
	int 	counter, n,m,i,bytes_to_read,bufsize;
	FILE 	*fptr;
	struct pdu packetrecieve, packetsend;
						                                                                    
	switch(argc){
		case 1:
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(1);
	}

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
                                                                                                 
    /* Allocate a socket */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
		fprintf(stderr, "can't creat socket\n");
                                                                                
    /* Bind the socket */
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n",port);
    listen(s, 5);	
	alen = sizeof(fsin);

	while (1) {
		if (recvfrom(s, &packetrecieve, BUFLEN+1, 0,(struct sockaddr *)&fsin, &alen) < 0)
			fprintf(stderr, "[ERROR] recvfrom error\n");
		// fprintf(stderr, "[INFO] Incoming File Request Packet Type: %c\n", packetrecieve.type);
		switch(packetrecieve.type){
			case 'C':
				//all the code
				fptr = fopen(packetrecieve.data, "r");
				if (fptr == NULL) {
					fprintf(stderr, "[ERROR] File Not Found Error\n");
					packetsend.type = 'E';
					strcpy(packetsend.data,"File Not Found Error");
					sendto(s, &packetsend, BUFLEN+1, 0,(struct sockaddr *)&fsin, sizeof(fsin));
					break;
				}
				fseek(fptr, 0, 2); sz = ftell(fptr); fseek(fptr, 0, 0);
				counter = 0;
				while(fgets(packetsend.data, BUFLEN, fptr)> 0) {
					packetsend.type = 'D';
					counter += sizeof(packetsend.data);
					if (counter >= sz) packetsend.type = 'F';
					sendto(s, &packetsend, BUFLEN+1, 0,(struct sockaddr *)&fsin, sizeof(fsin));
				}
				fprintf(stderr, "[INFO] File Transfer Successful: %s\n",packetrecieve.data);
				break;
			default:
				fprintf(stderr, "[ERROR] Unknown Packet Error\n");
				packetsend.type = 'E';
				strcpy(packetsend.data,"Unknown Packet Error");
				sendto(s, &packetsend, BUFLEN+1, 0,(struct sockaddr *)&fsin, sizeof(fsin));
		}
	}
}
