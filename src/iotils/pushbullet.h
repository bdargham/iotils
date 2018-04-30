#ifndef __PUSH_BULLET_H__
#define __PUSH_BULLET_H__

#define PUSHBULLET_HOST "api.pushbullet.com"
#define PUSHBULLET_PATH "/v2/pushes"
#define PUSHBULLET_PORT 443
#define PUSHBULLET_FINGERPRINT "BB FC 9F 1B C1 3C D9 96 F2 68 A2 E3 41 29 D1 47 8F B9 33 BE"

#include <WiFiClientSecure.h>
#include "utils.h"

typedef struct {
    const char *apikey;
    const char *channel_tag;
} pushbullet_config_t;

/**
 * It is assumed that the title and body strings are JSON-escaped already
 */
int pushbullet_send_notif(const pushbullet_config_t config, const char *title, const char *body)
{
    WiFiClientSecure client;
    if (client.connect(PUSHBULLET_HOST, PUSHBULLET_PORT))
    {
        // bool secure = client.verify(PUSHBULLET_FINGERPRINT, PUSHBULLET_HOST);

        String json = "{\"type\":\"note\"";
        if (title) {
            json += ",\"title\":\"" + escape_json(title) + "\"";
        }
        if (body) {
            json += ",\"body\":\"" + escape_json(body) + "\"";
        }
        if (config.channel_tag) {
            json += ",\"channel_tag\":\"" + escape_json(config.channel_tag) + "\"";   
        }
        json += "}";

        client.printf(
            "POST %s HTTP/1.1\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "Host: %s\r\n"
            "Content-Length: %d\r\n"
            "Access-Token: %s\r\n\r\n%s",
            PUSHBULLET_PATH, PUSHBULLET_HOST, json.length(), config.apikey, json.c_str());

        while (client.connected())
        {
            client.read();
            // TODO parse return to determine HTTP response code
        }
        return 0;
    }
    return -1;
}

#endif // __PUSH_BULLET_H__