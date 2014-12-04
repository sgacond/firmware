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

uint8_t _dir, _pin2;
    
void encoder_pulse_complete();
void encoder_irq_handler_pin1();
void encoder_irq_handler_pin2();

tm_event encoder_pulse_event = TM_EVENT_INIT(encoder_pulse_complete);
    
void hw_encoder_bind(uint8_t pin1, uint8_t pin2)
{
    // remember pin2
    _pin2 = pin2;
    
    // aquire interrupt id and attach handler on pin1, falling.
    uint8_t intr1 = hw_interrupt_acquire();
    hw_interrupt_watch(pin1, 1 << TM_INTERRUPT_MODE_FALLING, intr1, encoder_irq_handler_pin1);
}

void encoder_pulse_complete()
{
    // push the _colony_emit helper function onto the stack
    lua_State* L = tm_lua_state;
    if(!L) return;
    
    tm_event_unref(&encoder_pulse_event);
    
    lua_getglobal(L, "_colony_emit");
    
    // push a process message identifier, and the direction flag
    lua_pushstring(L, "encoder_pulse");
    lua_pushnumber(L, _dir);
    
    // call _colony_emit to run the JS callback
    tm_checked_call(L, 2);
}
    
void encoder_irq_handler_pin1()
{
    // check pin2, when pin1 falling
    // the state of pin2 indicates direction
    _dir = hw_digital_read(_pin2);
    
    // trigger event
    tm_event_trigger(&encoder_pulse_event);
}

#ifdef __cplusplus
}
#endif
