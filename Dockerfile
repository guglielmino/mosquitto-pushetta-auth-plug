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

RUN make
RUN make install

COPY docker-entrypoint.sh /
ENTRYPOINT ["/docker-entrypoint.sh"]

CMD ["/usr/sbin/mosquitto", "-c", "/mosquitto/config/mosquitto.conf"]
