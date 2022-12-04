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

#include <string>

#include "public.sdk/source/vst/vstcomponentbase.h"

namespace Util {

/**
 * @brief Sends a message
 * @param componentBase component used for sending the message
 * @param id message ID
 * @param attrId optional attribute ID
 * @param attrValue optional attribute value
 * @return kResultOk if the message was sent successfully
 */
Steinberg::tresult sendMessage(
    Steinberg::Vst::ComponentBase *componentBase,
    const Steinberg::FIDString id,
    const Steinberg::Vst::IAttributeList::AttrID attrId = nullptr,
    const std::string &attrValue = std::string()
);

/**
 * @brief Checks for a matching message and reads the specified attribute
 * @param message pointer to the message
 * @param id message ID
 * @param attrId attribute ID
 * @param attrValue attribute value
 * @return true if the message has the specified ID and attribute
 */
bool isMessageWithAttribute(
    Steinberg::Vst::IMessage *message,
    const Steinberg::FIDString id,
    const Steinberg::Vst::IAttributeList::AttrID attrId,
    std::string &attrValue
);

}
