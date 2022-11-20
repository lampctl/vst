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

#include <fstream>

#include "pluginterfaces/vst/ivstevents.h"

#include "lampctlcids.h"
#include "lampctlprocessor.h"
#include "util.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace Util;

LampctlProcessor::LampctlProcessor()
    : mSocket(nullptr)
{
    setControllerClass(kLampctlControllerUID);
}

LampctlProcessor::~LampctlProcessor()
{}

tresult LampctlProcessor::initialize(FUnknown *context)
{
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) {
        return result;
    }

    addEventInput(STR16("Event In"), 1);

    mSocket = new Socket([this](const std::string &description) {
        sendMessageWithAttribute(this,
                                 MSG_ID_STATUS,
                                 MSG_ATTR_DESCRIPTION,
                                 description);
    });

    return kResultOk;
}

tresult LampctlProcessor::terminate()
{
    return AudioEffect::terminate();
}

tresult LampctlProcessor::process(ProcessData &data)
{
    std::map<std::string, boost::json::array> eventsByProvider;

    // Process input events one by one
    IEventList *inputEvents = data.inputEvents;
    if (inputEvents) {
        int32 numEvents = inputEvents->getEventCount();
        for (int32 i = 0; i < numEvents; ++i) {
            Event event;
            if (inputEvents->getEvent(i, event) == kResultOk) {

                // We are only interested in note on / off events
                if (event.type != Event::kNoteOnEvent &&
                        event.type != Event::kNoteOffEvent) {
                    continue;
                }

                // Determine the pitch and value
                int eventPitch = event.type == Event::kNoteOnEvent ?
                            event.noteOn.pitch :
                            event.noteOff.pitch;
                bool eventValue = event.type == Event::kNoteOnEvent;

                // Attempt to find the lamp by index
                if (auto lampIt = mMap.find(eventPitch); lampIt != mMap.end()) {

                    // Create an event and add it to the provider map
                    auto lamp = lampIt->second;
                    auto obj = lamp.obj;
                    obj["value"] = eventValue;
                    eventsByProvider[lamp.provider].push_back(obj);
                }
            }
        }
    }

    // If there were events, send them
    for (const auto &kv : eventsByProvider) {
        sendEvents(kv.first, kv.second);
    }

    return kResultOk;
}

tresult LampctlProcessor::notify(IMessage *message)
{
    if (!message) {
        return kInvalidArgument;
    }

    std::string attr;

    if (isMessageWithAttribute(message, MSG_ID_CONNECT, MSG_ATTR_IP, attr)) {
        mSocket->connect(attr, "80");
        return kResultOk;
    }

    if (isMessageWithAttribute(message, MSG_ID_SET_MAP_FILE, MSG_ATTR_PATH, attr)) {
        std::string error;
        if (!loadMap(attr, error)) {
            sendMessageWithAttribute(this,
                                     MSG_ID_STATUS,
                                     MSG_ATTR_DESCRIPTION,
                                     error);
        }
        return kResultOk;
    }

    return kResultFalse;
}

bool LampctlProcessor::loadMap(const std::string &filename, std::string &error)
{
    std::map<int, Lamp> map;

    std::ifstream ifs(filename);
    if (!ifs.good()) {
        error = "unable to open map file";
        return false;
    }

    // Load the file contents
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    // Parse the JSON file
    boost::json::error_code ec;
    auto v = boost::json::parse(content, ec);
    if (ec) {
        error = ec.message();
        return false;
    }

    // For each value, create a lamp and assign it
    try {
        auto root = v.as_object();
        for (const auto &kv : root) {
            int key = std::stoi(kv.key());
            auto obj = kv.value().as_object();
            auto provider = boost::json::value_to<std::string>(obj["provider_id"]);
            obj.erase("provider_id");
            map.insert({key, Lamp{provider, obj}});
        }

    } catch (std::exception e) {
        error = e.what();
        return false;
    }

    // Assign the new map
    mMap = map;

    return true;
}

void LampctlProcessor::sendEvents(const std::string &provider,
                                  const boost::json::array &events)
{
    boost::json::object data;
    data["provider"] = provider;
    data["states"] = events;

    boost::json::object root;
    root["data"] = data;

    mSocket->send(boost::json::serialize(root));
}
