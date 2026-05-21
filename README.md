# Doigt robotique souple a raideur variable

Ce projet porte sur le developpement d'un prototype simplifie de doigt robotique souple a raideur variable. L'objectif n'est pas encore de realiser une pince complete, mais de valider le fonctionnement d'un seul doigt actionne par un seul moteur.

Le doigt est commande par un tendon: un fil est tire par un moteur Dynamixel afin de courber une structure souple imprimee en 3D. Lorsque le moteur relache le fil, le doigt revient vers sa position ouverte grace a son elasticite naturelle ou a un ressort de rappel.

## Objectif du projet

Le prototype sert a tester une premiere architecture de doigt pour un futur prehenseur a trois doigts. Cette premiere version doit permettre de comprendre:

- comment un tendon peut transmettre l'effort du moteur au doigt;
- quelle amplitude moteur donne une fermeture utile;
- comment le doigt revient en position ouverte;
- comment la forme du doigt influence la courbure;
- comment une raideur variable manuelle peut modifier le comportement mecanique;
- comment ajouter plus tard une limitation d'effort avec un capteur FSR.

La version actuelle reste volontairement simple. Elle permet de faire bouger un seul moteur Dynamixel entre une position ouverte et une position fermee, afin de tester progressivement le systeme mecanique.

## Principe de fonctionnement

Le moteur Dynamixel tire sur un fil Dyneema ou un fil de peche tresse. Ce fil passe dans un canal imprime ou dans une gaine PTFE, puis vient s'ancrer en bout de doigt.

Quand le moteur tourne dans le sens de fermeture, le tendon se tend et provoque la flexion du doigt. Quand le moteur revient vers la position ouverte, le tendon se detend et le doigt revient par elasticite.

```text
[Dynamixel] -- traction du fil --> [Tendon] -- flexion --> [Doigt souple]
```

Cette approche est adaptee aux grippers souples car elle permet de placer l'actionneur a distance, de garder un doigt leger et de conserver une certaine compliance lors du contact avec un objet.

## Raideur variable

La raideur variable n'est pas motorisee dans cette premiere version. Elle est reglee manuellement, par exemple en deplacant une piece, un insert ou une zone de rigidification sur le doigt.

Trois positions peuvent etre testees:

```text
Position basse  : doigt plus souple, grande courbure.
Position milieu : compromis entre souplesse et maintien.
Position haute  : doigt plus rigide, courbure plus localisee.
```

Cette solution permet de comparer simplement plusieurs comportements mecaniques sans ajouter un second actionneur. L'idee est de verifier si le changement de raideur modifie la zone de flexion, la surface de contact et la capacite a maintenir un objet.

## Architecture materielle

Le prototype minimal est compose de:

- un PC avec VS Code;
- une carte ESP32;
- un moteur Dynamixel;
- un fil tendon;
- un doigt imprime en TPU, PLA+ ou PETG;
- eventuellement un capteur FSR dans une version suivante.

```text
[PC / VS Code]
      |
      | USB
      |
[ESP32]
      |
      | bus serie Dynamixel
      |
[Dynamixel]
      |
      | traction
      |
[Fil tendon]
      |
      |
[Doigt TPU / PLA-PETG]
```

Le doigt peut etre compose d'une base rigide, d'une partie flexible, d'un canal pour le fil, d'un point d'ancrage en bout de doigt et d'un systeme manuel de reglage de raideur.

## Programme actuel

Le code embarque correspond a la premiere version de test. Il permet de:

- initialiser l'ESP32;
- initialiser le Dynamixel;
- activer le couple moteur;
- definir une position ouverte;
- definir une position fermee;
- alterner automatiquement ouverture et fermeture;
- modifier legerement l'amplitude de fermeture depuis le moniteur serie;
- afficher l'etat du moteur;
- reserver une future limitation d'effort par FSR.

Les parametres principaux sont dans [`include/config.h`](include/config.h):

```cpp
ID_MOTEUR
DXL_BAUDRATE
POSITION_OUVERTE
POSITION_FERMEE
VITESSE_MOTEUR
COUPLE_MAX
TEMPS_PAUSE_MS
```

## Etapes de developpement

1. Faire tourner le Dynamixel seul.
2. Faire tirer le fil sans le doigt.
3. Connecter le fil au doigt.
4. Tester ouverture et fermeture.
5. Mesurer la course utile.
6. Ajouter un capteur FSR.
7. Limiter la fermeture selon l'effort mesure.
8. Tester plusieurs positions manuelles de raideur.
9. Etendre ensuite le principe vers un prehenseur a trois doigts.

## Utilisation sous VS Code

Le projet utilise PlatformIO.

Pour compiler:

```bash
pio run -e esp32_dynamixel
```

Pour televerser sur l'ESP32:

```bash
pio run -e esp32_dynamixel -t upload
```

Pour ouvrir le moniteur serie:

```bash
pio device monitor -b 115200
```

Commandes disponibles dans le moniteur serie:

```text
a : mode automatique open/close
o : aller en position ouverte
c : aller en position fermee
s : arreter la boucle automatique
+ : augmenter legerement la fermeture
- : diminuer legerement la fermeture
p : afficher l'etat
h : afficher l'aide
```

## Cablage a verifier

Pour un Dynamixel avec ESP32:

- alimentation moteur adaptee au modele Dynamixel;
- masse commune entre alimentation moteur et ESP32;
- signal data relie au bus TTL Dynamixel;
- `DXL_BAUDRATE` identique a la configuration du moteur;
- `ID_MOTEUR` identique a l'ID programme dans le moteur;
- `DXL_DIR_PIN` a adapter si le convertisseur half-duplex utilise une broche de direction.

Les positions Dynamixel sont donnees en valeur brute, souvent sur une plage `0..4095` pour les modeles en protocole 2.0.
