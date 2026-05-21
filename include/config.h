#pragma once

#include <Arduino.h>

// Choisir le backend via platformio.ini:
// - MOTOR_BACKEND_DYNAMIXEL=1 pour un Dynamixel
// - MOTOR_BACKEND_SERVO=1 pour un servo RC classique
#ifndef MOTOR_BACKEND_DYNAMIXEL
#define MOTOR_BACKEND_DYNAMIXEL 0
#endif

#ifndef MOTOR_BACKEND_SERVO
#define MOTOR_BACKEND_SERVO 0
#endif

#if !MOTOR_BACKEND_DYNAMIXEL && !MOTOR_BACKEND_SERVO
#undef MOTOR_BACKEND_DYNAMIXEL
#define MOTOR_BACKEND_DYNAMIXEL 1
#endif

#if MOTOR_BACKEND_DYNAMIXEL && MOTOR_BACKEND_SERVO
#error "Choisir un seul backend moteur: Dynamixel ou Servo."
#endif

enum TestMode : uint8_t {
  MODE_STOP = 0,
  MODE_AUTO_OPEN_CLOSE = 1,
  MODE_OPEN_ONLY = 2,
  MODE_CLOSE_ONLY = 3,
};

// Serie PC.
static const uint32_t SERIAL_BAUDRATE = 115200;

// Variables principales du prototype.
static const TestMode MODE_TEST_DEFAUT = MODE_AUTO_OPEN_CLOSE;
static const uint32_t TEMPS_PAUSE_MS = 1500;
static const uint32_t LOG_STATE_PERIOD_MS = 1000;

#if MOTOR_BACKEND_DYNAMIXEL

// Configuration Dynamixel.
static const uint8_t ID_MOTEUR = 1;
static const uint32_t DXL_BAUDRATE = 57600;
static const float DXL_PROTOCOL_VERSION = 2.0;

// Sur beaucoup de cartes ESP32, Serial2 utilise RX=16 et TX=17 par defaut.
// Le pin DIR est utilise si votre convertisseur TTL half-duplex en a besoin.
static const int DXL_DIR_PIN = 4;

// Valeurs brutes typiques pour un Dynamixel en mode position et resolution 0..4095.
static const int32_t POSITION_OUVERTE = 2048;
static const int32_t POSITION_FERMEE = 2600;
static const int32_t POSITION_MIN = 0;
static const int32_t POSITION_MAX = 4095;
static const int32_t AMPLITUDE_STEP = 25;

// PROFILE_VELOCITY. Ajuster doucement pour les premiers essais.
static const uint32_t VITESSE_MOTEUR = 80;

// Reserve pour une limitation de couple/courant selon le modele Dynamixel.
// L'application directe est volontairement desactivee par defaut car le nom
// du registre depend du modele.
static const uint16_t COUPLE_MAX = 350;
static const bool DXL_APPLIQUER_LIMITE_COURANT = false;

#endif

#if MOTOR_BACKEND_SERVO

// Configuration servo RC simple.
static const uint8_t SERVO_PIN = 9;

// Positions en degres pour un servo RC classique.
static const int32_t POSITION_OUVERTE = 20;
static const int32_t POSITION_FERMEE = 110;
static const int32_t POSITION_MIN = 0;
static const int32_t POSITION_MAX = 180;
static const int32_t AMPLITUDE_STEP = 5;

// Delai entre deux petites variations pendant un mouvement servo.
static const uint32_t VITESSE_MOTEUR = 15;

// Pas de couple commande sur servo RC standard.
static const uint16_t COUPLE_MAX = 0;

#endif

// Emplacement reserve pour une future limitation d'effort par FSR.
static const bool FSR_ACTIVE = false;
#if defined(ARDUINO_ARCH_ESP32)
static const uint8_t FSR_PIN = 34;
#else
static const uint8_t FSR_PIN = A0;
#endif
static const int FSR_LIMITE_BRUTE = 750;
