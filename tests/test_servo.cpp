#include "rpi_pwm.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Map angle to pulsewidth（850μs -> 0°，2150μs -> 180°）
 */
int angleToPulse(int angle)
{
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;
    return SERVO_PULSE_MIN + (angle * (SERVO_PULSE_MAX - SERVO_PULSE_MIN)) / 180;
}

int main(int argc, char *argv[])
{
    int channel = 2;
    int frequency = 50; // Hz
    if (argc > 1)
    {
        channel = atoi(argv[1]);
    }
    printf("Enabling PWM on channel %d.\n", channel);
    RPI_PWM pwm;
    pwm.start(channel, frequency);

    std::cout << "Enter a servo angle (0-180), or 'q' to quit:" << std::endl;
    std::string input;
    while (true)
    {
        std::cout << "Angle > ";
        std::getline(std::cin, input);
        if (input == "q" || input == "Q")
            break;

        try
        {
            int angle = std::stoi(input);
            if (angle < 0 || angle > 180) {
                std::cout << "Angle out of range. Please enter 0–180." << std::endl;
                continue; // Continue to next input
            }
            int pulse = angleToPulse(angle);
            std::cout << "Angle: " << angle << "°, corresponding pulsewidth: " << pulse << "μs" << std::endl;
            int result = pwm.setPulseWidth(pulse);
            std::cout << (result > 0 ? "Write successful." : "Write failed!") << std::endl;
        }
        catch (...)
        {
            std::cout << "Invalid input. Please enter a number between 0-180, or 'q' to quit." << std::endl;
        }
    }
    pwm.stop();
    std::cout << "PWM stopped, exiting program." << std::endl;
    return 0;
}
