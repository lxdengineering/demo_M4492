#ifndef _PRODUCT_CONFIG_H_
#define _PRODUCT_CONFIG_H_

// product_config.h
//
// Items that configure options for a particular product, build-type,
// processor, etc.


// CPU clock speed
#define CPU_HZ 80000000L


// Uncomment one..
#define LCD_SERIAL
//#define LCD_PARALLEL

// Uncomment one...
//#define ST7565_NHD_PROTOTYPE_STARTERKIT
//#define ST7565_M4492_PROTOTYPE_OLIMEX_UEXTPORT
#define ST7565_M4492_OLIMEX_PINGUINO_OTG

#endif
