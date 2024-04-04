#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <limits.h>
#include "../calc_message.h"

/**
 * calc_server.c based on provided example Msg_Passing_Server.c
 */
int main(int argc, char *argv[]) {
	int rcvid;        // indicates who we should reply to
	int chid;         // the channel ID
	struct client_send *client_message;
	struct server_response *server_response;
	client_message = (struct client_send*)malloc(sizeof(struct client_send));
	server_response = (struct server_response*)malloc(sizeof(struct server_response));

	// create a channel --- Phase I
	chid = ChannelCreate(0);

	if (chid == -1) {
		perror("error - cannot create channel");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "CalcServer PID: %d\n", getpid());

	while (1) {
		// get the message, and print it
		rcvid = MsgReceive(chid, client_message, sizeof(struct client_send) + 1,
		NULL);
		fprintf(stderr, "CalcServer - Message rcvid %X received, %d %c %d\n", rcvid,
				client_message->left_hand, client_message->operator,
				client_message->right_hand);

		// calculate the result
		switch (client_message->operator) {
			case '+':
				// fprintf(stderr, "add case\n");
				if (client_message->left_hand
						> (INT_MAX - client_message->right_hand)) {
					server_response->statusCode = SRVR_OVERFLOW;
					sprintf(server_response->errorMsg, "Overflow occurred, integers are too large");
				} else {
					server_response->answer = (client_message->left_hand
							+ client_message->right_hand);
					server_response->statusCode = SRVR_OK;
				}
				break;
			case '-':
				// fprintf(stderr, "subtract case\n");
				server_response->answer = (client_message->left_hand
						- client_message->right_hand);
				server_response->statusCode = SRVR_OK;
				break;
			case 'x':
				// fprintf(stderr, "multiply case\n");
				if (client_message->left_hand
						> (INT_MAX / client_message->right_hand)) {
					server_response->statusCode = SRVR_OVERFLOW;
					sprintf(server_response->errorMsg, "Overflow occurred, integers are too large");
				} else {
					server_response->answer = (client_message->left_hand
							* client_message->right_hand);
					server_response->statusCode = SRVR_OK;
				}
				break;
			case '/':
				// fprintf(stderr, "divide case\n");
				if (client_message->right_hand == 0) {
					server_response->statusCode = SRVR_UNDEFINED;
					sprintf(server_response->errorMsg, "Cannot divide by 0");
				} else {
					server_response->answer = (float)client_message->left_hand
							/ (float)client_message->right_hand;
					server_response->statusCode = SRVR_OK;
				}
				break;
			default:
				// fprintf(stderr, "default case\n");
				server_response->statusCode = SRVR_INVALID_OPERATOR;
				sprintf(server_response->errorMsg, "Invalid Operator");
		}

		MsgReply(rcvid, EOK, server_response, sizeof(struct server_response) + 1);
	}

	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
