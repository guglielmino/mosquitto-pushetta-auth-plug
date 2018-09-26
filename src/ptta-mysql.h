#include <mysql/mysql.h>


struct django_auth_user{
   char *username;
   char *password;
   int user_id;
};

#define PRIVATE_CHANNEL 0
#define PUBLIC_CHANNEL 1

struct ptta_channel_data{
	int owner_id;
   	int kind;
};


void *ptta_mysql_init();
void ptta_mysql_destroy(void *conf);

struct django_auth_user *get_django_user_by_username(void *handle, const char *username);
struct django_auth_user *get_django_user_by_token(void *handle, const char *token);
struct ptta_channel_data *get_channel_owner_id(void *handle, const char *channel_name);
