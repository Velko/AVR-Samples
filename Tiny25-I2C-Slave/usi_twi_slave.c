/* USI TWI slave receiver/transmitter.
 *
 * Based on AppNote AVR312 - Using the USI module as a I2C slave,
 * Adjusted according to: http://www.aca-vogel.de/TINYUSII2C_AVR312/APN_TINYUSI_I2C.html
 *
 ***************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usi_twi_slave.h"

//********** Static Variables **********//

uint8_t uts_slaveAddress;

#define TWI_RX_BUFFER_SIZE  2

uint8_t          uts_rxBuf[TWI_RX_BUFFER_SIZE];
volatile uint8_t uts_rxCnt;
volatile uint8_t uts_txBuf;


enum UsiSlaveState {
    USI_SLAVE_CHECK_ADDRESS,
    USI_SLAVE_SEND_DATA,
    USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA,
    USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA,
    USI_SLAVE_REQUEST_DATA,
    USI_SLAVE_GET_DATA_AND_SEND_ACK
};

static volatile enum UsiSlaveState usi_slave_state;


#define DDR_USI DDRB
#define PORT_USI PORTB
#define PIN_USI PINB
#define PORT_USI_SDA PORTB0
#define PORT_USI_SCL PORTB2
#define PIN_USI_SDA PINB0
#define PIN_USI_SCL PINB2
#define USI_START_COND_INT USISIF
#define TIMER_INT_MASK_REG TIMSK
#define OUTPUT_COMPARE_REG OCR0A
#define TIMER_CONTROL_REG TCCR0B
#define CLK_SELECT_PRESC_8 CS01
#define OUTPUT_COMP_INT_EN OCIE0A
#define USI_START_VECTOR USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect
#define TIMER_COMP_VECTOR TIM0_COMPA_vect

static void SET_USI_TO_SEND_ACK(void)
{
    USIDR = 0;                      /* Prepare ACK                         */
    DDR_USI |= (1 << PORT_USI_SDA); /* Set SDA as output                   */
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)
            |                  /* Clear all flags, except Start Cond  */
            (0x0E << USICNT0); /* set USI counter to shift 1 bit. */
}

static void SET_USI_TO_READ_ACK(void)
{
    DDR_USI &= ~(1 << PORT_USI_SDA); /* Set SDA as input */
    USIDR = 0;                       /* Prepare ACK        */
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)
            |                  /* Clear all flags, except Start Cond  */
            (0x0E << USICNT0); /* set USI counter to shift 1 bit. */
}

static void SET_USI_TO_TWI_START_CONDITION_MODE(void)
{
    USICR = (1 << USISIE) | (0 << USIOIE) | /* Enable Start Condition Interrupt. Disable Overflow Interrupt.*/
            (1 << USIWM1) | (0 << USIWM0) | /* Set USI in Two-wire mode. No USI Counter overflow hold.      */
            (1 << USICS1) | (0 << USICS0) | (0 << USICLK)
            | /* Shift Register Clock Source = External, positive edge        */
            (0 << USITC);
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)
            | /* Clear all flags, except Start Cond                            */
            (0x0 << USICNT0);
}

static void SET_USI_TO_SEND_DATA(void)
{
    DDR_USI |= (1 << PORT_USI_SDA); /* Set SDA as output                  */
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)
            |                 /* Clear all flags, except Start Cond */
            (0x0 << USICNT0); /* set USI to shift out 8 bits        */
}

static void SET_USI_TO_READ_DATA(void)
{
    DDR_USI &= ~(1 << PORT_USI_SDA); /* Set SDA as input                   */
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)
            |                 /* Clear all flags, except Start Cond */
            (0x0 << USICNT0); /* set USI to shift out 8 bits        */
}




//********** USI_TWI functions **********//

/*----------------------------------------------------------
  Initialise USI for TWI Slave mode.
----------------------------------------------------------*/
void uts_init(void)
{
    uts_rxCnt = 0;

    PORT_USI |= (1 << PORT_USI_SCL);        // Set SCL high
    PORT_USI |= (1 << PORT_USI_SDA);        // Set SDA high
    DDR_USI |= (1 << PORT_USI_SCL);         // Set SCL as output
    DDR_USI &= ~(1 << PORT_USI_SDA);        // Set SDA as input
    USICR = (1 << USISIE) | (0 << USIOIE) | // Enable Start Condition Interrupt. Disable Overflow Interrupt.
            (1 << USIWM1) | (0 << USIWM0) | // Set USI in Two-wire mode. No USI Counter overflow prior
                                            // to first Start Condition (potentail failure)
            (1 << USICS1) | (0 << USICS0) | (0 << USICLK) | // Shift Register Clock Source = External, positive edge
            (0 << USITC);
    USISR = 0xF0; // Clear all flags and reset overflow counter
}

