// NativeTitleBarHeight.mm
#include <Cocoa/Cocoa.h>

extern "C" int getNativeTitleBarHeight()
{
    NSRect frame = [[NSApplication sharedApplication] mainWindow].frame;
    NSRect contentRect = [NSWindow contentRectForFrameRect:frame styleMask:[[NSApplication sharedApplication] mainWindow].styleMask];
    return frame.size.height - contentRect.size.height;
}