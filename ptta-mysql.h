#include <mysql/mysql.h>


struct django_auth_user{
   char *username;
   char *password;
   int user_id;
};


void *ptta_mysql_init();
void ptta_mysql_destroy(void *conf);

struct django_auth_user *get_django_user_by_username(void *handle, const char *username);
struct django_auth_user *get_django_user_by_token(void *handle, const char *token);
int get_channel_owner_id(void *handle, const char *channel_name);
