from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/")
def receive_data():
    print(f'Current temp: {request.args.get("temp")}')
    print(f'Current humidity: {request.args.get("hum")}')
    return "Current temperature is "+str(request.args.get("temp") + "\n" + 
                                         "Current humidity is "+str(request.args.get("hum"))
