#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo;
const int servoPin = 13;

const char* ssid = "WeaponControl";
const char* password = "topsecret";
WiFiServer server(80);

String header;
String servoValue = "5";
int pos1, pos2;

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    header = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>.slider{width:300px;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
            client.println("</head><body>");
            client.println("<h1>ESP32 Servo Control</h1>");
            client.println("<p>Position: <span id=\"servoPos\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" value=\"" + servoValue + "\">");
            client.println("<script>");
            client.println("$('#servoSlider').on('input', function() {");
            client.println("  var val = $(this).val();");
            client.println("  $('#servoPos').text(val);");
            client.println("  $.get('/?value=' + val);");
            client.println("});");
            client.println("$('#servoPos').text($('#servoSlider').val());");
            client.println("</script>");
            client.println("</body></html>");

            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              servoValue = header.substring(pos1 + 1, pos2);
              myservo.write(servoValue.toInt());
              Serial.println("Servo: " + servoValue);
            }
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
  }
}