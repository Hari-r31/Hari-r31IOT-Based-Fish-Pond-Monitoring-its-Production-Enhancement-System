#include "twilio.h"

bool Twilio::send_message_with_service(const char *account_sid, const char *auth_token, const char *messaging_service_sid, const char *to_number, const char *message, String &response) {
    // Craft the Twilio API URL
    String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(account_sid) + "/Messages.json";

    // Craft the message body
    String body = "To=" + String(to_number) + "&MessagingServiceSid=" + String(messaging_service_sid) + "&Body=" + String(message);

    // Send the HTTP POST request to Twilio
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Basic " + base64::encode(String(account_sid) + ":" + String(auth_token)));
    int httpResponseCode = http.POST(body);

    // Check the HTTP response code
    response = http.getString(); // Get the response body
    http.end(); // Close connection
  
    return httpResponseCode == HTTP_CODE_CREATED; // 201 indicates success
}
