Map configuration is done through config.xml file
# generic configuration
## parameter tag

port on which the tile server listens

    `<parameter name="ServerPort"    value="8081"/>`

number of threads that write tiles to clients

    `<parameter name="WriterThreads" value="25"/>`

number of threads which compute tiles

    `<parameter name="ExecThreads"   value="5"/>`

all tiles with data and zoom level lesser or equal than 11 wwill be cached (you should delete them from cache directory if you change configuration )

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
## filling patterns
*SVG* patterns for areas filling can be defined so ( refer to *SVG* documentation for details ).
    <pattern id="mudPattern" width="16" height="16" patternUnits="userSpaceOnUse" >
             <rect x="0" y="0" width="16" height="16" fill="#AAFFAA" style="opacity:0.2"/>
             <circle cx="8" cy="8" r="2" stroke-width="0" fill="skyblue" />
    </pattern>
## symbols
*SVG* symbols for points display can be defined so ( refer to *SVG* documentation for details ).

    <symbol id="parking" width="16" height="16" transform="translate(-8 -8)" >
        <circle  cx="8"  cy="8"  r="8"  style="fill:#000099;opacity:0.2;" />
        <text x="8" y="8" style="fill:white;opacity:0.9;font-family:sans-serif;font-weight:bold;font-size:13px">P</text>
    </symbol>
## styles
*SVG* styles can defined so ( refer to *SVG* documentation for details). Note that this example refers to mudpattern example pattern.

     <style id="mud" value="fill:url(#mudPattern); fill-rule:evenodd"/>
## indexes
openstreetmap items will be indexed and displayed according to indexes descriptions like example below :

    <index type="node" name="placesIdxLowRes">
        <restriction zLevels="3,4">
            <condition tagKey="place" closed="both">
                <class value="country"        zIndex="5002"    textZIndex="31" textStyle="#countryName"/>
            </condition>
        </restriction>
        <restriction zLevels="5,6,7">
            <condition tagKey="place" closed="both">
                <class value="country"        zIndex="5002"    textZIndex="31" textStyle="#countryName"/>
                <class value="state"          zIndex="5001"    textZIndex="30" textStyle="#stateName"/>
            </condition>
        </restriction>
        <restriction zLevels="8">
            <condition tagKey="place" closed="both">
                <class value="country"          zIndex="5010"    textZIndex="60" textStyle="#countryName"/>
                <class value="state"            zIndex="5009"    textZIndex="50" textStyle="#stateName"/>
                <class value="province"         zIndex="5008"    textZIndex="40" textStyle="#provinceName"/>
                <class value="district"         zIndex="5007"    textZIndex="30" textStyle="#districtName"/>
                <class value="county"           zIndex="5006"    textZIndex="20" textStyle="#countyName"/>
            </condition>
        </restriction>
    </index>
### index tag
#### type attribute
#### name attribute
### select tag
    <index type="node" name="capitalsNodeIdx">
        <select tagKey="place" tagValue="city"/>
        <select tagKey="place" tagValue="town"/>
        ...
    </index>

Openstreetmap nodes who don't have tag place with values city or town will be ignored.
#### tagKey attribute
the openstreetmap tag
#### tagValue attribute
the openstreetmap value ( * can be specified for all values )

### exclude tag

    <index type="relation" name="relationHeritageIdx">
        <exclude tagKey="location" tagValue="underground"/>
        ...
    </index>

Openstreetmap items with tag location having value underground will be ignored.
#### tagKey attribute
the openstreetmap tag
#### tagValue attribute
the openstreetmap value ( * can be specified for all values )


### restriction tag
#### zLevels attribute
### condition tag
#### tagKey attribute
#### closed attribute
### class tag
#### value attribute
#### zIndex attribute
#### textZIndex attribute
#### style attribute
#### textStyle attribute

