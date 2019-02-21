int echo_until_fn(int fd, const char *msg, int (*fn)(int c));
int echo_until(int fd, const char *msg);
