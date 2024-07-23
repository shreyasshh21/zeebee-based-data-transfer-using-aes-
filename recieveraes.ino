#include <SPI.h>
#include <RF24.h>
#include <AESLib.h>
#include <LiquidCrystal.h> // Include the LiquidCrystal library

RF24 radio(9, 10);  // CE, CSN
AESLib aes;

const uint64_t pipe = 0xE8E8F0F0E1LL;  // Pipe address for communication
const byte key[] = "16BytePassword";   // 128-bit encryption key

// LCD pin definitions
const int rs = 12;
const int en = 11;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Initialize the LCD object

void setup() {
  Serial.begin(9600);
  while (!Serial) {} // Wait for serial connection
  
  // LCD initialization
  lcd.begin(16, 2); // Set the LCD size (16 columns, 2 rows)

  if (!radio.begin()) {
    Serial.println("RF24 initialization failed!");
    while (1); // Hang here
  }
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    byte encrypted[16];
    while (radio.available()) {
      radio.read(encrypted, sizeof(encrypted));
    }

    byte decrypted[16];
    byte iv[16] = {0}; // Initialization vector

    aes.decrypt(encrypted, sizeof(encrypted), decrypted, key, 128, iv);

    // Convert decrypted byte array to string
    String decryptedMessage;
    for (int i = 0; i < sizeof(decrypted); i++) {
      decryptedMessage += (char)decrypted[i];
      
    }
    Serial.println("Recieved message: "+decryptedMessage);
    
    // Display the decrypted message on the LCD
    lcd.clear(); // Clear the LCD display
    lcd.setCursor(0, 0); // Set cursor to the first row, first column
    lcd.print("Received:"); // Print the text on the LCD
    lcd.setCursor(0, 1); // Set cursor to the second row, first column
    lcd.print(decryptedMessage); // Print the decrypted message
    delay(2000); // Delay for visibility
  }
}