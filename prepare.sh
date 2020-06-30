export GEOBIN=/home/expand/geodataserver
#wget planet.openstreetmap.org/planet/planet-latest.osm.bz2
wget download.geofabrik.de/europe/france/pays-de-la-loire-latest.osm.bz2
mv pays-de-la-loire-latest.osm.bz2 osm.bz2

wget osmdata.openstreetmap.de/download/water-polygons-split-4326.zip
unzip water-polygons-split-4326.zip -d .
rm water-polygons-split-4326.zip

wget www.naturalearthdata.com/http//www.naturalearthdata.com/download/10m/cultural/ne_10m_admin_0_boundary_lines_land.zip
mkdir ne_10m_admin_0_boundary_lines_land
unzip ne_10m_admin_0_boundary_lines_land.zip -d ne_10m_admin_0_boundary_lines_land
rm ne_10m_admin_0_boundary_lines_land.zip

wget www.naturalearthdata.com/http//www.naturalearthdata.com/download/10m/physical/ne_10m_lakes.zip
mkdir ne_10m_lakes
unzip ne_10m_lakes.zip -d ne_10m_lakes
rm  ne_10m_lakes.zip

lbzcat osm.bz2 | $GEOBIN/renumber
lbzcat osm.bz2 | $GEOBIN/compile .
rm osm.bz2

rm nodeIdIndex
rm wayIdIndex
rm relationIdIndex

$GEOBIN/index .

mkdir cache
rm -rf ne_110m_admin_0_boundary_lines_land
rm -rf ne_10m_lakes
rm -rf water-polygons-split-4326

$GEOBIN/geoserver .