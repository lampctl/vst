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

#include "pluginterfaces/vst/ivstevents.h"

#include "lampctlcids.h"
#include "lampctlprocessor.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

#include <ctime>
FILE *fp;
#define DBG(m, ...) \
    { \
      char buff[1024]; \
      snprintf(buff, sizeof(buff), m, __VA_ARGS__); \
      strncat(buff, "\n", sizeof(buff) - strlen(buff) - 1); \
      fwrite(buff, sizeof(char), strlen(buff), fp); \
      fflush(fp); \
    }

LampctlProcessor::LampctlProcessor()
{
    setControllerClass(kLampctlControllerUID);

    fp = fopen("F:\\debug.txt", "w");
}

LampctlProcessor::~LampctlProcessor()
{
    fclose(fp);
}

Steinberg::tresult LampctlProcessor::initialize(Steinberg::FUnknown *context)
{
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) {
        return result;
    }

    addEventInput(STR16("Event In"), 1);

    return kResultOk;
}

Steinberg::tresult LampctlProcessor::terminate()
{
    return AudioEffect::terminate();
}

Steinberg::tresult LampctlProcessor::process(Steinberg::Vst::ProcessData &data)
{
    // Process each of the events
    IEventList *inputEvents = data.inputEvents;
    if (inputEvents) {
        int32 numEvents = inputEvents->getEventCount();
        for (int32 i = 0; i < numEvents; ++i) {
            Event event;
            if (inputEvents->getEvent(i, event) == kResultOk) {

                if (event.type == Event::kNoteOnEvent) {
                    DBG("Note ON");
                    DBG("  Channel: %d", event.noteOn.channel);
                    DBG("  Pitch: %d", event.noteOn.pitch);
                    DBG("  Tuning: %f", event.noteOn.tuning);
                    DBG("  Velocity: %f", event.noteOn.velocity);
                    DBG("  Note ID: %ld", event.noteOn.noteId);
                    DBG("");
                }

                if (event.type == Event::kNoteOffEvent) {
                    DBG("Note OFF");
                    DBG("  Channel: %d", event.noteOff.channel);
                    DBG("  Pitch: %d", event.noteOff.pitch);
                    DBG("  Tuning: %f", event.noteOff.tuning);
                    DBG("  Velocity: %f", event.noteOff.velocity);
                    DBG("  Note ID: %ld", event.noteOff.noteId);
                    DBG("");
                }
            }

            /*
            // Send a message to the peer
            IMessage *message = allocateMessage();
            if (message) {
                FReleaser msgReleaser(message);
                message->setMessageID("test");
                message->getAttributes()->setString("Text", L"test");
                sendMessage(message);
            }
            */
        }
    }

    return kResultOk;
}
