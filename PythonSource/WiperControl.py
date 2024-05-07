from ioModule import ioModule

from collections import deque
from time import sleep


class WiperControl:
    """Known in the documentation as Text Processing"""

    def __init__(self):
        self._pipeline = deque(maxlen=10)  # Pipeline will contain Task Instances
        self._io_module = ioModule(self._pipeline)

    def run(self):
        """Main program loop"""
        print(f"Running, {self}")
        self._do_startup()
        self._run_app_loop()

    def _do_startup(self):
        """Call the Methods in other modules needed as a setup; Eg: Iniciar conexiones, cargar configuraciones, etc"""
        print("The startup should run")
        self._io_module.do_startup()
        print("Startup done \n\n")

    def _run_app_loop(self):
        """The main loop consists of awaiting a message, and when a message is received,
        handling it appropiately"""
        dot_counter = 0
        while True:
            # If the pipeline is empty, wait for a request to come
            if len(self._pipeline) == 0:
                print("Waiting for a request." + "." * ((dot_counter := dot_counter + 1) % 4))
                sleep(1)
                continue

            # If the pipeline is not empty, pop the first task and process it
            print("--TASK RECEIVED--")
            datapack = self._pipeline.pop()
            self.process(datapack)  # Procesing done in the Task module
            print("\n")

    def process(self, datapack):
        """Expects a dictionary as its argument"""
        self._io_module.send_to_thinkspeaks(datapack)
