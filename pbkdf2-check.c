#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include "base64.h"

#define KEY_LENGTH      24
#define SEPARATOR       "$"
#define TRUE	(1)
#define FALSE	(0)


/*
 * Split PBKDF2$... string into their components. The caller must free()
 * the strings.
 */

static int detoken(char *pbkstr, char **sha, int *iter, char **salt, char **key)
{
	char *p, *s, *save;
	int rc = 1;

	save = s = strdup(pbkstr);

	if ((p = strsep(&s, SEPARATOR)) == NULL)
		goto out;
	if (strcmp(p, "PBKDF2") != 0)
		goto out;

	if ((p = strsep(&s, SEPARATOR)) == NULL)
		goto out;
	*sha = strdup(p);

	if ((p = strsep(&s, SEPARATOR)) == NULL)
		goto out;
	*iter = atoi(p);

	if ((p = strsep(&s, SEPARATOR)) == NULL)
		goto out;
	*salt = strdup(p);

	if ((p = strsep(&s, SEPARATOR)) == NULL)
		goto out;
	*key = strdup(p);

	rc = 0;

     out:
	free(save);
	return rc;
}

int pbkdf2_check(char *password, char *hash)
{
        static char *sha, *salt, *h_pw;
        int iterations, saltlen, blen;
	char *b64;
	unsigned char key[128];
	int match = FALSE;
	const EVP_MD *evpmd;

        if (detoken(hash, &sha, &iterations, &salt, &h_pw) != 0)
		return match;

#ifdef PWDEBUG
	fprintf(stderr, "sha        =[%s]\n", sha);
	fprintf(stderr, "iterations =%d\n", iterations);
	fprintf(stderr, "salt       =[%s]\n", salt);
	fprintf(stderr, "h_pw       =[%s]\n", h_pw);
#endif

	saltlen = strlen((char *)salt);

	evpmd = EVP_sha256();
	if (strcmp(sha, "sha1") == 0) {
		evpmd = EVP_sha1();
	} else if (strcmp(sha, "sha512") == 0) {
		evpmd = EVP_sha512();
	}

	PKCS5_PBKDF2_HMAC(password, strlen(password),
                (unsigned char *)salt, saltlen,
		iterations,
		evpmd, KEY_LENGTH, key);

	blen = base64_encode(key, KEY_LENGTH, &b64);
	if (blen > 0) {
		int i, diff = 0, hlen = strlen(h_pw);
#ifdef PWDEBUG
		fprintf(stderr, "HMAC b64   =[%s]\n", b64);
#endif
		
		/* "manual" strcmp() to ensure constant time */
		for (i = 0; (i < blen) && (i < hlen); i++) {
			diff |= h_pw[i] ^ b64[i];
		}

		match = diff == 0;
		if (hlen != blen)
			match = 0;

		free(b64);
	}

	free(sha);
	free(salt);
	free(h_pw);

	return match;
}

#if TEST
int main()
{
        // char password[] = "hello";
	// char PB1[] = "PBKDF2$sha256$10000$eytf9sEo8EprP9P3$2eO6tROHiqI3bm+gg+vpmWooWMpz1zji";
        char password[] = "supersecret";
	char PB1[] = "PBKDF2$sha256$10000$YEbSTt8FaMRDq/ib$Kt97+sMCYg00mqMOBAYinqZlnxX8HqHk";
	// char PB1[] = "PBKDF2$sha1$10000$XWfyPLeC9gsD6SbI$HOnjU4Ux7RpeBHdqYxpIGH1R5qCCtNA1";
	// char PB1[] = "PBKDF2$sha512$10000$v/aaCgBZ+VZN5L8n$BpgjSTyb4weVxr9cA2mvQ+jaCyaAPeYe";
	int match;

	printf("Checking password [%s] for %s\n", password, PB1);

	match = pbkdf2_check(password, PB1);
	printf("match == %d\n", match);
	return match;
}
#endif