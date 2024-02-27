Groupe de travail SRT2024 : **Silvère CHEVRIER, Daniel MICHON, Giovanni NAULEAU & Antonin SARRAZIN**


# Tp_IoT_2022_LoRa_Bluetooth
Prérequis
**Partage de connexion iPhone** : activer l'option "*maximiser la compatibilité*" pour passer sur la bande des 2,4GHz, seule compatible avec la carte


## 1. Receiver :
### 1. Récupération d'un paquet Mqtt sur test.mosquitto.org :  srt/nom-groupe
### 2. Se connecter à une carte sur l'AP et faire une requete http donnant les valeurs nécessaire à LoRa.
### 3. Ecouter les données de LoRa.
Le code commenté est contenu dans les fichiers suivants :
**receiver.ino**
**sender_receiver.ino**
**Receiver.cpp**
Ces fichiers ne sont pas (ou peu) versionné : Un nouveau fichier à été créé pour chaque étape.

## 2. Sender :
### Envoyer les données de connexion LoRa sur test.mosquitto.org au topic srt/nom-groupe.
Nous exploitons le dépôt public Mosquito pour y déposer la configuration d'échange LoRa pour notre groupe sous forme de tableau :
**Groupe srt5/midi2
Fréquence 867 MHz
Spread Factor 9
BandeWith 125**
Ces données seront récupérées dans des variables afin de les exploiter en configuration LoRa

Le code commenté est contenu dans le fichier suivant :
**recupérationParamLoRa.ino**



## 3. Ensuite...
### 1. Ajouter une fonction permettant de switcher entre Sender et Receiver
*À partir du délai de connexion de mosquitto, nous devons établir la connexion directe de pair à pair entre les périphériques LoRa des deux groupes.
Nous devons ensuite échanger des informations directement entre les deux groupes.*

Le code commenté est contenu dans les fichiers suivants :
**RECEIVERLoRa.ino**
**DEV_LoRaSENDER.ino**


### 2. Sur le sender, échangez la partie LoRa par du bluetooth (ou BLE)
*Une fois, c'est la formation reçue, elles doivent être transmises par voie Bluetooth depuis le groupe receveur vers le groupe émetteur.*


### 3. Refaire la partie cliente pour répondre au bluetooth
*Enfin, l'émetteur leur est receveur bluetooth doit répondre.*
