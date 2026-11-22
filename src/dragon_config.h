#ifndef DRAGON_CONFIG_H
#define DRAGON_CONFIG_H

// Keep hardware disabled by default until the Arduino slave and actuators are
// connected. Override with -DDRAGON_VIRTUAL_MODE=0 for a hardware build.
#ifndef DRAGON_VIRTUAL_MODE
#define DRAGON_VIRTUAL_MODE 1
#endif

#endif