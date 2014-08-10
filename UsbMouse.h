/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 */
#ifndef __UsbMouse_h__
#define __UsbMouse_h__

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "usbdrv.h"


typedef uint8_t byte;


static uchar    idleRate;           // in 4 ms units 


/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses. 
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
PROGMEM char usbHidReportDescriptor[52] = { /* USB report descriptor, size must match usbconfig.h */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xA1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM
    0x29, 0x03,                    //     USAGE_MAXIMUM
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Const,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xC0,                          //   END_COLLECTION
    0xC0,                          // END COLLECTION
};
/* This is the same report descriptor as seen in a Logitech mouse. The data
 * described by this descriptor consists of 4 bytes:
 *      .  .  .  .  . B2 B1 B0 .... one byte with mouse button states
 *     X7 X6 X5 X4 X3 X2 X1 X0 .... 8 bit signed relative coordinate x
 *     Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0 .... 8 bit signed relative coordinate y
 *     W7 W6 W5 W4 W3 W2 W1 W0 .... 8 bit signed relative coordinate wheel
 */

/*
typedef struct{
    uchar   buttonMask;
    char    dx;
    char    dy;
    char    dWheel;
}report_t;

static report_t reportBuffer;
*/


class UsbMouseDevice {
 public:
  UsbMouseDevice () {
    PORTD = 0; // TODO: Only for USB pins?
    DDRD |= ~USBMASK;

    cli();
    usbDeviceDisconnect();
    usbDeviceConnect();

    usbInit();
      
    sei();

    // TODO: Remove the next two lines once we fix
    //       missing first keystroke bug properly.
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
  
  void move(char inX, char inY, char inZ){
    reportBuffer[1]=inX;	
    reportBuffer[2]=inY;	
    reportBuffer[3]=inZ;	
	}

  void set_buttons(char lb, char mb, char rb){
	reportBuffer[0]=lb;
	reportBuffer[0]+=(mb * 2); 
	reportBuffer[0]+=(rb * 4); 
	}

  void update() {
    usbPoll();
    if (usbInterruptIsReady()) {
    	usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
	}

  }
	
unsigned char reportBuffer[4];
    
};

UsbMouseDevice UsbMouse = UsbMouseDevice();

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"{
#endif
  // USB_PUBLIC uchar usbFunctionSetup
uchar usbFunctionSetup(uchar data[8])
  {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = UsbMouse.reportBuffer; //
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
      /* class request type */

      if(rq->bRequest == USBRQ_HID_GET_REPORT){
        /* wValue: ReportType (highbyte), ReportID (lowbyte) */

        /* we only have one report type, so don't look at wValue */
        // TODO: Ensure it's okay not to return anything here?    
        return 0;

      }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
        //            usbMsgPtr = &idleRate;
        //            return 1;
        return 0;
      }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
        idleRate = rq->wValue.bytes[1];
      }
    }else{
      /* no vendor specific requests implemented */
    }
    return 0;
  }
#ifdef __cplusplus
} // extern "C"
#endif


/* ------------------------------------------------------------------------- */

#endif // __UsbMouse_h__
