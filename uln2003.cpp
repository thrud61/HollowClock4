// cut down class to use uln2003 single steps

#include "uln2003.h"

#include "pico/stdlib.h"

#define sign(a) (a<0?-1:1)

Motor::Motor()
{
  // set them to output
  for (int pin : motor_pins)
  {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }

  // set non-energised state
  halt();
}


void Motor::halt()
{
//  sleep_ms(100); // wait 100ms to give the move time to notice if its async
  for (int pin : motor_pins)
  {
    gpio_put(pin, 0);
  }
}

// take a single step
void Motor::step(bool dir)
{
  int seq = position % 8;
  int i;

  if (dir)
  {
    position --;
    if (position < 0)
      position = 7;
  }
  else
    position ++;
  position = position % 8;
  
  for (i = 0; i < 4; i++)
  {
    gpio_put(motor_pins[i], sequence[seq][i]);
  }
}
