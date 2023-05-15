#include "usb/usb.h"

#include "usb/usb_host.h"

void usb_init(void) {
    MX_USB_HOST_Init();
}

void usb_process(void) {
    MX_USB_HOST_Process();
}

extern HCD_HandleTypeDef  hhcd_USB_OTG_FS;

void otg_fs_isr(void) {
    HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}
