#ifndef __WIFI_LOC_H__
#define __WIFI_LOC_H__

#define GOOGLE_GEOLOC_HOST "www.googleapis.com"
#define GOOGLE_GEOLOC_PATH "/geolocation/v1/geolocate"
#define GOOGLE_GEOLOC_PORT 443
// #define GOOGLE_GEOLOC_FINGERPRINT "C5 46 0D 6F A8 DA 4E 91 F0 D9 CC D6 C9 8B 3A 65 C4 2A E8 EC"
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

typedef struct
{
    float lat, lng, accuracy = -1.0;
} location_t;

/**
 * If returned accuracy is negative, it means an error has occured.
 */
location_t get_wifi_location(const char *googleApiKey)
{
    location_t result;

    int numOfNetworks = WiFi.scanNetworks(false, true);
    String req_data = "{\"wifiAccessPoints\":[";
    for (int i = 0; i < numOfNetworks; i++)
    {
        uint8_t *mac = WiFi.BSSID(i);
        char buffer[80];
        sprintf(buffer,
                "{\"macAddress\":\"%02x:%02x:%02x:%02x:%02x:%02x\","
                "\"channel\":%d,\"signalStrength\":%d}%s",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
                WiFi.channel(i), WiFi.RSSI(i),
                i == numOfNetworks - 1 ? "]}\n" : ",");
        req_data += buffer;
    }

    WiFiClientSecure client;
    if (client.connect(GOOGLE_GEOLOC_HOST, GOOGLE_GEOLOC_PORT))
    {
        client.printf(
            "POST %s?key=%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n%s",
            GOOGLE_GEOLOC_PATH, googleApiKey, GOOGLE_GEOLOC_HOST, req_data.length(), req_data.c_str());
        client.flush();

        if (client.find("\r\n\r\n")) // Skip headers
        {
            char buf[256];
            int len = 0;
            while (client.connected() && len < 256)
            {
                len += client.readBytes(buf + len, 256 - len);
            }

            // Basic response parsing
            const char delim[] = "\r\n\t:[]{}\", ";
            float *target = NULL;
            for (char *tok = strtok(buf, delim); tok; tok = strtok(NULL, delim))
            {
                if (target)
                {
                    *target = atof(tok);
                    target = NULL;
                }
                else
                {
                    target = NULL;
                    if (!strcmp(tok, "lat"))
                        target = &(result.lat);
                    if (!strcmp(tok, "lng"))
                        target = &(result.lng);
                    if (!strcmp(tok, "accuracy"))
                        target = &(result.accuracy);
                }
            }
        }
    }
    return result;
}

#endif // __WIFI_LOC_H__