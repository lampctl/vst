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

#include "public.sdk/source/main/pluginfactory.h"

#include "lampctlcids.h"
#include "lampctlcontroller.h"
#include "lampctlprocessor.h"

#include "projectversion.h"

#define stringPluginName "Lampctl"

BEGIN_FACTORY ("Nathan Osman",
               "https://nathanosman.com",
               "mailto:nathan@nathanosman.com",
               PFactoryInfo::kNoFlags)

    DEF_CLASS2 (INLINE_UID_FROM_FUID(kLampctlProcessorUID),
                PClassInfo::kManyInstances,
                kVstAudioEffectClass,
                stringPluginName,
                Vst::kDistributable,
                LampctlVST3Category,
                FULL_VERSION_STR,
                kVstVersionString,
                LampctlProcessor::createInstance)

    DEF_CLASS2 (INLINE_UID_FROM_FUID(kLampctlControllerUID),
                PClassInfo::kManyInstances,
                kVstComponentControllerClass,
                stringPluginName,
                0,
                "",
                FULL_VERSION_STR,
                kVstVersionString,
                LampctlController::createInstance)

END_FACTORY
