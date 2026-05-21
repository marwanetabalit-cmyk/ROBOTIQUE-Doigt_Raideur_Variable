# Protocole de test V1

Ce protocole sert a valider progressivement le doigt tendon-driven avec un seul moteur.

## 1. Moteur seul

- Deconnecter le fil.
- Alimenter le moteur correctement.
- Televerser le firmware.
- Verifier dans le moniteur serie que le moteur alterne entre `POSITION_OUVERTE` et `POSITION_FERMEE`.
- Reduire l'amplitude si le mouvement est trop grand.

## 2. Fil sans doigt

- Fixer le fil sur la poulie ou le palonnier.
- Laisser l'autre extremite libre.
- Verifier que la fermeture tend le fil et que l'ouverture le relache.
- Verifier que le fil ne sort pas de son guidage.

## 3. Fil avec doigt

- Attacher le fil au point d'ancrage en bout de doigt.
- Commencer avec une faible amplitude.
- Augmenter la fermeture avec la commande serie `+`.
- Diminuer avec la commande serie `-`.
- Noter la position utile maximale avant contrainte excessive.

## 4. Raideur manuelle

Tester trois positions de l'insert ou de la piece de rigidification:

```text
Position basse  : doigt plus souple.
Position milieu : compromis.
Position haute  : doigt plus rigide.
```

Pour chaque position, noter:

- position ouverte;
- position fermee;
- courbure obtenue;
- effort ressenti;
- retour elastique;
- risque de blocage du fil.

## 5. Future mesure FSR

Le code contient deja un emplacement reserve pour `FSR_ACTIVE`, `FSR_PIN` et `FSR_LIMITE_BRUTE`.

La version suivante pourra arreter ou limiter la fermeture si la mesure FSR depasse un seuil.
