#include "rpi_pwm.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int channel = 2;
    int frequency = 50; // Hz
    if (argc > 1) {
        channel = atoi(argv[1]);
    }
    printf("Enabling PWM on channel %d.\n",channel);
    RPI_PWM pwm;
    pwm.start(channel, frequency);

    printf("Angle: 0°\n");
    pwm.setPulseWidth(SERVO_PULSE_MIN);
    getchar();

    printf("Angle: 90°\n");
    pwm.setPulseWidth(SERVO_PULSE_CENTER);
    getchar();

    printf("Angle: 180°\n");
    pwm.setPulseWidth(SERVO_PULSE_MAX);
    getchar();

    printf("Back to 90°\n");
    pwm.setPulseWidth(SERVO_PULSE_CENTER);
    getchar();

    pwm.stop();
}
