#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "LiquidCrystal_I2C.h"
#include "ArduinoWire.h"

#define COLUMNS 16
#define LINES 2

ArduinoWire Wire;

// LCD address is 0x3F - 16 chars, 2 line display
LiquidCrystal_I2C lcd(0x3F, COLUMNS, LINES); 

int main()
{
    lcd.init();
    lcd.backlight();

    while(1)
    {
        int currentLine = 0;
        FILE *fp = popen("/root/bin.local/LCD.sh", "r");
        while(!feof(fp)) {
            lcd.setCursor(0, currentLine);
            static char line[COLUMNS+1];
            fgets(line, sizeof(line), fp);
            size_t ln = strlen(line)-1;
            if (ln < 2)
                continue;
            if (line[ln] == '\n') line[ln] = '\0';
            lcd.print(line);
            currentLine++;
        }
        pclose(fp);
        sleep(5);
    }
}
