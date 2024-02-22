# Tp_IoT_2022_LoRa_Bluetooth
Prérequis
Partage de connexion iPhone : activer l'option "maximiser la compatibilité" pour passer sur la bande des 2,4GHz


## 1. Receiver :
### 1. Récupération d'un paquet Mqtt sur test.mosquitto.org :  srt/nom-groupe
### 2. Se connecter à une carte sur l'AP et faire une requete http donnant les valeurs nécessaire à LoRa.
### 3. Ecouter les données de LoRa.
voir fichier receiver.ino
voir fichier sender_receiver.ino


## 2. Sender :
### Envoyer les données de connexion LoRa sur test.mosquitto.org au topic srt/nom-groupe.
Il faut exploiter le dépôt public Mosquito pour y déposer la configuration d'échange LoRa pour notre groupe:
Groupe srt5/midi2
Fréquence 867 MHz
Spread Factor 9
BandeWith 125

Message public de connexion : srt5/midi2;867;9;125
Données envoyé :
'''
Personne,Emoji
Daniel,:)
Giovanni,:>
Antonin,:/
Silvère,:(
'''



## 3. Ensuite...
### 1. Ajouter une fonction permettant de switcher entre Sender et Receiver
À partir du délai de connexion de mosquitto, nous devons établir la connexion directe de pair à pair entre les périphériques LoRa des deux groupes.
Nous devons ensuite échanger des informations directement entre les deux groupes.

### 2. Sur le sender, échangez la partie LoRa par du bluetooth (ou BLE)
Une fois, c'est la formation reçue, elles doivent être transmises par voie Bluetooth depuis le groupe receveur vers le groupe émetteur.

### 3. Refaire la partie cliente pour répondre au bluetooth
Enfin, l'émetteur leur est receveur bluetooth doit répondre.

