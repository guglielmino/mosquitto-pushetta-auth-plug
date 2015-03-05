#include <mysql/mysql.h>


void *ptta_mysql_init();
void ptta_mysql_destroy(void *conf);
char *ptta_mysql_validate_user(void *conf, const char *username, const char *password, int *authenticated);
