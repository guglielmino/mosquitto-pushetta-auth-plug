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
#include <openssl/evp.h>
#include <mosquitto.h>
#include <mosquitto_plugin.h>
#include <fnmatch.h>
#include <time.h>

#include "hash.h"

#include "ptta-mysql.h"
#include "userdata.h"


int pbkdf2_check(char *password, char *hash);

int mosquitto_auth_plugin_version(void)
{
	mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_plugin_version");

	return MOSQ_AUTH_PLUGIN_VERSION;
}

int mosquitto_auth_plugin_init(void **userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_plugin_init");
   
   *userdata = (struct userdata *)malloc(sizeof(struct userdata));
	if (*userdata == NULL) {
		perror("allocting userdata");
		return MOSQ_ERR_UNKNOWN;
	}

	memset(*userdata, 0, sizeof(struct userdata));
	struct userdata *ud = *userdata;
	ud->user_data_marker = strdup("pushetta_user_data");
	ud->validate_user = ptta_mysql_validate_user;
	
	
	mosquitto_log_printf(MOSQ_LOG_NOTICE, "** marker %s", ud->user_data_marker);
	
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_plugin_cleanup(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_plugin_cleanup");
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_init(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_security_init");

	struct mosquitto_auth_opt *o;
	int i;

	for (i = 0, o = auth_opts; i < auth_opt_count; i++, o++) {
		mosquitto_log_printf(MOSQ_LOG_NOTICE, "Options: key=%s, val=%s", o->key, o->value);
		p_add(o->key, o->value);
	}
	
	struct userdata *ud = (struct userdata *)userdata;
	ud->mysql_conf = ptta_mysql_init();
	mosquitto_log_printf(MOSQ_LOG_NOTICE, "** marker %s", ud->user_data_marker);
	
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_cleanup(void *userdata, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_security_cleanup");
   struct userdata *ud = (struct userdata *)userdata;
   if(ud->user_data_marker)
      free(ud->user_data_marker);
   free(ud);
	return MOSQ_ERR_SUCCESS;
}


int mosquitto_auth_unpwd_check(void *userdata, const char *username, const char *password)
{
   char *encrypted_password;
   int auth=0;
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_unpwd_check %s %s", username, password);
   struct userdata *ud = (struct userdata *)userdata;
   
   encrypted_password = (char *)ud->validate_user(ud->mysql_conf, username, password, &auth);
   
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_unpwd_check ENC %s", encrypted_password);
   
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_acl_check(void *userdata, const char *clientid, const char *username, const char *topic, int access)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_acl_check");
	return MOSQ_ERR_SUCCESS;

}

int mosquitto_auth_psk_key_get(void *userdata, const char *hint, const char *identity, char *key, int max_key_len)
{
   mosquitto_log_printf(MOSQ_LOG_NOTICE, "*** mosquitto_auth_psk_key_get");
	return MOSQ_ERR_SUCCESS;
}
