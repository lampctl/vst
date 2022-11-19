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

#include <list>

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include "lampctlconnectioncontroller.h"

/**
 * @brief Controller class for presenting the plugin user interface
 *
 * The controller class displays the controls for connecting to the websocket
 * and browsing for the map file. It receives status updates from the
 * processor class.
 */
class LampctlController :
        public Steinberg::Vst::EditControllerEx1,
        public VSTGUI::VST3EditorDelegate
{
public:

    LampctlController() = default;
    ~LampctlController() SMTG_OVERRIDE = default;

    static Steinberg::FUnknown *createInstance(void *)
    {
        return (Steinberg::Vst::IEditController *)new LampctlController;
    }

    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown *context) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API notify(Steinberg::Vst::IMessage *message) SMTG_OVERRIDE;

    Steinberg::IPlugView *createView(Steinberg::FIDString name) SMTG_OVERRIDE;

    VSTGUI::IController *createSubController(VSTGUI::UTF8StringPtr name,
                                             const VSTGUI::IUIDescription *description,
                                             VSTGUI::VST3Editor *editor) SMTG_OVERRIDE;

    void connect(const VSTGUI::UTF8String &url);
    void setMapPath(const VSTGUI::UTF8String &mapPath);
    void removeConnectionController(LampctlConnectionController *connectionController);

private:

    std::List<LampctlConnectionController *> mConnectionControllers;

    VSTGUI::UTF8String mURL;
    VSTGUI::UTF8String mMapPath;
    VSTGUI::UTF8String mStatus;
};
