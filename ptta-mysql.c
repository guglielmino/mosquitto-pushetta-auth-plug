#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include "ptta-mysql.h"
#include "hash.h"

#include "pushetta-auth-plug.h"

#define QUERY_GET_USER "select username, password, id from auth_user where username='%s'"
#define QUERY_GET_USER_BY_TOKEN "select username, password, a.id from auth_user as a join authtoken_token as b on a.id=b.user_id where b.key='%s'; "


struct mysql_config {
	MYSQL *mysql;
	char *host;
	int port;
	char *dbname;
	char *user;
	char *pass;
   bool auto_connect;
};


typedef enum __get_user_type{
   BY_USERNAME,
   BY_TOKEN
}get_user_type;
 


static bool auto_connect(struct mysql_config *conf);
static char *escape(void *handle, const char *value, long *vlen);
struct django_auth_user *internal_get_django_user(void *handle, const char *username_or_token, get_user_type get_type);

static char *get_bool(char *option, char *defval)
{
    char *flag = p_stab(option);
    flag = flag ? flag : defval;
    if(!strcmp("true", flag) || !strcmp("false", flag)) {
       return flag;
    }
    printf("WARN: %s is unexpected value -> %s", option, flag);
    return defval;
}


void *ptta_mysql_init()
{
  struct mysql_config *conf;
  char *host, *user, *pass, *dbname, *p;
  int port;
  my_bool reconnect = false;
  char *opt_flag;

  host 		= p_stab("ptta_dbhost");
  p		   = p_stab("ptta_dbport");
  user		= p_stab("ptta_dbuser");
  pass		= p_stab("ptta_dbpass");
  dbname	   = p_stab("ptta_dbname");

  host = (host) ? host : strdup("localhost");
  port = (!p) ? 3306 : atoi(p);
  
  fprintf(stderr, "host %s\n", host);
  fprintf(stderr, "user %s\n", user);
  fprintf(stderr, "pass %s\n", pass);

  if ((conf = (struct mysql_config *)malloc(sizeof(struct mysql_config))) == NULL)
		return (NULL);
  conf->mysql		= mysql_init(NULL);
  conf->host		= host;
  conf->port		= port;
  conf->user		= user;
  conf->pass		= pass;
  conf->auto_connect    = false;
  conf->dbname		= dbname;
  
    opt_flag = get_bool("mysql_auto_connect", "true");
    if (!strcmp("true", opt_flag)) {
        conf->auto_connect = true;
    }

    opt_flag = get_bool("mysql_opt_reconnect", "true");
    if (!strcmp("true", opt_flag)) {
        reconnect = true;
        mysql_options(conf->mysql, MYSQL_OPT_RECONNECT, &reconnect);
    }

	if (!mysql_real_connect(conf->mysql, host, user, pass, dbname, port, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conf->mysql));
        if (!conf->auto_connect && !reconnect) {
            free(conf);
            mysql_close(conf->mysql);
            return (NULL);
        }
	}

  return ((void *)conf);
}

void ptta_mysql_destroy(void *handle)
{
   struct mysql_config *conf = (struct mysql_config *)handle;

	if (conf) {
		mysql_close(conf->mysql);
		free(conf);
	}
}


struct django_auth_user *get_django_user_by_username(void *handle, const char *username)
{
   return internal_get_django_user(handle, username, BY_USERNAME);
}

struct django_auth_user *get_django_user_by_token(void *handle, const char *token)
{
   return internal_get_django_user(handle, token, BY_TOKEN);
}



/**
 * Purpose: Get user from django using username as key or token (authtoken_auth table)
 * Note: 
 */
struct django_auth_user *internal_get_django_user(void *handle, const char *username_or_token, get_user_type get_type)
{
   char *query = NULL, *u = NULL;
   struct mysql_config *conf = (struct mysql_config *)handle;
   long nrows, ulen;
   MYSQL_RES *res = NULL;
	MYSQL_ROW rowdata;
	struct django_auth_user *result = NULL;
   
    
   if (mysql_ping(conf->mysql)) {
        LOG(MOSQ_LOG_ERR, "%s\n", mysql_error(conf->mysql));
        if (!auto_connect(conf)) {
            return (NULL);
        }
   }
   
	if ((u = escape(conf, username_or_token, &ulen)) == NULL)
		return (NULL);
   
   char *defined_query = get_type == BY_USERNAME ? QUERY_GET_USER : QUERY_GET_USER_BY_TOKEN;
   
   LOG(MOSQ_LOG_NOTICE, "USED QUERY %s", defined_query);
   
   query = (char *)malloc(strlen(defined_query) + strlen(username_or_token));
   sprintf(query, defined_query, username_or_token);

   if (mysql_query(conf->mysql, query)) {
		LOG(MOSQ_LOG_ERR, "%s\n", mysql_error(conf->mysql));
		goto out;
	}
	
	res = mysql_store_result(conf->mysql);
	if ((nrows = mysql_num_rows(res)) != 1) {
		LOG(MOSQ_LOG_ERR, "rowcount = %ld; not ok\n", nrows);
		goto out;
	}
	
	if (mysql_num_fields(res) != 3) {
		LOG(MOSQ_LOG_ERR, "numfields not ok\n");
		goto out;
	}

	if ((rowdata = mysql_fetch_row(res)) == NULL) {
	   LOG(MOSQ_LOG_ERR, "mysql_fetch_row == NULL");
		goto out;
	}
   
   result = (struct django_auth_user*)malloc(sizeof(struct django_auth_user));
   result->username = strdup(rowdata[0]);
   result->password = strdup(rowdata[1]);
	 
out:

	mysql_free_result(res);
	free(query);

	return (result);
}

// Utilities

static char *escape(void *handle, const char *value, long *vlen)
{
	struct mysql_config *conf = (struct mysql_config *)handle;
	char *v;

	*vlen = strlen(value) * 2 + 1;
	if ((v = malloc(*vlen)) == NULL)
		return (NULL);
	mysql_real_escape_string(conf->mysql, v, value, strlen(value));
	return (v);
}

static bool auto_connect(struct mysql_config *conf)
{
    if (conf->auto_connect) {
        if (!mysql_real_connect(conf->mysql, conf->host, conf->user, conf->pass, conf->dbname, conf->port, NULL, 0)) {
            fprintf(stderr, "do auto_connect but %s\n", mysql_error(conf->mysql));
            return false;
        }
        return true;
	}
    return false;
}




