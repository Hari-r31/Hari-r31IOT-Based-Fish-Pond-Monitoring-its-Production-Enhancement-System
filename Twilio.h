#ifndef TWILIO_H
#define TWILIO_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "base64.h" // Make sure you have this library installed

class Twilio {
public:
    static bool send_message_with_service(const char *account_sid, const char *auth_token, const char *messaging_service_sid, const char *to_number, const char *message, String &response);
};

#endif // TWILIO_H
