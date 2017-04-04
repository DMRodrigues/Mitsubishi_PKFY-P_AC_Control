#include "Mitsubishi_PKFY-P.h"

//#define AC_MITSUBISHI_DEBUG;  // Un comment to access DEBUG information through Serial Interface

#define NR_TRAMES   2
#define D_SIZE      17


/****************************************************************************/
/* Temperature Map                                                          */
/****************************************************************************/
/*
 Min -> 17; Max -> 30
 
 17 => 1x = 0001 xxxx
 18 => 2x = 0010 xxxx
 ...
 29 => Dx = 1101 xxxx
 30 => Ex = 1110 xxxx
 */


/****************************************************************************/
/* Mode Map                                                                 */
/****************************************************************************/
/*
 Cold; Dry; Auto; Fan; Auto
 
 Cold => x1 = xxxx 0001
 Dry  => x5 = xxxx 0101
 Auto => x3 = xxxx 0011
 Fan  => x0 = xxxx 0000
 Hot  => x2 = xxxx 0010
 */


/****************************************************************************/
/* Vane Map                                                                 */
/****************************************************************************/
/*
 Vane_1; Vane_2; Vane_3; Vane_4; Vane_auto;
 
 Vane_1    => 3x = 0011 xxxx
 Vane_2    => 2x = 0010 xxxx
 Vane_3    => 1x = 0001 xxxx
 Vane_4    => 0x = 0000 xxxx
 Vane_auto => Cx = 1100 xxxx
 */


/****************************************************************************/
/* Fan Map                                                                  */
/****************************************************************************/
/*
 Fan_1; Fan_2; Fan_3; Fan_4
 
 Fan_1 => x1 = xxxx 0001
 Fan_2 => x3 = xxxx 0011
 Fan_3 => x5 = xxxx 0101
 Fan_4 => x7 = xxxx 0111
 */


/****************************************************************************/
/* Preinstantiate Objects                                                   */
/****************************************************************************/
ACsend IRsend = ACsend();


/****************************************************************************/
/* Constructors                                                             */
/* Creates a new instance of this driver.                                   */
/****************************************************************************/
ACsend::ACsend() {
}

