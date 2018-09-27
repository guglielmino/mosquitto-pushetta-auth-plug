#ifndef __PUSHETTA_AUTH_PLUG_H
#define __PUSHETTA_AUTH_PLUG_H

#include <openssl/evp.h>
#include <mosquitto.h>
#include <mosquitto_plugin.h>

#define PLUGIN_VERSION "1.0.9"

#define LOG(level, message, args...) mosquitto_log_printf(level, message, ##args)

#endif