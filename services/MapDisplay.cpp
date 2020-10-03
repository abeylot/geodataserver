#include "MapDisplay.hpp"

MapDisplay::MapDisplay()
{
}

MapDisplay::~MapDisplay()
{
}

Msg* MapDisplay::processRequest(Msg* request, CompiledDataManager& mger)
{
    std::string latitude = "47.218371";
    std::string longitude = "-1.553621";
    std::string zoom = "12";
    
    std::string resp =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
        "<title>map test</title>"
        "<link rel=\"stylesheet\" href=\"http://cdn.leafletjs.com/leaflet-0.7.3/leaflet.css\" />"
        "<script type=\"text/javascript\" src=\"http://cdn.leafletjs.com/leaflet-0.7.3/leaflet.js\"></script>"
        "<style>"
        "html, body { margin: 50px 0; padding: 0; }"
        "#map {"
        "    height: 600px; width: 80%; margin: 0 auto;"
        "    -webkit-box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    -webkit-border-radius: 5px;"
        "    border-radius: 5px;"
        "}"
        "</style>"
        "</head>"
        "<body>" 
        "<div id=\"map\"></div>"
        "<script type=\"text/javascript\">"
        "var map = L.map('map', {zoomControl: false}).setView([#lat#, #lon#], #zoom#);"
        "L.tileLayer('/{z}/{x}/{y}.svg', {"
        "    attribution: 'Map data: &copy; <a href=\"http://www.openstreetmap.org/copyright\">OpenStreetMap</a>)',"
        "    maxZoom: 19"
        "}).addTo(map);"
        "</script>"
        "</body>" 
        "</html>";
    
    
    if (request->getRecord(2)->getNamedValue("latitude") != "")
    {
        latitude = request->getRecord(2)->getNamedValue("latitude");
    }
    else if(request->getRecord(1)->getNamedValue("latitude") != "")
    {
        latitude = request->getRecord(1)->getNamedValue("latitude");
    }
    
    if (request->getRecord(2)->getNamedValue("longitude") != "")
    {
        longitude = request->getRecord(2)->getNamedValue("longitude");
    }
    else if(request->getRecord(1)->getNamedValue("longitude") != "")
    {
        longitude = request->getRecord(1)->getNamedValue("longitude");
    }
    
    if (request->getRecord(2)->getNamedValue("zoom") != "")
    {
        zoom = request->getRecord(2)->getNamedValue("zoom");
    }
    else if(request->getRecord(1)->getNamedValue("zoom") != "")
    {
        zoom = request->getRecord(1)->getNamedValue("zoom");
    }
    
    resp.replace(resp.find("#lon#"),5,longitude);
    resp.replace(resp.find("#lat#"),5,latitude);
    resp.replace(resp.find("#zoom#"),6,zoom);
    
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    encoder.addContent(rep,resp);
    return rep;
}

