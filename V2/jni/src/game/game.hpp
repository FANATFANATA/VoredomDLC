#pragma once

#include "../other/memory.hpp"
#include "../protect/oxorany.hpp"
#include <stdint.h>

struct matrix {
    float m11, m12, m13, m14;
    float m21, m22, m23, m24;
    float m31, m32, m33, m34;
    float m41, m42, m43, m44;
};

namespace offsets {
    inline uint64_t player_manager = oxorany(135070688);
}

template<typename T = uint64_t>
inline T get_static(uint64_t type_offset) noexcept {
    if (proc::lib == 0) return 0;
    uint64_t cls = rpm<uint64_t>(proc::lib + type_offset);
    if (!cls) return 0;
    uint64_t obj = rpm<uint64_t>(cls + oxorany(0x58));
    if (!obj) return 0;
    uint64_t fields = rpm<uint64_t>(obj + oxorany(0xB8));
    if (!fields) return 0;
    return rpm<T>(fields);
}

inline uint64_t get_player_manager() noexcept {
    return get_static(offsets::player_manager);
}
