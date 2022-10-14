FROM debian:11-slim
WORKDIR /geoserver
#COPY data /geoserver/data
RUN apt update
RUN apt upgrade
RUN apt install -yq g++ lbzip2 libboost-all-dev libbz2-dev git wget make unzip bash
RUN rm -rf /var/lib/apt/lists/*
RUN git clone https://github.com/abeylot/geodataserver.git
RUN mkdir data
RUN cp geodataserver/config.xml data
RUN cp geodataserver/prepare_docker.sh data
EXPOSE 8081
WORKDIR geodataserver
RUN make
WORKDIR ../data
RUN chmod 777 prepare_docker.sh
CMD ["/bin/bash","prepare_docker.sh"]  
