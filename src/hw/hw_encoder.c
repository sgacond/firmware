// Copyright 2014 Technical Machine, Inc. See the COPYRIGHT
// file at the top-level directory of this distribution.
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifdef __cplusplus
extern "C" {
#endif

// Includes
#include "tm.h"
#include "hw.h"
#include "colony.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <variant.h>

//uint8_t val = 0;
double _ts1 = 0;
double _ts2 = 0;
    
void encoder_pulse_complete();
void encoder_irq_handler_pin1();
void encoder_irq_handler_pin2();

void timer_callback();

tm_event encoder_pulse_event = TM_EVENT_INIT(encoder_pulse_complete);
    
void hw_encoder_bind(uint8_t pin1, uint8_t pin2)
{
    uint8_t intr1 = hw_interrupt_acquire();
    hw_interrupt_watch(pin1, 1 << TM_INTERRUPT_MODE_FALLING, intr1, encoder_irq_handler_pin1);

    uint8_t intr2 = hw_interrupt_acquire();
    hw_interrupt_watch(pin2, 1 << TM_INTERRUPT_MODE_FALLING, intr2, encoder_irq_handler_pin2);
}

void encoder_pulse_complete()
{
    // push the _colony_emit helper function onto the stack
    lua_State* L = tm_lua_state;
    if(!L) return;
    
    tm_event_unref(&encoder_pulse_event);
    
    lua_getglobal(L, "_colony_emit");
    
    // the process message identifier
    lua_pushstring(L, "encoder_pulse");
    
    // the state of the other i/o indicates direction
    lua_pushnumber(L, _ts2 - _ts1);
    _ts1 = 0;
    _ts2 = 0;
    
    // call _colony_emit to run the JS callback
    tm_checked_call(L, 2);
}
    
/*void encoder_irq_handler()
{
    tm_event_trigger(&encoder_pulse_event);
}*/
    
void timer_callback() {
    
}

    
void encoder_irq_handler_pin1()
{
    _ts1 = tm_timestamp();
    if(_ts2 > 0)
        tm_event_trigger(&encoder_pulse_event);
}

void encoder_irq_handler_pin2()
{
    _ts2 = tm_timestamp();
    if(_ts1 > 0)
        tm_event_trigger(&encoder_pulse_event);
}

#ifdef __cplusplus
}
#endif
