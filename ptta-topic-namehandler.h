#ifndef __TOPIC_NAME_HANDLER_H
#define __TOPIC_NAME_HANDLER_H

struct topic_name_hanler_data{
	regex_t * r;
};

struct topic_name_hanler_data* topic_namehandler_init();
char *get_channel_from_topic(struct topic_name_hanler_data *handler, const char *topic);
void topic_namehandler_cleanup(struct topic_name_hanler_data *handler);


#endif