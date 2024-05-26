#pragma once
#include "dynarray.h"

#include <poll.h>

dynarray *get_all_of_stdin();

void disable_echo_and_canonical();

void enable_echo_and_canonical();