/*----------------------------------------------------------
 Detects the USI_TWI Start Condition and intialises the USI
 for reception of the "TWI Address" packet.
----------------------------------------------------------*/

ISR(USI_START_vect)
{
    // Set default starting conditions for new TWI package
    usi_slave_state = USI_SLAVE_CHECK_ADDRESS;
    DDR_USI  &= ~(1<<PORT_USI_SDA);                                 // Set SDA as input

    while ( (PIN_USI & (1<<PORT_USI_SCL)) && (!(PIN_USI & (1<<PORT_USI_SDA))));

    if (!( PIN_USI & ( 1 << PIN_USI_SDA )))
    {
        // NO Stop
        USICR = (1<<USISIE)|(1<<USIOIE)|              // Enable Overflow and Start Condition Interrupt. (Keep StartCondInt to detect RESTART)
                (1<<USIWM0)|(1<<USIWM1)|              // Set USI in Two-wire mode.
                (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|  // Shift Register Clock Source = External, positive edge
                (0<<USITC);
    }
    else
    {
        // STOP
        USICR = (1<<USISIE)|(0<<USIOIE)|              // Enable Overflow and Start Condition Interrupt. (Keep StartCondInt to detect RESTART)
                (1<<USIWM1)|(0<<USIWM1)|              // Set USI in Two-wire mode.
                (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|  // Shift Register Clock Source = External, positive edge
                (0<<USITC);
    }

    USISR = (1<<USI_START_COND_INT)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC) |   // Clear flags
            (0x0<<USICNT0);                                               // Set USI to sample 8 bits i.e. count 16 external pin toggles.    

    uts_rxCnt = 0;
}

/*----------------------------------------------------------
 Handles all the comunication. Is disabled only when waiting
 for new Start Condition.
----------------------------------------------------------*/
ISR(USI_OVF_vect)
{
    switch (usi_slave_state) {
    // ---------- Address mode ----------
    // Check address and send ACK (and next USI_SLAVE_SEND_DATA) if OK, else reset USI.
    case USI_SLAVE_CHECK_ADDRESS:
        if ((USIDR == 0) || (( USIDR>>1 ) == uts_slaveAddress))
        {
            if ( USIDR & 0x01 )
            {
                usi_slave_state = USI_SLAVE_SEND_DATA;
            }
            else
            {
                usi_slave_state = USI_SLAVE_REQUEST_DATA;
            }
            SET_USI_TO_SEND_ACK();
        }
        else
        {
            SET_USI_TO_TWI_START_CONDITION_MODE();          // not my Adress.....
        }
        break;

    // ----- Master write data mode ------
    // Check reply and goto USI_SLAVE_SEND_DATA if OK, else reset USI.
    case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
        if (USIDR) // If NACK, the master does not want more data.
        {
            SET_USI_TO_TWI_START_CONDITION_MODE();
            return;
        }
    // From here we just drop straight into USI_SLAVE_SEND_DATA if the master sent an ACK

    // Copy data from buffer to USIDR and set USI to shift byte. Next USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA
    case USI_SLAVE_SEND_DATA:

        // Send whatever data is in Buffer
        USIDR      = uts_txBuf;

        usi_slave_state = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
        SET_USI_TO_SEND_DATA();
        break;

    // Set USI to sample reply from master. Next USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA
    case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA:
        usi_slave_state = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
        SET_USI_TO_READ_ACK();
        break;

    // ----- Master read data mode ------
    // Set USI to sample data from master. Next USI_SLAVE_GET_DATA_AND_SEND_ACK.
    case USI_SLAVE_REQUEST_DATA:
        usi_slave_state = USI_SLAVE_GET_DATA_AND_SEND_ACK;
        SET_USI_TO_READ_DATA();
        break;

    // Copy data from USIDR and send ACK. Next USI_SLAVE_REQUEST_DATA
    case USI_SLAVE_GET_DATA_AND_SEND_ACK:
        // Put data into Buffer
        if (uts_rxCnt < TWI_RX_BUFFER_SIZE)
            uts_rxBuf[uts_rxCnt++] = USIDR;

        usi_slave_state = USI_SLAVE_REQUEST_DATA;
        SET_USI_TO_SEND_ACK();
        break;
    }
}
