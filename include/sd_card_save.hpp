#pragma once

#include "readings.hpp"

void sd_card_save_init();
void sd_card_save_append(Readings reading);
void sd_card_save_close();
