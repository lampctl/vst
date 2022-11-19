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

#include "public.sdk/source/vst/utility/stringconvert.h"
#include "vstgui/lib/cstring.h"

#include "lampctlcids.h"
#include "lampctlcontroller.h"
#include "lampctlconnectioncontroller.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace VSTGUI;

tresult LampctlController::initialize(FUnknown *context)
{
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk) {
        return result;
    }

    return result;
}

tresult LampctlController::notify(IMessage *message)
{
    if (!message) {
        return kInvalidArgument;
    }

    if (!strcmp(message->getMessageID(), MSG_ID_STATUS)) {
        TChar description[256] = {0};
        if (message->getAttributes()->getString(MSG_ATTR_DESCRIPTION,
                                                description,
                                                sizeof(description) / sizeof(TChar)) == kResultOk) {
            mStatus = UTF8String(VST3::StringConvert::convert(description));
            for (auto e : mConnectionControllers) {
                e->setStatus(mStatus);
            }
        }
    }

    return kResultFalse;
}

IPlugView *LampctlController::createView(FIDString name)
{
    if (FIDStringsEqual(name, ViewType::kEditor)) {
        UIDescription *description = new UIDescription("plug.uidesc");
        if (description->parse()) {
            return new VST3Editor(description, this, "Editor");
        }
    }

    return nullptr;
}

IController *LampctlController::createSubController(VSTGUI::UTF8StringPtr name,
                                                    const VSTGUI::IUIDescription *,
                                                    VSTGUI::VST3Editor *)
{
    if (FIDStringsEqual(name, "ConnectionController")) {
        auto connectionController = new LampctlConnectionController(this);
        mConnectionControllers.push_back(connectionController);
        return connectionController;
    }

    return nullptr;
}

void LampctlController::connect(const VSTGUI::UTF8String &url)
{
    //...
}

void LampctlController::setMapPath(const VSTGUI::UTF8String &mapPath)
{
    mMapPath = mapPath;

    // Send a message to the processor indicating the map file has changed
    IMessage *message = allocateMessage();
    if (message) {
        FReleaser msgReleaser(message);
        message->setMessageID(MSG_ID_SET_MAP_FILE);
        TChar tPath[256] = {0};
        if (VST3::StringConvert::convert(mMapPath.getString(),
                                         tPath,
                                         sizeof(tPath) / sizeof(TChar))) {
            message->getAttributes()->setString(MSG_ATTR_PATH, tPath);
            sendMessage(message);
        }
    }

    // Update each of the controllers
    for (auto e : mConnectionControllers) {
        e->setStatus(mMapPath);
    }
}

void LampctlController::removeConnectionController(LampctlConnectionController *connectionController)
{
    auto it = std::find(mConnectionControllers.begin(),
                        mConnectionControllers.end(),
                        connectionController);
    if (it != mConnectionControllers.end()) {
        mConnectionControllers.erase(it);
    }
}
