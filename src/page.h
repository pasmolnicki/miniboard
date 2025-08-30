#pragma once

constexpr const char* PAGE_HTML = R"_(
    <!DOCTYPE html>
    <html>
        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <title>ESP32 Bluetooth Keyboard</title>
            <style>
                body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
                h1 { color: #333; }
                p { font-size: 18px; color: #666; }
                .btn {
                    background-color: #4CAF50;
                    color: white;
                    padding: 12px 20px;
                    border: none;
                    border-radius: 4px;
                    cursor: pointer;
                }
            </style>

            <script>
                let ledState = 'OFF';
                function switchLed() {
                    fetch('/switch_led', { method: 'POST' })
                    .then(response => {
                        if (!response.ok) {
                            throw new Error('Network response was not ok');
                        }
                        return response.json();
                    })
                    .then(data => {
                        ledState = data.led_state;
                        document.getElementById('ledState').innerText = ledState.toUpperCase();
                    });
                }
            </script>
        </head>
        <body>
            <h1>Miniboard setup</h1>
            <p>Press the button to switch led.</p>
            <button class="btn" id="switchLedButton" onclick="switchLed()">Switch LED</button>
            <p>State: <span id="ledState">OFF</span></p>
        </body>
    </html>
)_";