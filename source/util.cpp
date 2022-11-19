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

#include "util.h"

#include "public.sdk/source/vst/utility/stringconvert.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Util {

tresult sendMessageWithAttribute(Steinberg::Vst::ComponentBase *componentBase,
                                 const FIDString id,
                                 const IAttributeList::AttrID attrId,
                                 const std::string &attrValue)
{
    IMessage *message = componentBase->allocateMessage();
    if (!message) {
        return kResultFalse;
    }

    // Allocate an FReleaser to free the message and set its ID
    FReleaser msgReleaser(message);
    message->setMessageID(id);

    // Convert the supplied attribute
    TChar tAttr[256] = {0};
    if (!VST3::StringConvert::convert(attrValue,
                                      tAttr,
                                      sizeof(tAttr) / sizeof(TChar))) {
        return kResultFalse;
    }

    // Assign the attribute and send it
    message->getAttributes()->setString(attrId, tAttr);
    componentBase->sendMessage(message);

    return kResultOk;
}

bool isMessageWithAttribute(IMessage *message,
                            const FIDString id,
                            const IAttributeList::AttrID attrId,
                            std::string &attrValue)
{
    if (strcmp(message->getMessageID(), id)) {
        return false;
    }

    TChar tAttr[256] = {0};
    if (message->getAttributes()->getString(attrId,
                                            tAttr,
                                            sizeof(tAttr) / sizeof(TChar)) != kResultOk) {
        return false;
    }

    attrValue = VST3::StringConvert::convert(tAttr);
    return true;
}

}
