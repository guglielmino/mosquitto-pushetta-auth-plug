# Load our module (and misc) configuration from config.mk
# It also contains MOSQUITTO_SRC
include config.mk

prefix=/usr/local
OBJS = pushetta-auth-plug.o base64.o pbkdf2-check.o hash.o ptta-mysql.o ptta-topic-namehandler.o

OSSLINC = -I$(OPENSSLDIR)/include
OSSLIBS = -L$(OPENSSLDIR)/lib -lcrypto

CFLAGS = -I$(MOSQUITTO_SRC)/src/
CFLAGS += -I$(MOSQUITTO_SRC)/lib/
ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC -Wall -Werror
endif
CFLAGS += -I$(MOSQ)/src -DDEBUG=1 $(OSSLINC) `mysql_config --cflags`
LDFLAGS = -L$(MOSQUITTO_SRC)/lib/
LDFLAGS += -lcares
LDADD =  $(OSSLIBS) -lmosquitto `mysql_config --libs`

all: printconfig pushetta-auth-plug.so np

printconfig:
	@echo "Using mosquitto source dir: $(MOSQUITTO_SRC)"
	@echo "OpenSSL install dir:        $(OPENSSLDIR)"
	@echo
	@echo

pushetta-auth-plug.so : $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -shared -o $@ $(OBJS) $(LDADD)

pushetta-auth-plug.o: pushetta-auth-plug.c Makefile
pbkdf2-check.o: pbkdf2-check.c base64.h Makefile
base64.o: base64.c base64.h Makefile
hash.o: hash.c hash.h uthash.h Makefile
ptta-mysql.o: ptta-mysql.c Makefile
ptta-topic-namehandler.o: ptta-topic-namehandler.c Makefile

np: np.c base64.o
	$(CC) $(CFLAGS) $^ -o $@ $(OSSLIBS)

$(CDBLIB):
	(cd $(CDBDIR); make libcdb.a cdb )

pwdb.cdb: pwdb.in
	$(CDB) -c -m  pwdb.cdb pwdb.in

clean:
	rm -f *.o *.so np

install: pushetta-auth-plug.so
	install -m 0755 pushetta-auth-plug.so $(prefix)/lib

config.mk:
	@echo "Please create your own config.mk file"
	@echo "You can use config.mk.in as base"
	@false
