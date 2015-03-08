/*
 * Copyright (c) 2015 Fabrizio Guglielmino <guglielmino@gumino.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of mosquitto nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <time.h>

#include "pushetta-auth-plug.h"

#include "hash.h"

#include "ptta-mysql.h"
#include "userdata.h"


int pbkdf2_check(char *password, char *hash);

int mosquitto_auth_plugin_version(void)
{
	return MOSQ_AUTH_PLUGIN_VERSION;
}

int mosquitto_auth_plugin_init(void **userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
   *userdata = (struct userdata *)malloc(sizeof(struct userdata));
	if (*userdata == NULL) {
		perror("allocting userdata");
		return MOSQ_ERR_UNKNOWN;
	}

	memset(*userdata, 0, sizeof(struct userdata));
	struct userdata *ud = *userdata;
	ud->user_data_marker = strdup("pushetta_user_data");
	
#ifdef GET_BY_USERNAME
	ud->get_user = get_django_user_by_username;
#else
 	ud->get_user = get_django_user_by_token;
#endif	
	
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_plugin_cleanup(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_init(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
	struct mosquitto_auth_opt *o;
	int i;

	for (i = 0, o = auth_opts; i < auth_opt_count; i++, o++) {
		//mosquitto_log_printf(MOSQ_LOG_NOTICE, "Options: key=%s, val=%s", o->key, o->value);
		p_add(o->key, o->value);
	}
	
	struct userdata *ud = (struct userdata *)userdata;
	ud->mysql_conf = ptta_mysql_init();
	mosquitto_log_printf(MOSQ_LOG_NOTICE, "** marker %s", ud->user_data_marker);
	
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_cleanup(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
   struct userdata *ud = (struct userdata *)userdata;
   if(ud->user_data_marker)
      free(ud->user_data_marker);
   free(ud);
	return MOSQ_ERR_SUCCESS;
}


int mosquitto_auth_unpwd_check(void *userdata, const char *username, const char *password)
{
   struct django_auth_user *django_user;
   char *value_to_check;
   
   
#ifdef GET_BY_USERNAME 
   value_to_check = (char *)username;
#else
 	value_to_check = (char *)password;
#endif  
   struct userdata *ud = (struct userdata *)userdata;

   
   //django_user = (struct django_auth_user *)ud->get_user(ud->mysql_conf, value_to_check);
   django_user = get_django_user_by_token(ud->mysql_conf, value_to_check);
     
   if(django_user != NULL){

      if(django_user->password != NULL)
         free(django_user->password);
   
      if(django_user->username != NULL)
         free(django_user->username);
         
      free(django_user);
   }
   
	return django_user == NULL ? MOSQ_ERR_AUTH : MOSQ_ERR_SUCCESS;
}


/*
 * Check ACL.
 * username is auth token.
 * topic is the topic user is trying to access (may contain
 * wildcards)
 * acc is desired type of access: read/write
 *	for subscriptions (READ) (1)
 *	for publish (WRITE) (2)
 *
 */
int mosquitto_auth_acl_check(void *userdata, const char *clientid, const char *username, const char *topic, int access)
{
	return MOSQ_ERR_SUCCESS;

}

int mosquitto_auth_psk_key_get(void *userdata, const char *hint, const char *identity, char *key, int max_key_len)
{
	return MOSQ_ERR_SUCCESS;
}
