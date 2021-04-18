# Wifi-iNaturalist-Display
Link an Arduino Uno Wifi Rev 2 with the iNaturalist.org API to stay updated about the species being observed in your area.

## Stay updated about the nature being observed near you 
iNaturalist is an online application in which users share observations of biodiversity in their local area. The application has over 3 million users and has been used to observe more than 320,000 species to date. 

Using iNaturalist is a fantastic way to learn about nature, get involved with citizen science and keep track of the dynamic biodiversity in your area. The best way I have found to use iNaturalist is with the smart phone app which is available on android or apple. You can make observations with your phone, suggest an identification and have it confirmed or corrected by an active and helpful online community.

I often use the application to check what species have been observed near me most recently. There are many reasons this information is interesting. For example, there may have been a sighting of a rare species that is making a return to my local area. 

A general ethos in my life is to avoid using a smart phone where I can, and so I wanted to make a display that gives me this information without requiring me to check my phone or laptop. Hence the Arduino Wifi iNaturalist display was created. 
How the display works 

On powerup, the Arduino connects to the world time API to get the current time and date. After this the Arduino continues to keep time using the onboard clock. The Arduino then connects to the iNaturalist API and requests data for the three most recent 'research grade' observations in your area. Your area is defined by a set radius (default 20 km) around user supplied latitude and longitude decimal coordinates. The common name, scientific name, place and time of the observations are displayed on the TFT display. Every ten minutes the Arduino re-requests this information and updates the display. 

## Assembling the display 
The display requires an Arduino Uno Wifi Rev 2 and a TFT display Arduino Uno shield. Mount the shield on your Arduino, then download the code from the GitHub repository. In the 'arduino_secrets.h' tab define your WiFI SSID, password and your latitude and longitude coordinates. You will need to install the following libraries which are available from the library manager in the Arduino IDE. 
* WifiNiNA - make HTTP requests.
* Adafruit_GFX - draw graphics on the TFT display.
* ArduinoJson - deserialize json output.
* MCUFRIEND_kbv - hardware specific library to communicate with TFT shield.
Finally upload the code and the display should turn on. It will take ~ 1 minute for the time and species observations in your area to be displayed. If they aren't, use the Serial monitor to see what error message is being given. If the time is incorrectly given as midnight then the world time API has failed to respond. Reset the Arduino and try again. 

## Acknowledgements
This project was only possible due to the fantastic documentation and examples of two libraries and the iNaturalist API.
1. Arduino Json. This library is used throughout the code to deserialize the json output from the iNaturalist API following an HTTP request. The library allows you to stream the output directly into a json document. There is also this great tool which computes the memory requirements of a json output.
2. Wifi NiNa. This library is used to connect to the iNaturalist server and make HTTP requests. 
3. iNaturalist API. This API is used to communicate with iNaturalist.org.
In this program I make two kinds of GET request. 
GET #1 : worldtimeapi.org/api/ip - to retrieve the current time based on IP address location. 
GET #2: api.inaturalist.org/v1/observations - to retrieve the most recent species observations in the users area. 

## Future Work
If you decide to build the display, I hope you find it useful. There are a few features I haven't been able to include in version 1 of the display that I would like to incorporate in a future version. 
* eBird API support. This would allow the display to show recent bird species observed in your area. The display could switch between eBird and iNaturalist information. I am waiting for confirmation of an API key that would allow me to enable this feature. 
* IUCN Red List API support. This would allow the display to show the conservation status of the iNaturalist/eBird observations. Great for identifying rare species in your area. I am also waiting for confirmation of an API key that would allow me to include this feature. 
If you have any further suggestions let me know in the comments. 
