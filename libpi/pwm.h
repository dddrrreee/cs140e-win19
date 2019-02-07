// put N [off,on] decisions in pwm_choices.   error should be 
// minimal.
// 	- <pwm_choices> should be able to hold N entries.
 
void pwm_compute(unsigned *pwm_choices, unsigned on, unsigned n);
void pwm_print(unsigned *pwm_choices, unsigned on, unsigned n);
