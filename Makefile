#CC=clang++

GIT_VERSION = "$(shell git describe --tags)"

CC= g++
cc=$(CC)  -O2 -g -Wall -Wextra -pedantic-errors -std=c++17 -D_FILE_OFFSET_BITS=64 -DVERSION=\"$(GIT_VERSION)\"
BUILD=build

#cc=$(CC) -g -Wall -std=c++17 -D_FILE_OFFSET_BITS=64 -DVERSION=\"$(GIT_VERSION)\"

#headers with no matching .cpp
headersCommon=common/constants.hpp \
common/GeoTypes.hpp \
helpers/Sequence.hpp \
helpers/config.hpp \
helpers/FileIndex.hpp \
helpers/ExtThread.hpp \
helpers/NonGrowableQueue.hpp\
services/ServiceInterface.hpp

#objects used by all executables
objectsCommon = $(BUILD)/GeoBox.o \
$(BUILD)/helpers/hash.o \
$(BUILD)/Coordinates.o \
$(BUILD)/helpers/Rectangle.o \
$(BUILD)/CompiledDataManager.o

#objects used by server.cpp only
objectsServer = $(BUILD)/services/Ping.o \
$(BUILD)/services/Geolocation.o \
$(BUILD)/services/RelationList.o \
$(BUILD)/services/RelationDetail.o \
$(BUILD)/services/WayDetail.o \
$(BUILD)/services/IdxList.o \
$(BUILD)/services/IdxDetail.o \
$(BUILD)/messaging/HttpProtocol.o \
$(BUILD)/messaging/HttpEncoder.o \
$(BUILD)/services/ServicesFactory.o \
$(BUILD)/services/Svg.o \
$(BUILD)/services/Tile.o \
$(BUILD)/services/renderers/SvgRenderer.o \
$(BUILD)/messaging/Msg.o \
$(BUILD)/helpers/TcpListener.o \
$(BUILD)/helpers/TcpConnection.o \
$(BUILD)/services/MapDisplay.o \
$(BUILD)/helpers/StringBuffer.o

#libs=-lpthread  -lz -latomic -lstdc++fs -fsanitize=address -static-libsan
libs=-lpthread  -lz -latomic -lstdc++fs

all: $(BUILD) $(BUILD)/renumber $(BUILD)/compile $(BUILD)/index $(BUILD)/geoserver

$(objectsCommon): $(BUILD)/%.o: %.cpp %.hpp $(headersCommon)
	$(cc) -c $<  -o $@

$(objectsServer): $(BUILD)/%.o: %.cpp %.hpp $(headersCommon)
	$(cc) -c $<  -o $@


$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/helpers
	mkdir -p $(BUILD)/services
	mkdir -p $(BUILD)/services/renderers
	mkdir -p $(BUILD)/messaging

debug: cc=$(CC) -g -Wall -std=c++17 -D_FILE_OFFSET_BITS=64 -DVERSION=\"$(GIT_VERSION)\"
debug: $(BUILD) $(BUILD)/renumber $(BUILD)/compile $(BUILD)/index $(BUILD)/geoserver


$(BUILD)/renumber: renumber.cpp $(headersCommon) $(BUILD)/Coordinates.o
	$(cc) renumber.cpp $(BUILD)/Coordinates.o -o $(BUILD)/renumber

$(BUILD)/index: index.cpp $(headersCommon) $(BUILD)/Coordinates.o $(BUILD)/CompiledDataManager.o $(BUILD)/helpers/Rectangle.o $(BUILD)/GeoBox.o
	$(cc) index.cpp $(BUILD)/Coordinates.o $(BUILD)/CompiledDataManager.o $(BUILD)/helpers/Rectangle.o $(BUILD)/GeoBox.o -o $(BUILD)/index

$(BUILD)/compile: $(BUILD)/Coordinates.o compile.cpp $(headersCommon)
	$(cc) compile.cpp $(BUILD)/Coordinates.o -o $(BUILD)/compile

$(BUILD)/geoserver: server.cpp $(headersCommon) $(objectsCommon) $(objectsServer)
	$(cc) server.cpp $(objectsCommon) $(objectsServer) -o $(BUILD)/geoserver $(libs)

clean:
		rm -rf build debug
