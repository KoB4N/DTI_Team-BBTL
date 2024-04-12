from flask import Flask, request, jsonify
from flask_cors import CORS
import bluetooth
import time

app = Flask(__name__)
CORS(app)  # This will enable CORS for all routes and domains
# Define your beacon's UUID and service name
uuid = "00001101-0000-1000-8000-00805F9B34FB"  # Example UUID, you can use your own
service_name = "ESP32_Beacon"  # Example service name, you can use your own

@app.route('/receive_data', methods=['POST'])
def receive_data():
    if request.method == 'POST':
        data = request.json
        num_pax = data.get('numPax')
        hex_value = data.get('hexValue')
        output_string = f"{hex_value} {num_pax}"
        
        print(output_string)  # This will print "Hex value Number of passengers"

        return jsonify({'message': 'Data received successfully'})
    else:
        return jsonify({'message': 'Method not allowed'}), 405

if __name__ == '__main__':
    app.run(debug=False)
