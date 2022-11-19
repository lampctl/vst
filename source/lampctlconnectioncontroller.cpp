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

#include <string>

#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/cfileselector.h"
#include "vstgui/uidescription/uiattributes.h"

#include "lampctlcontroller.h"
#include "lampctlconnectioncontroller.h"

using namespace VSTGUI;

LampctlConnectionController::LampctlConnectionController(LampctlController *controller)
    : mURL(nullptr)
    , mMapPath(nullptr)
    , mStatus(nullptr)
    , mController(controller)
{}

LampctlConnectionController::~LampctlConnectionController()
{
    mController->removeConnectionController(this);
}

void LampctlConnectionController::valueChanged(CControl *)
{}

void LampctlConnectionController::controlEndEdit(CControl *pControl)
{
    switch (pControl->getTag()) {
    case kConnectTag:
        mController->connect(mURL->getText());
        break;
    case kBrowseTag:
        browse();
        break;
    }
}

void LampctlConnectionController::browse()
{
    CNewFileSelector *selector = CNewFileSelector::create(
        nullptr,
        CNewFileSelector::kSelectFile
    );
    if (selector) {
        selector->run([this](CNewFileSelector *selector) {
            int numSelectedFiles = selector->getNumSelectedFiles();
            if (numSelectedFiles == 1) {
                mController->setMapPath(selector->getSelectedFile(0));
            }
        });
        selector->forget();
    }
}

CView *LampctlConnectionController::verifyView(CView* view,
                                               const UIAttributes &,
                                               const IUIDescription *)
{
    CControl *pControl = dynamic_cast<CControl *>(view);
    if (pControl) {
        switch (pControl->getTag()) {
        case kURLTag:
            mURL = dynamic_cast<CTextEdit *>(view);
            break;
        case kMapPathTag:
            mMapPath = dynamic_cast<CTextLabel *>(view);
            mMapPath->setText(mController->getMapPath());
            break;
        case kStatusTag:
            mStatus = dynamic_cast<CTextLabel *>(view);
            break;
        }
    }

    return view;
}

void LampctlConnectionController::setMapPath(const VSTGUI::UTF8String &mapPath)
{
    mMapPath->setText(mapPath);
}

void LampctlConnectionController::setStatus(const UTF8String &status)
{
    mStatus->setText(status);
}
