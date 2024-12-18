#This script shall be run in data directory
set -x
echo "set environment variable to geodataserver binaries path"
export GEOBIN=/$HOME/geodataserver/build

echo "set environment variable to openstreet map file to download"
export OSMFILE=download.geofabrik.de/europe/france/pays-de-la-loire-latest.osm.bz2
#below, entire world, test with a smaller file first
#planet.openstreetmap.org/planet/planet-latest.osm.bz2


# install dependancies



echo "copy config.xml file to data path"
cp $GEOBIN/../config.xml .

echo "get coastline data, to be able to draw seas."
wget osmdata.openstreetmap.de/download/water-polygons-split-4326.zip
if [ $? -ne 0 ]
  then
  echo " failed to download water polygons"
  exit 1
fi
unzip water-polygons-split-4326.zip -d .
#rm water-polygons-split-4326.zip

echo "get countries boundaries from  natural earth"
wget https://naciscdn.org/naturalearth/10m/cultural/ne_10m_admin_0_boundary_lines_land.zip
if [ $? -ne 0 ]
  then
  echo " failed to download boundaries from natural earth"
else
  mkdir ne_10m_admin_0_boundary_lines_land
  unzip ne_10m_admin_0_boundary_lines_land.zip -d ne_10m_admin_0_boundary_lines_land
  rm ne_10m_admin_0_boundary_lines_land.zip
fi
echo "get lakes from natural earth"
wget https://naciscdn.org/naturalearth/10m/physical/ne_10m_lakes.zip
if [ $? -ne 0 ]
  then
  echo " failed to download lakes from natural earth"
else
  mkdir ne_10m_lakes
  unzip ne_10m_lakes.zip -d ne_10m_lakes
  rm  ne_10m_lakes.zip
fi
#wget www.naturalearthdata.com/http//www.naturalearthdata.com/download/10m/physical/ne_10m_lakes.zip

echo "extract data from openstreetmap file"
wget -O - $OSMFILE | tee osm.bz2 | lbzcat | $GEOBIN/renumber .
if [ $? -ne 0 ]
  then
  echo " first data extraction step failed"
  exit 1
fi

echo "compile data from openstreetmap file and other shp files from natural earth"
lbzcat osm.bz2 | $GEOBIN/compile .
if [ $? -ne 0 ]
  then
  echo " failed to compile data"
  exit 1
fi
rm osm.bz2

echo "remove indexes that are no longer useful"
rm nodeIdIndex
rm nodeIdIndex_key
rm nodeRefIndex_key
rm wayIdIndex
rm wayIdIndex_key
rm relationIdIndex
rm relationIdIndex_key

echo "create indexes"
$GEOBIN/index .
if [ $? -ne 0 ]
  then
  echo " index creation failed"
  exit 1
fi

echo "make cache directory"
mkdir cache

echo "remove shp files that are no nonger needed"
rm -rf ne_10m_admin_0_boundary_lines_land
rm -rf ne_10m_lakes
rm -rf water-polygons-split-4326

# launch tile server
# $GEOBIN/geoserver .
