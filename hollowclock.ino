//  hollow clock 4 c++ port of https://www.thingiverse.com/thing:5636482
//  ported the original micropython pico software to c++ with pico sdk
//  modified the tick control so that it is smoother, otherwise pin compatible
//  with the micropython code and original project
//
//  C++ port by James Wilson
//  6th March 2023


#include "uln2003.h"
#include "pico/stdlib.h"

Motor motor;

#define HOUR_STEPS 30720
#define TICK_TIME 117.1875d
#define FFWD 20
#define FBWD 21
#define REV 27

static uint32_t last_ticks = 0;
static uint32_t start_time;

bool check_button(int button)
{
  uint32_t now_time = to_ms_since_boot(get_absolute_time ());
  uint32_t ticks;

  // adjust time with the fast forward/backward button
  if (gpio_get(button) == false)
  {
    //initial step delay of 50ms
    int delay = 50000;
    do
    {
      // step the motor in the forward/backward direction
      if (button == FFWD)
        motor.step(!gpio_get(REV));
      else
        motor.step(gpio_get(REV));

      // wait for some time
      sleep_us(delay);

      // reduce delay by 2% for next step
      delay -= delay / 50;

      // ensure we don't exceed 15rpm on the stepper, this keeps it around 12rpm
      if (delay < 1200)
        delay = 1200;
    } while (gpio_get(button) == false);

    // no idea how long the button has been pressed but it should be correct time
    // so we will discard any ticks missed while the button was pressed
    double diff = double(now_time - start_time); // get the ms since start up

    // how many ticks of the stepper have passed since turn on
    ticks = diff / TICK_TIME;

    // make sure we don't try to adjust for ticks again
    last_ticks = ticks;
    return true;
  }
  return false;
}

void setup()
{
  // the base for counting the number of ticks thats past
  start_time = to_ms_since_boot(get_absolute_time ());

  // setup the gpio we are going to use
  gpio_init(FFWD);
  gpio_set_dir(FFWD, GPIO_IN);
  gpio_pull_up (FFWD);
  gpio_init(FBWD);
  gpio_set_dir(FBWD, GPIO_IN);
  gpio_pull_up (FBWD);
  gpio_init(REV);
  gpio_set_dir(REV, GPIO_IN);
  gpio_pull_up (REV);
}

void loop()
{
  uint32_t now_time = to_ms_since_boot(get_absolute_time ());
  uint32_t ticks;

  // get number of ms since we started checking
  double diff = double(now_time - start_time); // get the ms since start up

  // how many ticks of the stepper have passed since turn on
  ticks = diff / TICK_TIME;

  // time wrapped round ?
  if (ticks < last_ticks)
  {
    last_ticks = 0;
    if (ticks == 0)
      motor.step(false); // otherwise we will lose 1 tick every 49 days (maybe ?)
  }

  // stop powering the stepper, keeps it cool
  motor.halt();

  // adjust time with the fast forward button
  if (check_button(FFWD))
    return;

  // adjust time with the fast backward button
  if (check_button(FBWD))
    return;

  // how many ticks did we miss should normally be a max of 1
  int move_by = ticks - last_ticks;

  for (int i = 0; i < move_by; i++)
    motor.step(!gpio_get(REV));

  last_ticks = ticks;

  sleep_ms(10);

}
