/*
 * IRremote
 * Version 1.0 March, 2017
 * Copyright 2009 Ken Shirriff
 * For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.htm http://arcfn.com
 *
 * Edited by Diogo to add Mitsubishi controller AC PKFY-P
 *
 * Interrupt code based on NECIRrcv by Joe Knapp
 * http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
 * Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
 *
 */

#ifndef Mitsubishi_PKFY_P_h
#define Mitsubishi_PKFY_P_h

// general Arduino stuff
#include <Arduino.h>

// Provides ISR
#include <avr/interrupt.h>


#ifdef F_CPU
#define SYSCLOCK F_CPU     // main Arduino clock
#else
#define SYSCLOCK 16000000  // main Arduino clock
#endif

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// TUNE-UP for better performance -------
#define HVAC_MITSUBISHI_HDR_MARK    2610
#define HVAC_MITSUBISHI_HDR_SPACE   1260
#define HVAC_MITSUBISHI_BIT_MARK    405
#define HVAC_MISTUBISHI_ZERO_SPACE  400
#define HVAC_MITSUBISHI_ONE_SPACE   1190
// ---------------------------------------

// IR detector output is active low
#define MARK  0
#define SPACE 1

// defines for timer2 (8 bits)
#define TIMER_RESET
#define TIMER_ENABLE_PWM     (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM    (TCCR2A &= ~(_BV(COM2B1)))
#define TIMER_DISABLE_INTR   (TIMSK2 = 0)
#define TIMER_CONFIG_KHZ(val) ({ \
    const uint8_t pwmval = SYSCLOCK / 2000 / (val); \
    TCCR2A = _BV(WGM20); \
    TCCR2B = _BV(WGM22) | _BV(CS20); \
    OCR2A = pwmval; \
    OCR2B = pwmval / 3; \
})
#define TIMER_COUNT_TOP      (SYSCLOCK * USECPERTICK / 1000000)
#if (TIMER_COUNT_TOP < 256)
#define TIMER_CONFIG_NORMAL() ({ \
    TCCR2A = _BV(WGM21); \
    TCCR2B = _BV(CS20); \
    OCR2A = TIMER_COUNT_TOP; \
    TCNT2 = 0; \
})
#else
#define TIMER_CONFIG_NORMAL() ({ \
    TCCR2A = _BV(WGM21); \
    TCCR2B = _BV(CS21); \
    OCR2A = TIMER_COUNT_TOP / 8; \
    TCNT2 = 0; \
})
#endif

#if defined(CORE_OC2B_PIN)
#define TIMER_PWM_PIN        CORE_OC2B_PIN  /* Teensy */
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define TIMER_PWM_PIN        9  /* Arduino Mega */
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define TIMER_PWM_PIN        14 /* Sanguino */
#else
#define TIMER_PWM_PIN        3  /* Arduino Duemilanove, Diecimila, LilyPad, etc */
#endif

typedef enum HvacMode {
    HVAC_COLD,
    HVAC_DRY,
    HVAC_AUTO,
    HVAC_FAN,
    HVAC_HOT
} HvacMode_t; // HVAC  MODE

typedef enum HvacFanMode {
    FAN_SPEED_1,
    FAN_SPEED_2,
    FAN_SPEED_3,
    FAN_SPEED_4
} HvacFanMode_t;  // HVAC  FAN MODE

typedef enum HvacVanneMode {
    VANNE_H1,
    VANNE_H2,
    VANNE_H3,
    VANNE_H4,
    VANNE_AUTO_MOVE
} HvacVanneMode_t;  // HVAC  VANNE MODE


class ACsend {
    public:
        ACsend();
    
        void sendMitsubishi(
                            byte                ON,              // The state of AC
                            HvacMode            HVAC_Mode,       // The desired mode HvacMode
                            byte                HVAC_Temp,       // The temperature
                            HvacFanMode         HVAC_FanMode,    // The available fan speed HvacMitsubishiFanMode
                            HvacVanneMode       HVAC_VanneMode   // The available vanne move HvacMitsubishiVanneMode
        );
    
    private:
        void mark(int);
        void space(int);
        void enableIROut(byte);
};

extern ACsend IRsend;

#endif
