#ifndef PSYDAPT_VERSION_HPP
#define PSYDAPT_VERSION_HPP

#define PSYDAPT_VERSION_MAJOR 0
#define PSYDAPT_VERSION_MINOR 1
#define PSYDAPT_VERSION_PATCH 8

#if defined(PSYDAPT_DISABLE_EXCEPTIONS)
#include <iostream>
#define PSYDAPT_THROW(_, msg)          \
    {                                  \
        std::cerr << msg << std::endl; \
        std::abort();                  \
    }
#else
#define PSYDAPT_THROW(exception, msg) throw exception(msg)
#endif
#endif
