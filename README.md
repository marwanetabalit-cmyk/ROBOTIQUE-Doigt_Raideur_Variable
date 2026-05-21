# Doigt robotique souple a raideur variable

Prototype minimal d'un doigt robotique souple actionne par tendon.

L'objectif actuel est de valider un seul doigt avec un seul moteur:

- fermeture par traction d'un fil Dyneema ou fil de peche tresse;
- ouverture par elasticite du doigt ou ressort de rappel;
- raideur variable reglee manuellement;
- ajout possible d'un capteur FSR dans une version suivante.

## Architecture

```text
[PC / VS Code]
      |
      | USB
      |
[ESP32 ou Arduino]
      |
      | signal commande
      |
[Dynamixel ou servo]
      |
      | traction
      |
[Fil tendon]
      |
[Doigt TPU / PLA-PETG]
```

## Demarrage sous VS Code

Le projet est prepare pour PlatformIO.

1. Installer l'extension VS Code `PlatformIO IDE`.
2. Ouvrir ce dossier dans VS Code.
3. Modifier les valeurs dans [`include/config.h`](include/config.h).
4. Brancher la carte.
5. Compiler et televerser avec l'environnement adapte:

```bash
pio run -e esp32_dynamixel -t upload
```

Pour ouvrir le moniteur serie:

```bash
pio device monitor -b 115200
```

Un environnement servo simple existe aussi pour Arduino Uno:

```bash
pio run -e arduino_uno_servo -t upload
```

## Version 1 du programme

Le firmware actuel fait:

- initialisation de la carte;
- initialisation du moteur;
- activation du couple pour un Dynamixel;
- definition d'une position ouverte;
- definition d'une position fermee;
- alternance automatique ouverture / fermeture;
- affichage periodique de l'etat moteur;
- commandes serie simples pour tester plusieurs amplitudes.

## Commandes serie

Dans le moniteur serie:

```text
a : mode automatique open/close
o : aller en position ouverte
c : aller en position fermee
s : stop logique de la boucle automatique
+ : augmenter legerement la fermeture
- : diminuer legerement la fermeture
p : afficher l'etat
h : aide
```

## Variables principales

Les variables a modifier en premier sont dans [`include/config.h`](include/config.h):

```cpp
POSITION_OUVERTE
POSITION_FERMEE
VITESSE_MOTEUR
COUPLE_MAX
TEMPS_PAUSE_MS
MODE_TEST_DEFAUT
```

Pour un Dynamixel:

```cpp
ID_MOTEUR
DXL_BAUDRATE
DXL_PROTOCOL_VERSION
DXL_DIR_PIN
```

## Etapes de test

1. Faire tourner le moteur seul.
2. Faire tirer le fil sans le doigt.
3. Connecter le fil au doigt.
4. Tester ouverture / fermeture.
5. Mesurer la course utile.
6. Ajouter le FSR.
7. Limiter la fermeture selon l'effort mesure.
8. Tester plusieurs positions manuelles de raideur.

## Cablage a verifier

Pour un Dynamixel avec ESP32:

- alimentation moteur adaptee au modele Dynamixel;
- masse commune entre alimentation moteur et ESP32;
- signal data relie au bus TTL Dynamixel;
- `DXL_BAUDRATE` identique a la configuration du moteur;
- `ID_MOTEUR` identique a l'ID programme dans le moteur;
- `DXL_DIR_PIN` a adapter si vous utilisez un convertisseur half-duplex avec broche de direction.

Les positions Dynamixel sont donnees en valeur brute, souvent sur une plage `0..4095` pour les modeles en protocole 2.0.

## Notes mecaniques

Le doigt peut etre compose d'une base rigide en PLA+ ou PETG, d'une partie flexible en TPU 95A, d'un canal pour le fil, d'un point d'ancrage en bout de doigt et d'un systeme manuel de raideur variable.

Positions de raideur a tester:

```text
Position basse  : doigt plus souple, grande courbure.
Position milieu : compromis.
Position haute  : doigt plus rigide, courbure plus localisee.
```
