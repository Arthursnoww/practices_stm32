# STM32 Practices - Discipline of Microcontrollers

This repository contains a series of practical exercises developed using the **STM32F103** microcontroller. Each practice aims to deepen the understanding of embedded systems, covering topics such as GPIO control, external interruptions, timers, PWM, ADC, and communication protocols (USART).

## About STM32F103
The **STM32F103** is part of the STM32 family, based on the ARM Cortex-M3 core. It is widely used in embedded systems due to its balance between performance, power efficiency, and versatility. Some key features of the STM32F103 include:
- **ARM Cortex-M3 32-bit RISC core**, running at up to 72 MHz.
- Up to **128 KB of Flash memory** and **20 KB of SRAM**.
- Rich set of peripherals including **GPIOs**, **timers**, **ADCs**, **DACs**, **PWMs**, and **USART** for communication.
- **Low-power modes** and advanced clock management, making it suitable for energy-efficient applications.
- Support for **real-time applications** with built-in timers and interrupt management.

## Practical Activities

### Practice 1 - GPIO Control
**Objective**: Connect 3 push buttons and 3 LEDs to the STM32F103. Each push button should control a different LED using polling.

**Implementation**: Basic GPIO control, where each button press is detected through polling, and the corresponding LED is toggled.

### Practice 2 - External Interrupts
**Objective**: Using 3 push buttons and an RGB LED (or 3 separate LEDs), change the state of each LED (XOR logic) by using external interrupts.

**Implementation**: This practice introduces external interrupt handling, where each button press triggers an interrupt that toggles the state of the associated LED.

### Practice 3 - SysTick
**Objective**: Use the SysTick timer to cycle through all color combinations of an RGB LED every 1 second. The sequence includes: {Off, Red, Green, Blue, Yellow, Cyan, Purple, White}.

**Implementation**: The SysTick is configured to generate periodic interrupts, changing the LED state at 1-second intervals.

### Practice 4 - Timers
**Objective**: Use the microcontroller's timer to blink each color combination of an RGB LED twice in the following sequence: {Red, Green, Blue, Yellow, Cyan, Purple, White}.

**Implementation**: A timer is used to control the LED blinking pattern, with precise timing between each change.

### Practice 5 - PWM (Pulse Width Modulation)
**Objective**: Gradually increase and decrease the brightness of each color combination of an RGB LED using PWM. The colors should cycle through: {Red, Green, Blue, Yellow, Cyan, Purple, White}. Another timer is used to vary the PWM duty cycle.

**Implementation**: PWM is utilized for controlling LED brightness without using delays, ensuring smooth transitions between colors.

### Practice 6 - ADC (Analog-to-Digital Conversion)
**Objective**: Use 3 separate ADC channels to control the brightness of the primary colors in an RGB LED.

**Implementation**: PWM is used to adjust the brightness, while the ADC readings determine the duty cycle. The DMA and SCAN mode of the ADC are employed to automate the data acquisition.

### Practice 7 - USART Control
**Objective**: Control the PWM duty cycle for an RGB LED through commands received via USART. The STM32 should recognize the following commands:
1. **rXX\n**, **gXX\n**, **bXX\n** - Adjust the PWM duty cycle for the Red, Green, and Blue channels respectively.
2. **d\n** - Toggle demonstration mode, which simulates the behavior described in Practice 5.

**Implementation**: USART is used to receive commands from a PC or other device. The system enters a demonstration mode where the LED cycles through colors, ignoring other commands while in this mode.

### Conclusion
These practical activities demonstrate the power and versatility of the STM32F103 microcontroller, using its GPIO, timers, ADC, PWM, and communication peripherals to handle various tasks in real-time applications. Each practice builds on fundamental concepts in embedded systems programming, providing a solid foundation for more advanced development.

---

**Note**: Ensure that you have the necessary hardware components (e.g., push buttons, LEDs, resistors, etc.) and the STM32 development environment set up before proceeding with each practice.


