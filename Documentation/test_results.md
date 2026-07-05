# Test Results — ATmega32 LCD Menu System

**Project:** LCD-Based Menu Navigation and Parameter Setting System
**Simulation Tool:** SimulIDE
**Test Date:** _______________
**Tested By:** _______________

---

## 1. Functional Test Cases

| # | Test Case | Input / User Action | Expected Output | Observed Output | Result | Screenshot |
|:-:|:----------|:--------------------|:----------------|:----------------|:------:|:----------:|
| 1 | Power-on startup | System powered ON | LCD displays welcome message | | ☐ Pass / ☐ Fail | `startup.png` |
| 2 | Open main menu | Menu button pressed | Main menu (`MENU: 1.SET LIMIT`) is displayed | | ☐ Pass / ☐ Fail | `main_menu.png` |
| 3 | Navigate up | Up button pressed | Menu cursor moves upward | | ☐ Pass / ☐ Fail | `nav_up.png` |
| 4 | Navigate down | Down button pressed | Menu cursor moves downward | | ☐ Pass / ☐ Fail | `nav_down.png` |
| 5 | Select item | Select button pressed | Selected parameter screen opens (`TEMP LIMIT: 35 C`) | | ☐ Pass / ☐ Fail | `param_screen.png` |
| 6 | Adjust parameter | Increase button pressed | Parameter value increases | | ☐ Pass / ☐ Fail | `param_change.png` |
| 7 | Save parameter | Save button pressed | LCD displays `VALUE SAVED` | | ☐ Pass / ☐ Fail | `value_saved.png` |
| 8 | Invalid operation | Invalid button action | Buzzer gives short error beep | | ☐ Pass / ☐ Fail | `error_beep.png` |
| 9 | Reset system | Reset button pressed | System returns to welcome screen | | ☐ Pass / ☐ Fail | `reset.png` |

---

## 2. Peripheral / Output Verification

| # | Peripheral | Test Condition | Expected Behavior | Result |
|:-:|:-----------|:---------------|:------------------|:------:|
| 1 | 16×2 LCD | Power on | Characters visible, contrast correct | ☐ Pass / ☐ Fail |
| 2 | LCD 4-bit mode | Data transfer | Correct characters displayed (no garbage) | ☐ Pass / ☐ Fail |
| 3 | Status LED (PB0) | During operation | Lights on active state | ☐ Pass / ☐ Fail |
| 4 | Mode LED (PB1) | On parameter screen | Indicates edit/mode correctly | ☐ Pass / ☐ Fail |
| 5 | Buzzer (PB2) | Save confirm | Short confirmation beep | ☐ Pass / ☐ Fail |
| 6 | Buzzer (PB2) | Invalid op | Short error beep | ☐ Pass / ☐ Fail |
| 7 | Contrast pot | Adjust wiper | LCD contrast changes smoothly | ☐ Pass / ☐ Fail |

---

## 3. Button Debounce Test

| Button | Net Label | Rapid Press Test | Single Response? | Result |
|:-------|:----------|:-----------------|:----------------:|:------:|
| Menu | `BTN_MENU` | Pressed 5× quickly | | ☐ Pass / ☐ Fail |
| Up | `BTN_UP` | Pressed 5× quickly | | ☐ Pass / ☐ Fail |
| Down | `BTN_DOWN` | Pressed 5× quickly | | ☐ Pass / ☐ Fail |
| Select | `BTN_SELECT` | Pressed 5× quickly | | ☐ Pass / ☐ Fail |
| Save/Increase | `BTN_SAVE` | Pressed 5× quickly | | ☐ Pass / ☐ Fail |

---

## 4. Menu State Transition Test

| From State | Action | To State | Result |
|:-----------|:-------|:---------|:------:|
| Welcome | Menu button | Main Menu | ☐ Pass / ☐ Fail |
| Main Menu | Up / Down | Cursor moves | ☐ Pass / ☐ Fail |
| Main Menu | Select | Parameter Screen | ☐ Pass / ☐ Fail |
| Parameter Screen | Increase | Value updates | ☐ Pass / ☐ Fail |
| Parameter Screen | Save | Save Screen (`VALUE SAVED`) | ☐ Pass / ☐ Fail |
| Any state | Invalid op | Error beep, no state change | ☐ Pass / ☐ Fail |
| Any state | Reset | Welcome Screen | ☐ Pass / ☐ Fail |

---

## 5. Extension Test (Optional — EEPROM Storage)

| # | Test Case | Action | Expected Output | Result |
|:-:|:----------|:-------|:----------------|:------:|
| 1 | Save to EEPROM | Save parameter, then reset | Saved value is retained after reset | ☐ Pass / ☐ Fail |
| 2 | Power cycle retention | Power off then on | Last saved value is restored | ☐ Pass / ☐ Fail |

---

## 6. Summary

| Metric | Count |
|:-------|:-----:|
| Total test cases | |
| Passed | |
| Failed | |
| Pass rate (%) | |

**Overall Status:** ☐ All tests passed  ☐ Issues found (see notes)

### Notes / Observations
```
_______________________________________________________________

_______________________________________________________________

_______________________________________________________________
```

---

> **Instructions:** Capture a SimulIDE screenshot for each test case listed above,
> save them in `simulide/screenshots/`, and reference the filenames in the
> "Screenshot" column. Fill in the "Observed Output" and mark Pass/Fail
> after running each test.
