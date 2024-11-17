# amigaMon

This is a simple [JUCE](https://github.com/juce-framework/JUCE) front end for the Amiga emulator [vAmiga](https://github.com/dirkwhoffmann/vAmiga).

It is a work in progress, but is already functional.  The plan is to provide an Amiga monitor that provides an interface to the internals of the emulator much like the coppenheimer website, but in a more flexible native app.

## Building

1. Clone the repository
2. Run `cmake -B build`
3. Run `cmake --build build`
4. Find the build in `build` and run `amigaMon`.

On Windows I currently have to add the following to `juce_gui_extra/misc/juce_PushNotifications.h`:
```cpp
#undef small
```
at line 306 just before:
```cpp
enum BadgeIconType
```
I'm sure JUCE will fix this soon.