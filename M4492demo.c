/*********************************************************************
 *
 *      LXD M4492 ("Napa", ST6575P based) LCD demo.
 *
 *********************************************************************/

#include <stdlib.h>
#include <plib.h>

#include "product_config.h"
#include "joystick.h"
#include "p32_utils.h"
#include "st7565.h"
#include "gfx.h"


//
// Demo states - Determines where we are in the hierachy of menus/demos
#define DEMO_TOP_MENU 1
#define DEMO_BITMAP 2     /* Cycle thru a few bitmaps */
#define DEMO_BARGRAPH 3   /* An "audio spectrum analyser" like moving bar graph */
#define DEMO_GFX 4        /* Random lines, circles, rectangles, etc. */
#define DEMO_ADJUST 5     /* View/set resistor-ratio, volume (contrast), etc. */

// Globals for Napa demo
static int demoState = DEMO_TOP_MENU;  // Initial demo state
//static int demoState = DEMO_BARGRAPH;  // Initial demo state
uint8_t st7565_resistorRatio;          // Contrast settings
uint8_t st7565_volume;

// LCD sizes
#define LCD_COLS       128
#define LCD_ROWS        64
#define LCD_PAGES        8
#define LCD_BUFSIZE   1024


// Bitmaps
extern const unsigned char demo_logo[];     // New LXD logo
//extern const unsigned char dimmer1[];       // 
//extern const unsigned char dimmer2[];
extern const unsigned char dimmerSwitch[];
extern const unsigned char imageinuse4[];
extern const unsigned char phasePlot[];
extern const unsigned char bmpAudio[];
extern const unsigned char napaTop_bmp[];

// Graphics buffer
uint8_t testBuf[LCD_BUFSIZE];


// If a key is pressed, returns the key value (1..4? or 6?).
// Otherwise , return 0 (no key pressed).
//
int keyPress()
{

#if defined ST7565_M4492_PROTOTYPE_OLIMEX_UEXTPORT
    // TEMP ONLY - Use joystick on '460 demo board until we get buttons
    //             Note 0 is returned if nothing is pressed.
    enum joys_direction dir;
    int ret = 0;
    dir = joys_get_direction();
    switch(dir)
    {
        case JOYS_DIR_N: ret = 1; break;
        case JOYS_DIR_S: ret = 2; break;
        case JOYS_DIR_W: ret = 3; break;
        case JOYS_DIR_E: ret = 4; break;
        default: ret = 0; break;
    }

    return ret;

#elif defined ST7565_M4492_OLIMEX_PINGUINO_OTG

    int tmp, ret;

    // Get the four (inverted) bits at RD4..7, isolate and invert.
    tmp = (~PORTD >> 4) & 0x0f;
    switch(tmp)
    {
        case 0x01: ret = 1; break;
        case 0x02: ret = 2; break;
        case 0x04: ret = 3; break;
        case 0x08: ret = 4; break;
        default: ret = 0;
    }
    return ret;

#else
    #error need a product #define
#endif
}


// demoDelay
//
//    Delays for a given number of microseconds (or forever, if ms=0)
//    While delay'ing, checks for user input via buttons/switches.
//    If any input is detected, the key press is "debounced" and
//    the routine returns on the key release (without waiting for
//    the timeout specified in "ms").
//
//    Warning: This is not an accurate delay; It is intended for
//             "user interface" type delays only.
//
//    Returns switch number (if one was pressed before delay expired),
//    or 0 if no switch was pressed.
//
int demoDelay(int ms)
{
    int key, chunks;

    if(ms < 0) return -1;    // Invalid

    if(ms > 0 && ms < 20)    // A "short" delay. Just do the delay and return.
    {
        delay_ms(ms);
        return -1;
    }
    
    // This is a "long" delay. Split into chunks of 10ms, and check
    // for switch presses along the way. (If ms == 0, stay forever
    // until key is pressed).

    if(ms) chunks = ms / 10;
    else   chunks = 1;

    while(chunks > 0)
    {
		key = keyPress();
        if(key)
        {
            // Debounce: Wait 10ms, and see if key still pressed
            delay_ms(10);
			key = keyPress();
            if(key)
            {
                // Wait for release, then return the key
                while(keyPress());
                return key;
            }
        }
		delay_ms(10);       // Delay a 10ms chunk
        if(ms) chunks--;    // Update the delay count
    }

    return 0;  // No keypress
}


// Show Napa demo top-level menu, and wait for key-presses
void demoTopMenu()
{
    gfxFill(0);
    /*
    gfxString(10,0,"LXD M4492");   // TODO: Reverse
    gfxString(10,1,"");
    gfxString(10,2,"1 Bitmaps");
    gfxString(10,3,"2 Bar graph");
    gfxString(10,4,"3 Lines");
    gfxString(10,5,"");
    gfxString(10,6,"4 Adjust");
    gfxString(10,7,"");
	lcdWriteBuffer(testBuf);
    */
    memcpy(testBuf, napaTop_bmp, 1024);  // Top level menu bitmap
    gfxRect(0,0, 127,63, 1);             // Add a border
    lcdWriteBuffer(testBuf);
}


