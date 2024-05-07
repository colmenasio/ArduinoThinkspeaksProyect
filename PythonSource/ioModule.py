import collections
import requests
import serial
import threading


class ioModule:
    """Handles the connection with Arduino and Thingspeaks"""

    def __init__(self, pipeline: collections.deque):
        self._pipeline = pipeline
        self._threads = {}

        self._ts_api_key = None # Insert you Api Key here
        self._ts_url = 'https://api.thingspeak.com/update'

    def do_startup(self):
        print("Starting up IOModule")
        # Start up the connections, etc
        self._do_startup_arduino()
        self._do_startup_thinkspeaks()

        # Set up the threads
        self._threads["_arduino_listener"] = threading.Thread(target=self._arduino_listener)
        # self._threads["_thingspeaks_listener"] = threading.Thread(target=self._thingspeaks_listener)

        # Start the threads
        for thread in self._threads.values():
            thread.start()

    def send_to_thinkspeaks(self, data):
        """Sends data to the thingspeaks web."""

        # Datos a enviar
        data_local = data.copy()
        data_local["api_key"] = self._ts_api_key

        # Enviar los datos a ThingSpeak
        response = requests.post(self._ts_url, data=data)

        # Comprobar si la solicitud fue exitosa
        if response.status_code == 200:
            print("Datos enviados correctamente a ThingSpeak.")
        else:
            print("Error al enviar datos a ThingSpeak. Código de estado:", response.status_code)

    def _arduino_listener(self):
        """Function that will interface with the arduino board via USB connection
        When a signal comes, wrap the data in a Task instance and add it to the pipeline"""
        BAUDRATE = 9600
        with serial.Serial('COM3', BAUDRATE) as ser:
            while True:
                message = ser.readline().decode().rstrip()
                descriptors = ["Temperature", "Humidity", "Luminosity"]
                values = message.split("/")
                data = {}
                for descriptor, value in zip(descriptors, values):
                    data[descriptor] = int(value)
                self._pipeline.append(data)

    def _do_startup_arduino(self):
        """Start connection with arduino"""
        pass

    def _do_startup_thinkspeaks(self):
        """Start connection with thinkspeaks"""
        pass
