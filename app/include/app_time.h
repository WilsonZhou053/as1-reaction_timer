#ifndef APP_TIME_H
#define APP_TIME_H

//Getting current time (ms)
long long time_getTimeInMs(void);

//Sleep using nanosleep
void time_sleepForMs(long long delayInMs);

#endif