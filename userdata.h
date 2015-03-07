#ifndef __USERDATA_H
#define __USERDATA_H


typedef struct django_auth_user*(f_get_user)(void *handle, const char *username_or_token);


struct userdata {
   char *user_data_marker;
   void *mysql_conf;
   f_get_user *get_user;
};

#endif