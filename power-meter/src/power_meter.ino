/*
 * AC/DC Power Meter
 * Agris Miezitis
 *
 * Measures voltage and current on AC or DC loads and computes the full power
 * triangle in firmware: RMS values, real / apparent / reactive power, power
 * factor, and the phase relationship between voltage and current.
 *
 * Sampling bypasses the Arduino analogRead() API. The ADC is configured
 * directly and driven from a Timer1 compare interrupt; the ADC ISR alternates
 * the input multiplexer between the voltage and current channels so both are
 * captured on every sampling tick.
 *
 * Hardware:
 *   Current  - ACS712 Hall-effect sensor (works for AC and DC)
 *   Voltage  - DC:  56k / 330k resistive divider
 *              AC:  op-amp front end (bias + scale into ADC range)
 *
 * Channels:
 *   ADC0 - voltage
 *   ADC1 - current
 */

#include <avr/io.h>
#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#define BUFFER_SIZE 200

volatile uint16_t bufferU[BUFFER_SIZE]; // Voltage buffer
volatile uint16_t bufferI[BUFFER_SIZE]; // Current buffer
volatile uint8_t sampleIndex = 0;
volatile bool bufferReady = false;

/* Empirically derived scale factors: ADC counts -> volts / amps.
 * These are specific to this board's divider and sensor. See README. */
#define SCALE_U 0.21f
#define SCALE_I 0.077f

void setup() {
    Serial.begin(115200);

    // ADC: AVcc reference, interrupt on conversion complete, prescaler /16
    ADMUX  = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2);
    ADCSRB = 0;

    // Timer1: CTC mode, prescaler /1024, compare at 15 -> sampling tick
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A  = 15;
    TIMSK1 = (1 << OCIE1A);

    sei();
}

// Sampling tick: select voltage channel and start a conversion
ISR(TIMER1_COMPA_vect) {
    ADMUX = (1 << REFS0);
    ADCSRA |= (1 << ADSC);
}

// Conversion complete: store the result, then chain to the other channel
ISR(ADC_vect) {
    uint16_t adcValue = ADC;

    if (ADMUX == (1 << REFS0)) {
        // That was the voltage channel - switch to current and convert again
        bufferU[sampleIndex] = adcValue;
        ADMUX = (1 << REFS0) | (1 << MUX0);
        ADCSRA |= (1 << ADSC);
    } else {
        // That was the current channel - advance the buffer
        bufferI[sampleIndex] = adcValue;
        sampleIndex++;
        if (sampleIndex >= BUFFER_SIZE) {
            sampleIndex = 0;
            bufferReady = true;
        }
        ADMUX = (1 << REFS0);
    }
}

/* Find the first index where the signal crosses its own mean.
 * The difference between the voltage and current crossing indices gives
 * the phase relationship. Returns -1 if no crossing was found. */
int findZCIndex(volatile uint16_t* buffer, float average) {
    for (int i = 1; i < BUFFER_SIZE; i++) {
        if ((buffer[i-1] - average) * (buffer[i] - average) <= 0) {
            return i;
        }
    }
    return -1;
}

void processBuffer(volatile uint16_t* bufferU, volatile uint16_t* bufferI) {
    // Pass 1: means (the DC offset the AC waveform rides on)
    uint32_t sumU = 0;
    uint32_t sumI = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        sumU += bufferU[i];
        sumI += bufferI[i];
    }
    float averageU = sumU / (float)BUFFER_SIZE;
    float averageI = sumI / (float)BUFFER_SIZE;

    // Pass 2: squared sums for RMS, and the instantaneous product for real power
    uint32_t sq_sumU = 0;
    uint32_t sq_sumI = 0;
    int64_t realPowerSum = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        int32_t diffU = ((int32_t)bufferU[i] - (int32_t)averageU) * SCALE_U;
        int32_t diffI = ((int32_t)bufferI[i] - (int32_t)averageI) * SCALE_I;
        sq_sumU += diffU * diffU;
        sq_sumI += diffI * diffI;
        realPowerSum += (int64_t)diffU * (int64_t)diffI;
    }

    float rmsU          = sqrt(sq_sumU / (float)BUFFER_SIZE);
    float rmsI          = sqrt(sq_sumI / (float)BUFFER_SIZE);
    float realPower     = realPowerSum / (float)BUFFER_SIZE;
    float apparentPower = rmsU * rmsI;
    float reactivePower = sqrt(apparentPower * apparentPower - realPower * realPower);
    float powerFactor   = realPower / apparentPower;

    int zeroCrossingIndexU = findZCIndex(bufferU, averageU);
    int zeroCrossingIndexI = findZCIndex(bufferI, averageI);

    Serial.print("Avg U: ");    Serial.print(averageU);
    Serial.print(" | Avg I: "); Serial.print(averageI);
    Serial.print(" | RMS U: "); Serial.print(rmsU);
    Serial.print(" | RMS I: "); Serial.print(rmsI);
    Serial.print(" | P: ");     Serial.print(realPower);
    Serial.print(" | S: ");     Serial.print(apparentPower);
    Serial.print(" | Q: ");     Serial.print(reactivePower);
    Serial.print(" | cos phi: ");   Serial.print(powerFactor);

    if (zeroCrossingIndexU >= 0 && zeroCrossingIndexI >= 0) {
        int phaseDifference = zeroCrossingIndexI - zeroCrossingIndexU;
        Serial.print(" | Phase difference: ");
        Serial.println(phaseDifference);

        // Current lagging voltage -> inductive; leading -> capacitive
        if (phaseDifference > 0) {
            Serial.println("Inductive.");
        } else if (phaseDifference < 0) {
            Serial.println("Capacitive.");
        } else {
            Serial.println("Resistive.");
        }
    } else {
        Serial.println("Zero crossing error");
    }
}

void loop() {
    if (bufferReady) {
        bufferReady = false;
        processBuffer(bufferU, bufferI);
    }
}
