#ifndef PSYDAPT_VERSION_HPP
#define PSYDAPT_VERSION_HPP

#define PSYDAPT_VERSION_MAJOR 0
#define PSYDAPT_VERSION_MINOR 1
#define PSYDAPT_VERSION_PATCH 4

#if defined(PSYDAPT_DISABLE_EXCEPTIONS)
#if defined(__EMSCRIPTEN__)
#define PSYDAPT_THROW(_, msg)   \
    {                           \
        EM_ASM({ throw msg; }); \
    }
#else
#include <iostream>
#define PSYDAPT_THROW(_, msg)          \
    {                                  \
        std::cerr << msg << std::endl; \
        std::abort();                  \
    }
#endif
#else
#define PSYDAPT_THROW(exception, msg) throw exception(msg)
#endif

#endif
