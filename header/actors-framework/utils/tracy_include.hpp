#pragma once

#pragma once

#ifndef TRACY_ENABLE

#define ZoneScoped
#define ZoneScopedN(txt)
#define TracySecureAlloc(ptr, count)
#define TracySecureFree(ptr)

#else

#include "Tracy.hpp"

#ifdef TRACY_MEM_PROFILE

inline void* operator new(std::size_t count) {
    auto ptr = malloc(count);
    TracySecureAlloc(ptr, count);
    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    TracySecureFree(ptr);
    free(ptr);
}

inline void operator delete(void* ptr, std::size_t) noexcept {
    TracySecureFree(ptr);
    free(ptr);
}

#endif

#endif
