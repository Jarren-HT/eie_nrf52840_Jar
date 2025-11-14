/*
    @file my_state_machine.c
*/

#include <zephyr/smf.h>
#include <zephyr/kernel.h>

#include "LED.h"
#include "BTN.h"
#include "my_state_machine.h"

// button help
static inline bool btn1() { return BTN_check_clear_pressed(BTN0); }
static inline bool btn2() { return BTN_check_clear_pressed(BTN1); }
static inline bool btn3() { return BTN_check_clear_pressed(BTN2); }
static inline bool btn4() { return BTN_check_clear_pressed(BTN3); }


// prototypes
static void s0_exit(void* o);
static enum smf_state_result s0_run(void* o);

static void s1_exit(void* o);
static enum smf_state_result s1_run(void* o);

static void s2_exit(void* o);
static enum smf_state_result s2_run(void* o);

static void s3_exit(void* o);
static enum smf_state_result s3_run(void* o);

static void s4_exit(void* o);
static enum smf_state_result s4_run(void* o);






// typedefs
enum pos_states {
    s0, // all off
    s1, // blink all at 4hz
    s2, // LED1, 3 ON   LED2, 4 OFF
    s3, // LED2, 4 ON   LED1, 3 OFF
    s4  // blink all at 16hz

};

typedef struct {
    struct smf_ctx ctx;

    uint16_t count;
} state_object_t;

static state_object_t obj;



// locals
static const struct smf_state states[] = {
    [s0] = SMF_CREATE_STATE(NULL, s0_run, s0_exit, NULL, NULL),
    [s1] = SMF_CREATE_STATE(NULL, s1_run, s1_exit, NULL, NULL),
    [s2] = SMF_CREATE_STATE(NULL, s2_run, s2_exit, NULL, NULL),
    [s3] = SMF_CREATE_STATE(NULL, s3_run, s3_exit, NULL, NULL),
    [s4] = SMF_CREATE_STATE(NULL, s4_run, s4_exit, NULL, NULL)
};


void state_machine_init() {
    obj.count = 0;
    smf_set_initial(SMF_CTX(&obj), &states[s0]);
}

int state_machine_run() {
    return smf_run_state(SMF_CTX(&obj));
}


// states 
// 0    |   all leds off
static enum smf_state_result s0_run(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);

    if (btn1()) smf_set_state(SMF_CTX(&obj), &states[s1]);

    return SMF_EVENT_HANDLED;
}

static void s0_exit(void* o) {
    // none :)
}


// 1    |   blink led1 at 4hz
static enum smf_state_result s1_run(void* o) {
    if (++obj.count >= 125) {
        obj.count = 0;
        LED_toggle(LED0);
    }

    if (btn4()) smf_set_state(SMF_CTX(&obj), &states[s0]);
    if (btn2()) smf_set_state(SMF_CTX(&obj), &states[s2]);
    if (btn3()) smf_set_state(SMF_CTX(&obj), &states[s4]);

    return SMF_EVENT_HANDLED;
}

static void s1_exit(void* o) {
    LED_set(LED0, LED_OFF);
}


// 2 |  leds 1, 3 ON leds 2, 4 OFF
static enum smf_state_result s2_run(void* o) {
    LED_set(LED0, LED_ON);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_ON);
    LED_set(LED3, LED_OFF);

    if (btn4()) smf_set_state(SMF_CTX(&obj), &states[s0]);

    if (++obj.count >= 1000) {
        obj.count = 0;
        smf_set_state(SMF_CTX(&obj), &states[s3]);
    }

    return SMF_EVENT_HANDLED;
}

static void s2_exit(void* o) {
    // none :)
}


// 3 |  leds 2, 4 ON leds 1, 3 OFF
static enum smf_state_result s3_run(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_ON);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_ON);

    if (btn4()) smf_set_state(SMF_CTX(&obj), &states[s0]);

    if (++obj.count >= 2000) {
        obj.count = 0;
        smf_set_state(SMF_CTX(&obj), &states[s2]);
    }

    return SMF_EVENT_HANDLED;
}

static void s3_exit(void* o) {
    // none :)
}


// 4    |   blink led1 at 16hz
static enum smf_state_result s4_run(void* o) {
    if (++obj.count >= 31) {
        obj.count = 0;
        LED_toggle(LED0);
        LED_toggle(LED1);
        LED_toggle(LED2);
        LED_toggle(LED3);
    }

    if (btn4()) smf_set_state(SMF_CTX(&obj), &states[s0]);

    return SMF_EVENT_HANDLED;
}

static void s4_exit(void* o) {
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
}