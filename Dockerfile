FROM ubuntu

RUN apt-get update && \
    apt-get install \
    cmake \
    wget \
    g++ \
    gdb \
    netcat \
    git \
    curl \
    uuid-dev \
    libmysqlclient-dev \
    libcurl4-openssl-dev \
    librdkafka-dev \
    pkg-config \
    libglib2.0-dev \
    libpq-dev \
    libhiredis-dev \
    -y

WORKDIR /development

RUN git clone https://github.com/civetweb/civetweb.git && \
    mkdir -p civetweb/_build && \
    cd civetweb/_build && \
    cmake .. && \
    make install && \
    rm -rf /development/civetweb

RUN wget https://www.sqlite.org/2024/sqlite-autoconf-3450100.tar.gz && \
    tar -xvf sqlite-autoconf-3450100.tar.gz && \
    cd sqlite-autoconf-3450100 && \
    ./configure --prefix=/usr/local && \
    make install && \
    cd .. && \
    rm -rf sqlite-autoconf-3450100.tar.gz

RUN cd /development && \
    git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && \
    make install && \
    cd .. && \
    rm -rf paho.mqtt.c

RUN cd /development && \
    git clone https://github.com/NakedSolidSnake/sat.git && \
    mkdir -p sat/build && \
    cd sat/build && \
    cmake \
    -DSAT_WEBSERVER=ON \
    -DSAT_CURL=ON \
    -DSAT_PROPERTIES=ON \
    -DSAT_JSON=ON \
    -DSAT_KAFKA=ON \
    -DSAT_MQTT=ON \
    -DSAT_MYSQL=ON \
    -DSAT_POSTGRES=ON \
    -DSAT_REDIS=ON \
    -DSAT_SQLITE=ON \
    .. && \
    make install && \
    rm -rf /development/sat && \
    ldconfig

RUN useradd -ms /bin/bash cssouza && chown -R cssouza:cssouza /development

USER cssouza

EXPOSE 1234

ENTRYPOINT [ "/bin/tail", "-f", "/dev/null" ]