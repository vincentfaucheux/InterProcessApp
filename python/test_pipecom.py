import sys
import time

sys.path.append('../build/debug/python')  # Chemin vers le module compilé

from pipecom import EndPointCom

bWriteCreated = False
bDataReceived = False

def on_pipe_ready():
    print("WRITE pipe créé")
    global bWriteCreated
    bWriteCreated = True

def on_data():
    print("Data reçue")
    global bDataReceived
    bDataReceived = True
    global ep
    data = ep.read()
    print(f"Data: {data}")

ep = EndPointCom(
    "/home/vincent/InterProcess/PipeComApp/build/debug/PipeComSo/libPipeComSo.so",
    "/home/vincent/InterProcess/Pipes/Client2Serv",
    "/home/vincent/InterProcess/Pipes/Client2Serv",
    on_pipe_ready
)

if( ep != None ):
    while not bWriteCreated:
        ep.pool_write_created()
        time.sleep(0.01)

    ep.set_callback(on_data)

    ep.write(bytes([1, 0, 5]) + b"hello")

    while not bDataReceived:
        ep.pool_data_received()
        time.sleep(0.01)

    del ep
    print("Fini")
    
else:
    print("Erreur de création de l'EndPointCom")