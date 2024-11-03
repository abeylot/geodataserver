# geodataserver
![sample](/docs/sample.jpg)


a light SVG tile map renderer and server that uses openstreetmap data and shp files.

## requirements
1. a debian based linux
2. at least 4gb memory
2. a fast 500gb ssd for whole world or some gb free space for smaller maps

## dependencies
1. ~~boost-threads~~
2. libzlib

## performances

### warning
High disk usage, using an usb key to store map datas may damage it !

### data extraction
can be awfull slow, up to one week on my old laptop for full world data processing, with a ssd external drive.

### map tile generation
Processor use is not the problem, file access is.
Fast for small scales ( high zoom levels ) but can take 10 or 20 seconds for large ones ( but those ones will be cached for next use)

## how to try the app

###  compile from source
#### first install some dependancies
    sudo apt update
    sudo apt upgrade
    sudo apt install g++ lbzip2 libbz2-dev git libpng++-dev
    
#### clone c++ code :
    cd $HOME
    git clone https://github.com/abeylot/geodataserver.git
    
#### build :
    cd $HOME/geodataserver
    make
    
#### download and compile map data :
( the example puts data dir in your home directory, but you can put it where you want, changing followings commands to follow your choice )

    mkdir data
    cp prepare.sh data
    cd data
    sh prepare.sh̀
    
You may change downloaded openstreetmap file in prepare.sh script.
example data file is pays-de-la-loire-latest.osm.bz2 ( part from france ) which will be fast processed.
#### Launch tile server :  
~~$HOME/geodataserver/geoserver $HOME/geodataserver/data~~

    $HOME/geodataserver/build/geoserver $HOME/geodataserver/data

server is listening on port 8081<br/>
test page is available at : http://localhost:8081
tiles are available at : http://localhost:8081/{z}/{x}/{y}.svg


### Try in a docker :
In case you have docker installed, and you know how to use it :
a Dockerfile is present in

    docker/sandbox

in the directory just perform those commands :

    docker build -t geoserver .  --network=host 
    docker run  --network host  geoserver

first run prepares data, so you'll have to wait for some time.

### [geolocation](geolocation.md)

### [configuration](configuration.md)

### [how it works](specifications.md)
