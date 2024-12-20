# Copyright 2024 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# From https://wiki.tcl-lang.org/page/constants
proc const {name value} {
    uplevel 1 [list set $name $value]
    uplevel 1 [list trace var $name w {error constant;} ]
}

const ARG_IDX_SRC_FREQ 0
const ARG_IDX_TARGET_FREQ 1

const SUCCESS 0
const ERROR_ARG_COUNT 1
const ERROR_ARG_VALUE 3

# input frequency range
const MIN_IN_FREQ 1
const MAX_IN_FREQ 40000

# Lifetime counter divider range
const PRESCALER_MIN 2
const PRESCALER_MAX 32

# wakeup value range
const WAKEUP_MIN 1
const WAKEUP_MAX 4294967295

const PRESCALER_KEY "prescalerCalc"
const WAKEUP_KEY "wakeupCalc"

set channelName stdout

if {[chan names ModusToolbox] eq "ModusToolbox"} {
    set channelName ModusToolbox
}

proc solve_ltc {} {
    if {$::argc != $::ARG_IDX_TARGET_FREQ + 1} {
        error "Lifetime counter solver requires 2 arguments:
\tdouble srcFreq - Source clock frequency for the Lifetime counter in KHz.
\tdouble wakeupPeriod - Desired wakeup time for calculation.
        return $::ERROR_ARG_COUNT
    }

    set srcFreq [lindex $::argv $::ARG_IDX_SRC_FREQ]
    set wakeupPeriod [lindex $::argv $::ARG_IDX_TARGET_FREQ]
    if {![string is double $wakeupPeriod]} {
        error "Unable to parse argument values: either $wakeupPeriod is not a floating-point number."
        return $::ERROR_ARG_VALUE
    }

    if {![is_freq_in_range $srcFreq]} {
        error "Invalid Lifetime counter input frequency '$srcFreq'. Must be within the range $::MIN_IN_FREQ-$::MAX_IN_FREQ."
        return $::ERROR_ARG_VALUE
    }

    set wakeupPeriod [expr {double($wakeupPeriod)}]
    if {![is_wakeup_period_in_range $wakeupPeriod $srcFreq]} {
        error "Invalid Lifetime counter wakeup period value '$wakeupPeriod' seconds. Must be within the range."
        return $::ERROR_ARG_VALUE
    }

    return [solve_ltc_internal $srcFreq $wakeupPeriod]
}

proc solve_ltc_internal {srcFreq wakeupPeriod} {
    for {set div $::PRESCALER_MIN} {$div <= $::PRESCALER_MAX} {set div [expr {$div * 2}]} {
        set op_res [expr {$wakeupPeriod * $srcFreq / $div}]
        if {[expr {$::WAKEUP_MIN <= $op_res && $op_res <= $::WAKEUP_MAX}]} {
            set prescalerCalc $div
            break
        }
    }

    set wakeupCalc [expr {round($wakeupPeriod * $srcFreq / $prescalerCalc)}]

    puts $::channelName "param:$::WAKEUP_KEY=$wakeupCalc"
    puts $::channelName "param:$::PRESCALER_KEY=$prescalerCalc"
    return $::SUCCESS
}

proc is_wakeup_period_in_range {wakeupPeriod freq} {
    set wakeupPeriodMin [expr {$::WAKEUP_MIN * $::PRESCALER_MIN / double($freq)}]
    set wakeupPeriodMax [expr {$::WAKEUP_MAX * $::PRESCALER_MAX / double($freq)}]
    return [expr {$wakeupPeriodMin <= $wakeupPeriod && $wakeupPeriod <= $wakeupPeriodMax}]
}

proc is_freq_in_range {freq} {
    return [expr {$::MIN_IN_FREQ <= $freq && $freq <= $::MAX_IN_FREQ}]
}

solve_ltc
