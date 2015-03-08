#include "ptta-topic-namehandler.h"

#include "pushetta-auth-plug.h"

#define REGEX_TEXT "([^/]*)$"
#define MAX_ERROR_MSG 100

struct topic_name_hanler_data* topic_namehandler_init(){

	struct topic_name_hanler_data* handler_data = (struct topic_name_hanler_data*)malloc(sizeof(struct topic_name_hanler_data));
	int status = regcomp (handler_data->r, REGEX_TEXT, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
		char error_message[MAX_ERROR_MSG];
		regerror (status, handler_data->r, error_message, MAX_ERROR_MSG);
		return NULL;
    }
    return handler_data;
}

char *get_channel_from_topic(struct topic_name_hanler_data* handler, const char *topic){
    const char * p = topic;
    const int n_matches = 1;
    /* "M" contains the matches found. */
    regmatch_t m[n_matches];
    char *found = NULL;

	int nomatch = regexec (handler->r, p, n_matches, m, 0);
    if (nomatch) {
        printf ("No more matches.\n");
        return nomatch;
    }
    for (i = 0; i < n_matches; i++) {
        int start;
        int finish;
        if (m[i].rm_so == -1) {
            break;
        }
        start = m[i].rm_so + (p - topic);
        finish = m[i].rm_eo + (p - topic);
        
        LOG(MOSQ_LOG_NOTICE, "Match s:%d e:%d", start, finish);
    }

    return found;
}

void topic_namehandler_cleanup(struct topic_name_hanler_data* handler){
	if(handler != NULL){
		free(handler);
	}
}