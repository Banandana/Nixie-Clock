// Rough nixie tube clock
//  - Casey Hall


// Pin configurations
int datapin = 2; 
int clockpin = 3;
int latchpin = 4;
int oePin = 5;

// Store values of current time
// TO-DO: Read time data from DS1302 RTC IC
char seconds = 0;
char minutes = 0;
char hours = 0;

// Update the bounds of seconds, minutes, and hours
void updateTimeBounds() {
  // When seconds roll over, increment minutes
  if (seconds > 59) {
    seconds -= 60;
    minutes += 1;
  }

  // When minutes roll over, increment hours
  if (minutes > 59) {
    minutes -= 60;
    hours += 1;
  }

  // Roll hours back 12 when 
  if (hours > 12) {
    hours -= 12;
  }
}

// Bump time up one second
void incrementSecond() {
  seconds += 1;
}

// Cycle through 0-9 for every digit to prevent cathode poisoning
void preventCathodePoisoning() {
  incrementSecond();
  clearNumbers();
  int output = 11;
  for (int i = 0; i < 100; i++) {
    writeNumber(output, false);
    writeNumber(output, false);
    writeNumber(output, false);
    delay(10);
    output += 11;
    if (output > 100) {
      output = 0;
    }
  }
}

// Clear shift registers
void clearNumbers() {
  for (int i = 0; i < 12; i++) {
    digitalWrite(datapin, HIGH);
    digitalWrite(clockpin, HIGH);
    digitalWrite(clockpin, LOW);
  }
  digitalWrite(latchpin, HIGH);
  digitalWrite(latchpin, LOW);
}

// Write two digits, and optionally set the left digit to blank in case
// left digit is 0
void writeNumber(char number, bool blankLeftDigit) {
  char part1 = number % 10;
  char part2 = (number / 10) % 10;
  if (part2 == 0 && blankLeftDigit) {
    part2 = 15;
  }
  char output = 0;
  output += (part1 << 4) + part2;
  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, MSBFIRST, output);
  digitalWrite(latchpin, HIGH);
}

void setup()
{
  // Set the pin configuration
  randomSeed(analogRead(0));
  pinMode(datapin, OUTPUT);
  pinMode(clockpin, OUTPUT);  
  pinMode(latchpin, OUTPUT);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, LOW);
  digitalWrite(latchpin, LOW);
}

void loop()
{
  while (true) {
    // Clear display
    clearNumbers();

    // Write seconds, minutes, and hours in reverse order
    writeNumber(seconds, false);
    writeNumber(minutes, false);
    writeNumber(hours, true);

    // Wait one second
    delay(1000);
    // And account for that second
    incrementSecond();

    // On the 60th second, prevent cathode poisoning by cycling digits
    if (seconds == 60) {
      preventCathodePoisoning();
    }

    // Update the time bounds so the current time
    // remains valid
    updateTimeBounds();
  }
}
