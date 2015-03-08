
#include "ptta-topic-namehandler.h"

#include <string.h>

#include "pushetta-auth-plug.h"

// Online RegExt tester https://regex101.com/
#define REGEX_TEXT "^\\/pushetta.co{1}([^\\/]+\\/?)+$"
#define MAX_ERROR_MSG 100

struct topic_name_hanler_data* topic_namehandler_init(){

	struct topic_name_hanler_data* handler_data = (struct topic_name_hanler_data*)malloc(sizeof(struct topic_name_hanler_data));
	handler_data->r = (regex_t *)malloc(sizeof(regex_t));

	int status = regcomp (handler_data->r, REGEX_TEXT, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
		char error_message[MAX_ERROR_MSG];
		regerror(status, handler_data->r, error_message, MAX_ERROR_MSG);
		LOG(MOSQ_LOG_ERR, error_message);
		return NULL;
    }
    return handler_data;
}

char *get_channel_from_topic(struct topic_name_hanler_data* handler, const char *topic){
    const int n_matches = 5;
    int i = 0;
    regmatch_t m[n_matches];
    char *found = NULL;

    if (handler->r == NULL){
    	LOG(MOSQ_LOG_ERR, "Wrong regex parser handler");
    	return found;
    }

	int nomatch = regexec (handler->r, topic, n_matches, m, 0);
    if (nomatch) {
        LOG (MOSQ_LOG_NOTICE, "No matches.\n");
        return NULL;
    }

    LOG(MOSQ_LOG_NOTICE, "MATCHES %d", n_matches);

    for (i=0; i < n_matches; i++) {
        int start;
        int finish;
        if (m[i].rm_so == -1) {
        	LOG(MOSQ_LOG_ERR, "Pattern not found");
        	return NULL;
        }
        start = m[i].rm_so;
        finish = m[i].rm_eo;
        if(finish > start){
        	LOG(MOSQ_LOG_NOTICE, "FOUND %d - %d", start, finish);
        	found = strndup(topic + start, finish - start);
    	}
    }

    return found;
}

void topic_namehandler_cleanup(struct topic_name_hanler_data* handler){
	if(handler != NULL){
		if(handler->r)
			free(handler->r);
		free(handler);
	}
}