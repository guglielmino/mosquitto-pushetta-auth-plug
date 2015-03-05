#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "base64.h"

#define KEY_LENGTH      32
#define SEPARATOR       "$"
#define SALTLEN 12

#define SALTBASE64       "5w8X87cjDvQM"

#define USAGE() fprintf(stderr, "Usage: %s [-i iterations] [-p password]\n", progname)

int main(int argc, char **argv)
{
        int iterations = 12000, saltlen, blen;
	char *salt, *b64;
	unsigned char key[128];
	char *pw1, *pw2, *password;
	char *progname = argv[0];
	int c;
	int prompt;

	prompt = 1;

	while ((c = getopt(argc, argv, "ip:")) != EOF) {
		switch (c) {
			case 'i':
				iterations = atoi(optarg);
				break;
			case 'p':
				pw1 = strdup(optarg);
				pw2 = strdup(optarg);	
				prompt = 0;
				break;
			default:
				exit(USAGE());
		}
	}

	argc -= optind - 1;
	argv += optind - 1;

	if (argc != 1) {
		exit(USAGE());
	}

	if ( prompt ) {
		pw1 = strdup(getpass("Enter password: "));
		pw2 = getpass("Re-enter same password: ");
	}

	if (strcmp(pw1, pw2) != 0) {
		fprintf(stderr, "Passwords don't match!\n");
		return (1);
	}

	password = pw1;

	//base64_encode(saltbytes, SALTLEN, &salt);
        salt = (char*)malloc(SALTLEN);
        strncpy(salt, SALTBASE64, SALTLEN);
        printf("Salt=%s\n", salt);
	saltlen = strlen(salt);

	PKCS5_PBKDF2_HMAC(password, strlen(password),
                (unsigned char *)salt, saltlen,
		iterations,
		EVP_sha256(), KEY_LENGTH, key);

	blen = base64_encode(key, KEY_LENGTH, &b64);
	if (blen > 0) {
		printf("pbkdf2_%s$%d$%s$%s\n",
			"sha256",
			iterations,
			salt,
			b64);
		
		free(b64);
	}

	free(password);
	return 0;
}
