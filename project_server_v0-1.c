/*#include <iostream>*/
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
 
#define SERVERPORT	50000
#define MAXBUF 		2048



/*using namespace std;*/

int main(int argc, char* argv[])
{
		
	int socket1,socket2;
    	int addrlen;
   	struct sockaddr_in xferServer, xferClient;
	int returnStatus = 0;
	/* create socket */
	
	socket1 = socket(AF_INET, SOCK_STREAM, 0);
	/* test is socket creation worked */
	if (socket1 == -1)
	{
		fprintf(stderr,  "Could not create socket\n");
		exit(1);
	}

	/* bind to a socket, use INADDR_ANY for all local addresses */
	xferServer.sin_family = AF_INET;
	xferServer.sin_addr.s_addr = INADDR_ANY;
	xferServer.sin_port = htons(SERVERPORT);

	returnStatus = bind(socket1, (struct sockaddr*)&xferServer, sizeof(xferServer));

	if (returnStatus == -1)
	{
		fprintf(stderr,  "Could not bind to socket\n");
		exit(1);
	}

	/* make an infinite loop for the server to run until terminated or crash */
	for (;;)
	{
		int testFileStatus;
		int i, readCounter, writeCounter;
		char* bufptr;
		char buf[MAXBUF];
		char filename[] = "testdata";

		/* wait for an incoming connection */
		addrlen = sizeof(xferClient);

		/* use accept() to handle incoming connection requests        */
		/* and free up the original socket for other requests         */
		socket2 = accept(socket1, (struct sockaddr*)&xferClient, &addrlen);
	    	printf("Details %i, %i\n", socket1, addrlen);

		if (socket2 == -1)
		{
		    fprintf(stderr, "Could not accept connection!\n");
		    exit(1);
		}
 
		
		/*filename[i+1] = '\0';*/

       		printf("Reading file %s\n", filename);

		/* open the file for reading */ 
		testFileStatus = open(filename, O_RDONLY); 

		if (testFileStatus == -1)
		{
		    fprintf(stderr,  "Could not open file for reading!\n");
		    shutdown(socket2,2);
		    continue;
		}
		/* reset the read counter */
		readCounter = 0;

		/* read the file, and send it to the client in chunks of size MAXBUF */
		while((readCounter = read(testFileStatus, buf, MAXBUF)) > 0)/* read returns the amount of bytes it has got from the file */
		{
			int size = readCounter;
			writeCounter = 0;
			bufptr = buf;
			/* 
			  capture time 'A' here 
			  A = currentTime();
			*/
			while (writeCounter < readCounter)
			{
				readCounter -= writeCounter;
				bufptr += writeCounter;
				
				writeCounter = write(socket2, bufptr, readCounter);/* write will return the number of bytes it sent */
				
				if (writeCounter == -1) 
				{
				    fprintf(stderr, "Could not write file to client!\n");
				    shutdown(socket2,2);
				    continue;
				}
			}
			/* 
			  capture time 'B' here 
			  B = currentTime();
			*/
        	}
		/* 
		  int rate = size / (B -A);
		  fprintf(stderr,  "Test file sent, average send speed was " << rate << "Mbps";
		*/
		shutdown(socket2,2); 
		shutdown(testFileStatus,2);

	}

	shutdown (socket1,2);
	return 0;

}

