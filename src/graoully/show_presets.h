#ifndef SHOW_PRESETS_H
#define SHOW_PRESETS_H

#include "show.h"
#include "log.h"

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

const ShowItem PROGMEM kWakeSeqData[] = {
    { 0,     "breath",   "on"   },
    { 0,     "mouth",    "0.1"  },
    { 1500,  "smoke",    "3000" }
};

const ShowItem PROGMEM kJc1SeqData[] = {
    { 0,     "tilt",   "0.6"  },
    { 0,     "eyes",   "on"   },
    { 3000,  "tilt",   "0.1"  },
    { 3000,  "pan",    "0.1"  },
    { 5000,  "mouth",  "1.0"  },
    { 6000,  "breath", "on"   },
    { 6000,  "smoke",  "7000" },
    { 15000, "pan",    "0.9"  }
};

const ShowItem PROGMEM kHissRoarSeqData[] = {
    { 0,     "eyes",    "on"     },
    { 0,     "eyelids", "1.0"    },
    { 0,     "ears",    "1.0"    },
    { 0,     "pan",     "0.7"    },
    { 0,     "tilt",    "0.1"    },
    { 0,     "roll",    "1.0"    },
    { 1000,  "mp3",     "play?2" },
    { 1000,  "mouth",   "0.3"    },
    { 1000,  "breath",  "500"    },
    { 1000,  "smoke",   "500"    },
    { 2000,  "mouth",   "0.6"    },
    { 2000,  "breath",  "500"    },
    { 2000,  "smoke",   "500"    },
    { 3000,  "mouth",   "1.0"    },
    { 3000,  "breath",  "on"     },
    { 3000,  "smoke",   "9000"   },
    { 10000, "pan",     "0.3"    },
    { 16000, "ears",    "0.0"    }
};

const ShowItem PROGMEM kSleepGruntSeqData[] = {
    { 0,     "eyes",    "off"     },
    { 0,     "eyelids", "0.0"    },
    { 0,     "tilt",    "0.1"    },
    { 0,     "pan",     "0.7"    },
    { 0,     "roll",    "1.0"    },
    { 0,     "mouth",   "0.0"    },
    { 2000,  "breath",  "on"     },
    { 2000,  "mp3",     "play?4" },
    { 2000,  "mouth",   "0.1"    },
    { 2000,  "smoke",   "1500"   },
    { 4000,  "breath",  "off"    },
    { 4000,  "mouth",   "0.0"    },

    // Second puff
    { 8000,   "pan",     "0.5"   },
    { 8000,   "roll",    "0.8"   },
    { 10000,  "breath",  "on"    },
    { 10000,  "mouth",   "0.1"   },
    { 10000,  "smoke",   "1500"  },
    { 12000,  "breath",  "off"   },
    { 12000,  "mouth",   "0.0"   },

    // Third puff
    { 18000,   "pan",     "0.9"  },
    { 18000,   "roll",    "0.3"  },
    { 20000,  "breath",  "on"    },
    { 20000,  "mouth",   "0.1"   },
    { 20000,  "smoke",   "1500"  },
    { 22000,  "breath",  "off"   },
    { 22000,  "mouth",   "0.0"   },

    // Final
    { 26000, "pan",     "0.7"  },
    { 26000, "roll",    "1.0"  },
    { 28000, "mouth",   "0.1" }
};

// Vibecoded show presets (DO NOT USE)

// 1. SNOOZE: A restless, twitchy sleeping shift
const ShowItem PROGMEM kSnoozeSeqData[] = {
    { 0,     "ears",    "0.2" },
    { 1500,  "roll",    "0.4" },
    { 3000,  "ears",    "0.0" },
    { 5000,  "mouth",   "0.2" },
    { 7000,  "mouth",   "0.0" },
    { 9000,  "breath",  "on"  },
    { 12000, "breath",  "off" }
};

// 2. CURIOUS: Head tilting and scanning around inquisitively
const ShowItem PROGMEM kCuriousSeqData[] = {
    { 0,     "eyes",    "on"  },
    { 0,     "eyelids", "0.7" },
    { 1000,  "pan",     "0.7" },
    { 2500,  "tilt",    "0.3" },
    { 4000,  "ears",    "0.8" },
    { 6000,  "pan",     "0.3" },
    { 8000,  "tilt",    "0.6" },
    { 10000, "eyelids", "0.2" },
    { 12000, "pan",     "0.5" },
    { 14000, "eyes",    "off" }
};

// 3. SURPRISE: A sudden jump, wide eyes, and smoke burst
const ShowItem PROGMEM kSurpriseSeqData[] = {
    { 0,     "eyes",    "on"   },
    { 0,     "eyelids", "1.0"  },
    { 0,     "ears",    "1.0"  },
    { 200,   "tilt",    "0.8"  },
    { 200,   "pan",     "0.5"  },
    { 400,   "mouth",   "0.6"  },
    { 600,   "smoke",   "3000" },
    { 3000,  "mouth",   "0.1"  },
    { 5000,  "eyelids", "0.4"  },
    { 7000,  "ears",    "0.3"  }
};

// 4. GROWL: Aggressive posture, heavy breathing, and a mouth snap
const ShowItem PROGMEM kGrowlSeqData[] = {
    { 0,     "breath",  "on"   },
    { 0,     "ears",    "0.2"  },
    { 1000,  "tilt",    "0.2"  },
    { 2000,  "breath",  "on"   },
    { 2000,  "mouth",   "0.5"  },
    { 2000,  "smoke",   "4000" },
    { 4000,  "roll",    "0.6"  },
    { 6000,  "mouth",   "0.0"  },
    { 8000,  "pan",     "0.4"  },
    { 10000, "breath",  "off"  }
};

// 5. STRETCH: A slow yawn and physical reset
const ShowItem PROGMEM kStretchSeqData[] = {
    { 0,     "pan",     "0.5"  },
    { 0,     "roll",    "0.5"  },
    { 1500,  "tilt",    "0.8"  },
    { 3000,  "breath",  "on"   },
    { 3000,  "mouth",   "0.8"  },
    { 3000,  "smoke",   "3000" },
    { 5000,  "ears",    "0.9"  },
    { 7000,  "mouth",   "0.0"  },
    { 9000,  "tilt",    "0.5"  },
    { 11000, "ears",    "0.0"  }
};

// 6. SENTRY: Methodical guard scan left and right
const ShowItem PROGMEM kSentrySeqData[] = {
    { 0,     "eyes",    "on"  },
    { 0,     "eyelids", "0.5" },
    { 1000,  "pan",     "0.9" },
    { 4000,  "pan",     "0.1" },
    { 7000,  "pan",     "0.5" },
    { 9000,  "ears",    "0.6" },
    { 11000, "eyes",    "off" }
};

// 7. BLINK: A quick, subtle reset of expression
const ShowItem PROGMEM kBlinkSeqData[] = {
    { 0,     "eyelids", "0.0" },
    { 300,   "eyelids", "0.5" },
    { 600,   "eyelids", "0.0" },
    { 1200,  "ears",    "0.4" },
    { 2000,  "ears",    "0.0" }
};

const ShowPreset PROGMEM kPresetLibrary[] = {
    { "HISSROAR",   kHissRoarSeqData,   arrayCount(kHissRoarSeqData)   }
};

constexpr int kNumPresets = arrayCount(kPresetLibrary);

inline const ShowPreset* getPresetByName(const String& name)
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

inline const ShowPreset* getRandomPreset()
{
    if (kNumPresets == 0) return nullptr;

    int randomIndex = random(0, kNumPresets);

    return &kPresetLibrary[randomIndex];
}

#endif
