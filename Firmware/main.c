#ifndef F_CPU
#define F_CPU 8000000UL // 8MHz Internal or External Oscillator
#endif

#include <avr/io.h>
#include <util/delay.h>

// --- LCD PIN DEFINITIONS (PORTC) ---
#define LCD_DIR      DDRC
#define LCD_PORT     PORTC
#define RS           PC0
#define EN           PC1
// LCD D4-D7 lines are mapped to PC2-PC5

// --- BUTTON PIN DEFINITIONS (PORTA) ---
#define BUTTON_PIN   PINA
#define BTN_MENU     PA0
#define BTN_UP       PA1
#define BTN_DOWN     PA2
#define BTN_SELECT   PA3
#define BTN_SAVE     PA4

// --- PERIPHERAL PIN DEFINITIONS (PORTB) ---
#define LED_DIR      DDRB
#define LED_PORT     PORTB
#define LED_STATUS   PB0
#define LED_MODE     PB1
#define BUZZER       PB2

// --- SYSTEM STATE MACHINE ---
typedef enum {
    STATE_WELCOME,
    STATE_MENU_NAV,
    STATE_PARAM_EDIT
} SystemState;

// --- MENU PARAMETER STRUCTURE ---
typedef struct {
    char name[13];
    int value;
    int min_val;
    int max_val;
} Parameter;

// Preloaded sample parameters for user interaction
Parameter menu_list[] = {
    {"1. Temp Set ", 24, 16, 30},
    {"2. Humid Set", 55, 20, 90},
    {"3. Limit Set", 10, 0,  100}
};
#define TOTAL_PARAMS (sizeof(menu_list) / sizeof(menu_list[0]))

// --- LCD DRIVER FUNCTIONS (4-BIT MODE) ---
void lcd_command(unsigned char cmd) {
    // Send High Nibble (D7-D4) shifted to match PC2-PC5
    LCD_PORT = (LCD_PORT & 0x03) | ((cmd & 0xF0) >> 2);
    LCD_PORT &= ~(1 << RS); // RS = 0 for instruction
    LCD_PORT |= (1 << EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << EN);
    _delay_us(200);

    // Send Low Nibble (D3-D0) shifted to match PC2-PC5
    LCD_PORT = (LCD_PORT & 0x03) | ((cmd & 0x0F) << 2);
    LCD_PORT |= (1 << EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << EN);
    _delay_ms(2);
}

void lcd_char(unsigned char data) {
    // Send High Nibble
    LCD_PORT = (LCD_PORT & 0x03) | ((data & 0xF0) >> 2);
    LCD_PORT |= (1 << RS); // RS = 1 for data
    LCD_PORT |= (1 << EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << EN);
    _delay_us(200);

    // Send Low Nibble
    LCD_PORT = (LCD_PORT & 0x03) | ((data & 0x0F) << 2);
    LCD_PORT |= (1 << EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << EN);
    _delay_ms(2);
}

void lcd_init(void) {
    _delay_ms(20); // Wait for LCD power-up stabilization

    // Force initialization sequence into 4-bit mode
    LCD_PORT = (LCD_PORT & 0x03) | (0x20 >> 2);
    LCD_PORT &= ~(1 << RS);
    LCD_PORT |= (1 << EN); _delay_us(1); LCD_PORT &= ~(1 << EN);
    _delay_ms(5);

    lcd_command(0x28); // 4-bit mode, 2 lines, 5x7 matrix font
    lcd_command(0x0C); // Display ON, Cursor OFF
    lcd_command(0x06); // Automatic entry mode shift (Increment)
    lcd_command(0x01); // Clear Display RAM
    _delay_ms(2);
}

void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_command(pos);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_char(*str++);
    }
}

// --- INTEGER TO LCD (replaces sprintf, which often fails on AVR/XC8) ---
// Prints a signed integer directly to the LCD at the current cursor.
void lcd_print_int(int num) {
    char buffer[7];      // enough for -32768 + null
    unsigned char i = 0;
    unsigned char j;
    unsigned int n;      // work in unsigned to handle the digits safely

    if (num < 0) {
        lcd_char('-');
        n = (unsigned int)(-num);
    } else {
        n = (unsigned int)num;
    }

    // Build digits in reverse
    if (n == 0) {
        buffer[i++] = '0';
    } else {
        while (n > 0) {
            buffer[i++] = (char)('0' + (n % 10));
            n /= 10;
        }
    }

    // Print in correct order
    for (j = i; j > 0; j--) {
        lcd_char(buffer[j - 1]);
    }
}

// --- HARDWARE INITIALIZATION & PERIPHERALS ---
void buzzer_beep(unsigned int duration_ms) {
    // 1kHz tone: 500us HIGH + 500us LOW per cycle
    unsigned int cycles = duration_ms;

    for (unsigned int i = 0; i < cycles; i++) {
        LED_PORT |= (1 << BUZZER);  // Turn buzzer ON
        _delay_us(500);             // Half-period delay

        LED_PORT &= ~(1 << BUZZER); // Turn buzzer OFF
        _delay_us(500);             // Half-period delay
    }
}

