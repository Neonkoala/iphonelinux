#include <openiboot.h>
#include <clock.h>
#include <vibrator.h>
#ifdef CONFIG_IPHONE
#include <util.h>
#include <radio.h>
#endif
#ifdef CONFIG_3G
#include <timer.h>
#include <hardware/timer.h>
#endif

#ifdef CONFIG_IPHONE
void vibrator_loop(int frequency, int period, int timeOn)
{
	char buf[100];
	sprintf(buf, "at+xdrv=4,0,2,%d,%d,%d\r\n", frequency, period, timeOn);

	// write the command
	radio_write(buf);

	// clear the response
	radio_read(buf, sizeof(buf));
}

void vibrator_once(int frequency, int time)
{
	char buf[100];
	sprintf(buf, "at+xdrv=4,0,1,%d,%d,%d\r\n", frequency, time + 1, time);

	// write the command
	radio_write(buf);

	// clear the response
	radio_read(buf, sizeof(buf));
}

void vibrator_off()
{
	char buf[100];

	// write the command
	radio_write("at+xdrv=4,0,0,0,0,0\r\n");

	// clear the response
	radio_read(buf, sizeof(buf));
}
#endif

#ifdef CONFIG_3G
void vibrator_loop(int frequency, int period, int time)
{
	// The second option represents the duty cycle of the PWM that drives the vibrator.
	if(frequency > 0)
	{
		int prescaler = 1;
		uint32_t count = TicksPerSec / frequency;

		while(count > 0xFFFF)
		{
			count >>= 1;
			prescaler <<= 1;
		}

		uint32_t countOn = (count * (100 - period))/100;

		timer_init(VibratorTimer, countOn, count, prescaler - 1, 0, FALSE, FALSE, FALSE, FALSE);
		timer_on_off(VibratorTimer, ON);
	}

	udelay(time);

	if(frequency > 0)
		vibrator_off();
}

void vibrator_once(int time) {
	// init the timer
	timer_init(VibratorTimer, 0, 1, 0, 0, FALSE, FALSE, FALSE, FALSE);

	// turn timer on, wait, turn it off
	timer_on_off(VibratorTimer, ON);
	udelay(time);
	vibrator_off();
}

void vibrator_off() {
	// init the timer for turning off and turning it off
	timer_init(VibratorTimer, 0, 1, 0, 0, FALSE, FALSE, TRUE, FALSE);
	timer_on_off(VibratorTimer, ON);
	timer_on_off(VibratorTimer, OFF);
}
#endif
