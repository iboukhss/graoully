#ifndef SHOW_PRESETS_H
#define SHOW_PRESETS_H

#include "show.h"

// This sequence is best used when starting at a neutral position, ie. head tilt
// of 0.5 or higher
const ShowItem PROGMEM kSleepSeqData[] = {
    { 0,     "breath",  "off" },
    { 0,     "pan",     "0.5" },
    { 0,     "roll",    "0.5" },
    { 2000,  "tilt",    "0.1" },
    { 8000,  "pan",     "0.1" },
    { 10000, "eyelids", "0.0" },
    { 10000, "mouth",   "0.0" },
    { 13000, "eyes",    "off" },
    { 17000, "ears",    "0.0" },
    { 22000, "mouth",   "0.1" }
};

constexpr int kSleepSeqCount = sizeof(kSleepSeqData) / sizeof(kSleepSeqData[0]);

// This sequence works really well when the dragon is already sleeping
const ShowItem PROGMEM kAlertSeqData[] = {
    { 0,     "ears",    "0.5"  },
    { 4000,  "eyes",    "on"   },
    { 4000,  "breath",  "on"   },
    { 6000,  "eyelids", "0.3"  },
    { 10000, "pan",     "0.5"  },
    { 20000, "mouth",   "0.3"  },
    { 21000, "smoke",   "3000" },
    { 26000, "mouth",   "0.0"  },
    { 30000, "pan",     "0.1"  },
    { 32000, "eyelids", "0.0"  },
    { 35000, "eyes",    "off"  },
    { 35000, "breath",  "off"  },
    { 40000, "ears",    "0.0"  },
    { 45000, "mouth",   "0.1"  }
};

constexpr int kAlertSeqCount = sizeof(kAlertSeqData) / sizeof(kAlertSeqData[0]);

const ShowItem PROGMEM kWakeSeqData[] = {
    { 0,     "breath",   "on"   },
    { 0,     "mouth",    "0.1"  },
    { 1500,  "smoke",    "3000" }
};

constexpr int kWakeSeqCount = sizeof(kWakeSeqData) / sizeof(kWakeSeqData[0]);

const ShowPreset PROGMEM kPresetLibrary[] = {
    { "SLEEP", kSleepSeqData, kSleepSeqCount },
    { "ALERT", kAlertSeqData, kAlertSeqCount },
    { "WAKE",  kWakeSeqData,  kWakeSeqCount  }
};

constexpr int kNumPresets = sizeof(kPresetLibrary) / sizeof(kPresetLibrary[0]);

inline const ShowPreset* getPreset(const String& name)
{
    for (int i = 0; i < kNumPresets; i++) {
        ShowPreset localPreset;

        memcpy_P(&localPreset, &kPresetLibrary[i], sizeof(kPresetLibrary[i]));

        if (String(localPreset.name).equalsIgnoreCase(name)) {
            return &kPresetLibrary[i];
        }
    }

    return nullptr;
}

#endif
