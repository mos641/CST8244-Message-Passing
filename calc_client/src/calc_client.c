#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include "../../calc_server/calc_message.h"

/**
 * calc_client.c based on provided example of Msg_Passing_Client.c
 */
int main (int argc, char* argv[])
{
	int  coid;
	char *ptr;
	pid_t serverpid;
	struct client_send *client_message;
	struct server_response *server_response;
	client_message = (struct client_send*)malloc(sizeof(struct client_send));
	server_response = (struct server_response*)malloc(sizeof(struct server_response));

	// check number of command line arguments
	if (argc != 5){
		fprintf (stderr, "Must be 5 command line arguments \n");
		exit (EXIT_FAILURE);
	}

	// check for overflow
	if ( (strtol(argv[2], &ptr, 10) > INT_MAX) || (strtol(argv[4], &ptr, 10) > INT_MAX) ){
		fprintf (stderr, "Client error - integers too large, will cause overflow \n");
		exit (EXIT_FAILURE);
	}
	// store message to send
	serverpid = atoi(argv[1]);
	client_message->left_hand = atoi(argv[2]);
	client_message->operator = argv[3][0];
	client_message->right_hand = atoi(argv[4]);

	//fprintf (stderr, "client message is %d %c %d\n", client_message->left_hand, client_message->operator,	client_message->right_hand);

	coid = ConnectAttach (ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		fprintf (stderr, "Could not ConnectAttach\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	if (MsgSend (coid, client_message, sizeof(struct client_send) + 1, server_response, sizeof(struct server_response) + 1) == -1L) {
		fprintf (stderr, "Error during MsgSend\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	if (server_response->statusCode == SRVR_OK) {
		fprintf(stderr, "The server has calculated the result of %d %c %d as %.2f\n", client_message->left_hand, client_message->operator, client_message->right_hand, server_response->answer);
	} else {
		fprintf(stderr, "The server has responded with error code %d - %s\n", server_response->statusCode, server_response->errorMsg);
	}

	//Disconnect from the channel --- Phase III
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