/****************************************************************************/
/* Send IR command to Mitsubishi HVAC - sendHvacMitsubishi                  */
/****************************************************************************/
void ACsend::sendMitsubishi(
                              byte              ON,             // Example: true
                              HvacMode          HVAC_Mode,      // Example: HVAC_HOT -> HvacMitsubishiMode
                              byte              HVAC_Temp,      // Example: 21 (°c)
                              HvacFanMode       HVAC_FanMode,   // Example: FAN_SPEED_AUTO -> HvacMitsubishiFanMode
                              HvacVanneMode     HVAC_VanneMode  // Example: VANNE_AUTO_MOVE -> HvacMitsubishiVanneMode
){
    byte i; // shared for iterate
    byte mask = 1; // our bitmask
    byte data[D_SIZE] = { 0x23, 0xCB, 0x26, 0x21, 0x00, 0x40, 0x52, 0x37, 0x04, 0x00, 0x00, 0xBF, 0xAD, 0xC8, 0xFB, 0xFF, 0xFF };
    // data array is a valid trame, only byte to be changed will be updated.
    
    // Byte 6 - On / Off
    if (ON)
        data[5] = (byte) 0x40; // Turn ON HVAC
    else
        data[5] = (byte) 0x00; // Tuen OFF HVAC
    
    
    // Byte 7 - Mode
    switch (HVAC_Mode)
    {
        case HVAC_COLD:  data[6] = (byte) 0x01; break;
        case HVAC_DRY:   data[6] = (byte) 0x05; break;
        case HVAC_AUTO:  data[6] = (byte) 0x03; break;
        case HVAC_FAN:   data[6] = (byte) 0x00; break;
        case HVAC_HOT:   data[6] = (byte) 0x02; break;
        default: break;
    }
    
    
    // Byte 7 - Temperature
    if (HVAC_Temp > 30) { i = 13;} // 30 - 17 = 13
    else if (HVAC_Temp < 17) { i = 1; } // 17 - 16 = 1
    else { i = HVAC_Temp - 16; };
    i = (i << 4);   // shift left
    data[6] = (byte) data[6] | ((byte) i); // temperature begin whith 0x01
    
    
    // Byte 9 - FAN / VANNE
    switch (HVAC_FanMode)
    {
        case FAN_SPEED_1:       data[7] = (byte) 0x01; break;
        case FAN_SPEED_2:       data[7] = (byte) 0x03; break;
        case FAN_SPEED_3:       data[7] = (byte) 0x05; break;
        case FAN_SPEED_4:       data[7] = (byte) 0x07; break;
        default: break;
    }
    
    
    // Byte 9 - VANNE
    switch (HVAC_VanneMode)
    {
        case VANNE_H1:          data[7] = (byte) data[7] | B00110000; break;
        case VANNE_H2:          data[7] = (byte) data[7] | B00100000; break;
        case VANNE_H3:          data[7] = (byte) data[7] | B00010000; break;
        case VANNE_H4:          data[7] = (byte) data[7] | B00000000; break;
        case VANNE_AUTO_MOVE:   data[7] = (byte) data[7] | B11000000; break;
        default: break;
    }
    
    
    /*  START MODULE BITS */
    /* not of State, Temperature & Mode */
    data[11] = ~data[5];
    data[12] = ~data[6];
    data[13] = ~data[7];
    
    
#ifdef AC_MITSUBISHI_DEBUG
    // Enable Serial.begin() FIRST
    Serial.print("Packet to send: ");
    for(mask = 0; mask < D_SIZE; mask++) {
        Serial.print(data[mask], HEX);
        Serial.print("_");
    }
    Serial.println(data[D_SIZE], HEX);
    mask = 1;  // safe reset
#endif
    
    
    enableIROut(38);    // 38khz
    space(0);
    
    for (byte j = 0; j < NR_TRAMES; j++) {  // Apparently it's 2 data trames
        // Header for the Packet
        mark(HVAC_MITSUBISHI_HDR_MARK);
        space(HVAC_MITSUBISHI_HDR_SPACE);
        for(i = 0, mask = 1; i < D_SIZE; i++) {
            // Send all Bits from Byte Data in Reverse Order
            for (mask = 00000001; mask > 0; mask <<= 1) { // Iterate through bit mask
                if (data[i] & mask) {   // Bit ONE
                    mark(HVAC_MITSUBISHI_BIT_MARK);
                    space(HVAC_MITSUBISHI_ONE_SPACE);
                }
                else {  // Bit ZERO
                    mark(HVAC_MITSUBISHI_BIT_MARK);
                    space(HVAC_MISTUBISHI_ZERO_SPACE);
                }
            }
        }
        mark(HVAC_MITSUBISHI_BIT_MARK);
        space(0); // Just to be sure
    }
    
    //    // Alternative way to send
    //    // Header for the Packet
    //    mark(HVAC_MITSUBISHI_HDR_MARK);
    //    space(HVAC_MITSUBISHI_HDR_SPACE);
    //    for (byte i = 0; i < D_SIZE; i++) {
    //
    //        byte my_char = data[i];
    //        for (int i = 0; i < 8; ++i)
    //        {
    //            int b = ((my_char >> i) & 1);
    //            if(b) {  // Bit ONE
    //                mark(HVAC_MITSUBISHI_BIT_MARK);
    //                space(HVAC_MITSUBISHI_ONE_SPACE);
    //            }
    //            else {
    //                mark(HVAC_MITSUBISHI_BIT_MARK);
    //                space(HVAC_MISTUBISHI_ZERO_SPACE);
    //            }
    //        }
    //    }
    //    mark(HVAC_MITSUBISHI_BIT_MARK);
    //    space(0); // Just to be sure
    
}


void ACsend::mark(int time) {
    // Sends an IR mark for the specified number of microseconds.
    // The mark output is modulated at the PWM frequency.
    TIMER_ENABLE_PWM; // Enable pin 3 PWM output
    //delayMicroseconds(time);
    // Use IRlib containment for delay in µsecond
    if(time){
        if(time > 16000) {
            delayMicroseconds(time % 1000);
            delay(time / 1000);
        }
        else {
            delayMicroseconds(time);
        }
    }
}


/* Leave pin off for time (given in microseconds) */
void ACsend::space(int time) {
    // Sends an IR space for the specified number of microseconds.
    // A space is no output, so the PWM output is disabled.
    TIMER_DISABLE_PWM; // Disable pin 3 PWM output
    //delayMicroseconds(time);
    // Use IRlib containment for delay in µsecond
    if(time) {
        if(time > 16000) {
            delayMicroseconds(time % 1000);
            delay(time / 1000);
        }
        else {
            delayMicroseconds(time);
        }
    }
}


void ACsend::enableIROut(byte khz) {
    // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
    // The IR output will be on pin 3 (OC2B).
    // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
    // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
    // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
    // controlling the duty cycle.
    // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
    // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
    // A few hours staring at the ATmega documentation and this will all make sense.
    // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
    
    
    // Disable the Timer2 Interrupt (which is used for receiving IR)
    TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt
    
    pinMode(TIMER_PWM_PIN, OUTPUT);
    digitalWrite(TIMER_PWM_PIN, LOW); // When not sending PWM, we want it low
    
    // COM2A = 00: disconnect OC2A
    // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
    // WGM2 = 101: phase-correct PWM with OCRA as top
    // CS2 = 000: no prescaling
    // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
    TIMER_CONFIG_KHZ(khz);
}
