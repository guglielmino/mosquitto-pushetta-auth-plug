#ifndef __USERDATA_H
#define __USERDATA_H


typedef struct django_auth_user*(f_get_user)(void *handle, const char *username_or_token);


struct userdata {
	void *mysql_handle;
	void *topicname_handler;
	// Auth credential for pushetta API (for authorize publishing on any channel)
	char *api_username;
	char *api_password;
};

#endif