void gfxGrid()
{
    int16_t x;
    for(x=0;x<128;x+=2) {
        //gfxPixel(x,0, 1);
        gfxPixel(x,15, 1);
        gfxPixel(x,31, 1);
        gfxPixel(x,47, 1);
        //gfxPixel(x,63, 1);
    }
}


// Show a bitmap or test pattern. An index is maintained and incremented
// on each call, that moves the bitmap/testpattern ahead for each call.
void demoBitmap()
{
    // TODO: Do this in some way that does not tie us to the index number.

    static int idx = 0;    // Which of several bitmaps/other things to show

    switch(idx)
    {
        case 0:               // Vertical bars
            gfxFill(0x0f);
            lcdWriteBuffer(testBuf);
            break;
        
        case 1:               // 4 pixel horiz bars (invers of above)
            gfxFill(0xf0);
            lcdWriteBuffer(testBuf);
            break;
        
        case 2:         // Rectangles
            gfxFill(0x00);
            //gfxLine(0, 0, 100, 50, 1);
            gfxRect(0,0, 127,63, 1);
            gfxRect(5,5, 122,58, 1);
            gfxRect(10,10, 117,53, 1);
            gfxRect(15,15, 112,48, 1);
            gfxRect(20,20, 107,43, 1);
            lcdWriteBuffer(testBuf);
            break;

        case 3:   // Circles, bar graph
            gfxFill(0x00);
            gfxCircle(10,10,10,1);
            gfxCircle(20,20,20,1);
            gfxCircle(30,30,30,1);
            gfxFRect(70,(63-10), 8, 10, 1);
            gfxFRect(80,(63-60), 8, 60, 1);
            gfxFRect(90,(63-50), 8, 50, 1);
            gfxFRect(100,(63-25), 8, 25, 1);
            gfxFRect(110,(63-55), 8, 55, 1);
            lcdWriteBuffer(testBuf);
            break;

            // Bitmaps

        case 4: lcdWriteBuffer(demo_logo);    break;
        case 5: lcdWriteBuffer(imageinuse4);  break;
        case 6: lcdWriteBuffer(dimmerSwitch); break;
//        case 7: lcdWriteBuffer(dimmer1);      break;
        case 7: lcdWriteBuffer(phasePlot);    break;
//        case 8: lcdWriteBuffer(dimmer2);      break;
        case 8: lcdWriteBuffer(bmpAudio);      break;
        default: idx = 0;
    }

    //if(++idx > 8) idx = 0;
    if(++idx > 8) idx = 4;
}


// View/set resistor-ratio and volume (contrast) settings
//
void demoAdjust()
{
    char s[64];    // Temp string buffer

    gfxFill(0);
    gfxString(0, 0 ,"-- M4492  Settings --");

    sprintf(s,"   Contrast :%2d  ", st7565_volume);
    gfxString(0, 1, s);
    sprintf(s,"   Res-ratio:%2d  ", st7565_resistorRatio);
    gfxString(0, 2, s);

    gfxString(0, 3, "");
    
    gfxString(0, 4, "O -   Contrast    + O");

    gfxString(0, 6, "O +ResRatio    Quit O");

	lcdWriteBuffer(testBuf);

    int key = 0;
    while(key != 4)           // Key 4 exits
    {
        key = demoDelay(0);   // Wait for a key (forever)
        switch(key)
        {
            case 1:  // Top-left button: Decrement volume (contrast)
                if(st7565_volume > 0) st7565_volume--;
                sprintf(s,"   Contrast :%2d  ", st7565_volume);
                gfxString(0, 1, s);
                lcdCmd(cVOLUME);
                lcdCmd(st7565_volume);
                break;
            case 3:  // Top-right button: Increment volume (contrast)
                if(st7565_volume < 63) st7565_volume++;
                sprintf(s,"   Contrast :%2d  ", st7565_volume);
                gfxString(0, 1, s);
                lcdCmd(cVOLUME);
                lcdCmd(st7565_volume);
                break;
            case 2:  // Bottom-left button: Increment resistor-ratio
                st7565_resistorRatio++;
                if(st7565_resistorRatio > 7) st7565_resistorRatio = 0;
                sprintf(s,"   Res-ratio:%2d  ", st7565_resistorRatio);
                gfxString(0, 2, s);
                lcdCmd(cRESISTOR_RATIO | st7565_resistorRatio);
                break; 
            case 4:  // Bottom-right button
                // Do nothing (let while loop exit)
                break;
            default: 
                break;
        }
        lcdWriteBuffer(testBuf);
    }
}

