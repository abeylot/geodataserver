Map configuration is done through config.xml file
# generic configuration
## parameter tag

port on which the tile server listens

    `<parameter name="ServerPort"    value="8081"/>`

number of threads that write tile to clients

    `<parameter name="WriterThreads" value="25"/>`

number of threads which compute tiles

    `<parameter name="ExecThreads"   value="5"/>`

all tiles with data and zoom level lsser or equal than 11 wwill be cached (you should delete them from cace directory if you change configuration )

    `<parameter name="CacheLevel"    value="11"/>`

default language for labels : french if exist or english if exists, or default one, in this order.

    `<parameter name="locale"        value="fr;en"/>`
    
default color of land    

    `<parameter name="DefaultColor"  value="#EEFFEE"/>`

# input data configuration    
## shp_file tag
the shp file water-polygons-split-4326/water_polygons will be loaded with shape tag having value ocean

    <shp_file name="water-polygons-split-4326/water_polygons" tag="shape" value="ocean"/>

# styling configuration
to do
