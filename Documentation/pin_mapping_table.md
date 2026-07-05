# ATmega32 Pin Mapping Table

**Project:** LCD-Based Menu Navigation and Parameter Setting System
**Microcontroller:** ATmega32
**LCD Mode:** 4-bit (to reduce pin count)

---

## Port C — LCD Control & Data Lines

| ATmega32 Pin | Direction | Net Label | Connected Device | Function |
|:------------:|:---------:|:---------:|:-----------------|:---------|
| PC0 | Output | `LCD_RS` | 16×2 LCD | Register Select (Command / Data) |
| PC1 | Output | `LCD_EN` | 16×2 LCD | Enable pulse |
| PC2 | Output | `LCD_D4` | 16×2 LCD | Data bit 4 |
| PC3 | Output | `LCD_D5` | 16×2 LCD | Data bit 5 |
| PC4 | Output | `LCD_D6` | 16×2 LCD | Data bit 6 |
| PC5 | Output | `LCD_D7` | 16×2 LCD | Data bit 7 |

## Port A — Push-Button Inputs

| ATmega32 Pin | Direction | Net Label | Connected Device | Function |
|:------------:|:---------:|:----------:|:-----------------|:---------|
| PA0 | Input | `BTN_MENU` | Push button | Open main menu |
| PA1 | Input | `BTN_UP` | Push button | Move cursor upward |
| PA2 | Input | `BTN_DOWN` | Push button | Move cursor downward |
| PA3 | Input | `BTN_SELECT` | Push button | Open selected menu item |
| PA4 | Input | `BTN_SAVE` | Push button | Save / increase parameter |

## Port B — LED & Buzzer Outputs

| ATmega32 Pin | Direction | Net Label | Connected Device | Function |
|:------------:|:---------:|:----------:|:-----------------|:---------|
| PB0 | Output | `STATUS_LED` | LED (via resistor) | Status indication |
| PB1 | Output | `MODE_LED` | LED (via resistor) | Mode indication |
| PB2 | Output | `BUZZER` | Buzzer | Confirmation / error beep |

## Support Pins

| ATmega32 Pin | Direction | Net Label | Connected Device | Function |
|:------------:|:---------:|:----------:|:-----------------|:---------|
| RESET | Input | `RESET` | Reset push button + pull-up | Manual MCU reset |
| XTAL1 | Input | `XTAL1` | Crystal oscillator | External clock input |
| XTAL2 | Output | `XTAL2` | Crystal oscillator | External clock output |
| VCC | Power | `VCC` | 5 V supply | Power |
| GND | Power | `GND` | Ground | Ground reference |
| AVCC | Power | `VCC` | 5 V supply | Analog supply |
| VEE (LCD) | Analog | `LCD_VEE` | Potentiometer wiper | LCD contrast adjustment |

---

## Data Direction Register (DDR) Configuration

| Register | Value | Meaning |
|:--------:|:------:|:--------|
| `DDRC` | `0x3F` | PC0–PC5 configured as **outputs** (LCD RS, EN, D4–D7) |
| `DDRA` | `0x00` | PA0–PA4 configured as **inputs** (push buttons) |
| `DDRB` | `0x07` | PB0–PB2 configured as **outputs** (Status LED, Mode LED, Buzzer) |

```c
DDRC = 0x3F;   // PC0-PC5 -> LCD output
DDRA = 0x00;   // PORTA   -> button input
DDRB = 0x07;   // PB0-PB2 -> LED / buzzer output
```

---

## Pin Usage Summary

| Port | Pins Used | Role |
|:----:|:---------:|:-----|
| Port C | PC0–PC5 (6 pins) | LCD control + data (4-bit mode) |
| Port A | PA0–PA4 (5 pins) | Push-button inputs |
| Port B | PB0–PB2 (3 pins) | LEDs and buzzer |

**Total GPIO used:** 14 pins
