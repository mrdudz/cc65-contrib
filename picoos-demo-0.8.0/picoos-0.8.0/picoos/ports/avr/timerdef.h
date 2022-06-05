
#ifndef TIMERDEF_H
#define TIMERDEF_H 1

#if defined (__AVR_ATmega32__) || defined (__AVR_ATmega323__)

/**
 * The flags for the prescaler in the TCCR1B register.
 * 
 *  CS12  CS11  CS10            prescaler value
 *    0     0     1    (0x01)   CRYSTAL_CLOCK / 1
 *    0     1     0    (0x02)   CRYSTAL_CLOCK / 8
 *    0     1     1    (0x03)   CRYSTAL_CLOCK / 64
 *    1     0     0    (0x04)   CRYSTAL_CLOCK / 256
 *    1     0     1    (0x05)   CRYSTAL_CLOCK / 1024
 * 
 * This value defines the possible range for the timer tick (HZ).
 */
// set WGM12 for Clear Timer on Compare match (CTC) mode
#  define TIMER_CONFIG_VALUE           _BV(WGM12) | 0x04
#  define TIMER_CONFIG_REG             TCCR1B

/**
 * The resulting value for the presacler.
 */
#  define TIMER_PRESCALER_VALUE        256

#  define TIMER_COUNTER_VALUE          ((CRYSTAL_CLOCK / TIMER_PRESCALER_VALUE) / HZ)
#  define TIMER_COUNTER_REG            OCR1A

#  define TIMER_INTERRUPT_REG          TIMSK
#  define TIMER_INTERRUPT_ENABLE_BIT   OCIE1A

#else

#  error no timer configuration for this MCU type

#endif


#endif
