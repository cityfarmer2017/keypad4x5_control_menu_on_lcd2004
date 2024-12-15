#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// create a LCD 2004 instance
LiquidCrystal_I2C lcd(0x27,20,4); // device address: 0x27

// setup the enum with all the menu pages options
enum pageType_ {
    ROOT_MENU, SUB_MENU1, SUB_MENU2, SUB_MENU3
};
using pageType = pageType_;

// holds which page is currently selected
pageType currPage = ROOT_MENU;

constexpr uint8_t ROWS = 5; // number of keypad rows
constexpr uint8_t COLS = 4; // number of keypad columns

// keymap defines the key pressed according to the row and columns just as appears on the keypad
char KEYMAP[ROWS][COLS] = {
    {'A', 'B', '#', '*'},
    {'1', '2', '3', 'U'},
    {'4', '5', '6', 'D'},
    {'7', '8', '9', 'C'},
    {'L', '0', 'R', 'E'}
};

uint8_t ROWPINS[ROWS] = {10, 9, 8, 7, 6};
uint8_t COLPINS[COLS] = {2, 3, 4, 5};

// define a keypad instance
auto myKeypad = Keypad(makeKeymap(KEYMAP), ROWPINS, COLPINS, ROWS, COLS);

// the maximum index of sub-menus on the page of root menu
constexpr uint8_t ROOT_MENU_SUB_IDX_MAX = 3;

// the maximum index of items on the page of sub menu 1
constexpr uint8_t SUB_MENU1_ITEM_IDX_MAX = 4;

// the maximum index of items on the page of sub menu 2
constexpr uint8_t SUB_MENU2_ITEM_IDX_MAX = 5;

// the maximum index of items on the page of sub menu 3
constexpr uint8_t SUB_MENU3_ITEM_IDX_MAX = 6;

// the minimum index of all kinds of menus
constexpr uint8_t MENU_IDX_MIN = 1;

// the count of lines the LCD can dispaly except the first line
constexpr uint8_t LCD_DISPLAY_LINES = 3;

// -----------------------------------------------------------------------------
// ||                            TOOLS - DISPALY                              ||
// -----------------------------------------------------------------------------
inline void printSelected(uint8_t p1, uint8_t p2) {
    if (p1 == p2) {
        Serial.print(F("--> "));
    } else {
        Serial.print(F("    "));
    }
}

// -----------------------------------------------------------------------------
// ||                          TOOLS - CLEAR SCREEN                           ||
// -----------------------------------------------------------------------------
inline void clearScreen(void) {
    for (auto i = 0; i < 100; ++i) {
        Serial.println();
    }
}

// -----------------------------------------------------------------------------
// ||                          TOOLS - PRINT DIVIDER                          ||
// -----------------------------------------------------------------------------
inline void printDivider(void) {
    for (auto i = 0; i < 40; ++i) {
        Serial.print(F("-"));
    }
    Serial.println();
}

// -----------------------------------------------------------------------------
// ||                  TOOLS - DISPALY ROOT PAGE ON Serial                    ||
// -----------------------------------------------------------------------------
void printRootPageOnSerial(uint8_t pos) {
    // clean the display
    clearScreen();

    // menu title
    Serial.println(F("[ MAIN MENU ]"));

    // print a divider line
    printDivider();

    // print the display
    printSelected(1, pos);
    Serial.println(F("SUB MENU 1"));
    printSelected(2, pos);
    Serial.println(F("SUB MENU 2"));
    printSelected(3, pos);
    Serial.println(F("SUB MENU 3"));
    Serial.println();
    Serial.println();

    // print a divider line
    printDivider();
}

