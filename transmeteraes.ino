#include <SPI.h>
#include <RF24.h>
#include <AESLib.h>

RF24 radio(9, 10);  // CE, CSN
AESLib aes;

const uint64_t pipe = 0xE8E8F0F0E1LL;  // Pipe address for communication
const byte key[] = "16BytePassword";   // 128-bit encryption key

bool messageSent = false; // Flag to track if message has been sent

void setup() {
  Serial.begin(9600);
  while (!Serial) {} // Wait for serial connection
  
  if (!radio.begin()) {
    Serial.println("RF24 initialization failed!");
    while (1); // Hang here
  }
  radio.openWritingPipe(pipe);
}

void loop() {
  if (!messageSent) {
    String message = "Hello, Receiver!";
  
    // Padding the message if its length is less than 16 bytes
    while (message.length() < 16) {
      message += " "; // Adding spaces for padding
    }
  
    encryptAndSend(message);
    messageSent = true; // Set flag to indicate message has been sent
  }

  // Your other code here
}

void encryptAndSend(String plainText) {
  byte encrypted[16]; // AES 128-bit encryption produces a 16-byte (128-bit) ciphertext
  byte plaintext[16]; // Buffer for the plaintext

  // Convert string to byte array
  plainText.getBytes(plaintext, sizeof(plaintext));

  // Define an initialization vector (IV) if needed
  byte iv[16] = {0}; // For AES-128, IV should be 16 bytes

  aes.encrypt(plaintext, sizeof(plaintext), encrypted, key, 128, iv);

  radio.write(encrypted, sizeof(encrypted));
  Serial.println("Message sent: " + plainText);
}