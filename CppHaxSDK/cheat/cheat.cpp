#include "cheat.h"

#include "../haxsdk.h"
#include "../third_party/imgui/imgui.h"

#define HAXSDK_FUNCTION(a, n, c, m, s)     static BackendMethod* c ## __ ## m
#define HAXSDK_STATIC_FIELD(a, n, c, f, t) static t* c ## __ ## f
#define HAXSDK_FIELD_OFFSET(a, n, c, f)    static int c ## __ ## f
#include "cheat_data.h"

void HaxSdk::InitializeGameData() {
    #define HAXSDK_FUNCTION(a, n, c, m, s)     c ## __ ## m = BackendClass::find(a, n, #c)->find_method(#m, s)
    #define HAXSDK_STATIC_FIELD(a, n, c, f, t) c ## __ ## f = (t*)BackendClass::find(a, n, #c)->find_static_field(#f)
    #define HAXSDK_FIELD_OFFSET(a, n, c, f)    c ## __ ## f = BackendClass::find(a, n, #c)->find_field(#f)->offset()
    #include "cheat_data.h"
}

void HaxSdk::ApplyStyle() {

}

void HaxSdk::RenderBackground() {

}

void HaxSdk::RenderMenu() {
    ImGui::ShowDemoWindow();
}