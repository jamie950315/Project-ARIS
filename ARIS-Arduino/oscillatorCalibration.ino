#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- CUSTOM ARGUMENTS ---
#define FREQUENCY            50       // target 50 Hz
#define PWM_CHANNEL          0        // monitor PWM channel 0
#define FEEDBACK_PIN         2        // interrupt pin
#define INTERVAL_MS          1000UL   // 
#define CALIBRATION_ROUNDS   100      // each average takes 100 rounds
#define STABILITY_PERCENTAGE 95       // 
#define TOLERANCE_HZ         0.5f     // ±0.5 Hz

Adafruit_PWMServoDriver pwm(0x40);
volatile uint16_t pulseCount = 0;
uint16_t prescaleValue = 0;

void onPulse() {
  pulseCount++;
}

uint32_t measureAverageOsc(float &avgUpdateRate, uint16_t rounds) {
  uint64_t sumOsc = 0;
  float  sumUpdate = 0.0f;
  for (uint16_t i = 1; i <= rounds; i++) {
    pulseCount = 0;
    uint32_t startMs = millis();
    uint32_t startUs = micros();
    while (millis() - startMs < INTERVAL_MS) { }
    uint32_t deltaUs = micros() - startUs;
    float actualSec = deltaUs / 1000000.0f;

    uint16_t count = pulseCount;
    sumUpdate += count / actualSec;
    uint32_t realOsc = uint32_t(prescaleValue + 1) * 4096UL * count;
    sumOsc += realOsc;

    float updateRate = count / actualSec;
    float iterMs = deltaUs / 1000.0f;
    float oscPeriodUs = 1000000.0f / realOsc;

    Serial.print(F("[M")); Serial.print(i); Serial.print(F("/"));
    Serial.print(rounds); Serial.print(F("] cnt="));
    Serial.print(count); Serial.print(F("  updateRate="));
    Serial.print(updateRate, 4); Serial.print(F(" Hz  iterTime="));
    Serial.print(iterMs, 3);    Serial.print(F(" ms  oscFreq="));
    Serial.print(realOsc);       Serial.print(F(" Hz  oscPeriod="));
    Serial.print(oscPeriodUs, 6);Serial.println(F(" µs"));
  }
  uint32_t avgOsc = sumOsc / rounds;
  avgUpdateRate = sumUpdate / rounds;
  return avgOsc;
}

bool stabilityTest() {
  uint16_t good = 0;
  for (uint16_t i = 1; i <= CALIBRATION_ROUNDS; i++) {
    pulseCount = 0;
    uint32_t startMs = millis();
    uint32_t startUs = micros();
    while (millis() - startMs < INTERVAL_MS) { }
    uint32_t deltaUs = micros() - startUs;
    float updateRate = pulseCount / (deltaUs / 1000000.0f);

    bool ok = (updateRate >= FREQUENCY - TOLERANCE_HZ) &&
              (updateRate <= FREQUENCY + TOLERANCE_HZ);
    if (ok) good++;

    Serial.print(F("[S")); Serial.print(i); Serial.print(F("/"));
    Serial.print(CALIBRATION_ROUNDS); Serial.print(F("] cnt="));
    Serial.print(pulseCount); Serial.print(F("  updateRate="));
    Serial.print(updateRate, 4); Serial.print(ok ? " OK" : " FAIL");
    Serial.print(F("  iterTime=")); Serial.print(deltaUs/1000.0f,3); Serial.println(F(" ms"));
  }
  int percent = (good * 100 + CALIBRATION_ROUNDS/2) / CALIBRATION_ROUNDS;
  Serial.print(F("Stability Percentage: ")); Serial.print(percent); Serial.println(F("%"));
  return percent >= STABILITY_PERCENTAGE;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("=== PCA9685 Auto Calibration (AVR) ==="));

  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  pwm.setPWM(PWM_CHANNEL, 0, 2048);
  prescaleValue = pwm.readPrescale();
  Serial.print(F("Original prescale = ")); Serial.println(prescaleValue);

  pinMode(FEEDBACK_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(FEEDBACK_PIN),
                  onPulse, RISING);

  Serial.println(F("--- First Measurement ---"));
  float avgUpdateRate;
  uint32_t avgOsc = measureAverageOsc(avgUpdateRate, CALIBRATION_ROUNDS);

  Serial.print(F(">> Average PWM update frequency: ")); 
  Serial.print(avgUpdateRate, 4); Serial.println(F(" Hz"));
  Serial.print(F(">> Set OSC frequency to ")); Serial.print(avgOsc); Serial.println(F(" Hz"));
  pwm.setOscillatorFrequency(avgOsc);
  pwm.setPWMFreq(FREQUENCY);

  while (true) {
    Serial.println(F("--- Stability Test ---"));
    if (stabilityTest()) {
      Serial.println(F("Calibration Complete!"));
      break;
    }
    prescaleValue = pwm.readPrescale();
    Serial.print(F("Read prescale = ")); Serial.println(prescaleValue);
    Serial.println(F("--- Recalibration ---"));
    avgOsc = measureAverageOsc(avgUpdateRate, CALIBRATION_ROUNDS);
    Serial.print(F(">> Average PWM update frequency: ")); 
    Serial.print(avgUpdateRate, 4); Serial.println(F(" Hz"));
    Serial.print(F(">> Set OSC frequency to ")); Serial.print(avgOsc); Serial.println(F(" Hz"));
    pwm.setOscillatorFrequency(avgOsc);
    pwm.setPWMFreq(FREQUENCY);
  }
}

void loop() {

}
