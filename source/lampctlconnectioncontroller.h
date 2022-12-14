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

#include "vstgui/uidescription/icontroller.h"

// Forward declare LampctlController
class LampctlController;

/**
 * @brief Controller class for the UI elements on the page
 *
 * This class is used to facilitate access to control state by the
 * LampctlController class and pass along events (ex. button clicks).
 */
class LampctlConnectionController : public VSTGUI::IController
{
public:

    enum {
        kIPTag            = 1000,
        kConnectTag       = 1001,
        kConnectStatusTag = 1002,
        kMapPathTag       = 1003,
        kBrowseTag        = 1004,
        kMapStatusTag     = 1005
    };

    explicit LampctlConnectionController(LampctlController *controller);
    ~LampctlConnectionController();

    void valueChanged(VSTGUI::CControl* pControl) override;
    void controlEndEdit(VSTGUI::CControl* pControl) override;

    VSTGUI::CView* verifyView(VSTGUI::CView *view,
                              const VSTGUI::UIAttributes &attributes,
                              const VSTGUI::IUIDescription *description) override;

    void update();

private:

    VSTGUI::CTextEdit *mIP;
    VSTGUI::CTextLabel *mConnectStatus;
    VSTGUI::CTextLabel *mMapPath;
    VSTGUI::CTextLabel *mMapStatus;

    LampctlController *mController;
};
