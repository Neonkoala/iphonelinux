#include <openiboot.h>
#include <timer.h>
#include <radio.h>
#include <clock.h>
#include <hardware/timer.h>

void vibrator_loop(int frequency, int period, int timeOn)
{
#ifndef CONFIG_3G
       char buf[100];
       sprintf(buf, "at+xdrv=4,0,2,%d,%d,%d\r\n", frequency, period, timeOn);

       // write the command
       radio_write(buf);

       // clear the response
       radio_read(buf, sizeof(buf));
#else
#endif
}

void vibrator_once(int frequency, int time)
{
#ifndef CONFIG_3G
       char buf[100];
       sprintf(buf, "at+xdrv=4,0,1,%d,%d,%d\r\n", frequency, time + 1, time);

       // write the command
       radio_write(buf);

       // clear the response
       radio_read(buf, sizeof(buf));
#else
	// The second option represents the duty cycle of the PWM that drives the buzzer. In this case it is always 50%.
	if(frequency > 0)
	{
		int prescaler = 1;
		uint32_t count = TicksPerSec / frequency;

		while(count > 0xFFFF)
		{
			count >>= 1;
			prescaler <<= 1;
		}

		timer_init(VibratorTimer, count >> 1, count, prescaler - 1, 0, FALSE, FALSE, FALSE, FALSE);
		timer_on_off(VibratorTimer, ON);
	}

	udelay(time);

	if(frequency > 0)
		timer_on_off(VibratorTimer, OFF);
#endif
}

void vibrator_off()
{
#ifndef CONFIG_3G
       char buf[100];

       // write the command
       radio_write("at+xdrv=4,0,0,0,0,0\r\n");

       // clear the response
       radio_read(buf, sizeof(buf));
#else
#endif
}

