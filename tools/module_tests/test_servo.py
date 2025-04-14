import gpiod
import time

# Settings
CHIP_NAME = "gpiochip4"  # Check out by gpioinfo
LINE_OFFSET = 18         # BCM GPIO18
PERIOD = 0.02            # 20ms

# Angle to pulse
PULSE_MIN = 0.0005       # 0.5ms
PULSE_MAX = 0.0025       # 2.5ms

def angle_to_pulse(angle):
    return PULSE_MIN + (PULSE_MAX - PULSE_MIN) * (angle / 180.0)

def set_servo(line, pulse_width):
    line.set_value(1)
    time.sleep(pulse_width)
    line.set_value(0)
    time.sleep(PERIOD - pulse_width)

def test_servo():
    chip = gpiod.Chip(CHIP_NAME)
    line = chip.get_line(LINE_OFFSET)
    line.request(consumer="soft_pwm", type=gpiod.LINE_REQ_DIR_OUT)

    try:
        for angle in [0, 30, 60]:
            print(f"To angle: {angle}")
            pulse = angle_to_pulse(angle)
            for _ in range(50):  # 保持 1 秒
                set_servo(line, pulse)
    finally:
        line.set_value(0)
        line.release()
        print("Test done")

if __name__ == "__main__":
    test_servo()
