# STM32G474RET6 Nucleo - LED Blink Projects

## Overview
Two separate projects using the STM32G474RET6 Nucleo development board, each blinking an LED using a different HAL GPIO function:
1. **Onboard LED Blink** - using `HAL_GPIO_TogglePin`
2. **Breadboard LED Blink** - using `HAL_GPIO_WritePin`

## Hardware Used
- STM32G474RET6 Nucleo board
- Breadboard
- 1x external LED
- 1x current-limiting resistor (e.g. 220Ω)
- Jumper wires

## STM32CubeMX Configuration
Both pins were configured as **GPIO_Output** in STM32CubeMX.

<img width="424" height="294" alt="image" src="https://github.com/user-attachments/assets/86184dbc-eb1b-4b56-86c6-260897984389" />
this  given screenshot is for the onboard led blink
<img width="362" height="299" alt="Screenshot 2026-08-26 214138" src="https://github.com/user-attachments/assets/73472058-3963-496d-a5a2-607838d53136" />
this given screenshot is the pin config for the external led



---

## Project 1: Onboard LED Blink

**Pin used:** PA5

Uses `HAL_GPIO_TogglePin` to flip the pin state each loop iteration.

```c
while (1)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    HAL_Delay(500);
}
```

No external wiring needed - the LED is built into the Nucleo board.

---

## Project 2: Breadboard LED Blink

**Pin used:** PC7

Uses `HAL_GPIO_WritePin` to explicitly set the pin HIGH and LOW.

```c
while (1)
{//created my own PWM, you can also use the PWM function given in the pin
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
    HAL_Delay(500);

}
```

**Circuit Setup:**
PC7 -> led +ve ->
led -ve -> resistor ->
resistor -> gnd 

---

## Tools Used
- STM32CubeMX
- STM32CubeIDE
- STM32 HAL Library

## Author
Manas
