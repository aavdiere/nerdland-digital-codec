/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file            : usb_host.c
 * @version         : v2.0_Cube
 * @brief           : This file implements the USB Host
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb/usb_host.h"
#include "usb/usbh_conf.h"
#include "usb/usbh_core.h"
#include "usb/usbh_hid.h"

void Error_Handler(void);

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
 * Init USB host library, add supported class and start the library
 * @retval None
 */
void MX_USB_HOST_Init(void) {
    /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

    /* USER CODE END USB_HOST_Init_PreTreatment */

    /* Init host Library, add supported class and start the library. */
    if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK) {
        Error_Handler();
    }
    if (USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS) != USBH_OK) {
        Error_Handler();
    }
    if (USBH_Start(&hUsbHostFS) != USBH_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

    /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void) {
    /* USB Host Background task */
    USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
    /* USER CODE BEGIN CALL_BACK_1 */
    switch (id) {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            Appli_state = APPLICATION_DISCONNECT;
            break;

        case HOST_USER_CLASS_ACTIVE:
            Appli_state = APPLICATION_READY;
            break;

        case HOST_USER_CONNECTION:
            Appli_state = APPLICATION_START;
            break;

        default:
            break;
    }
    /* USER CODE END CALL_BACK_1 */
}

static uint8_t keycode_buffer[6];

void compare_keys(uint8_t new_keys[6],
                  uint8_t old_keys[6],
                  uint8_t pressed[6],
                  uint8_t released[6]) {
    uint8_t i, j;
    uint8_t rollover;
    uint8_t key_pressed, key_released;
    uint8_t pressed_idx, released_idx;

    rollover = 0x1;

    for (i = 0; i < 6; i++) {
        rollover &= new_keys[i];
    }

    // check for n-key rollover
    if (rollover == 1) {
        return;
    }

    pressed_idx  = 0;
    released_idx = 0;

    for (i = 0; i < 6; i++) {
        // assume the new_key is new
        key_pressed = 1;

        // assume the old_key is released
        key_released = 1;

        // check if new_key is already present in the old key list
        // check if old_key is still present in the new key list
        for (j = 0; j < 6; j++) {
            // if it is present, it means it wasn't pressed
            // or if it is empty
            if (new_keys[i] == old_keys[j] || new_keys[i] == 0) {
                key_pressed = 0;
            }
            // if it is present, it means it wasn't released
            // or if it is empty
            if (old_keys[i] == new_keys[j] || old_keys[i] == 0) {
                key_released = 0;
            }
        }

        if (key_pressed == 1) {
            pressed[pressed_idx++] = new_keys[i];
        }

        if (key_released == 1) {
            released[released_idx++] = old_keys[i];
        }
    }

    // copy new keys to old keys
    for (i = 0; i < 6; i++) {
        old_keys[i] = new_keys[i];
    }
}

void convert_to_ascii(uint8_t buffer[6], HID_KEYBD_Info_TypeDef *info) {
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t t     = info->keys[0];
        info->keys[0] = buffer[i];
        buffer[i]     = USBH_HID_GetASCIICode(info);
        info->keys[0] = t;
    }
}

extern volatile uint8_t tx_done;
extern volatile uint8_t ghost_typer;

volatile uint8_t usb_done = 0;
uint8_t pressed[6] = {0, 0, 0, 0, 0, 0};

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
    if (USBH_HID_GetDeviceType(phost) == HID_KEYBOARD) {
        HID_KEYBD_Info_TypeDef *keyboard_info;
        keyboard_info = USBH_HID_GetKeybdInfo(phost);

        for (uint8_t k = 0; k < sizeof(pressed); pressed[k++] = 0);
        uint8_t released[6] = {0, 0, 0, 0, 0, 0};
        compare_keys(keyboard_info->keys, keycode_buffer, pressed, released);

        if (keyboard_info->lctrl > 0 && keyboard_info->ralt > 0) {
            ghost_typer = 1 - ghost_typer;
            tx_done = 0;
        }

        convert_to_ascii(pressed, keyboard_info);
        usb_done = 1;
    }
}

/**
 * @}
 */

/**
 * @}
 */
