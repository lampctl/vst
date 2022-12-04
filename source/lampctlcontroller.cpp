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

#include "lampctlcids.h"
#include "lampctlcontroller.h"
#include "lampctlconnectioncontroller.h"
#include "lampctlparamids.h"
#include "util.h"

#include "vstgui/lib/cfileselector.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace VSTGUI;

using namespace Util;

LampctlController::LampctlController()
    : mConnectStatus("Not connected")
    , mMapStatus("No file loaded")
{}

tresult LampctlController::initialize(FUnknown *context)
{
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk) {
        return result;
    }

    parameters.addParameter(
        STR16("Brightness"),
        nullptr,
        1, // step count
        0, // default value
        ParameterInfo::kCanAutomate,
        kBrightnessId
    );

    return result;
}

tresult LampctlController::notify(IMessage *message)
{
    if (!message) {
        return kInvalidArgument;
    }

    // Connection succeeded
    if (!strcmp(message->getMessageID(), MSG_ID_CONNECT_SUCCEEDED)) {
        mConnectStatus = "Connected";
        updateControllers();
        return kResultOk;
    }

    // Load map succeeded
    if (!strcmp(message->getMessageID(), MSG_ID_LOAD_MAP_SUCCEEDED)) {
        mMapStatus = "Map loaded";
        updateControllers();
        return kResultOk;
    }

    std::string attr;

    // Connection failed
    if (isMessageWithAttribute(message,
                               MSG_ID_CONNECT_FAILED,
                               MSG_ATTR_DESCRIPTION,
                               attr)) {
        mConnectStatus = UTF8String(attr);
        updateControllers();
        return kResultOk;
    }

    // Load map failed
    if (isMessageWithAttribute(message,
                               MSG_ID_LOAD_MAP_FAILED,
                               MSG_ATTR_DESCRIPTION,
                               attr)) {
        mMapStatus = UTF8String(attr);
        updateControllers();
        return kResultOk;
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

const UTF8String &LampctlController::getConnectStatus() const
{
    return mConnectStatus;
}

const UTF8String &LampctlController::getMapPath() const
{
    return mMapPath;
}

const UTF8String &LampctlController::getMapStatus() const
{
    return mMapStatus;
}

void LampctlController::setIP(const VSTGUI::UTF8String &ip)
{
    mIP = ip;
}

void LampctlController::connectToServer()
{
    ::sendMessage(this,
                  MSG_ID_CONNECT,
                  MSG_ATTR_IP,
                  mIP.getString());
}

void LampctlController::browse()
{
    CNewFileSelector *selector = CNewFileSelector::create(
        nullptr,
        CNewFileSelector::kSelectFile
    );
    if (selector) {
        selector->run([this](CNewFileSelector *selector) {
            int numSelectedFiles = selector->getNumSelectedFiles();
            if (numSelectedFiles == 1) {
                mMapPath = selector->getSelectedFile(0);
                ::sendMessage(this,
                              MSG_ID_SET_MAP_FILE,
                              MSG_ATTR_PATH,
                              mMapPath.getString());
            }
        });
        selector->forget();
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

void LampctlController::updateControllers()
{
    for (auto e : mConnectionControllers) {
        e->update();
    }
}
