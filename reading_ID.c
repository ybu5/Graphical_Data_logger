#include "msp.h"
#include "i2c.h"

/**
 * main.c
 */
void main(void)
{
    int read_data = 0;
    int read_data2 = 0;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    i2cInit();

    while (1)
    {
        read_data = i2cRead(TMP006_SLAVE_ADDRESS, TMP006_DEVICE_ID_REG);
        read_data2 = i2cRead(OPT3001_SLAVE_ADDRESS, OPT3001_DEVICE_ID_REG);
    }
}
