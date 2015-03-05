#ifndef __USERDATA_H
# define _USERDATA_H


typedef char *(f_validate_user)(void *conf, const char *username, const char *password, int *authenticated);

struct userdata {
   char *user_data_marker;
   void *mysql_conf;
   f_validate_user *validate_user;

};

#endif