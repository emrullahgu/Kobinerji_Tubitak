# TÜBİTAK 7260634 — VGX BMS Firmware Datasheet

**Proje Adı:** Yapay Zeka Destekli Lityum İyon Batarya Analizi ve Geri Dönüşüm Sistemi  
**Kuruluş:** KOBİNERJİ A.Ş. — Kobinerji Mühendislik ve Enerji Verimliliği Danışmanlık A.Ş.  
**Proje Kodu:** TÜBİTAK 1507 — 7260634  
**Doküman Sürümü:** 1.0.0  
**Tarih:** 19 Nisan 2026  
**Hazırlayan:** Emrullah GÜNAY  
**Adres:** Ceran Plaza, Kemalpaşa OSB, Gazi Blv. No:177/19, 35730 Kemalpaşa/İzmir  

---

## İçindekiler

1. [Genel Bakış](#1-genel-bakış)
2. [Donanım Platformu](#2-donanım-platformu)
3. [Firmware Mimarisi](#3-firmware-mimarisi)
4. [I2C Haberleşme Protokolü](#4-i2c-haberleşme-protokolü)
5. [BMS Koruma Fonksiyonları](#5-bms-koruma-fonksiyonları)
6. [Fuel Gauge (SoC/SoH)](#6-fuel-gauge-socsoh)
7. [Dengeleme Sistemi](#7-dengeleme-sistemi)
8. [Flash Konfigürasyon](#8-flash-konfigürasyon)
9. [Register Haritası](#9-register-haritası)
10. [Pin Bağlantıları](#10-pin-bağlantıları)
11. [Elektriksel Özellikler](#11-elektriksel-özellikler)
12. [Geliştirme Ortamı](#12-geliştirme-ortamı)

---

## 1. Genel Bakış

### 1.1. Amaç

VGX Batarya Yönetim Sistemi (BMS) gömülü yazılımı, 7–16S Li-ion pil paketleri için tam kapsamlı koruma, izleme, fuel gauge ve haberleşme fonksiyonları sağlayan yüksek güvenilirlikli bir firmware çözümüdür.

### 1.2. Temel Özellikler

| Özellik | Açıklama |
|---------|----------|
| Hücre Konfigürasyonu | 7S – 16S seri bağlı Li-ion hücre desteği |
| Koruma Fonksiyonları | OVP, UVP, OCP, SCP, OTP, UTP (hücre ve paket seviyesinde) |
| Fuel Gauge | MPS MPF42793 tabanlı SoC/SoH tahmini |
| Analog Front-End (AFE) | MPS MP2797 — 16 kanallı hücre ölçümü |
| Haberleşme | I2C (AFE & FG), UART (harici iletişim) |
| Dengeleme | Pasif hücre dengeleme (otomatik/manuel) |
| Watchdog | Bağımsız IWDG + MP279x dahili WDT |
| Güç Yönetimi | Stand-by modu, uyandırma (wake-up) mekanizmaları |
| Flash Konfigürasyon | Kalıcı parametre saklama (STM32 dahili Flash) |
| NTC Sıcaklık Ölçümü | 4x batarya NTC + 1x PCB NTC + 1x die sıcaklığı |

### 1.3. Firmware Sürüm Bilgisi

| Parametre | Değer |
|-----------|-------|
| Firmware Sürümü | 1.0 |
| Başlangıç Tarihi | 06/05/2024 |
| MCU | STM32F030C8Tx |
| AFE IC | MP2797 |
| Fuel Gauge IC | MPF42793 |
| Geliştirici | Emrullah GÜNAY — KOBİNERJİ A.Ş. |

---

## 2. Donanım Platformu

### 2.1. STM32F030C8 Mikrodenetleyici

| Parametre | Değer |
|-----------|-------|
| Çekirdek | ARM Cortex-M0 |
| Maksimum Frekans | 48 MHz |
| Flash Bellek | 64 KB |
| SRAM | 8 KB |
| I2C Arayüzü | 2x I2C (I2C1: MP279x, I2C2: MPF4279x) |
| UART | 1x USART (harici haberleşme) |
| Timer | TIM3 (başlatma), TIM6 (UART timeout) |
| RTC | Dahili RTC (periyodik görevler) |
| ADC | 12-bit ADC (NTC sıcaklık ölçümü) |
| GPIO | LQFP48 paket, çoklu GPIO |
| Çalışma Gerilimi | 2.4V – 3.6V |
| Çalışma Sıcaklığı | -40°C ~ +85°C |

### 2.2. MPS MP2797 — Analog Front-End (AFE)

MP2797, yüksek doğruluklu batarya izleme ve koruma IC'sidir.

| Parametre | Değer |
|-----------|-------|
| Hücre Sayısı | 7S – 16S |
| Hücre Gerilim Ölçümü | 16-bit çözünürlük |
| Akım Ölçümü | Coulomb counter dahili |
| NTC Girişi | 4 adet harici NTC |
| Dahili Sıcaklık | Die sıcaklık sensörü |
| Haberleşme | I2C (CRC8 korumalı) |
| FET Kontrolü | Şarj + Deşarj MOSFET sürücü |
| Dengeleme | Pasif dengeleme (16 kanal) |
| Self-Test | Dahili ADC kendini test fonksiyonu |
| Open-Wire Detection | Açık-hat tespiti |
| Watchdog | Dahili hardware watchdog |
| I2C Adresi | 0x01 |

### 2.3. MPS MPF42793 — Fuel Gauge IC

| Parametre | Değer |
|-----------|-------|
| Fonksiyon | Gelişmiş SoC/SoH hesaplama motoru |
| Haberleşme | I2C (CRC opsiyonel) |
| I2C Adresi | 0x08 |
| SoC Çözünürlüğü | Hücre bazında (cell-level) |
| SoH Hesaplama | ESR + Rcxn öğrenme tabanlı |
| Öğrenme Parametreleri | IDIS_AVG, IDIS_END, ICHG_CC, ICHG_END, VCHG_CV |
| Güç Tahmini | Deşarj (Pdis) ve Şarj (Pchg) güç limiti |
| Boşalma Süresi | Kalan süre tahmini (emty_rtime) |
| Doluluk Süresi | Tam şarj süresi tahmini (full_rtime) |
| Reset Kontrolü | PB9 (nRST), PB5 (EN) |

---

## 3. Firmware Mimarisi

### 3.1. Modül Yapısı

```
VGX/
├── main.c                 — Ana program döngüsü ve durum makinesi
├── Init.c / Init.h        — Sistem başlatma (GPIO, I2C, UART, Timer, RTC, IWDG)
├── MP279x.h               — MP2797 register haritası ve I2C fonksiyonları
├── MP279x_I2C.c           — MP2797 I2C haberleşme katmanı
├── MPF4279x.h             — MPF42793 veri yapıları ve komut seti
├── MPF4279x_I2C.c         — MPF42793 I2C haberleşme katmanı
├── BMS_MP279x.h           — BMS parametreleri, pin haritası, veri yapıları
├── Flash_Config.h         — Flash bellek konfigürasyon haritası
└── main_all.c             — Tüm fonksiyon implementasyonları
```

### 3.2. Durum Makinesi (State Machine)

Firmware, ana döngüde aşağıdaki durumlar arasında geçiş yapar:

| Durum | Enum Değeri | Açıklama |
|-------|-------------|----------|
| `WAKE_UP` | 0 | Sistem uyanma (deşarj yönü) |
| `WAKE_UP_CHG` | 1 | Sistem uyanma (şarj yönü) |
| `CHG` | 2 | Şarj modu — aktif şarj izleme |
| `DCHG` | 3 | Deşarj modu — aktif deşarj izleme |
| `STAND_BY` | 4 | Düşük güç bekleme modu |
| `ALARM` | 5 | Hata durumu — FET'ler kapalı |
| `SAFE` | 6 | Güvenli mod — kritik hata |
| `RESET_ALARM` | 7 | Alarm sıfırlama geçiş durumu |
| `AUTO_RECOVERY` | 8 | Otomatik hata giderme |

### 3.3. Zamanlama ve Kesme Yapısı

| Kesme Kaynağı | Öncelik | Fonksiyon |
|---------------|---------|-----------|
| EXTI0 (FAULT Pin) | 0 (En Yüksek) | MP279x hata pin kesmesi |
| EXTI2 (xALERT Pin) | 1 | MP279x alarm pin kesmesi |
| RTC | 2 | Periyodik ölçüm tetikleme |
| UART RX | 3 | UART veri alımı |
| TIM6 | 4 | UART timeout sayacı |
| EXTI9_5 (FG_IT) | 5 | Fuel gauge iterasyon kesmesi |
| TIM3 | 6 | Başlangıç bekleme süreci |

### 3.4. Ana Fonksiyon Listesi

| Fonksiyon | Açıklama |
|-----------|----------|
| `InitConfiguration()` | Flash'tan veya varsayılandan konfigürasyon yükleme |
| `MP279xCInit()` | AFE ilk konfigürasyon yazma |
| `MP279xCReConfig()` | AFE parametrelerini güncelleme |
| `MP279xCConfigCalc()` | Koruma eşiklerini register formatına dönüştürme |
| `ReadADCResults()` | Hücre gerilimleri, akım, sıcaklık okuma |
| `SendFGDataMPF4279x()` | Fuel gauge'a ölçüm verisi gönderme |
| `ReadFGResultsMPF4279x()` | Fuel gauge sonuçlarını okuma |
| `Fault_Handle()` | Hata durum yönetimi |
| `xALARM_Handle()` | Alarm durum yönetimi |
| `BMSAlarmHandler()` | Alarm türüne göre aksiyon |
| `SaveConfigtoFLASH()` | Konfigürasyonu Flash'a kaydetme |
| `CheckUARTRx()` | UART mesaj kontrolü |
| `Calc_OC_SC_Lim()` | Aşırı akım/kısa devre limiti hesaplama |
| `ADC_to_Temperature()` | ADC değerini sıcaklığa çevirme |

---

## 4. I2C Haberleşme Protokolü

### 4.1. I2C Bus Konfigürasyonu

| Parametre | MP279x (I2C1) | MPF4279x (I2C2) |
|-----------|---------------|-----------------|
| Slave Adresi | 0x01 | 0x08 |
| CRC Kullanımı | CRC8 (zorunlu) | CRC (opsiyonel, varsayılan: aktif) |
| Veri Genişliği | 16-bit register | Değişken uzunluklu blok |
| Timeout | 750 µs | 3000 µs |
| Frekans | 400 kHz (Fast Mode) | Standart/Fast Mode |

### 4.2. MP279x I2C İletişim Formatı

**Yazma İşlemi (16-bit):**
```
[START] [Slave Addr + W] [Register] [DataL] [DataH] [CRC8] [STOP]
```

**Okuma İşlemi (16-bit):**
```
[START] [Slave Addr + W] [Register] [RESTART] [Slave Addr + R] [DataL] [DataH] [CRC8] [STOP]
```

### 4.3. CRC8 Hesaplama

MP279x haberleşmesinde CRC8 kullanılarak veri bütünlüğü doğrulanır. Her I2C işleminde CRC byte'ı eklenir/kontrol edilir.

### 4.4. MPF4279x I2C İletişim Formatı

**Fuel Gauge Tetikleme Komutu:**
```
Trigger Address: 0x7FFE
```

MPF4279x ile blok halinde veri transferi yapılır. Ölçüm yapısı (`Meas_REG`) 16 hücre gerilimi, akım, sıcaklık ve durum bilgilerini içerir.

### 4.5. I2C Hata Yönetimi

- I2C timeout mekanizması (MP279x: 750 µs, MPF4279x: 3000 µs)
- Hata sayacı (`ErrorCount`) ile haberleşme güvenilirliği takibi
- `I2Cerror` flag ile hata durumu bildirimi
- `CommError` ile haberleşme hatası yönetimi

---

## 5. BMS Koruma Fonksiyonları

### 5.1. Hücre Gerilim Korumaları

| Koruma | Parametre | Değer | Birim | Açıklama |
|--------|-----------|-------|-------|----------|
| Hücre Aşırı Gerilim (OV) | `CellOV_Thres` | 36500 | ×0.1 mV (3.650V) | Hücre üst gerilim limiti |
| Hücre Düşük Gerilim (UV) | `CellUV_Thres` | 20000 | ×0.1 mV (2.000V) | Hücre alt gerilim limiti |
| Hücre Ölü (Dead Cell) | `CellDead_Thres` | 18000 | ×0.1 mV (1.800V) | Arızalı hücre tespiti |
| Hücre Ölçüm Uyumsuzluğu | `CellMSMT_Thres` | 3 | ×39.0625 mV | Ölçüm tutarsızlık eşiği |

### 5.2. Paket Gerilim Korumaları

| Koruma | Parametre | Değer | Birim | Açıklama |
|--------|-----------|-------|-------|----------|
| Paket Düşük Gerilim (UV) | `PackUV_Thres` | 16000 | mV | Paket alt gerilim limiti |
| Paket Aşırı Gerilim (OV) | `PackOV_Thres` | 29200 | mV | Paket üst gerilim limiti |

### 5.3. Akım Korumaları (Deşarj)

| Koruma | Parametre | Akım Limiti | Gecikme | Açıklama |
|--------|-----------|-------------|---------|----------|
| Deşarj Aşırı Akım 1 (DOC1) | `OC_THRES_1` | 45000 mA (45A) | 20 ms | Seviye-1 aşırı deşarj akımı |
| Deşarj Aşırı Akım 2 (DOC2) | `OC_THRES_2` | 52500 mA (52.5A) | 10 ms | Seviye-2 aşırı deşarj akımı |
| Deşarj Kısa Devre (DSC) | `DSC_THRES` | 36000 mA (36A) | 8×200 µs = 1.6 ms | Deşarj kısa devre koruması |

### 5.4. Akım Korumaları (Şarj)

| Koruma | Parametre | Akım Limiti | Gecikme | Açıklama |
|--------|-----------|-------------|---------|----------|
| Şarj Aşırı Akım (COC) | `COC_THRES` | 15000 mA (15A) | 20 ms | Aşırı şarj akımı koruması |
| Şarj Kısa Devre (CSC) | `CSC_THRES` | 36000 mA (36A) | 8×200 µs = 1.6 ms | Şarj kısa devre koruması |

### 5.5. Sıcaklık Korumaları

| Koruma | Parametre | Değer | Birim | Açıklama |
|--------|-----------|-------|-------|----------|
| Batarya Deşarj Aşırı Sıcaklık | `BAT_DOT_THRES` | 5500 | ×0.01°C (55°C) | Deşarj OT limiti |
| Batarya Deşarj Düşük Sıcaklık | `BAT_DUT_THRES` | -1800 | ×0.01°C (-18°C) | Deşarj UT limiti |
| Batarya Şarj Aşırı Sıcaklık | `BAT_COT_THRES` | 4300 | ×0.01°C (43°C) | Şarj OT limiti |
| Batarya Şarj Düşük Sıcaklık | `BAT_CUT_THRES` | 200 | ×0.01°C (2°C) | Şarj UT limiti |
| PCB Aşırı Sıcaklık | `PCB_OT_THRES` | 8500 | ×0.01°C (85°C) | PCB OT limiti |

### 5.6. NTC Sıcaklık Ölçüm Parametreleri

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| `R_TEMP_MEAS_PCB` | 10000 Ω | PCB NTC referans direnci |
| `BETA_NTC_PCB` | 3380 | PCB NTC Beta katsayısı |
| `R_TEMP_MEAS_BAT` | 10000 Ω | Batarya NTC referans direnci |
| `BETA_NTC_BAT` | 3380 | Batarya NTC Beta katsayısı |
| Harici NTC (Heater) | R1=10kΩ, BETA=3950 | STM32 ADC üzerinden ölçüm |

### 5.7. Hata Etkinleştirme ve Otomatik Kurtarma

**Hata Etkinleştirme Kaydı (`FAULTS_EN` = 0x1FFF):**

| Bit | Hata | Durum |
|-----|------|-------|
| b0 | Cell UV | Aktif |
| b1 | Cell OV | Aktif |
| b2 | Cell Dead | Aktif |
| b3 | Pack UV | Aktif |
| b4 | Pack OV | Aktif |
| b5 | DOC1 | Aktif |
| b6 | DOC2 | Aktif |
| b7 | COC | Aktif |
| b8 | DSC | Aktif |
| b9 | CSC | Aktif |
| b10 | Cell OT | Aktif |
| b11 | Cell UT | Aktif |
| b12 | PCB OT | Aktif |

**Otomatik Kurtarma Kaydı (`FAULTS_AUTORECOVERY` = 0x0283):**

| Bit | Hata | Otomatik Kurtarma |
|-----|------|-------------------|
| b0 | Cell UV | Aktif |
| b1 | Cell OV | Aktif |
| b7 | Cell OT/UT | Aktif |
| b9 | PCB OT | Aktif |
| Diğerleri | — | Pasif (manuel kurtarma) |

### 5.8. Özellik Etkinleştirme (`FEATURES_EN` = 0x0173)

| Bit | Özellik | Durum |
|-----|---------|-------|
| b0 | Open Wire Detection | Aktif |
| b1 | Charger Detection | Aktif |
| b4 | PFET Stand-by | Aktif |
| b5 | WDT Reset | Aktif |
| b6 | ADC Self Test | Aktif |
| b8 | SC&OC during SAFE | Aktif |

---

## 6. Fuel Gauge (SoC/SoH)

### 6.1. Genel Çalışma Prensibi

MPF42793, gerçek zamanlı batarya modelleme motoru kullanarak SoC (State of Charge) ve SoH (State of Health) hesaplaması yapar. Firmware, periyodik olarak ölçüm verilerini fuel gauge'a gönderir ve sonuçları okur.

### 6.2. Fuel Gauge Veri Akışı

```
STM32 → [Measurements] → MPF42793
         Hücre Gerilimleri (16x)
         Paket Akımı
         Sıcaklık Vektörü (4x)
         Chip Sıcaklığı
         Dengeleme Durumu

MPF42793 → [Results] → STM32
            SoC (hücre bazlı + paket)
            SoH (hücre bazlı + paket)
            ESR (şarj + deşarj)
            Kalan Süre (boşalma/doluluk)
            Güç Limitleri
            Öğrenme Parametreleri
```

### 6.3. Ölçüm Yapısı (Meas_REG)

| Alan | Tip | Açıklama |
|------|-----|----------|
| `Cells[16]` | uint16_t | 16 hücre gerilimi |
| `VPack` | uint16_t | Paket toplam gerilimi |
| `Current[16]` | int32_t | Hücre akımları |
| `PackCurrent` | int32_t | Paket toplam akımı |
| `I_CC` | int32_t | Coulomb counter akımı |
| `Tamb` | int16_t | Ortam sıcaklığı |
| `TempVect[4]` | int16_t | 4 adet NTC sıcaklık değeri |
| `TChip` | int16_t | IC die sıcaklığı |
| `CellsBalancing` | uint16_t | Dengeleme bitmap'i |

### 6.4. Fuel Gauge Sonuç Yapısı (def_fg_out)

| Alan | Açıklama |
|------|----------|
| `soc_abs_cells[16]` | Her hücre için mutlak SoC (%) |
| `soc_pack` | Paket SoC değeri |
| `emty_rtime` | Boşalmaya kalan süre |
| `full_rtime` | Tam dolmaya kalan süre |
| `soresr_dis_cells[16]` | Deşarj ESR (her hücre) |
| `soresr_chg_cells[16]` | Şarj ESR (her hücre) |
| `rcxn_cells[16]` | Reaksiyon direnci |
| `hconv` | Isı dönüşüm katsayısı |
| `soh_pack` | Paket SoH değeri |
| `soh_cells[16]` | Her hücre SoH değeri |
| `pdis` / `pchg` | Anlık deşarj/şarj güç kapasitesi |
| `idis_avg` / `idis_end` | Ortalama/bitiş deşarj akımı (öğrenme) |
| `ichg_cc` / `ichg_end` | CC/bitiş şarj akımı (öğrenme) |
| `vchg_cv` | CV gerilimi (öğrenme) |

### 6.5. Fuel Gauge Durum Bayrakları

| Durum | Açıklama |
|-------|----------|
| `CHARGE` | Şarj modunda |
| `DISCHARGE` | Deşarj modunda |
| `REST` | Dinlenme modunda |

### 6.6. Öğrenme Durumları

| Durum | Açıklama |
|-------|----------|
| `Disabled` | Öğrenme devre dışı |
| `Paused` | Öğrenme duraklatıldı |
| `Ongoing` | Öğrenme devam ediyor |

### 6.7. Virtual Fuel Gauge (VFG)

Firmware, MPF42793 donanımına ek olarak sanal bir FG yapısı (`VirtualFG_Struct`) içerir:

| Alan | Açıklama |
|------|----------|
| `VFG_En` | Virtual FG aktif/pasif |
| `VFG_IT_Running` | Iterasyon çalışıyor |
| `VFG_IT_Done` | Iterasyon tamamlandı |
| `VFG_Reading` | Sonuç okunuyor |
| `VFG_First` | İlk çalıştırma |

### 6.8. Coulomb Counter

| Parametre | Açıklama |
|-----------|----------|
| `CC_Accum` | Birikimli CC değeri (int32) |
| `CC_BMS_Accum` | BMS seviyesinde birikimli CC (int64) |
| `CC_BMS_Cntr` | CC sayacı (uint32) |
| `ENABLE_CC` | CC fonksiyonu aktif (define) |

---

## 7. Dengeleme Sistemi

### 7.1. Dengeleme Konfigürasyonu

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| `Min_Bal_V` | 30000 (×0.1 mV = 3.000V) | Dengeleme başlangıç minimum hücre gerilimi |
| `Bal_Config` | 0x0B | Dengeleme konfigürasyon kaydı |
| Mod | Otomatik (`MP279x_AutoBalance` tanımlı) | MP279x dahili otomatik dengeleme |
| Dengeleme Tipi | Pasif | Dirençli boşaltma |

### 7.2. Dengeleme Çalışma Prensibi

1. Hücre gerilimleri `Min_Bal_V` (3.000V) üzerindeyse dengeleme aktif olabilir
2. En yüksek gerilimli hücre ile en düşük gerilimli hücre arasındaki fark `BAL_THR` eşiğini aştığında dengeleme başlar
3. MP279x dahili dengeleme switch'leri üzerinden yüksek gerilimli hücreler boşaltılır
4. Dengeleme durumu `CellsBalance` register'ı üzerinden izlenir

### 7.3. İlgili Register'lar

| Register | Adres | Açıklama |
|----------|-------|----------|
| `REG_BAL_STS` | 0xA4 | Dengeleme durumu |
| `REG_BAL_LIST` | 0xA5 | Aktif dengeleme listesi |
| `REG_BAL_CTRL` | 0xA6 | Dengeleme kontrolü |
| `REG_BAL_CONFIG` | 0xA7 | Dengeleme konfigürasyonu |
| `REG_BAL_THR` | 0xA8 | Dengeleme eşik değeri |

---

## 8. Flash Konfigürasyon

### 8.1. Flash Bellek Adresi

| Parametre | Değer |
|-----------|-------|
| Başlangıç Adresi | `0x0800F800` |
| Bellek Bölgesi | STM32F030 dahili Flash — Son sayfa |
| Sayfa Boyutu | 1 KB |

### 8.2. Flash Konfigürasyon Haritası

| Offset | Parametre | Boyut | Açıklama |
|--------|-----------|-------|----------|
| +0 | Cell_UV | 16-bit | Hücre düşük gerilim eşiği |
| +2 | Cell_UV_Hyst | 16-bit | Hücre UV histerezis |
| +4 | Cell_UV_Dgl | 16-bit | Hücre UV gecikme |
| +6 | Cell_OV | 16-bit | Hücre aşırı gerilim eşiği |
| +8 | Cell_OV_Hyst | 16-bit | Hücre OV histerezis |
| +10 | Cell_OV_Dgl | 16-bit | Hücre OV gecikme |
| +12 | Cell_Dead | 16-bit | Ölü hücre eşiği |
| +14 | Cell_Dead_Dgl | 16-bit | Ölü hücre gecikme |
| +16 | Cell_MSMT | 16-bit | Ölçüm uyumsuzluk eşiği |
| +18 | Cell_MSMT_Dgl | 16-bit | Ölçüm uyumsuzluk gecikme |
| +20 | Pack_UV | 16-bit | Paket düşük gerilim |
| +22 | Pack_UV_Hyst | 16-bit | Paket UV histerezis |
| +24 | Pack_UV_Dgl | 16-bit | Paket UV gecikme |
| +26 | Pack_OV | 16-bit | Paket aşırı gerilim |
| +28 | Pack_OV_Hyst | 16-bit | Paket OV histerezis |
| +30 | Pack_OV_Dgl | 16-bit | Paket OV gecikme |
| +32 | Pack_DOC1 | 16-bit | Deşarj aşırı akım-1 |
| +34 | Pack_DOC1_Dgl | 16-bit | DOC1 gecikme |
| +36 | Pack_DOC2 | 16-bit | Deşarj aşırı akım-2 |
| +38 | Pack_DOC2_Dgl | 16-bit | DOC2 gecikme |
| +40 | Pack_DSC | 16-bit | Deşarj kısa devre |
| +42 | Pack_DSC_Dgl | 16-bit | DSC gecikme |
| +44 | Pack_COC | 16-bit | Şarj aşırı akım |
| +46 | Pack_COC_Dgl | 16-bit | COC gecikme |
| +48 | Pack_CSC | 16-bit | Şarj kısa devre |
| +50 | Pack_CSC_Dgl | 16-bit | CSC gecikme |
| +52 | Bat_DCH_OT | 16-bit | Batarya deşarj OT |
| +54 | Bat_DCH_UT | 16-bit | Batarya deşarj UT |
| +56 | Bat_CHG_OT | 16-bit | Batarya şarj OT |
| +58 | Bat_CHG_UT | 16-bit | Batarya şarj UT |
| +60 | Cells_NTC_Config | 16-bit | NTC konfigürasyonu |
| +62 | PCB_OT | 16-bit | PCB aşırı sıcaklık |
| +64 | PCB_OT_Hyst | 16-bit | PCB OT histerezis |
| +66 | I_Gain | 16-bit | Akım kazancı |
| +68 | CurrOffset | 16-bit (signed) | Akım ofseti |
| +70 | NumofSeries | 16-bit | Seri hücre sayısı |
| +72 | BetaNTCBat | 16-bit | Batarya NTC Beta |
| +74 | Sby_Time | 16-bit | Stand-by süresi |
| +76 | Sby_I_Thr | 16-bit | Stand-by akım eşiği |
| +78 | MinBalV | 16-bit | Minimum dengeleme gerilimi |
| +80 | BalConfig | 16-bit | Dengeleme konfigürasyonu |
| +82 | Faults_Enable | 16-bit | Hata etkinleştirme bitmap |
| +84 | Faults_AutoRecovery | 16-bit | Otomatik kurtarma bitmap |
| +86 | Features_Enable | 16-bit | Özellik etkinleştirme bitmap |
| +88 | I_Sense_R | 16-bit | Akım algılama direnci |

### 8.3. Flash Yazma/Okuma

- Sistem başlatmada Flash konfigürasyon alanı okunur
- Geçerli veri yoksa (`DefaultConfig=0`) varsayılan `#define` değerleri kullanılır
- UART üzerinden yeni konfigürasyon alındığında `SaveConfigtoFLASH()` ile kalıcı olarak saklanır
- Flash silme/yazma işlemi sırasında kesmeler devre dışı bırakılır

---

## 9. Register Haritası

### 9.1. MP279x Temel Register Grupları

#### Konfigürasyon Register'ları (0x00–0x0D)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x00 | `REG_CELL_CONFIG` | Hücre konfigürasyonu |
| 0x05 | `REG_ACT_CONFIG` | Aktif mod konfigürasyonu |
| 0x06 | `REG_STB_CONFIG` | Stand-by konfigürasyonu |
| 0x07 | `REG_SAFE_CONFIG` | Güvenli mod konfigürasyonu |
| 0x08 | `REG_REG_CONFIG` | Regülatör konfigürasyonu |
| 0x09 | `REG_LOAD_CHGR_CONFIG` | Yük/Şarj aleti konfigürasyonu |
| 0x0C | `REG_GPIO_CONFIG` | GPIO konfigürasyonu |
| 0x0D | `REG_PINS_CONFIG` | Pin konfigürasyonu |

#### Durum Register'ları (0x01–0x03, 0x0A, 0x0E)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x01 | `REG_PWR_STATUS` | Güç durumu |
| 0x02 | `REG_STB_STATUS` | Stand-by durumu |
| 0x03 | `REG_LOAD_CHGR_STATUS` | Yük/Şarj aleti durumu |
| 0x0A | `REG_GPIO_STATUS` | GPIO durumu |
| 0x0E | `REG_WDT_STATUS` | Watchdog durumu |

#### FET Kontrol Register'ları (0x11–0x14)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x11 | `REG_FET_STATUS` | FET durumu |
| 0x12 | `REG_FET_CTRL` | FET kontrolü |
| 0x13 | `REG_FET_MODE` | FET modu |
| 0x14 | `REG_FET_CONFIG` | FET konfigürasyonu |

#### Kesme Register'ları (0x15–0x1F)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x15 | `REG_RD_INT0` | Kesme-0 okuma |
| 0x16 | `REG_RD_INT1` | Kesme-1 okuma |
| 0x17 | `REG_INT0_CLR` | Kesme-0 temizleme |
| 0x18 | `REG_INT1_CLR` | Kesme-1 temizleme |
| 0x19 | `REG_INT0_EN` | Kesme-0 etkinleştirme |
| 0x1A | `REG_INT1_EN` | Kesme-1 etkinleştirme |
| 0x1E | `REG_MASK_INT0` | Kesme-0 maskeleme |
| 0x1F | `REG_MASK_INT1` | Kesme-1 maskeleme |

#### Akım Koruma Register'ları (0x20–0x2C)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x20 | `REG_OC_STATUS` | Aşırı akım durumu |
| 0x23 | `REG_OCFT_CTRL` | OC hata kontrolü |
| 0x24 | `REG_DSGOC_LIM` | Deşarj OC limiti |
| 0x25 | `REG_DSGOC_DEG` | Deşarj OC gecikme |
| 0x26 | `REG_CHGOC_DEG` | Şarj OC gecikme |
| 0x27 | `REG_SC_STATUS` | Kısa devre durumu |
| 0x2A | `REG_SCFT_CTRL` | SC hata kontrolü |
| 0x2B | `REG_DSGSC_CONFIG` | Deşarj SC konfigürasyonu |
| 0x2C | `REG_CHGSC_CONFIG` | Şarj SC konfigürasyonu |

#### Hücre Gerilim Koruma Register'ları (0x2D–0x3D)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x2D | `REG_RD_CELL_UV` | Hücre UV durum okuması |
| 0x2E | `REG_RD_CELL_OV` | Hücre OV durum okuması |
| 0x2F | `REG_RD_CELL_MSMT` | Hücre ölçüm uyumsuzluk okuması |
| 0x30 | `REG_RD_CELL_DEAD` | Ölü hücre durum okuması |
| 0x36 | `REG_CELL_HYST` | Hücre histerezis ayarı |
| 0x37 | `REG_PACK_UV_OV` | Paket UV/OV eşikleri |
| 0x38 | `REG_CELL_UV` | Hücre UV eşik yazma |
| 0x39 | `REG_CELL_OV` | Hücre OV eşik yazma |
| 0x3A | `REG_PACK_UV` | Paket UV eşik yazma |
| 0x3B | `REG_PACK_OV` | Paket OV eşik yazma |
| 0x3C | `REG_CELL_DEAD_THR` | Ölü hücre eşik yazma |
| 0x3D | `REG_CELL_MSMT` | Ölçüm uyumsuzluk eşik yazma |

#### Sıcaklık Register'ları (0x3E–0x4D)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x3E | `REG_RD_NTC_DIE` | NTC/Die sıcaklık okuması |
| 0x3F–0x42 | `REG_RD_VANTC1–4` | Analog NTC gerilim okumaları |
| 0x43 | `REG_RD_T_DIE` | Die sıcaklık okuması |
| 0x46 | `REG_DIE_CONFIG` | Die sıcaklık konfigürasyonu |
| 0x47 | `REG_NTC_CONFIG` | NTC konfigürasyonu |
| 0x48 | `REG_OTHR_DSG` | Deşarj OT eşiği |
| 0x49 | `REG_UTHR_DSG` | Deşarj UT eşiği |
| 0x4A | `REG_OTHR_CHG` | Şarj OT eşiği |
| 0x4B | `REG_UTHR_CHG` | Şarj UT eşiği |
| 0x4C | `REG_NTCM_OTHR` | NTC M OT eşiği |
| 0x4D | `REG_DIE_OT` | Die OT eşiği |

#### Hücre Ölçüm Register'ları (0x69–0x8F)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x69 | `REG_RD_VPACKP` | Paket gerilimi okuması |
| 0x6A | `REG_RD_VTOP` | VTOP gerilimi okuması |
| 0x6B | `REG_RD_ITOP` | Toplam akım okuması |
| 0x6C–0x8B | `REG_RD_VCELL1–16` / `REG_RD_ICELL1–16` | Hücre gerilim/akım okumaları |
| 0x8C–0x8F | `REG_RD_VNTC1–4` | NTC gerilim okumaları |

#### Coulomb Counter & ADC Register'ları (0x65–0x68, 0x97–0x9A)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x65 | `REG_RD_CCIRQL` | CC kesme düşük byte |
| 0x66 | `REG_RD_CCIRQH` | CC kesme yüksek byte |
| 0x67 | `REG_RD_CCACCQL` | CC birikim düşük word |
| 0x68 | `REG_RD_CCACCQH` | CC birikim yüksek word |
| 0x97 | `REG_CC_STS` | CC durumu |
| 0x98 | `REG_ADC_STS` | ADC durumu |
| 0x99 | `REG_ADC_CTRL` | ADC kontrolü |
| 0x9A | `REG_CC_CONFIG` | CC konfigürasyonu |

#### Dengeleme Register'ları (0xA4–0xA8)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0xA4 | `REG_BAL_STS` | Dengeleme durumu |
| 0xA5 | `REG_BAL_LIST` | Dengeleme listesi |
| 0xA6 | `REG_BAL_CTRL` | Dengeleme kontrolü |
| 0xA7 | `REG_BAL_CONFIG` | Dengeleme konfigürasyonu |
| 0xA8 | `REG_BAL_THR` | Dengeleme eşiği |

#### Hata Yönetim Register'ları (0x5D–0x63)

| Adres | Register | Açıklama |
|-------|----------|----------|
| 0x5D | `REG_FT_STS1` | Hata durum-1 |
| 0x5E | `REG_FT_STS2` | Hata durum-2 |
| 0x5F | `REG_FT_CLR` | Hata temizleme |
| 0x60 | `REG_FT_REC` | Hata kurtarma |
| 0x61 | `REG_FT0_CONFIG` | Hata-0 konfigürasyonu |
| 0x62 | `REG_FT1_CONFIG` | Hata-1 konfigürasyonu |
| 0x63 | `REG_FT2_CONFIG` | Hata-2 konfigürasyonu |

---

## 10. Pin Bağlantıları

### 10.1. STM32F030C8 GPIO Çıkışları

| Pin | Fonksiyon | Açıklama |
|-----|-----------|----------|
| PA3 | GPIO1 / FET_EN | FET etkinleştirme (ENABLE_FETs) |
| PA4 | GPIO2 / FET_EN | FET etkinleştirme (ENABLE_FETs) |
| PA5 | ADC_IN5 / NTC_EXT | Harici NTC ölçümü |
| PA7 | ADC_IN7 / NTC_HEATER | Heater NTC ölçümü |
| PA11 | LD4 | LED-4 göstergesi |
| PA12 | LD3 | LED-3 göstergesi |
| PB0 | GPIO3 / FAULT | MP279x FAULT girişi |
| PB1 | MP_nSHDN | MP279x shutdown kontrolü |
| PB2 | xALERT | MP279x alarm girişi |
| PB5 | FG_EN | Fuel Gauge enable |
| PB8 | FG_IRQ | Fuel Gauge kesme girişi |
| PB9 | FG_NRST | Fuel Gauge reset |
| PC13 | LD1 | LED-1 göstergesi |
| PC14 | LD2 | LED-2 göstergesi |
| PC15 | LD0 | LED-0 göstergesi |

### 10.2. I2C Bağlantıları

| I2C Portu | SCL | SDA | Bağlı Cihaz |
|-----------|-----|-----|-------------|
| I2C1 | PB6 | PB7 | MP2797 (AFE) |
| I2C2 | PB10 | PB11 | MPF42793 (FG) |

### 10.3. UART Bağlantısı

| Sinyal | Pin | Açıklama |
|--------|-----|----------|
| TX | PA9 | UART1 veri çıkışı |
| RX | PA10 | UART1 veri girişi |

### 10.4. Kesme Pin Konfigürasyonu

| Pin | Kesme | Tetikleme | Öncelik |
|-----|-------|-----------|---------|
| PB0 | EXTI0 | Düşen kenar | 0 |
| PB2 | EXTI2 | Düşen kenar | 1 |
| PB8 | EXTI9_5 | Düşen kenar | 5 |

---

## 11. Elektriksel Özellikler

### 11.1. Gerilim Aralıkları

| Parametre | Min | Nom | Maks | Birim |
|-----------|-----|-----|------|-------|
| Hücre Gerilimi (çalışma) | 2.000 | 3.600 | 3.650 | V |
| Hücre Ölü Eşiği | — | — | 1.800 | V |
| Paket Gerilimi (8S) | 16.000 | — | 29.200 | V |
| MCU Besleme (VDD) | 2.4 | 3.3 | 3.6 | V |
| Dahili Regülatör (1.8V) | — | 1.8 | — | V |
| Dahili Regülatör (3.3V) | — | 3.3 | — | V |
| Dahili Regülatör (5V) | — | 5.0 | — | V |

### 11.2. Akım Limitleri

| Parametre | Değer | Birim | Açıklama |
|-----------|-------|-------|----------|
| DOC Seviye-1 | 45.0 | A | Aşırı deşarj akımı (20 ms gecikme) |
| DOC Seviye-2 | 52.5 | A | Kritik deşarj akımı (10 ms gecikme) |
| DSC | 36.0 | A | Deşarj kısa devre (1.6 ms gecikme) |
| COC | 15.0 | A | Aşırı şarj akımı (20 ms gecikme) |
| CSC | 36.0 | A | Şarj kısa devre (1.6 ms gecikme) |
| Stand-by Akım Eşiği | ~0.75 | mA | SBY_I_THR=4 (Offset=250µV, LSB=125µV) |

### 11.3. Akım Ölçüm Parametreleri

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| Akım Algılama Direnci | 2 × 0.025 = 0.050 Ω | Sense resistor |
| Akım Kazancı (`Current_Gain`) | 32767 | Kalibrasyon kazancı |
| Akım Ofseti (`Current_Offset`) | 0 | Kalibrasyon ofseti |

### 11.4. Sıcaklık Aralıkları

| Parametre | Min | Maks | Birim |
|-----------|-----|------|-------|
| Deşarj Çalışma Sıcaklığı | -18 | +55 | °C |
| Şarj Çalışma Sıcaklığı | +2 | +43 | °C |
| PCB Maksimum Sıcaklık | — | +85 | °C |
| MCU Çalışma Sıcaklığı | -40 | +85 | °C |

### 11.5. Zamanlama Parametreleri

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| Stand-by Süresi | 600 s | Düşük akımda bekleme sonrası uyku |
| Watchdog Reload | 6250 (~5s) | IWDG sayacı yenileme |
| Ölçüm Aralığı | BT_LENGTH_TX = 297 | ADC örnekleme periyodu |
| FG Periyodu | FG_Ts = 40 | Fuel gauge güncelleme periyodu |

---

## 12. Geliştirme Ortamı

### 12.1. IDE ve Araç Zinciri

| Araç | Sürüm/Detay |
|------|-------------|
| IDE | Keil µVision 5 |
| Derleyici | ARM Compiler (ARMCC) |
| Proje Dosyası | `MP2797_Ref_Design_FW_M0.uvprojx` |
| Target | Target 1 (STM32F030C8Tx) |
| Debugger | Segger J-Link |
| Debug Konfigürasyonu | `DebugConfig/Target_1_STM32F030C8Tx.dbgconf` |

### 12.2. Proje Yapılandırması

| Parametre | Değer |
|-----------|-------|
| CPU Clock | 48 MHz |
| Startup | `startup_stm32f030x8.s` |
| System | `system_stm32f0xx.c` |
| HAL Katmanı | STM32F0xx LL (Low-Level) Drivers |
| Linker Script | `Objects/OPU_FW_1.sct` |

### 12.3. Kullanılan LL Sürücüleri

| Sürücü | Açıklama |
|--------|----------|
| `stm32f0xx_ll_i2c` | I2C düşük seviye sürücüsü |
| `stm32f0xx_ll_rcc` | Saat yapılandırma sürücüsü |
| `stm32f0xx_ll_pwr` | Güç yönetimi sürücüsü |
| `stm32f0xx_ll_utils` | Yardımcı fonksiyonlar |

### 12.4. Derleme Direktifleri

| Direktif | Açıklama | Varsayılan Durum |
|----------|----------|------------------|
| `WDG_EN` | Watchdog etkinleştirme | Aktif |
| `PACK_CELLS_SERIES_8` | 8S pil konfigürasyonu | Aktif |
| `MP279x_AutoBalance` | Otomatik dengeleme | Aktif |
| `PFET_Enable` | P-FET kontrolü | Aktif |
| `ENABLE_CC` | Coulomb counter | Aktif |
| `FG_NewGen` | Yeni nesil FG (MPF42791/3) | Aktif |
| `MPF42793` | MPF42793 seçimi | Aktif |
| `ADDRESS_MP279x` | AFE I2C adresi = 1 | Aktif |
| `Use_PreChg` | Ön-şarj devresi | Pasif |
| `High_P` | Yüksek güç modu | Pasif |
| `Direct_Mode` | Doğrudan FET kontrolü | Pasif |
| `MP2791` | MP2791 seçimi (alternatif) | Pasif |

### 12.5. Programlama ve Debug

1. **J-Link bağlantısı:** SWD arayüzü üzerinden (SWDIO, SWCLK)
2. **Flash programlama:** Keil µVision "Download" komutu
3. **Debug log:** `JLinkLog.txt` dosyasında kayıtlı
4. **Batch programlama:** `Target 1.BAT` dosyası ile toplu programlama

### 12.6. UART Haberleşme Protokolü (Harici)

Firmware, UART üzerinden harici yazılım/dongle ile haberleşir:

| Parametre | Değer |
|-----------|-------|
| Dongle ID | 0x0BA0 |
| BMS ID (MP2797) | {0x97, 0x00} |
| Frame Başlangıç | 0x7E |
| Frame Bitiş | 0x0A |
| ACK Yanıtı | {0x7E, 0x00, ..., 0x0A} |
| NACK Yanıtı | {0x7E, 0x00, 0x00, 0xFF, 0xFF, ..., 0x0A} |

---

## Revizyon Geçmişi

| Sürüm | Tarih | Açıklama |
|-------|-------|----------|
| 1.0.0 | 19 Nisan 2026 | İlk sürüm — VGX BMS Firmware Datasheet |

---

*Bu doküman, TÜBİTAK 1507 — 7260634 numaralı proje kapsamında KOBİNERJİ A.Ş. tarafından hazırlanmıştır.*
