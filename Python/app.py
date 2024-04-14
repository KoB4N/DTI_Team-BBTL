from flask import Flask, request, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # This will enable CORS for all routes and domains

@app.route('/receive_data', methods=['POST', 'OPTIONS'])
def receive_data():
    if request.method == 'OPTIONS':
        # Handle OPTIONS request
        response = jsonify({'message': 'CORS preflight request received'})
        response.headers.add('Access-Control-Allow-Origin', '*')
        response.headers.add('Access-Control-Allow-Headers', 'Content-Type')
        response.headers.add('Access-Control-Allow-Methods', 'POST')
        return response

    if request.method == 'POST':
        data = request.json
        hexVal_dCount = data.get('hexVal_dCount')
        output_string = f"{hexVal_dCount}"
        
        print(output_string)  # This will print "Hex value Number of passengers"

        return jsonify({'message': 'Data received successfully'})
    else:
        return jsonify({'message': 'Method not allowed'}), 405

if __name__ == '__main__':
    app.run(debug=False)
