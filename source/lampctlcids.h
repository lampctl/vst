/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

#define LampctlVST3Category Steinberg::Vst::PlugType::kFxInstrument

// Messages sent from the controller(s) to the processor
#define MSG_ID_CONNECT      "connect"
#define MSG_ID_SET_MAP_FILE "set_map_file"

// Messages sent from the processor to the controller(s)
#define MSG_ID_CONNECT_SUCCEEDED  "connect_succeeded"
#define MSG_ID_CONNECT_FAILED     "connect_failed"
#define MSG_ID_LOAD_MAP_SUCCEEDED "load_map_succeeded"
#define MSG_ID_LOAD_MAP_FAILED    "load_map_failed"

// Message attributes
#define MSG_ATTR_IP          "ip"
#define MSG_ATTR_PATH        "path"
#define MSG_ATTR_DESCRIPTION "description"

static const Steinberg::FUID kLampctlProcessorUID (0x605D8D9A, 0xB4E15C49, 0xADF0A282, 0xDD62978A);
static const Steinberg::FUID kLampctlControllerUID (0x89E36D93, 0xF2A55854, 0x8609F7C7, 0x2DEA89F9);