// -----------------------------------------------------------------------------
// ||                   TOOLS - DISPALY ROOT PAGE ON LCD                      ||
// -----------------------------------------------------------------------------
void printRootPageOnLcd(uint8_t pos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("[ MAIN MENU        ]"));
    for (auto i = 1; i <= ROOT_MENU_SUB_IDX_MAX; ++i) {
        lcd.setCursor(0, i);
        lcd.print(F("   SUB MENU "));
        lcd.print(i);
    }
    lcd.setCursor(0, pos);
    lcd.print(F("-> "));
}

// -----------------------------------------------------------------------------
// ||                            PAGE - ROOT MENU                             ||
// -----------------------------------------------------------------------------
void page_RootMenu(void) {
    // flag for update the dispaly
    bool updateDisplay = true;
    // tracks when entered top of loop
    uint32_t loopStartMs = 0;
    // selectex item pointer
    static uint8_t sub_pos = 1;

    // inner loop
    while(true) {
        loopStartMs = millis();

        // get which has been pressed
        char keyPressed = myKeypad.getKey();

        if (updateDisplay) {
            // clear the update flag
            updateDisplay = false;

            printRootPageOnLcd(sub_pos);

            printRootPageOnSerial(sub_pos);
        }

        switch (keyPressed)
        {
        case 'D': // move the pointer down
            if (ROOT_MENU_SUB_IDX_MAX == sub_pos) {
                sub_pos = MENU_IDX_MIN;
            } else {
                ++sub_pos;
            }
            updateDisplay = true;
            break;
        case 'U': // move the pointer up
            if (MENU_IDX_MIN == sub_pos) {
                sub_pos = ROOT_MENU_SUB_IDX_MAX;
            } else {
                --sub_pos;
            }
            updateDisplay = true;
            break;
        case 'E': // enter the selected page of sub-menu
            switch (sub_pos)
            {
            case 1:
                currPage = SUB_MENU1;
                return;
            case 2:
                currPage = SUB_MENU2;
                return;
            case 3:
                currPage = SUB_MENU3;
                return;
            default:
                break;
            }
            break;
        default:
            break;
        }

        // keep a specific pace
        while(millis() - loopStartMs < 25) {
            delay(2);
        }
    }
}

// -----------------------------------------------------------------------------
// ||                  TOOLS - DISPALY SUB PAGE1 ON Serial                    ||
// -----------------------------------------------------------------------------
void printSubPage1OnSerial(uint8_t pos) {
    // clean the display
    clearScreen();

    // menu title
    Serial.println(F("[ SUB MENU #1 ]"));

    // print a divider line
    printDivider();

    // print the display
    printSelected(1, pos);
    Serial.println(F("1st Item"));
    printSelected(2, pos);
    Serial.println(F("2nd Item"));
    printSelected(3, pos);
    Serial.println(F("3rd Item"));
    printSelected(4, pos);
    Serial.println(F("4th Item"));
    Serial.println();

    // print a divider line
    printDivider();
}

// -----------------------------------------------------------------------------
// ||                TOOLS - DISPALY ITEMS OF SUB PAGE ON LCD                 ||
// -----------------------------------------------------------------------------
void printItemsOnLCD(uint8_t itemCnt, uint8_t pos) {
    auto page_cnt = (itemCnt - 1) / LCD_DISPLAY_LINES + 1;
    auto curr_page_idx = (pos - 1) / LCD_DISPLAY_LINES;
    auto last_item_idx = LCD_DISPLAY_LINES;
    if ((curr_page_idx == page_cnt - 1) && (itemCnt % LCD_DISPLAY_LINES)) {
        last_item_idx = itemCnt % LCD_DISPLAY_LINES;
    }
    for (auto i = 1; i <= last_item_idx; ++i) {
        lcd.setCursor(0, i);
        lcd.print(F("   SUB MENU ITEM "));
        lcd.print(i + curr_page_idx * LCD_DISPLAY_LINES);
    }
    auto curr_pos = pos % LCD_DISPLAY_LINES;
    lcd.setCursor(0, curr_pos ? curr_pos : curr_pos + LCD_DISPLAY_LINES);
    lcd.print(F("-> "));
}