// A dynamic bargraph display
//
//static int16_t barLevels[25];  TODO: Scrolling levels, instead of all random?
void demoBargraph()
{
	int i;
    uint16_t x0,y0,x1,y1;

    gfxFill(0);
    gfxGrid();
    gfxString(0,0,"       Levels ");
    gfxString(0,7,"0   5   10   15   20");
    for(i=0; i<25; i++)
    {
        x0 = i*5 + 1;
        y0 = 54 - (rand() % 48);
        x1 = x0 + 3;
        y1 = 54;
        gfxFRect(x0,y0,x1,y1,1);
    }
    lcdWriteBuffer(testBuf);
}

// demoGfx() - Random lines, circles, rectangles, etc
//
void demoGfx()
{
    static char whatToGenerate = 0;   // 0:lines, 1:circles, 2:rectangles, 3:chars
    int i;
	const int n = 5;   // Number of elements to draw

    gfxFill(0);

    switch(whatToGenerate)
    {
        case 0:           // lines
            for(i=0; i<n; i++)
            {
                gfxLine(rand() & 127, rand() & 63,
                        rand() & 127, rand() & 63, 1);
            }
            break;
        case 1:           // circles
            for(i=0; i<n; i++)
            {
                gfxCircle(rand() & 127, rand() & 63,   // Center (x,y)
                          rand() & 31, 1);             // Radius, color=1(black)
            }
            break;
        case 2:           // rectangles
            for(i=0; i<n; i++)
            {
                gfxRect(rand() & 127, rand() & 63,
                        rand() & 127, rand() & 63, 1);
            }
            break;
        case 3:           // chars TODO...
            for(i=0; i<n; i++)
            {
                gfxFCircle(rand() & 127, rand() & 63,   // Center (x,y)
                          rand() & 31, 1);             // Radius, color=1(black)
            }
            break;
    }
    lcdWriteBuffer(testBuf);
    whatToGenerate++;
    whatToGenerate &= 0x03;
}

// demoChart() - Strip-chart demo
//
/*
void demoChart()
{
    static char x0 = 64;  // x-axis offset
    const float pi = 3.1459;

    gfxFill(0);
    for(x=0; x<128; x++)
    {
        if((x-x0)==0)
        {
            y = 1.0;
        }
        else
        {
            y = 
        }
    }

}
*/

void napaInit(void)
{
    // LXD / ST7565 LCD init. Pass in resistor-ratio and "volume" settings.
    st7565_resistorRatio = 4;
    //st7565_resistorRatio = 5;    // TEST ONLY
    st7565_volume = 28;
    lcdInit(st7565_resistorRatio, st7565_volume);

    Nop();

    // Init graphics utilities, passing bitmap width, height, and pointer
    // to working bitmap buffer.
	gfxInit(LCD_COLS, LCD_ROWS, testBuf);
}

// napaDemo() - Top level menu for MM4492 demo
//
void napaDemo(void)
{
    int bNum;     // Button/key number

	srand(0);

    while(1)
    {
        switch(demoState)
        {
            case DEMO_TOP_MENU:
                demoTopMenu();        // Show top level menu
                bNum = demoDelay(0);  // Wait forever for a key-press
                switch(bNum)
                {
                    case 1: demoState = DEMO_BITMAP; break;
                    case 2: demoState = DEMO_BARGRAPH; break;
                    case 3: demoState = DEMO_GFX; break;
                    case 4: demoState = DEMO_ADJUST; break;
                    default: demoState = DEMO_TOP_MENU; break;
                }
                break;
            case DEMO_BITMAP:
                demoBitmap();           // Show a bitmap
                bNum = demoDelay(1000); // Delay & look for keypress
                if(bNum)                // Was a key pressed?
                {
					// If key 4, freeze bitmap display; Other keys: return to top
					if(bNum == 4) demoDelay(0);
					else demoState = DEMO_TOP_MENU;
                }
                break;
            case DEMO_BARGRAPH:    /* An "audio spectrum analyser" like moving bar graph */
                demoBargraph();
                bNum = demoDelay(400); // Delay & look for keypress
                if(bNum)                // Was a key pressed?
                {
					// If key 4, freeze display; Other keys: return to top
					if(bNum == 4) demoDelay(0);
					else demoState = DEMO_TOP_MENU;
                }
                break;
            case DEMO_GFX:       /* Random lines, circles, rectangles, etc. */
                demoGfx();
                bNum = demoDelay(400); // Delay & look for keypress
                if(bNum)                // Was a key pressed?
                {
					// If key 4, freeze display; Other keys: return to top
					if(bNum == 4) demoDelay(0);
					else demoState = DEMO_TOP_MENU;
                }
                break;
            case DEMO_ADJUST:      /* View/set resistor-ratio, volume (contrast), etc. */
                demoAdjust();
                demoState = DEMO_TOP_MENU;
                break;
            default:
                demoState = DEMO_TOP_MENU;
                break;
        }
    }
}

