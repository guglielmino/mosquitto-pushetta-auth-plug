#ifndef __API_CLIENT_H
#define __API_CLIENT_H


int valitate_token(const char* token);
int check_acl(const char*token, const char* channel_name);

#endif