// -----------------------------------------------------------------------------
// ||                   TOOLS - DISPALY SUB PAGE1 ON LCD                      ||
// -----------------------------------------------------------------------------
void printSubPage1OnLcd(uint8_t pos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("[ SUB MENU #01     ]"));
    printItemsOnLCD(SUB_MENU1_ITEM_IDX_MAX, pos);
}

// -----------------------------------------------------------------------------
// ||                            PAGE - SUB MENU1                             ||
// -----------------------------------------------------------------------------
void page_SubMenu1(void) {
    // flag for update the dispaly
    bool updateDisplay = true;
    // tracks when entered top of loop
    uint32_t loopStartMs = 0;
    // selectex item pointer
    uint8_t item_pos = 1;

    // inner loop
    while(true) {
        loopStartMs = millis();

        // get which has been pressed
        char keyPressed = myKeypad.getKey();

        if (updateDisplay) {
            // clear the update flag
            updateDisplay = false;

            printSubPage1OnLcd(item_pos);

            printSubPage1OnSerial(item_pos);
        }

        switch (keyPressed)
        {
        case 'D': // move the pointer down
            if (SUB_MENU1_ITEM_IDX_MAX == item_pos) {
                item_pos = MENU_IDX_MIN;
            } else {
                ++item_pos;
            }
            updateDisplay = true;
            break;
        case 'U': // move the pointer up
            if (MENU_IDX_MIN == item_pos) {
                item_pos = SUB_MENU1_ITEM_IDX_MAX;
            } else {
                --item_pos;
            }
            updateDisplay = true;
            break;
        case 'C': // esCapge / Cancel to go back to root-menu
            currPage = ROOT_MENU;
            return;
        default:
            break;
        }

        // keep a specific pace
        while(millis() - loopStartMs < 25) {
            delay(2);
        }
    }
}

// -----------------------------------------------------------------------------
// ||                  TOOLS - DISPALY SUB PAGE2 ON Serial                    ||
// -----------------------------------------------------------------------------
void printSubPage2OnSerial(uint8_t pos) {
    // clean the display
    clearScreen();

    // menu title
    Serial.println(F("[ SUB MENU #2 ]"));

    // print a divider line
    printDivider();

    // print the display
    printSelected(1, pos);
    Serial.println(F("1st Item"));
    printSelected(2, pos);
    Serial.println(F("2nd Item"));
    printSelected(3, pos);
    Serial.println(F("3rd Item"));
    printSelected(4, pos);
    Serial.println(F("4th Item"));
    printSelected(5, pos);
    Serial.println(F("5th Item"));

    // print a divider line
    printDivider();
}

// -----------------------------------------------------------------------------
// ||                   TOOLS - DISPALY SUB PAGE2 ON LCD                      ||
// -----------------------------------------------------------------------------
void printSubPage2OnLcd(uint8_t pos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("[ SUB MENU #02     ]"));
    printItemsOnLCD(SUB_MENU2_ITEM_IDX_MAX, pos);
}

// -----------------------------------------------------------------------------
// ||                            PAGE - SUB MENU2                             ||
// -----------------------------------------------------------------------------
void page_SubMenu2(void) {
    // flag for update the dispaly
    bool updateDisplay = true;
    // tracks when entered top of loop
    uint32_t loopStartMs = 0;
    // selectex item pointer
    uint8_t item_pos = 1;

    // inner loop
    while(true) {
        loopStartMs = millis();

        // get which has been pressed
        char keyPressed = myKeypad.getKey();

        if (updateDisplay) {
            // clear the update flag
            updateDisplay = false;

            printSubPage2OnLcd(item_pos);

            printSubPage2OnSerial(item_pos);
        }

        switch (keyPressed)
        {
        case 'D': // move the pointer down
            if (SUB_MENU2_ITEM_IDX_MAX == item_pos) {
                item_pos = MENU_IDX_MIN;
            } else {
                ++item_pos;
            }
            updateDisplay = true;
            break;
        case 'U': // move the pointer up
            if (MENU_IDX_MIN == item_pos) {
                item_pos = SUB_MENU2_ITEM_IDX_MAX;
            } else {
                --item_pos;
            }
            updateDisplay = true;
            break;
        case 'C': // esCapge / Cancel to go back to root-menu
            currPage = ROOT_MENU;
            return;
        default:
            break;
        }

        // keep a specific pace
        while(millis() - loopStartMs < 25) {
            delay(2);
        }
    }
}

