/*
 * Copyright 2009, 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PlatformBridge_h
#define PlatformBridge_h

#include "KURL.h"
#include "npapi.h"
#include "PlatformString.h"

#include <wtf/Vector.h>

// V8 bindings use the ARRAYSIZE_UNSAFE macro. This macro was copied
// from http://src.chromium.org/viewvc/chrome/trunk/src/base/basictypes.h
//
// ARRAYSIZE_UNSAFE performs essentially the same calculation as arraysize,
// but can be used on anonymous types or types defined inside
// functions. It's less safe than arraysize as it accepts some
// (although not all) pointers. Therefore, you should use arraysize
// whenever possible.
//
// The expression ARRAYSIZE_UNSAFE(a) is a compile-time constant of type
// size_t.
//
// ARRAYSIZE_UNSAFE catches a few type errors. If you see a compiler error
//
//   "warning: division by zero in ..."
//
// when using ARRAYSIZE_UNSAFE, you are (wrongfully) giving it a pointer.
// You should only use ARRAYSIZE_UNSAFE on statically allocated arrays.
//
// The following comments are on the implementation details, and can
// be ignored by the users.
//
// ARRAYSIZE_UNSAFE(arr) works by inspecting sizeof(arr) (the # of bytes in
// the array) and sizeof(*(arr)) (the # of bytes in one array
// element). If the former is divisible by the latter, perhaps arr is
// indeed an array, in which case the division result is the # of
// elements in the array. Otherwise, arr cannot possibly be an array,
// and we generate a compiler error to prevent the code from
// compiling.
//
// Since the size of bool is implementation-defined, we need to cast
// !(sizeof(a) & sizeof(*(a))) to size_t in order to ensure the final
// result has type size_t.
//
// This macro is not perfect as it wrongfully accepts certain
// pointers, namely where the pointer size is divisible by the pointee
// size. Since all our code has to go through a 32-bit compiler,
// where a pointer is 4 bytes, this means all pointers to a type whose
// size is 3 or greater than 4 will be (righteously) rejected.

#define ARRAYSIZE_UNSAFE(a) \
  ((sizeof(a) / sizeof(*(a))) / \
   static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))


class NPObject;

namespace WebCore {

#if USE(ACCELERATED_COMPOSITING)
class LayerAndroid;
#endif

class FrameView;
class Widget;

// An interface to the embedding layer, which has the ability to answer
// questions about the system and so on...
// This is very similar to ChromiumBridge and the two are likely to converge
// in the future.
//
// The methods in this class all need to reach across a JNI layer to the Java VM
// where the embedder runs. The JNI machinery is currently all in WebKit/android
// but the long term plan is to move to the WebKit API and share the bridge and its
// implementation with Chromium. The JNI machinery will then move outside of WebKit,
// similarly to how Chromium's IPC layer lives outside of WebKit.
class PlatformBridge {
public:
    // KeyGenerator
    static WTF::Vector<String> getSupportedKeyStrengthList();
    static String getSignedPublicKeyAndChallengeString(unsigned index, const String& challenge, const KURL&);
    // Cookies
    static void setCookies(const KURL&, const String& value);
    static String cookies(const KURL&);
    static bool cookiesEnabled();
    // Plugin
    static NPObject* pluginScriptableObject(Widget*);
    // Popups
    static bool popupsAllowed(NPP);

    // These ids need to be in sync with the constants in BrowserFrame.java
    enum rawResId {
        NoDomain = 1,
        LoadError,
        DrawableDir,
        FileUploadLabel,
        ResetLabel,
        SubmitLabel
    };
    static String* globalLocalizedName(rawResId resId);

#if USE(ACCELERATED_COMPOSITING)
    // Those methods are used by the layers system
    static void setUIRootLayer(const FrameView* view, const LayerAndroid* layer);
    static void immediateRepaint(const FrameView* view);
#endif // USE(ACCELERATED_COMPOSITING)
    static int screenWidth(const FrameView* view);
    static int screenHeight(const FrameView* view);

    // Whether the WebView is paused.
    // ANDROID
    // TODO: Upstream to webkit.org. See https://bugs.webkit.org/show_bug.cgi?id=34082
    static bool isWebViewPaused(const FrameView*);

    static bool canScroll(const FrameView*);
    static String resolveFileNameForContentUri(const String&);
};

}
#endif // PlatformBridge_h
