#include <Arduino.h>
#include <Dynamixel2Arduino.h>

#include "config.h"

#define DXL_SERIAL Serial2

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
using namespace ControlTableItem;

int32_t position_ouverte = POSITION_OUVERTE;
int32_t position_fermee = POSITION_FERMEE;
uint32_t vitesse_moteur = VITESSE_MOTEUR;
uint16_t couple_max = COUPLE_MAX;
uint32_t temps_pause = TEMPS_PAUSE_MS;
TestMode mode_test = MODE_TEST_DEFAUT;

uint8_t ID_moteur = ID_MOTEUR;
uint32_t baudrate = DXL_BAUDRATE;

int32_t position_goal = POSITION_OUVERTE;
int32_t position_present = POSITION_OUVERTE;
bool torque_enable = false;
bool motor_ready = false;

uint32_t last_auto_change_ms = 0;
uint32_t last_log_ms = 0;
bool auto_target_closed = false;

void printHelp();
void printMotorState(const char *label);
void handleSerialCommands();
void updateAutoCycle();
void initMotor();
void moveToPosition(int32_t target);
int32_t readMotorPosition();
void setTorque(bool enabled);
bool forceLimitReached();
int32_t clampPosition(int32_t position);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  delay(300);

  Serial.println();
  Serial.println(F("Prototype doigt tendon-driven - test Dynamixel"));

  initMotor();
  printHelp();

  moveToPosition(position_ouverte);
  last_auto_change_ms = millis();
}

void loop() {
  handleSerialCommands();

  if (mode_test == MODE_AUTO_OPEN_CLOSE) {
    updateAutoCycle();
  }

  if (LOG_STATE_PERIOD_MS > 0 && millis() - last_log_ms >= LOG_STATE_PERIOD_MS) {
    last_log_ms = millis();
    printMotorState("etat");
  }
}

void initMotor() {
  dxl.begin(baudrate);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  if (!dxl.ping(ID_moteur)) {
    Serial.println(F("Erreur: Dynamixel non detecte. Verifier ID, baudrate et cablage."));
    motor_ready = false;
    return;
  }

  dxl.torqueOff(ID_moteur);
  dxl.setOperatingMode(ID_moteur, OP_POSITION);
  dxl.writeControlTableItem(PROFILE_VELOCITY, ID_moteur, vitesse_moteur);
  setTorque(true);
  motor_ready = true;

  Serial.println(F("Dynamixel initialise en mode position."));
  if (DXL_APPLIQUER_LIMITE_COURANT) {
    Serial.println(F("Note: limite de courant a configurer selon le modele Dynamixel."));
  }
}

void setTorque(bool enabled) {
  if (enabled) {
    torque_enable = dxl.torqueOn(ID_moteur);
  } else {
    dxl.torqueOff(ID_moteur);
    torque_enable = false;
  }
}

void moveToPosition(int32_t target) {
  if (!motor_ready) {
    Serial.println(F("Moteur non pret: commande ignoree."));
    return;
  }

  target = clampPosition(target);

  if (forceLimitReached() && target == position_fermee) {
    Serial.println(F("Limite FSR atteinte: fermeture ignoree."));
    return;
  }

  position_goal = target;

  if (!torque_enable) {
    setTorque(true);
  }

  dxl.setGoalPosition(ID_moteur, position_goal);
}

int32_t readMotorPosition() {
  if (!motor_ready) {
    return position_present;
  }

  position_present = dxl.getPresentPosition(ID_moteur);
  return position_present;
}

bool forceLimitReached() {
  if (!FSR_ACTIVE) {
    return false;
  }

  int raw = analogRead(FSR_PIN);
  return raw >= FSR_LIMITE_BRUTE;
}

void updateAutoCycle() {
  if (millis() - last_auto_change_ms < temps_pause) {
    return;
  }

  auto_target_closed = !auto_target_closed;
  int32_t target = auto_target_closed ? position_fermee : position_ouverte;

  moveToPosition(target);
  printMotorState(auto_target_closed ? "fermeture" : "ouverture");

  last_auto_change_ms = millis();
}

void handleSerialCommands() {
  if (!Serial.available()) {
    return;
  }

  char command = Serial.read();

  switch (command) {
    case 'a':
    case 'A':
      mode_test = MODE_AUTO_OPEN_CLOSE;
      last_auto_change_ms = millis();
      Serial.println(F("Mode auto open/close."));
      break;

    case 'o':
    case 'O':
      mode_test = MODE_OPEN_ONLY;
      moveToPosition(position_ouverte);
      printMotorState("ouverture manuelle");
      break;

    case 'c':
    case 'C':
      mode_test = MODE_CLOSE_ONLY;
      moveToPosition(position_fermee);
      printMotorState("fermeture manuelle");
      break;

    case 's':
    case 'S':
      mode_test = MODE_STOP;
      Serial.println(F("Boucle auto stoppee."));
      break;

    case '+':
      position_fermee = clampPosition(position_fermee + AMPLITUDE_STEP);
      Serial.println(F("Amplitude fermeture augmentee."));
      printMotorState("amplitude");
      break;

    case '-':
      position_fermee = clampPosition(position_fermee - AMPLITUDE_STEP);
      Serial.println(F("Amplitude fermeture diminuee."));
      printMotorState("amplitude");
      break;

    case 'p':
    case 'P':
      printMotorState("manuel");
      break;

    case 'h':
    case 'H':
    case '?':
      printHelp();
      break;

    default:
      break;
  }
}

void printMotorState(const char *label) {
  readMotorPosition();

  Serial.print(F("["));
  Serial.print(label);
  Serial.print(F("] goal="));
  Serial.print(position_goal);
  Serial.print(F(" present="));
  Serial.print(position_present);
  Serial.print(F(" ouverte="));
  Serial.print(position_ouverte);
  Serial.print(F(" fermee="));
  Serial.print(position_fermee);
  Serial.print(F(" vitesse="));
  Serial.print(vitesse_moteur);
  Serial.print(F(" couple_max="));
  Serial.print(couple_max);
  Serial.print(F(" torque="));
  if (torque_enable) {
    Serial.print(F("on"));
  } else {
    Serial.print(F("off"));
  }
  Serial.print(F(" ready="));
  if (motor_ready) {
    Serial.println(F("yes"));
  } else {
    Serial.println(F("no"));
  }
}

void printHelp() {
  Serial.println(F("Commandes serie:"));
  Serial.println(F("  a : mode automatique open/close"));
  Serial.println(F("  o : aller en position ouverte"));
  Serial.println(F("  c : aller en position fermee"));
  Serial.println(F("  s : stop logique de la boucle automatique"));
  Serial.println(F("  + : augmenter la position fermee"));
  Serial.println(F("  - : diminuer la position fermee"));
  Serial.println(F("  p : afficher l'etat"));
  Serial.println(F("  h : afficher cette aide"));
}

int32_t clampPosition(int32_t position) {
  if (position < POSITION_MIN) {
    return POSITION_MIN;
  }

  if (position > POSITION_MAX) {
    return POSITION_MAX;
  }

  return position;
}
