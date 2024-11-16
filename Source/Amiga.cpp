//
// Created by Rich Elmes on 15.11.2024.
//

#include "Amiga.h"

#include <Emulator.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <utility>

#include "juce_gui_basics/positioning/juce_RelativeCoordinate.h"

namespace amigaMon {
    Amiga::Amiga()
    {
    }

    void Amiga::start()
    {
        vAmiga.mem.loadRom(std::filesystem::current_path().parent_path() += std::filesystem::path("/roms/kick.rom"));
        auto disk = vamiga::FloppyFile::make(std::filesystem::current_path().parent_path() += std::filesystem::path("/disks/df0.adf"));
        vAmiga.df0.insertMedia(*disk, true);
        vAmiga.emu->initialize();
        vAmiga.launch(this, &Amiga::callback);
    }

    vamiga::VAmiga& Amiga::getAmiga()
    {
        return vAmiga;
    }

    void Amiga::callback(const void *thisRef, Message message)
    {
        auto* amiga = static_cast<const amigaMon::Amiga*>(thisRef);
        auto& vAmiga = amiga->vAmiga;

        DBG("Message received: " << MsgTypeEnum::key(message.type));

        if(message.type == MSG_RSH_UPDATE)
        {
            vAmiga.emu->run();
        }
    }
} // amigaMon namespace