void setup_io(void) {
    // --- DISABLE ADC PERIPHERAL ---
    // Force PORTA to operate strictly as digital pins.
    ADCSRA &= ~(1 << ADEN);

    // Configure LCD control & data pins as outputs (PC0 - PC5)
    LCD_DIR |= 0x3F;

    // Configure Status LED, Mode LED, and Buzzer as outputs (PB0, PB1, PB2)
    LED_DIR |= (1 << LED_STATUS) | (1 << LED_MODE) | (1 << BUZZER);
    LED_PORT &= ~((1 << LED_STATUS) | (1 << LED_MODE) | (1 << BUZZER));

    // Configure Menu/Navigation keys as inputs (PA0 - PA4)
    DDRA &= ~0x1F;
    PORTA |= 0x1F; // Enable internal pull-ups (Active Low buttons)
}

// --- BUTTON HELPER: debounced, edge-detected press ---
// Returns 1 exactly once per physical press, then waits for release.
unsigned char button_pressed(unsigned char pin) {
    if (!(BUTTON_PIN & (1 << pin))) {       // Button appears pressed (LOW)
        _delay_ms(50);                      // Debounce delay
        if (!(BUTTON_PIN & (1 << pin))) {   // Still pressed -> confirmed
            while (!(BUTTON_PIN & (1 << pin))); // Wait for release
            _delay_ms(20);                  // Debounce on release
            return 1;
        }
    }
    return 0;
}

// --- SCREEN DRAWING HELPERS ---
void show_welcome(void) {
    lcd_command(0x01);
    lcd_set_cursor(0, 4);
    lcd_print("WELCOME");
    lcd_set_cursor(1, 1);
    lcd_print("Press Menu Key");
}

void show_menu(int selection) {
    lcd_command(0x01);
    lcd_set_cursor(0, 0);
    lcd_print("---MAIN MENU---");
    lcd_set_cursor(1, 0);
    lcd_print(menu_list[selection].name);
}

void show_edit(int selection) {
    lcd_command(0x01);
    lcd_set_cursor(0, 0);
    lcd_print(menu_list[selection].name);
    lcd_set_cursor(1, 0);
    lcd_print("Value:");
    lcd_print_int(menu_list[selection].value);
}

// Refresh only the value line during editing (no full-clear flicker)
void refresh_value(int selection) {
    lcd_set_cursor(1, 0);
    lcd_print("                "); // Clear bottom line (16 spaces)
    lcd_set_cursor(1, 0);
    lcd_print("Value:");
    lcd_print_int(menu_list[selection].value);
}

// --- MAIN RUNTIME ROUTINE ---
int main(void) {
    setup_io();
    lcd_init();

    SystemState current_state = STATE_WELCOME;
    int current_selection = 0;
    int backup_value = 0; // Value on entry to edit mode (for MENU cancel)

    LED_PORT |= (1 << LED_STATUS); // Status LED = system active
    show_welcome();

    while (1) {
        switch (current_state) {

        // =====================================================
        // STATE 1: WELCOME — wait for MENU
        // =====================================================
        case STATE_WELCOME:
            if (button_pressed(BTN_MENU)) {
                buzzer_beep(100);
                LED_PORT |= (1 << LED_MODE);
                current_selection = 0;
                current_state = STATE_MENU_NAV;
                show_menu(current_selection);
            }
            break;

        // =====================================================
        // STATE 2: MENU NAVIGATION — UP/DOWN wrap, SELECT edits
        // =====================================================
        case STATE_MENU_NAV:
            if (button_pressed(BTN_UP)) {
                buzzer_beep(50);
                if (current_selection > 0)
                    current_selection--;
                else
                    current_selection = TOTAL_PARAMS - 1;
                show_menu(current_selection);
            }
            else if (button_pressed(BTN_DOWN)) {
                buzzer_beep(50);
                if (current_selection < (TOTAL_PARAMS - 1))
                    current_selection++;
                else
                    current_selection = 0;
                show_menu(current_selection);
            }
            else if (button_pressed(BTN_SELECT)) {
                buzzer_beep(100);
                backup_value = menu_list[current_selection].value;
                current_state = STATE_PARAM_EDIT;
                show_edit(current_selection);
            }
            break;

        // =====================================================
        // STATE 3: PARAMETER EDIT
        //   UP   -> increase (clamped at max)
        //   DOWN -> decrease (clamped at min)
        //   SAVE -> store value, "Saving...", back to menu
        //   MENU -> cancel, restore backup, back to menu
        // =====================================================
        case STATE_PARAM_EDIT:
            if (button_pressed(BTN_UP)) {
                if (menu_list[current_selection].value < menu_list[current_selection].max_val) {
                    menu_list[current_selection].value++;
                    buzzer_beep(30);
                    refresh_value(current_selection);
                } else {
                    buzzer_beep(80); // At max — boundary feedback
                }
            }
            else if (button_pressed(BTN_DOWN)) {
                if (menu_list[current_selection].value > menu_list[current_selection].min_val) {
                    menu_list[current_selection].value--;
                    buzzer_beep(30);
                    refresh_value(current_selection);
                } else {
                    buzzer_beep(80); // At min — boundary feedback
                }
            }
            else if (button_pressed(BTN_SAVE)) {
                lcd_command(0x01);
                lcd_set_cursor(0, 0);
                lcd_print("Saving...");
                lcd_set_cursor(1, 0);
                lcd_print("Value Saved");
                buzzer_beep(250);
                _delay_ms(1000);

                current_state = STATE_MENU_NAV;
                show_menu(current_selection);
            }
            else if (button_pressed(BTN_MENU)) {
                menu_list[current_selection].value = backup_value; // Restore
                buzzer_beep(60);
                current_state = STATE_MENU_NAV;
                show_menu(current_selection);
            }
            break;
        }

        _delay_ms(10); // Polling stability buffer
    }
    return 0;
}
