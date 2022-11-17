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

#include "vstgui/lib/cstring.h"

#include "lampctlcontroller.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

using namespace VSTGUI;

tresult LampctlController::initialize(FUnknown *context)
{
    mTextLabel = nullptr;

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

    if (!strcmp(message->getMessageID(), "test") && mTextLabel) {

        TChar buffer[256];
        if (message->getAttributes()->getString("Text", buffer, sizeof(buffer)) == kResultOk) {
            char8 cstr[256];
            Steinberg::String tmp (buffer);
            tmp.copyTo8 (cstr, 0, 255);
            mTextLabel->setText(cstr);
            return kResultOk;
        }

        return kResultOk;
    }

    return kResultFalse;
}

IPlugView *LampctlController::createView(FIDString name)
{
    if (FIDStringsEqual(name, ViewType::kEditor)) {
        return new VST3Editor(this, "Editor", "plug.uidesc");
    }

    return nullptr;
}

CView *LampctlController::createCustomView(VSTGUI::UTF8StringPtr name,
                                           const VSTGUI::UIAttributes& attributes,
                                           const VSTGUI::IUIDescription* description,
                                           VSTGUI::VST3Editor* editor)
{
    if (name && strcmp(name, "TextLabel") == 0) {
        CRect size;
        mTextLabel = new CTextLabel(size);
        return mTextLabel;
    }
    return nullptr;
}

void LampctlController::willClose(VST3Editor *editor)
{
    mTextLabel = nullptr;
}
