#pragma once

#ifndef NSROOT
#define NSROOT Myth
#endif

#if (defined(_WIN32) || defined(_WIN64))
#include "windows/os-types.h"
#else
#include "unix/os-types.h"
#endif
