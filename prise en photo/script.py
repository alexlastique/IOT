import paho.mqtt.client as mqtt
import sqlite3
from datetime import datetime
import ast
import subprocess  # Pour lancer la commande caméra
import os

# --- Configuration MQTT ---
BROKER = "localhost"
PORT = 1883
TOPIC = "donne/topic"

# --- Base de données SQLite ---
conn = sqlite3.connect("capteurs.db")
cursor = conn.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS mesures (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    Dist INTEGER,
    Temp REAL,
    Hum INTEGER,
    Son INTEGER,
    Lum INTEGER,
    timestamp TEXT
)
""")
conn.commit()

# --- Dernière distance pour comparaison ---
last_distance = None

# --- Callback MQTT ---
def on_message(client, userdata, msg):
    # get last_distance from BDD
    cursor.execute("SELECT Dist FROM mesures ORDER BY id DESC LIMIT 1")
    result = cursor.fetchone()
    global last_distance
    if result:
        last_distance = result[0]
    try:
        payload = msg.payload.decode()
        print("Message reçu:", payload)

        data = ast.literal_eval(payload)

        Dist = data.get("Dist")
        Temp = data.get("Temp")
        Hum  = data.get("Hum")
        Son  = data.get("Son")
        Lum  = data.get("Lum")
        timestamp = datetime.now().isoformat()

        # Enregistrement en BDD
        cursor.execute("""
            INSERT INTO mesures (Dist, Temp, Hum, Son, Lum, timestamp)
            VALUES (?, ?, ?, ?, ?, ?)
        """, (Dist, Temp, Hum, Son, Lum, timestamp))
        conn.commit()
        print("Données enregistrées en BDD")

        # --- Vérifier si la distance a changé ---
        if last_distance is None or Dist != last_distance:
            last_distance = Dist
            # Nom de fichier basé sur la date et l'heure
            file_name = datetime.now().strftime("%Y%m%d%H%M%S.jpg")
            file_path = os.path.join("/home/iotg2/api-capteurs/photos", file_name)  # dossier à créer si il ecxiste pas
            os.makedirs(os.path.dirname(file_path), exist_ok=True)

            # Capture photo avec rpicam-still
            subprocess.run(["rpicam-still", "-o", file_path])
            print(f"Photo prise: {file_path}")

    except Exception as e:
        print("Erreur:", e)


# --- Connexion MQTT ---
client = mqtt.Client()
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC)

print("En attente des messages MQTT...")
client.loop_forever()
