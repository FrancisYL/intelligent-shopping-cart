#include "Cart.h"

/* This constructor returns an XBee instance with '000000' stores in the message box */
XBee::XBee() {
  for (int i = 0; i < N; i++) {
    message[i] = '0';
  }
}

/* This function emit the message from the message box */
void XBee::emit() {
  // Send '<' to start
  Serial.write('<');

  // Send message from the message box
  for (int i = 0; i < N; i++) {
    Serial.write(message[i]);
  }

  // Send '>' to end
  Serial.write('>');

  delay(1);
  Serial.flush();
}

/* Waiting for a synchronized-signal and prepare to receive the meesage */
bool XBee::receive() {
  // Receive the message
  for (int i = 0; i < N; i++) {
    while (Serial.available() == 0);
    message[i] = Serial.read();
  }

  // Wait for '>'
  while (Serial.available() == 0);
  if (Serial.read() != '>') {
    return false;
  }

  Serial.write('!');
  Serial.flush();
  return true;
}

/* Print the message box to the serial output */
void XBee::printMes() {
  for (int i = 0; i < N; i++) {
    Serial.print(message[i]);
  }
  Serial.println();
}

