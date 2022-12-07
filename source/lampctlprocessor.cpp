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
#include <list>

#include "pluginterfaces/vst/ivstevents.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

#include "lampctlcids.h"
#include "lampctlprocessor.h"
#include "util.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace Util;

// TODO: make this configurable
const float VelocityMult = 10000.f;

LampctlProcessor::LampctlProcessor()
    : mSocket(nullptr)
    , mActive(false)
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

    mSocket = new Socket(
        [this]() {
            ::sendMessage(this, MSG_ID_CONNECT_SUCCEEDED);
        },
        [this](const std::string &description) {
            ::sendMessage(this,
                          MSG_ID_CONNECT_FAILED,
                          MSG_ATTR_DESCRIPTION,
                          description);
        }
    );

    return kResultOk;
}

tresult LampctlProcessor::terminate()
{
    return AudioEffect::terminate();
}

tresult LampctlProcessor::setActive(TBool state)
{
    mActive = state;
    return kResultOk;
}

tresult LampctlProcessor::process(ProcessData &data)
{
    if (!mActive) {
        return kResultOk;
    }

    std::map<int32, boost::json::object> notesById;
    std::map<std::string, std::list<int>> noteIDByProvider;

    // TODO handle notes that continue (kNoteOff followed by kNoteOn)

    // Process input events one by one
    IEventList *inputEvents = data.inputEvents;
    if (inputEvents) {
        int32 numEvents = inputEvents->getEventCount();
        for (int32 i = 0; i < numEvents; ++i) {
            Event event;
            if (inputEvents->getEvent(i, event) == kResultOk) {

                switch (event.type) {
                case Event::kNoteOnEvent:
                {
                    if (auto lampIt = mMap.find(event.noteOn.pitch); lampIt != mMap.end()) {
                        auto lamp = lampIt->second;
                        auto obj = lamp.obj;
                        auto id = event.noteOn.noteId;
                        obj["state"] = true;
                        obj["duration"] = int((1.f - event.noteOn.velocity) * VelocityMult);
                        notesById.insert({id, obj});
                        noteIDByProvider[lamp.provider].push_back(id);
                    }
                    break;
                }
                case Event::kNoteOffEvent:
                {
                    if (auto lampIt = mMap.find(event.noteOff.pitch); lampIt != mMap.end()) {
                        auto lamp = lampIt->second;
                        auto obj = lamp.obj;
                        auto id = event.noteOff.noteId;
                        obj["state"] = false;
                        obj["duration"] = int((1.f - event.noteOff.velocity) * VelocityMult);
                        notesById.insert({id, obj});
                        noteIDByProvider[lamp.provider].push_back(id);
                    }
                    break;
                }
                case Event::kNoteExpressionTextEvent:
                {
                    if (auto objIt = notesById.find(event.noteExpressionText.noteId); objIt != notesById.end()) {
                        auto &obj = objIt->second;

                        // Parse the JSON
                        auto text = VST3::StringConvert::convert(event.noteExpressionText.text);
                        boost::json::error_code ec;
                        auto v = boost::json::parse(text, ec).as_object();
                        if (ec) {
                            continue;
                        }

                        // Merge the keys into the object
                        for (const auto& kv : v) {
                            obj.insert(kv);
                        }
                    }
                    break;
                }
                }
            }
        }
    }

    // If there were events, send them
    for (const auto &kv : noteIDByProvider) {
        boost::json::array ar;
        for (const auto &id : kv.second) {
            ar.push_back(notesById[id]);
        }
        sendEvents(kv.first, ar);
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
        if (loadMap(attr, error)) {
            return ::sendMessage(this, MSG_ID_LOAD_MAP_SUCCEEDED);
        } else {
            return ::sendMessage(this,
                                 MSG_ID_LOAD_MAP_FAILED,
                                 MSG_ATTR_DESCRIPTION,
                                 error);
        }
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
    data["provider_id"] = provider;
    data["changes"] = events;

    boost::json::object root;
    root["data"] = data;

    mSocket->send(boost::json::serialize(root));
}
