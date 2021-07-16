Map configuration is done through config.xml file
# generic configuration
## *parameter* tag

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
## *shp_file* tag
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
*Openstreetmap* items will be indexed and displayed according to indexes descriptions like example below :

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
### *index* tag
the index tag causes an index file to be created when indexing *Openstreetmap* data, and related *SVG* styles
#### *type* attribute
possible values are

node : indexing of nodes

way : indexing of ways

relation : indexing or relations

(nodes, ways and relations are related to *Openstreetmap* data definitions )
#### *name* attribute
the name of the index ( must be unique )
### *select* tag

    <index type="node" name="capitalsNodeIdx">
        <select tagKey="place" tagValue="city"/>
        <select tagKey="place" tagValue="town"/>
        ...
    </index>
*Openstreetmap* nodes who don't have tag place with values city or town will be ignored.
#### *tagKey* attribute
the *Openstreetmap* tag
#### *tagValue* attribute
the *Openstreetmap* value ( "default" can be specified for all values )
### *exclude* tag

    <index type="relation" name="relationHeritageIdx">
        <exclude tagKey="location" tagValue="underground"/>
        ...
    </index>
*Openstreetmap* items with tag location having value underground will be ignored.
#### *tagKey* attribute
the *Openstreetmap* tag
#### *tagValue* attribute
the *Openstreetmap* value ( "default" can be specified for all values )


### *restriction* tag
    <index type="node" name="placesIdxLowRes">
        <restriction zLevels="3,4">
            <condition tagKey="place" closed="both">
                <class value="country"        zIndex="5002"    textZIndex="31" textStyle="#countryName"/>
            </condition>
        </restriction>
        ...
    </index>

restrict the content to some zoom levels
#### *zLevels* attribute
the list of correspondig zoom levels
### *condition* tag
    <index type="node" name="placesIdxLowRes">
        <restriction zLevels="3,4">
            <condition tagKey="place" closed="both">
                <class value="country"        zIndex="5002"    textZIndex="31" textStyle="#countryName"/>
            </condition>
        </restriction>
        ...
    </index>

inner class descriptions are related to openstreetmap tag *place*
#### *tagKey* attribute
an *Openstreetmap* tag
#### *closed* attribute
values are yes no and both ( will select closed shapes not closed shapes, or both ) this is meaningless for nodes.
### *class* tag
    <index type="node" name="placesIdxLowRes">
        <restriction zLevels="3,4">
            <condition tagKey="place" closed="both">
                <class value="country"        zIndex="5002"    textZIndex="31" textStyle="#countryName"/>
            </condition>
        </restriction>
        ...
    </index>
*Openstreetmap* nodes with tag *place* having value *country* will be drawn with style which id is *countryName*. 
#### *value* attribute
the *Openstreetmap* tag value, or "default" for all remaining values
#### *zIndex* attribute
a number, the higher, the most on top the item will be drawn.
#### *textZIndex* attribute
a number, the higher, the most on top the items name will be drawn.
#### *style* attribute
the *SVG* style, can refer to a previous style id if starts with #, or a plain *SVG* style.
#### *textStyle* attribute
the *SVG* text style, can refer to a previous style id if starts with #, or a plain *SVG* style.
if unspecified, no text is displayed.
### *textField* attribute
if specified, the openstreetmap tag used for item label, if unspecified, name is used.
