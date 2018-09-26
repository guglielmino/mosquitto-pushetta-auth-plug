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

COPY . .

RUN make




