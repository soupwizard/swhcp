
void setupRelays() {
  for (int i = 0; i < numRelays; i++) {
    pinMode(allRelayPins[i], OUTPUT);
    relayOff(allRelayPins[i]);
  }
}

// routines for relays connected directly to adruino

void relayOff(int relayPin) {
  digitalWrite(relayPin, HIGH);
}

void relayOn(int relayPin) {
  digitalWrite(relayPin, LOW);
}





