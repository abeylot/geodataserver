#include "MapDisplay.hpp"

MapDisplay::MapDisplay()
{
}

MapDisplay::~MapDisplay()
{
}

Msg* MapDisplay::processRequest(Msg* request, [[maybe_unused]] CompiledDataManager& mger)
{
    bool pin = false;
    std::string longitude = "-1.554136";
    std::string latitude = "47.218637";
    std::string zoom = "12";
    std::string zofs="0";
    std::string tsz="256";

    std::string resp =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
        "<title>map test</title>"
        "<link rel=\"stylesheet\" href=\"https://unpkg.com/leaflet@1.9.4/dist/leaflet.css\" />"
        "<script type=\"text/javascript\" src=\"https://unpkg.com/leaflet@1.9.4/dist/leaflet.js\"></script>"
        "<style>"
        "html, body { margin: 1% 0; padding: 0;height: 99%;background-color: #B2B3C8 }"
        "#map {"
        "    height: 80%; width: 80%; margin: 0 auto;"
        "    -webkit-box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    -webkit-border-radius: 1%;"
        "    border-radius: 1%;"
        "}"
        "#form {"
        "    height: 2em; width: 80%; margin: 0 auto;"
        "    -webkit-box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    box-shadow: 0 2px 3px 3px rgba(0, 0, 0, 0.2);"
        "    -webkit-border-radius: 5px;"
        "    border-radius: 5px;"
        "}"
        "</style>"
        "</head>"
        "<body>"

        "<div id=\"form\">"
        "<form method=\"post\" action=\"/geoloc\" accept-charset=\"utf-8\">"
        "<label>Locate :"
        "<input name=\"name\" >"
        "</label>"
        "<select id=\"mode\" name=\"mode\">"
        "<option value=\"test\" selected >test</option>"
        "<option value=\"json\">json</option>"
        "<option value=\"xml\">xml</option>"
        "</select>"
        "<select id=\"mag\" name=\"mag\">"
        "<option value=\"X05\" #mag05#>&#x1F50D; X0.5</option>"
        "<option value=\"X1\" #mag1#>&#x1F50D; X1</option>"
        "<option value=\"X2\" #mag2#>&#x1F50D; X2</option>"
        "</select>"
        "<button><b>Send</b></button>"
        "</form></div>"


        "<div id=\"map\"></div>"

        "<script type=\"text/javascript\">"
        "var map = L.map('map', {zoomControl: false}).setView([#lat#, #lon#], #zoom#);"
        "L.tileLayer('/{z}/{x}/{y}.svg', {"
        "    attribution: 'Map data: &copy; <a href=\"http://www.openstreetmap.org/copyright\">OpenStreetMap</a>)',"
        "    maxZoom: 19, tileSize:#tsz#, zoomOffset:#zofs#"
        "}).addTo(map);#pin#"
        "</script>"
        "</body>"
        "</html>";


    if (request->getRecord(2)->getNamedValue("lattitude") != "")
    {
        latitude = request->getRecord(2)->getNamedValue("lattitude");
    }
    else if(request->getRecord(1)->getNamedValue("lattitude") != "")
    {
        latitude = request->getRecord(1)->getNamedValue("lattitude");
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

    if (request->getRecord(2)->getNamedValue("pin") != "")
    {
        pin = true;
    }

    if (request->getRecord(2)->getNamedValue("mag") == "X2")
    {
        tsz = "512" ;
        zofs = "-1";
        resp.replace(resp.find("#mag1#"),6,"");
        resp.replace(resp.find("#mag05#"),7,"");
        resp.replace(resp.find("#mag2#"),6,"selected");
    } else if (request->getRecord(2)->getNamedValue("mag") == "X05")
    {
        tsz = "128" ;
        zofs = "1";
        resp.replace(resp.find("#mag1#"),6,"");
        resp.replace(resp.find("#mag05#"),7,"selected");
        resp.replace(resp.find("#mag2#"),6,"");
    } else {
        resp.replace(resp.find("#mag1#"),6,"selected");
        resp.replace(resp.find("#mag05#"),7,"");
        resp.replace(resp.find("#mag2#"),6,"");
    }


    resp.replace(resp.find("#lon#"),5,longitude);
    resp.replace(resp.find("#lat#"),5,latitude);
    resp.replace(resp.find("#zoom#"),6,zoom);
    resp.replace(resp.find("#tsz#"),5,tsz);
    resp.replace(resp.find("#zofs#"),6,zofs);



    if(pin)
    {
        resp.replace(resp.find("#pin#"),5,"var marker = L.marker(["+latitude+","+longitude+"]).addTo(map);");
    } else {
        resp.replace(resp.find("#pin#"),5,"");
    }
    Msg* rep = new Msg;
    encoder.build200Header(rep, "text/html");
    encoder.addContent(rep,resp);
    return rep;
}

