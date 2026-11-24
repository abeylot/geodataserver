FROM debian:trixie-slim
WORKDIR /geoserver
#COPY data /geoserver/data
RUN apt-get update && apt-get upgrade -y \
 && apt-get install -y --no-install-recommends \
    g++ make git wget unzip bash ca-certificates pkg-config \
    lbzip2 osmium-tool libbz2-dev libz-dev libpng++-dev librsvg2-dev libcairo2-dev \
 && rm -rf /var/lib/apt/lists/*
# temporary: build the feature branch
RUN git clone --branch feature/png https://github.com/abeylot/geodataserver.git
RUN mkdir data
RUN cp geodataserver/config.xml data
RUN cp geodataserver/prepare_docker.sh data
EXPOSE 8081
WORKDIR geodataserver
RUN make
WORKDIR ../data
RUN chmod 777 prepare_docker.sh
CMD ["/bin/bash","prepare_docker.sh"]
