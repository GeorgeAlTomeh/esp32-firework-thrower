#include <WiFi.h>
#include <ESP32Servo.h>
#include <Stepper.h>

// Servo
Servo myservo;
const int servoPin = 13;

// Stepper (28BYJ-48 with ULN2003)
#define STEPS_PER_REV 2048
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 15
Stepper stepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// WiFi credentials – change these
const char* ssid = "WeaponControl";
const char* password = "topsecret";
WiFiServer server(80);

String header;
String servoValue = "5";
int pos1, pos2;

int stepperStepsToMove = 0;
int stepperSpeed = 30;          // RPM
const int stepperMaxSpeed = 40; // 28BYJ-48 practical max

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);
  stepper.setSpeed(stepperSpeed);

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
  // Handle queued stepper movement (one step per loop)
  if (stepperStepsToMove != 0) {
    stepper.step(stepperStepsToMove > 0 ? 1 : -1);
    stepperStepsToMove += (stepperStepsToMove > 0 ? -1 : 1);
  }

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

            // HTML page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>.slider{width:300px;} button{margin:5px;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
            client.println("</head><body><h1>ESP32 Motor Control</h1>");

            // Servo panel
            client.println("<div><h2>Servo</h2>");
            client.println("<p>Position: <span id=\"servoPos\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" value=\"" + servoValue + "\">");
            client.println("</div>");

            // Stepper panel
            client.println("<div><h2>Stepper</h2>");
            client.println("<button onclick=\"stepperMove(-512)\">-1/4 Turn</button>");
            client.println("<button onclick=\"stepperMove(-128)\">-1/16 Turn</button>");
            client.println("<button onclick=\"stepperMove(128)\">+1/16 Turn</button>");
            client.println("<button onclick=\"stepperMove(512)\">+1/4 Turn</button><br>");
            client.println("<button onclick=\"stepperMove(-2048)\">-Full Turn</button>");
            client.println("<button onclick=\"stepperMove(2048)\">+Full Turn</button><br>");
            client.println("<button onclick=\"stepperStop()\">STOP</button><br><br>");
            client.println("<p>Speed: <span id=\"stepperSpeed\">" + String(stepperSpeed) + "</span> RPM</p>");
            client.println("<input type=\"range\" min=\"1\" max=\"" + String(stepperMaxSpeed) + "\" class=\"slider\" id=\"speedSlider\" value=\"" + String(stepperSpeed) + "\">");
            client.println("</div>");

            // JavaScript
            client.println("<script>");
            client.println("$('#servoSlider').on('input', function() {");
            client.println("  var val = $(this).val();");
            client.println("  $('#servoPos').text(val);");
            client.println("  $.get('/?value=' + val);");
            client.println("});");
            client.println("$('#servoPos').text($('#servoSlider').val());");
            client.println("function stepperMove(steps) { $.get('/?stepperMove=' + steps); }");
            client.println("function stepperStop() { $.get('/?stepperStop=1'); }");
            client.println("function stepperSetSpeed(speed) {");
            client.println("  $('#stepperSpeed').text(speed);");
            client.println("  $.get('/?stepperSpeed=' + speed);");
            client.println("}");
            client.println("$('#speedSlider').on('input', function() { stepperSetSpeed($(this).val()); });");
            client.println("</script></body></html>");

            // Parse GET requests
            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              servoValue = header.substring(pos1 + 1, pos2);
              myservo.write(servoValue.toInt());
              Serial.print("Servo: "); Serial.println(servoValue);
            }
            else if (header.indexOf("GET /?stepperMove=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              stepperStepsToMove = header.substring(pos1 + 1, pos2).toInt();
              Serial.print("Stepper move: "); Serial.println(stepperStepsToMove);
            }
            else if (header.indexOf("GET /?stepperStop=1") >= 0) {
              stepperStepsToMove = 0;
              Serial.println("Stepper stopped");
            }
            else if (header.indexOf("GET /?stepperSpeed=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              stepperSpeed = header.substring(pos1 + 1, pos2).toInt();
              stepper.setSpeed(stepperSpeed);
              Serial.print("Stepper speed: "); Serial.println(stepperSpeed);
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