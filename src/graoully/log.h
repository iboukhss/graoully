#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

#define LOG_ERR(msg)  do { Serial.print(F("[ERR ] ")); Serial.println(msg); } while (0)
#define LOG_WARN(msg) do { Serial.print(F("[WARN] ")); Serial.println(msg); } while (0)
#define LOG_INFO(msg) do { Serial.print(F("[INFO] ")); Serial.println(msg); } while (0)
#define LOG_DBG(msg)  do { Serial.print(F("[DBG ] ")); Serial.println(msg); } while (0)

#define LOG_ERR_BEGIN()  do { Serial.print(F("[ERR ] ")); } while (0)
#define LOG_ERR_END()    do { Serial.println(); } while (0)

#define LOG_WARN_BEGIN() do { Serial.print(F("[WARN] ")); } while (0)
#define LOG_WARN_END()   do { Serial.println(); } while (0)

#define LOG_INFO_BEGIN() do { Serial.print(F("[INFO] ")); } while (0)
#define LOG_INFO_END()   do { Serial.println(); } while (0)

#define LOG_DBG_BEGIN()  do { Serial.print(F("[DBG ] ")); } while (0)
#define LOG_DBG_END()    do { Serial.println(); } while (0)

#define DRAGON_ASSERT(expr)                         \
    do {                                            \
        if (!(expr)) {                              \
            LOG_ERR_BEGIN();                        \
            Serial.print(F("assert: Failed '"));    \
            Serial.print(F(#expr));                 \
            Serial.print(F("' at "));               \
            Serial.print(__FILE__);                 \
            Serial.print(F(":"));                   \
            Serial.print(__LINE__);                 \
            LOG_ERR_END();                          \
            Serial.flush();                         \
            abort();                                \
        }                                           \
    } while (0)

#define DRAGON_UNREACHABLE()                        \
    do {                                            \
        LOG_ERR_BEGIN();                            \
        Serial.print(F("assert: Unreachable at ")); \
        Serial.print(__FILE__);                     \
        Serial.print(F(":"));                       \
        Serial.print(__LINE__);                     \
        LOG_ERR_END();                              \
        Serial.flush();                             \
        abort();                                    \
    } while(0)

#endif
