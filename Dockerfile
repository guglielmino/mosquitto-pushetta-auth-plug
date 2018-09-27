FROM eclipse-mosquitto:1.4.12


RUN set -e; \
    apk add --no-cache --update alpine-sdk \
    libc-dev \
    linux-headers \
    musl-dev \
    mariadb-dev \
    mosquitto-dev \
    libxml2-dev \
    c-ares-dev

WORKDIR /usr/src/app

COPY ./src .

COPY ./mosquitto-pushetta-auth.conf /etc/mosquitto/mosquitto.conf 
# Workaround to fix the mysql lib compiled to use the Unix socket (unable to overwrite by config)
RUN mkdir -p /run/mysqld/
RUN touch /run/mysqld/mysqld.sock

RUN make
RUN make install

COPY docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]

CMD ["/usr/sbin/mosquitto", "-c", "/etc/mosquitto/mosquitto.conf"]