// -----------------------------------------------------------------------------
// ||                  TOOLS - DISPALY SUB PAGE3 ON Serial                    ||
// -----------------------------------------------------------------------------
void printSubPage3OnSerial(uint8_t pos) {
    // clean the display
    clearScreen();

    // menu title
    Serial.println(F("[ SUB MENU #3 ]"));

    // print a divider line
    printDivider();

    // print the display
    printSelected(1, pos);
    Serial.println(F("1st Item"));
    printSelected(2, pos);
    Serial.println(F("2nd Item"));
    Serial.println();
    Serial.println();
    Serial.println();

    // print a divider line
    printDivider();
}

// -----------------------------------------------------------------------------
// ||                   TOOLS - DISPALY SUB PAGE3 ON LCD                      ||
// -----------------------------------------------------------------------------
void printSubPage3OnLcd(uint8_t pos) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("[ SUB MENU #03     ]"));
    printItemsOnLCD(SUB_MENU3_ITEM_IDX_MAX, pos);
}

// -----------------------------------------------------------------------------
// ||                            PAGE - SUB MENU3                             ||
// -----------------------------------------------------------------------------
void page_SubMenu3(void) {
    // flag for update the dispaly
    bool updateDisplay = true;
    // tracks when entered top of loop
    uint32_t loopStartMs = 0;
    // selectex item pointer
    uint8_t item_pos = 1;

    // inner loop
    while(true) {
        loopStartMs = millis();

        // get which has been pressed
        char keyPressed = myKeypad.getKey();

        if (updateDisplay) {
            // clear the update flag
            updateDisplay = false;

            printSubPage3OnLcd(item_pos);

            printSubPage3OnSerial(item_pos);
        }

        switch (keyPressed)
        {
        case 'D': // move the pointer down
            if (SUB_MENU3_ITEM_IDX_MAX == item_pos) {
                item_pos = MENU_IDX_MIN;
            } else {
                ++item_pos;
            }
            updateDisplay = true;
            break;
        case 'U': // move the pointer up
            if (MENU_IDX_MIN == item_pos) {
                item_pos = SUB_MENU3_ITEM_IDX_MAX;
            } else {
                --item_pos;
            }
            updateDisplay = true;
            break;
        case 'C': // esCapge / Cancel to go back to root-menu
            currPage = ROOT_MENU;
            return;
        default:
            break;
        }

        // keep a specific pace
        while(millis() - loopStartMs < 25) {
            delay(2);
        }
    }
}

// -----------------------------------------------------------------------------
// ||                                 SETUP                                   ||
// -----------------------------------------------------------------------------
void setup() {
    // init the serial port to be used as a dispay return
    Serial.begin(9600);

    // initialize the lcd
    lcd.init();
    // turn on the back light
    lcd.backlight();
    // clear the screen
    lcd.clear();
}

// -----------------------------------------------------------------------------
// ||                               MAIN LOOP                                 ||
// -----------------------------------------------------------------------------
void loop() {
    switch(currPage) {
        case ROOT_MENU:
            page_RootMenu();
            break;
        case SUB_MENU1:
            page_SubMenu1();
            break;
        case SUB_MENU2:
            page_SubMenu2();
            break;
        case SUB_MENU3:
            page_SubMenu3();
            break;
    }
}