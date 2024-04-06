from http.server import BaseHTTPRequestHandler, HTTPServer
import json
from bleak import BleakClient

# Define the HTTP request handler
class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        data = json.loads(post_data)

        # Extract numPax and hexValue from the received data
        numPax = data.get('numPax')
        hexValue = data.get('hexValue')

        # Establish BLE connection with ESP32 device
        async def send_data_to_esp32():
            async with BleakClient("ESP32_MAC_ADDRESS") as client:
                # Send numPax and hexValue data to ESP32
                await client.write_gatt_char("CHARACTERISTIC_UUID", bytearray([numPax, hexValue]))

        # Run the async function to send data to ESP32
        asyncio.run(send_data_to_esp32())

        # Send response back to the JavaScript code
        self.send_response(200)
        self.end_headers()
        self.wfile.write(json.dumps({'status': 'success'}).encode())

# Define the HTTP server
def run(server_class=HTTPServer, handler_class=RequestHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting server on port {port}...')
    httpd.serve_forever()

# Run the HTTP server
if __name__ == "__main__":
    run()


# import asyncio
# from bleak import BleakScanner

# # UUID of the iBeacon
# UUID = "87b99b2c-90fd-11e9-bc42-526af7764f64"

# # Function to pack numPax and hexValue into iBeacon data
# def pack_data(numPax, hexValue):
#     numPax_bytes = bytes([numPax])  # Assuming numPax is a single byte (0 to 255)
#     hexValue_bytes = bytes.fromhex(hexValue)  # Convert hex string to bytes
#     return numPax_bytes + hexValue_bytes

# # Callback function to simulate advertisement
# def advertisement_callback(device, advertisement_data):
#     print(f"Advertising numPax: {advertisement_data.service_data.get(UUID)[0]} hexValue: {advertisement_data.service_data.get(UUID)[1:].hex()}")

# # Function to simulate BLE advertisement
# async def simulate_advertisement(numPax, hexValue):
#     try:
#         while True:
#             beacon_data = pack_data(numPax, hexValue)
#             await asyncio.sleep(1)  # Advertisement interval
#     except KeyboardInterrupt:
#         pass

# if __name__ == "__main__":
#     numPax = 1  # Example numPax value
#     hexValue = "d699ee"  # Example hexValue

#     loop = asyncio.get_event_loop()
#     loop.create_task(simulate_advertisement(numPax, hexValue))  # Start the advertisement task

#     scanner = BleakScanner()
#     scanner.register_detection_callback(advertisement_callback)  # Register the callback for advertisement simulation
#     loop.run_until_complete(scanner.start())
