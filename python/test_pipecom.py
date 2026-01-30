import sys
import time
from pathlib import Path
import yaml
import struct
import threading

#
# Charger le fichier de configuration
config_path = Path(__file__).resolve().parent / "configuration.yaml"
if( config_path.exists() == False ):
    print("Fichier de configuration introuvable")
    sys.exit( -1 )

with open(config_path, "r") as f:
    config = yaml.safe_load(f)

print(config)

#
#charger la librairie interprocess
#
if "interprocess_client_lib" not in config:
    print("Clé interprocess_client_lib introuvable dans le fichier de configuration")
    sys.exit( -1 )

sys.path.append(config["interprocess_client_lib"])  # Chemin vers le module compilé

from pipecom import EndPointCom

#
# init variables
bWriteCreated = False
bDataReceived = False
bQuit = False

#
# Fonction d'envoi de requête, réception réponse (dans le thread)
#
def send_request( ep, cmd, str_data):
    global bDataReceived
    # les données à envoyer
    data_bytes = str_data.encode('utf-8')
    # longueur des données
    length = len(data_bytes)
    # construire le buffer à envoyer
    buffer = struct.pack(">B H", cmd, length) + data_bytes
    #
    # Envoyer des données
    #
    ep.write( buffer)

    #
    # Attendre la réception des données
    #
    while not bDataReceived:
        ep.pool_data_received()
        time.sleep(0.01)
    bDataReceived = False

#
# Thread d'envoi de requête, reception réponse
#
def send_request_thread(ep, cmd, str_data):
    thread = threading.Thread(target=send_request, args=(ep, cmd, str_data))
    thread.start()
    return thread  # tu peux le join plus tard si besoin

#
# Callback pipe write created
#
def on_pipe_ready():
    global bWriteCreated
    print("WRITE pipe créé")
    bWriteCreated = True

#
# Callback data received
#
def on_data():
    global bDataReceived
    global ep
    print("Data reçue")
    bDataReceived = True
    data = ep.read()
    print(f"Data: {data}")

#
# Créer l'EndPointCom
#
if (("app2serv_pipe_name" not in config) or
    ("serv2app_pipe_name" not in config) or
    ("interprocess_pipes_lib" not in config)):
    print("Clé(s) manquante(s) dans le fichier de configuration")
    sys.exit( -1 )

ep = EndPointCom(
    config["interprocess_pipes_lib"],
    config["app2serv_pipe_name"],
    config["app2serv_pipe_name"],
    on_pipe_ready
)

if( ep != None ):
    #
    # Attendre la création du pipe write
    #
    while not bWriteCreated:
        ep.pool_write_created()
        time.sleep(0.01)

    #
    # Définir le callback data received
    #
    ep.set_callback(on_data)

    #
    # boucle pour envoi de données
    #
    while( bQuit == False ):
        str_input = input("Appuyer sur 'q' pour quitter ou entrer pour envoyer des données: ")
        if( str_input == "q" ):
            bQuit = True
        else:
            # thread = send_request_thread(ep, 1, str_input)
            thread = send_request_thread(ep, 1, str_input)
            # attendre la fin du thread avant de continuer
            thread.join()

    #
    # Tout fermer   
    #
    del ep
    print("Fini")
    
else:
    print("Erreur de création de l'EndPointCom")