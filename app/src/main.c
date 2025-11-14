/*
 * main.c

 when on:
  LED0 = locked, time to guess password!
  LED1 = input custom password to guess later
  LED3 = window to set custom pass is open press btn3 then input desired
    password combo, then click btn3 again to set it! then you can input it
    as a proper.. password... guess... check? idk
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "BTN.h"
#include "LED.h"


#define SLEEP_MS 1
#define MAX_PASS_SIZE 16

typedef enum {
  ST_INIT,
  ST_WAIT_SET_WINDOW,
  ST_SET_PASSWORD,
  ST_USE_DEFAULT,
  ST_READY,
  ST_INPUT_GUESS,
  ST_CHECK_GUESS,
  ST_UNLOCKED
} state_t;

// define default password params in case user doesn't input them
static const uint8_t def_password[] = {4, 4, 2, 1, 4, 2}; // [1]
uint8_t def_password_size = (sizeof(def_password) / sizeof(uint8_t));


btn_id btns[] = {BTN0, BTN1, BTN2, BTN3};


// gets bitmask of input!!!
uint8_t get_state_mask(void) {
  uint8_t mask = 0;

  for (int i = 0; i < 4; i++) {
    if (BTN_check_clear_pressed(btns[i])) mask |= (1 << i);
  }
  return mask;
}


int main(void) {
  if (0 > BTN_init() || 0 > LED_init()) return 0;
  uint8_t password[MAX_PASS_SIZE];
  uint8_t pass_len = 0;

  uint8_t in_stream[MAX_PASS_SIZE];
  uint8_t in_len = 0;

  state_t state = ST_INIT;

  uint16_t j = 0;

  while (1) {
    uint8_t b = get_state_mask();

    switch (state) {

      case ST_INIT:
        LED_set(LED3, LED_ON);
        j = 3000;
        state = ST_WAIT_SET_WINDOW;
        break;

      case ST_WAIT_SET_WINDOW:
        if (b == 8) {
          LED_set(LED2, LED_ON);
          pass_len = 0;
          state = ST_SET_PASSWORD;
        } else {
          if (j == 0) {
            state = ST_USE_DEFAULT;
          } else {
            j--;
          }
        }
        break;

      case ST_SET_PASSWORD:
        if (b == 8) {
          LED_set(LED2, LED_OFF);
          LED_set(LED3, LED_OFF);
          state = ST_READY;
          break;
        }

        if (b == 1 || b == 2 || b == 4){
          if (pass_len < MAX_PASS_SIZE) password[pass_len++] = b;
        }
        break;
      
      case ST_USE_DEFAULT:
        memcpy(password, def_password, def_password_size);
        pass_len = def_password_size;
        LED_set(LED3, LED_OFF);
        state = ST_READY;
        break;

      case ST_READY:
        printk("password: ");
        for (j = 0; j < pass_len; j++) printk("%d", password[j]);
        printk("\n");

        LED_set(LED0, LED_ON);
        in_len = 0;
        state = ST_INPUT_GUESS;
        break;

      case ST_INPUT_GUESS:
        if (b == 1 || b == 2 || b == 4) {
          in_stream[in_len++] = b;
          printk("%d", b);
        } else if (b == 8) {
          state = ST_CHECK_GUESS;
          printk("\n");
        }
        break;

      case ST_CHECK_GUESS:
        bool correct = true;

        if (in_len != pass_len) correct = false;
        else {
          for (j = 0; j < in_len; j++) {
            if (password[j] != in_stream[j]) {
              correct = false;
              break;
            }
          }
        }
          
        
          printk("%s\n", correct ? "Correct!" : "Incorrect!");

          if (correct) {
            LED_set(LED0, LED_OFF);
            state = ST_UNLOCKED;
          } else {
            in_len = 0;
            state = ST_INPUT_GUESS;
          }
        break;

      case ST_UNLOCKED:
        if (b != 0) {
          LED_set(LED0, LED_ON);
          in_len = 0;
          state = ST_INPUT_GUESS;
        }
        break;
     }
     k_msleep(SLEEP_MS);

  }

  return 0;

}

/*
[1]
static const uint8_t PASSWORD[] = {4, 4, 2, 1, 4, 2};
password is a combination of 1, 2, 4 -> 0001, 0010, 0100 in binary.
*/