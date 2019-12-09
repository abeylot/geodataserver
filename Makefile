#cc=g++ -s -O2 -Wall -std=c++11 -D_FILE_OFFSET_BITS=64
cc=g++ -g -Wall -std=c++11 -D_FILE_OFFSET_BITS=64


headersCommon=CompiledDataManager.hpp \
common/constants.hpp \
common/GeoTypes.hpp \
Coordinates.hpp \
helpers/Sequence.hpp \
helpers/Rectangle.hpp \
helpers/config.hpp \
helpers/FileIndex.hpp \
helpers/hash.hpp \
GeoBox.hpp

headersServer = services/Ping.hpp \
services/RelationList.hpp \
services/RelationDetail.hpp \
services/WayDetail.hpp \
services/IdxList.hpp \
services/IdxDetail.hpp \
services/ServicesFactory.hpp \
messaging/HttpProtocol.hpp \
messaging/HttpEncoder.hpp \
services/renderers/SvgRenderer.hpp \
services/Tile.hpp \
services/Svg.hpp \
helpers/TcpConnection.hpp \
helpers/TcpListener.hpp


objectsCommon = GeoBox.o \
hash.o \
Coordinates.o \
Rectangle.o \
CompiledDataManager.o

objectsServer = Ping.o \
RelationList.o \
RelationDetail.o \
WayDetail.o \
IdxList.o \
IdxDetail.o \
HttpProtocol.o \
HttpEncoder.o \
ServicesFactory.o \
Svg.o \
Tile.o \
SvgRenderer.o \
Msg.o \
TcpListener.o \
TcpConnection.o

libs=-lpthread  -lboost_system -lboost_thread -lboost_atomic -lz -latomic

all: renumber compile index geoserver

renumber: renumber.cpp $(headersCommon) Coordinates.o
	$(cc) renumber.cpp Coordinates.o -o renumber

index: index.cpp $(headersCommon) Coordinates.o CompiledDataManager.o Rectangle.o GeoBox.o
	$(cc) index.cpp Coordinates.o CompiledDataManager.o Rectangle.o GeoBox.o -o index

Coordinates.o: Coordinates.cpp Coordinates.hpp
	$(cc) -c Coordinates.cpp -o Coordinates.o
	
compile: Coordinates.o compile.cpp $(headersCommon)	
	$(cc) compile.cpp Coordinates.o -o compile

CompiledDataManager.o: CompiledDataManager.hpp CompiledDataManager.cpp
	$(cc) -c CompiledDataManager.cpp -o CompiledDataManager.o

hash.o: helpers/hash.hpp helpers/hash.cpp
	$(cc) -c helpers/hash.cpp -o hash.o

TcpConnection.o: helpers/TcpConnection.hpp helpers/TcpConnection.cpp
	$(cc) -c helpers/TcpConnection.cpp -o TcpConnection.o


TcpListener.o: helpers/TcpListener.hpp helpers/TcpListener.cpp
	$(cc) -c helpers/TcpListener.cpp -o TcpListener.o


Rectangle.o: helpers/Rectangle.hpp helpers/Rectangle.cpp
	$(cc) -c helpers/Rectangle.cpp -o Rectangle.o

GeoBox.o: GeoBox.hpp GeoBox.cpp
	$(cc) -c GeoBox.cpp -o GeoBox.o
	
Ping.o: services/Ping.cpp services/Ping.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/Ping.cpp -o Ping.o

IdxList.o: services/IdxList.cpp services/IdxList.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/IdxList.cpp -o IdxList.o

IdxDetail.o: services/IdxDetail.cpp services/IdxDetail.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/IdxDetail.cpp -o IdxDetail.o

RelationList.o: services/RelationList.cpp services/RelationList.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/RelationList.cpp -o RelationList.o

RelationDetail.o: services/RelationDetail.cpp services/RelationDetail.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/RelationDetail.cpp -o RelationDetail.o

WayDetail.o: services/WayDetail.cpp services/WayDetail.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/WayDetail.cpp -o WayDetail.o

ServicesFactory.o: services/ServicesFactory.cpp services/ServicesFactory.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/ServicesFactory.cpp -o ServicesFactory.o
	
HttpProtocol.o: messaging/HttpProtocol.cpp messaging/HttpProtocol.hpp $(headersCommon) $(headersServer)
	$(cc) -c messaging/HttpProtocol.cpp   -o HttpProtocol.o

Msg.o: messaging/Msg.cpp messaging/Msg.hpp
	$(cc) -c messaging/Msg.cpp            -o Msg.o

HttpEncoder.o: messaging/HttpEncoder.cpp messaging/HttpEncoder.hpp $(headersCommon) $(headersServer)
	$(cc) -c messaging/HttpEncoder.cpp    -o HttpEncoder.o

Svg.o: services/Svg.cpp services/Svg.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/Svg.cpp             -o Svg.o

Tile.o: services/Tile.cpp services/Tile.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/Tile.cpp             -o Tile.o


SvgRenderer.o: services/renderers/SvgRenderer.cpp services/renderers/SvgRenderer.hpp $(headersCommon) $(headersServer)
	$(cc) -c services/renderers/SvgRenderer.cpp   -o SvgRenderer.o	

geoserver: server.cpp $(headersCommon) $(headersServer) $(objectsCommon) $(objectsServer)
	$(cc) server.cpp $(objectsCommon) $(objectsServer) -o geoserver $(libs)	

clean:
		rm -f *.o geoserver renumber normalize index compile geoserver
