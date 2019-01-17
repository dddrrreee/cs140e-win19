#ifndef GPIO_H_INCLUDED
#define GPIO_H_INCLUDED

enum {
  GPIO_FUNC_INPUT   = 0,
  GPIO_FUNC_OUTPUT  = 1,
  GPIO_FUNC_ALT0    = 4,
  GPIO_FUNC_ALT1    = 5,
  GPIO_FUNC_ALT2    = 6,
  GPIO_FUNC_ALT3    = 7,
  GPIO_FUNC_ALT4    = 3,
  GPIO_FUNC_ALT5    = 2,
  GPIO_FUNC_INVALID = 8 // Only returned for invalid pins
};

void gpio_init();
int gpio_set_input(unsigned pin);
int gpio_set_output(unsigned pin);
int gpio_set_function(unsigned pin, unsigned function);

int gpio_set_on(unsigned pin);
int gpio_set_off(unsigned pin);

void gpio_set_pullup(unsigned int pin);
void gpio_set_pulldown(unsigned int pin);

int gpio_broken_example(unsigned pin);
#endif

