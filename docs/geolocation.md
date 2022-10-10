## Geolocation

a geolocation service is available at:

http://localhost:8081/geoloc

you have to perform a POST request with two parameters :
type : the type of response, test, xml or json
name : the text of your search. for example :

`16, rue montselet, nantes`

test html page allows to try this feature.


### examples :
the first returned item, shoulb be the more interesting.

#### json

```  
> curl "localhost:8081/geoloc -X POST" -H "Content-Type: application/x-www-form-urlencoded" -d "mode=json&name=paris,+gare+de+lyon" 
[{"lon_min":2.371674, "lon_max":2.381072, "lat_min":48.841691, "lat_max":48.845739, "found":"Relation_497481:Gare de Lyon, Relation_111:Paris",   score":2027}
, {"lon_min":2.371674, "lon_max":2.381072, "lat_min":48.841691, "lat_max":48.845739, "found":"Relation_391354:Gare de Lyon, Relation_111:Paris", score":2027}
, {"lon_min":2.372468, "lon_max":2.375836, "lat_min":48.842822, "lat_max":48.844703, "found":"Relation_475970:Gare de Lyon, Relation_111:Paris", score":2027}
, {"lon_min":2.372317, "lon_max":2.375665, "lat_min":48.842680, "lat_max":48.844567, "found":"Relation_475971:Gare de Lyon, Relation_111:Paris", score":2027}
, {"lon_min":2.372897, "lon_max":2.374699, "lat_min":48.845699, "lat_max":48.846066, "found":"Relation_463535:Gare de Lyon, Relation_111:Paris", score":2027}
, {"lon_min":2.371674, "lon_max":2.381072, "lat_min":48.841691, "lat_max":48.845739, "found":"Way_58434912:Gare de Lyon, Relation_111:Paris", score":2026}
...
...
]
```
note that "pin" fields are available when a point in the returned area is the better point to show as result of query.

```
curl "localhost:8081/geoloc -X POST" -H "Content-Type: application/x-www-form-urlencoded" -d "mode=json&name=nantes"
[{"lon_min":-1.641812, "lon_max":-1.478841, "lat_min":47.180586, "pin_lon":-1.554136, "pin_lat":47.218637, "lat_max":47.295858, "found":"Relation_1563:Nantes", "score":526}
, {"lon_min":-1.939344, "lon_max":-1.114758, "lat_min":46.860337, "pin_lon":-1.554136, "pin_lat":47.218637, "lat_max":47.353268, "found":"Relation_148545:Nantes", "score":517}
, {"lon_min":2.319531, "lon_max":2.319596, "lat_min":48.865081, "lat_max":48.865125, "found":"Way_1702316:Nantes", "score":500}
, {"lon_min":-0.487529, "lon_max":-0.487529, "lat_min":46.340350, "lat_max":46.340350, "found":"Node_441224140:Nantes", "score":500}
, {"lon_min":-1.508979, "lon_max":-1.508979, "lat_min":47.257379, "lat_max":47.257379, "found":"Node_428306149:Nantes", "score":500}
, {"lon_min":-1.499387, "lon_max":-1.499387, "lat_min":47.260047, "lat_max":47.260047, "found":"Node_428208839:Nantes", "score":500}
, {"lon_min":-1.586484, "lon_max":-1.586484, "lat_min":47.237056, "lat_max":47.237056, "found":"Node_427857339:Nantes", "score":500}
, {"lon_min":-1.582847, "lon_max":-1.582847, "lat_min":47.243561, "lat_max":47.243561, "found":"Node_426028542:Nantes", "score":500}
, {"lon_min":2.586701, "lon_max":2.586701, "lat_min":48.961754, "lat_max":48.961754, "found":"Node_425901460:Nantes", "score":500}

...
...

]
```



### xml

```
curl "localhost:8081/geoloc -X POST" -H "Content-Type: application/x-www-form-urlencoded" -d "mode=xml&name=paris,+gare+de+lyon"
<root>
   <area lon_min="2.371674" lon_max="2.381072" lat_min="48.841691" lat_max="48.845739" found="Relation_497481:Gare de Lyon, Relation_111:Paris" score="2027" />
   <area lon_min="2.371674" lon_max="2.381072" lat_min="48.841691" lat_max="48.845739" found="Relation_391354:Gare de Lyon, Relation_111:Paris" score="2027" />
   <area lon_min="2.372468" lon_max="2.375836" lat_min="48.842822" lat_max="48.844703" found="Relation_475970:Gare de Lyon, Relation_111:Paris" score="2027" />
...
...
</root>
```
