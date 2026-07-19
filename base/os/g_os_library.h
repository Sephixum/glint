#ifndef GLINT_BASE_OS_OS_LIBRARY_H
#define GLINT_BASE_OS_OS_LIBRARY_H

#include "../../base/string/g_string.h"

typedef struct os_library
{
    void* opaque[1];
} os_library;

// Load a shared library (.so / .dylib / .dll)
internal os_library os_library_load(string path);

// Look up a symbol (function or data) by name — cast the result as needed
internal void* os_library_symbol(os_library lib, string name);

// Unload the library and free resources
internal void os_library_unload(os_library lib);

// Return a string describing the last error (empty string if none)
internal string os_library_error(void);

#endif // GLINT_BASE_OS_OS_LIBRARY_H