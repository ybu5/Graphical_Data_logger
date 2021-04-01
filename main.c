#include "msp.h"
#include "i2c.h"
#include "lcd.h"

#define TICKS_PER_100MS 37500
#define TWICE 2
#define MAX_LIGHT 83865.6
#define MIN_LIGHT 40.95
#define RATIO 0.001515
/**
 * main.c
 */

double raw_temp_data = 0;
double raw_light_data = 0;
int x_position = 127;
int light_data = 0;
int temp_data = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    i2cInit();

    i2cWrite(OPT3001_SLAVE_ADDRESS, OPT3001_CONFIG_REG, OPT3001_CONFIG_100);

    lcdInit();

    //enable timer A0 in continuous mode with SMCLK divided by 4
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP
            | TIMER_A_CTL_ID__8 | TIMER_A_CTL_IE;

    //set the first interrupt interval to 1.9ms
    TIMER_A2->CCR[0] = TICKS_PER_100MS - 1;

    //enable NVIC for timer A0 channel 0
    NVIC_EnableIRQ(TA2_N_IRQn);

    //global interrupt enable
    __enable_interrupts();

    while (1)
    {
        //light_data = (raw_light_data - MIN_LIGHT)
               // * (LCD_MAX_Y / (MAX_LIGHT - MIN_LIGHT));
        //temp_data = raw_temp_data / 145.0;
        light_data=(raw_light_data-MIN_LIGHT)*RATIO;
        lcdSetPixel(x_position, light_data, RED);
        lcdSetPixel(x_position, raw_temp_data, BLUE);

        if (x_position == 0)
            {
                x_position = 127;
                lcdClear(BLACK);
            }

    }
}

void TA2_N_IRQHandler(void)
{
    static int count = 0;
    //clear flag
    TIMER_A2->CTL &= ~TIMER_A_CTL_IFG;
    TIMER_A2->CTL &= ~TIMER_A_CTL_IFG;

    count++;

    if (count == TWICE)
    {

        x_position--;

        raw_temp_data = (double) i2cRead(TMP006_SLAVE_ADDRESS,
                                         TMP006_TEMPERATURE_REG) / 145.0;
        raw_light_data = (double) i2cRead(OPT3001_SLAVE_ADDRESS,
                                          OPT3001_RESULT_REG);
        count = 0;
    }

}

