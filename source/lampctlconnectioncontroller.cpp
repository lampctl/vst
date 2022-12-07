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
#include "vstgui/uidescription/uiattributes.h"

#include "lampctlcontroller.h"
#include "lampctlconnectioncontroller.h"

using namespace VSTGUI;

LampctlConnectionController::LampctlConnectionController(LampctlController *controller)
    : mIP(nullptr)
    , mConnectStatus(nullptr)
    , mMapPath(nullptr)
    , mMapStatus(nullptr)
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
    case kIPTag:
        mController->setIP(mIP->getText());
        break;
    case kConnectTag:
        mController->connectToServer();
        break;
    case kBrowseTag:
        mController->browse();
        break;
    }
}

CView *LampctlConnectionController::verifyView(CView* view,
                                               const UIAttributes &,
                                               const IUIDescription *)
{
    CControl *pControl = dynamic_cast<CControl *>(view);
    if (pControl) {
        switch (pControl->getTag()) {
        case kIPTag:
            mIP = dynamic_cast<CTextEdit *>(view);
            mIP->setText(mController->getIP());
            break;
        case kConnectStatusTag:
            mConnectStatus = dynamic_cast<CTextLabel *>(view);
            mConnectStatus->setText(mController->getConnectStatus());
            break;
        case kMapPathTag:
            mMapPath = dynamic_cast<CTextLabel *>(view);
            mMapPath->setText(mController->getMapPath());
            break;
        case kMapStatusTag:
            mMapStatus = dynamic_cast<CTextLabel *>(view);
            mMapStatus->setText(mController->getMapStatus());
            break;
        }
    }

    return view;
}

void LampctlConnectionController::update()
{
    mConnectStatus->setText(mController->getConnectStatus());
    mMapPath->setText(mController->getMapPath());
    mMapStatus->setText(mController->getMapStatus());
}
