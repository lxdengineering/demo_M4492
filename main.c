// ----------------------------------------------------------------------------
// LXD M4492 ("Napa", ST7565P based) LCD demo.
//
// Display several items (selectable via 4 buttons, or joystick positions)
// to show the operation of the M4492 128x64 LCD module.
//
// Platforms:
//    Olimex PIC32-MX460 (pic32mx460f512h)
//    Olimex PIC32-PINGUINO-OTG (pic32mx440f256h)
//
// 2011 LXD Reasearch & Display
//
#include <stdlib.h>
#include <plib.h>

#include "product_config.h"

#include "joystick.h"
#include "p32_utils.h"
#include "st7565.h"
#include "gfx.h"
#include "M4492demo.h"


// PIC32 Configuration Bit Settings:
//		- SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
//		- PBCLK = 40 MHz
//		- Primary Osc w/PLL (XT+,HS+,EC+PLL)
//		- Watchdog timer OFF
//		- Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select

#define SYS_FREQ 		(80000000L)


// ---------------------------------------------------------------------------
//
int main(void)
{
	int	pbClk;
	
	DDPCON &= ~(1<<3);	// we don't use JTAG (for now)

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //STEP 1. Configure cache, wait states and peripheral bus clock
	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	pbClk = SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);		
	

    // Note: It is recommended to disable vector interrupts prior to
    // configuring the change notice module, if they are enabled.
    // The user must read one or more IOPORTs to clear the IO pin
    // change notice mismatch condition, then clear the change notice
    // interrupt flag before re-enabling the vector interrupts.

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // STEP 7. enable multi-vector interrupts
    INTEnableSystemMultiVectoredInt();
    
#if defined ST7565_M4492_PROTOTYPE_OLIMEX_UEXTPORT

    // Initialize joystick interface
	joys_init();
    delay_ms(20);

    // Init st7565 LCD control lines
    // Olimex PIC32-MX460
    LATFSET = BIT_8; TRISFCLR = BIT_8;  // A0    (UEXT-3)
    LATFSET = BIT_2; TRISFCLR = BIT_2;  // /RES  (UEXT-4)
    LATDSET = BIT_9; TRISDCLR = BIT_9;  // /CS1  (UEXT-10)

	// Init the SPI serial port
    SpiChnOpen(SPI_CHANNEL1,
               SPI_CON_MSTEN |         // Master mode
			   SPI_OPEN_MODE8 |        // 8 bits
			   SPI_OPEN_CKP_HIGH,      // Clock idles high (? check ?)
               pbClk / 400000);       // SPI clock 400kHz (Can do 20MHz??)
//               pbClk / 100000);       // SPI clock 400kHz (Can do 20MHz??)

#elif defined ST7565_M4492_OLIMEX_PINGUINO_OTG

    // Set up for our 4 input switches. One side of our 4 switches will
    // connected to the Pinguino's D2..D5 inputs. The other side of the
    // four switches are all connected to ground. Mapping to pic32
    // ports and Change-notice inputs:
    //    
    //    Sw             Port   CN    Note
    //    --  ------     ----   ----  --------------------------------------
    //    1   D2(BUT)    RD4    CN13  Has 4.7k pull-up; On-board "BUT"
    //    2   D3         RD5    CN14
    //    3   D4         RD6    CN15
    //    4   D5         RD7    CN16
    //
    // Enable weak pull-ups for CN14,15, and 16
    CNPUESET = BIT_14 | BIT_15 | BIT_16;

    // Init st7565 LCD control lines
    // Olimex PIC32-PINGUINO-OTG
    LATFSET = BIT_5; TRISFCLR = BIT_5;  // A0    (UEXT-3)
    LATFSET = BIT_4; TRISFCLR = BIT_4;  // /RES  (UEXT-4)
    LATFSET = BIT_0; TRISFCLR = BIT_0;  // /CS1  (UEXT-10)

	// Init the SPI serial port
    SpiChnOpen(SPI_CHANNEL2,
               SPI_CON_MSTEN |         // Master mode
			   SPI_OPEN_MODE8 |        // 8 bits
			   SPI_OPEN_CKP_HIGH,      // Clock idles high (? check ?)
//               pbClk / 400000);       // SPI clock 400kHz (Can do 20MHz??)
               pbClk / 100000);       // SPI clock 400kHz (Can do 20MHz??)
#else
  #error must define port setup macro
#endif

    delay_ms(2);

    // Initialize and run the Napa M4492 demo
    napaInit();
    napaDemo();

    return 0;
}
