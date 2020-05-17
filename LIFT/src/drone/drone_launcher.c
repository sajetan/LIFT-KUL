#include <pthread.h>
#include "../include/common/globals.h"
#include "../include/common/chacha20_poly1305_ae.h"


//for user interaction
uint32_t *user_interaction_thread_(void *data){

}


//for session handling
uint32_t *session_handler_thread_(void *data){

}

//for user interaction
uint32_t *communication_handler_thread_(void *data){

}


uint32_t main() {

	pthread_t user_interaction;
	pthread_t session_handler;
	pthread_t communication_handler;


	pthread_create(&user_interaction, NULL, user_interaction_thread_, NULL);
	pthread_detach(user_interaction);

	pthread_create(&session_handler, NULL, session_hanlder_thread_, NULL);
	pthread_detach(session_handler);


	pthread_create(&communication_handler, NULL, communication_handler_thread_, NULL);
	pthread_detach(communication_handler);



	return (0);
}
