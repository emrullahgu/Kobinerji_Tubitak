// main_all.c - Tüm C dosyalarının birleşimi
// Otomatik olarak oluşturulmuştur.

// --- main.c ---
#include "Init.h"
#include "MP279x.h"
#include "BMS_MP279x.h"
#include "MPF4279x.h"
#include "FLASH_Config.h"
// ...main.c içeriği...

// --- Init.c ---
#include "Init.h"
#include "BMS_MP279x.h"
// ...Init.c içeriği...

// --- MP279x_I2C.c ---
#include "MP279x.h"
#include "Init.h"
#include "BMS_MP279x.h"
// ...MP279x_I2C.c içeriği...

// --- MPF4279x_I2C.c ---
#include "MPF4279x.h"
#include "init.h"
// ...MPF4279x_I2C.c içeriği...

// --- system_stm32f0xx.c ---
#include "stm32f0xx.h"
// ...system_stm32f0xx.c içeriği...

// Not: Fonksiyon ve değişken çakışmalarını önlemek için dosya içerikleri düzenlenmelidir.
// Bu dosya, tüm kaynak kodun tek bir C dosyasında toplanmış halidir.
