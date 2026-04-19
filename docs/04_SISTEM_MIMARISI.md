# TÜBİTAK 7260634 — Sistem Mimarisi Dokümanı

**Sürüm:** 1.0.0  
**Tarih:** 19 Nisan 2026  

---

## İçindekiler

1. [Mimari Genel Bakış](#1-mimari-genel-bakış)
2. [Katman Mimarisi](#2-katman-mimarisi)
3. [Modül Tasarımları](#3-modül-tasarımları)
4. [Veri Modelleri](#4-veri-modelleri)
5. [Entegrasyon Noktaları](#5-entegrasyon-noktaları)
6. [Güvenlik Mimarisi](#6-güvenlik-mimarisi)
7. [Ölçeklenebilirlik](#7-ölçeklenebilirlik)
8. [Tasarım Kararları](#8-tasarım-kararları)

---

## 1. Mimari Genel Bakış

### 1.1. Üst Düzey Mimari

Sistem üç temel katmandan oluşan bir monolitik mimari üzerine inşa edilmiştir:

```
╔════════════════════════════════════════════════════════════╗
║                   SUNUM KATMANI                           ║
║  ┌──────────────────────────────────────────────────────┐ ║
║  │              Web Dashboard (SPA)                     │ ║
║  │  HTML5 / CSS3 / Vanilla JavaScript                  │ ║
║  │  6 Sekme: Dashboard, Timeline, Milestones,          │ ║
║  │           WorkPackages, Budget, Team                 │ ║
║  │  LocalStorage Kalıcılığı                            │ ║
║  └────────────────────────┬─────────────────────────────┘ ║
╠═══════════════════════════╪═══════════════════════════════╣
║                           │ HTTP/JSON                     ║
║                   SERVİS KATMANI                          ║
║  ┌────────────────────────▼─────────────────────────────┐ ║
║  │           Flask REST API Sunucusu                    │ ║
║  │  BatteryAPIWrapper — 13 Endpoint                    │ ║
║  │  JSON İstek/Yanıt, CORS Etkin                       │ ║
║  └────┬──────────┬─────────────┬────────────────────────┘ ║
╠═══════╪══════════╪═════════════╪══════════════════════════╣
║       │          │             │                           ║
║                   İŞ MANTIK KATMANI                       ║
║  ┌────▼───┐ ┌───▼──────┐ ┌───▼───────┐ ┌──────────────┐ ║
║  │  Veri  │ │ Özellik  │ │   Model   │ │     BMS      │ ║
║  │ İşleme │→│ Çıkarımı │→│  (SoH)    │→│  Kontrolcü   │ ║
║  └────────┘ └──────────┘ └───────────┘ └──────────────┘ ║
║       │          │             │              │           ║
║  ┌────▼──────────▼─────────────▼──────────────▼─────────┐ ║
║  │          Altyapı Servisleri                          │ ║
║  │  constants.py │ logger.py │ config.yaml              │ ║
║  └──────────────────────────────────────────────────────┘ ║
╚════════════════════════════════════════════════════════════╝
```

### 1.2. Temel Tasarım İlkeleri

| İlke | Açıklama | Uygulama |
|------|----------|----------|
| **Modülerlik** | Her bileşen bağımsız çalışabilir | Her modülde `__main__` örneği |
| **Tek Sorumluluk** | Her sınıf tek bir işe odaklanır | BatteryData sadece veri, SoHModel sadece tahmin |
| **Merkezi Yapılandırma** | Sabitler ve ayarlar tek noktada | constants.py + config.yaml |
| **Tekil Örüntü** | Logger paylaşılan tek örnek | Logger sınıfı `__new__` override |
| **Gevşek Bağlantı** | Modüller arası minimal bağımlılık | Arayüz bazlı iletişim |
| **Katmanlı Mimari** | Bağımlılık tek yöne akar | Alt katman üst katmanı bilmez |

---

## 2. Katman Mimarisi

### 2.1. Sunum Katmanı (Frontend)

```
┌─────────────────────────────────────────────┐
│                index.html                    │
│  ┌─────────────────────────────────────────┐ │
│  │           <nav> (6 Sekme)               │ │
│  │  Dashboard | Timeline | Milestones |    │ │
│  │  WorkPkgs  | Budget   | Team            │ │
│  └─────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────┐ │
│  │           <main> (Aktif Sekme)          │ │
│  │  Dinamik içerik — tek seferde 1 sekme   │ │
│  └─────────────────────────────────────────┘ │
└──────────────┬───────────┬──────────────────┘
               │           │
         styles.css    script.js
```

**script.js İç Yapısı:**

```
script.js
├── Navigasyon Yönetimi
│   ├── Tab geçişi (data-section)
│   └── Klavye kısayolları (Alt+1..6)
├── Veri Yönetimi
│   ├── projectData nesnesi (7 İP, 7 KT, 5 üye)
│   ├── LocalStorage otomatik kayıt (60s)
│   └── JSON/CSV dışa aktarma
├── UI Güncelleme
│   ├── updateWorkPackageProgress()
│   ├── updateOverallProgress()
│   └── Bildirim sistemi (toast)
└── Yardımcı İşlevler
    ├── getDateStatus()
    ├── calculateDaysBetween()
    └── formatDate() (Türkçe yerel ayar)
```

### 2.2. Servis Katmanı (API)

```
BatteryAPIWrapper
├── Flask App (CORS etkin)
├── Dahili Depolar (bellek içi)
│   ├── controllers: dict[str, BMSController]
│   ├── feature_extractors: dict[str, FeatureExtractor]
│   └── soh_models: dict[str, SoHModel]
└── 13 Route
    ├── Sistem: /health
    ├── Batarya (7): init, update, monitor, predict_soh, decide, charge, discharge, status
    ├── Özellik (1): extract
    └── Model (3): train, predict, info
```

**İstek Yaşam Döngüsü:**

```
HTTP İsteği
    ↓
Flask Route Eşleme
    ↓
JSON Ayrıştırma (request.get_json())
    ↓
Parametre Doğrulama
    ↓
İş Mantığı Çağrısı (Controller/Model)
    ↓
Sonuç JSON Serileştirme
    ↓
HTTP Yanıtı (200/201/400/404)
```

### 2.3. İş Mantık Katmanı

```
┌─────────────────────────────────────────────────────────┐
│                    İŞ MANTIK KATMANI                    │
│                                                         │
│  Veri Boru Hattı (sol → sağ):                          │
│                                                         │
│  ┌─────────────┐    ┌──────────────┐    ┌────────────┐ │
│  │ BatteryData │───→│FeatureExtract│───→│  SoHModel  │ │
│  │             │    │  or          │    │            │ │
│  │ • load      │    │ • stat(20)   │    │ • train    │ │
│  │ • clean     │    │ • freq(5)    │    │ • predict  │ │
│  │ • normalize │    │ • time(8)    │    │ • evaluate │ │
│  │ • save      │    │ • impedance(4│    │ • save/load│ │
│  └─────────────┘    │ • battery(10)│    └─────┬──────┘ │
│                     └──────────────┘          │        │
│                                               ▼        │
│  BMS Alt Sistemi:                                      │
│                                                         │
│  ┌──────────────┐    ┌──────────────┐                  │
│  │BatteryMonitor│◄───│ CellBalancer │                  │
│  │              │    │              │                  │
│  │ • check_*    │    │ • calculate  │                  │
│  │ • monitor    │    │ • execute    │                  │
│  │ • get_pack_* │    │ • is_needed  │                  │
│  └──────┬───────┘    └──────────────┘                  │
│         │                                               │
│  ┌──────▼───────┐                                      │
│  │BMSController │                                      │
│  │              │                                      │
│  │ • monitoring │                                      │
│  │ • permissions│                                      │
│  │ • decisions  │                                      │
│  │ • reporting  │                                      │
│  └──────────────┘                                      │
└─────────────────────────────────────────────────────────┘
```

### 2.4. Altyapı Katmanı

```
┌────────────────────────────────────────────┐
│            ALTYAPI KATMANI                  │
│                                            │
│  ┌──────────────┐   ┌──────────────┐       │
│  │ constants.py │   │  config.yaml │       │
│  │              │   │              │       │
│  │  300+ sabit  │   │ 10 bölüm    │       │
│  │  Fiziksel    │   │  Çalışma zam.│       │
│  │  parametreler│   │  ayarları    │       │
│  └──────┬───────┘   └──────────────┘       │
│         │                                   │
│  ┌──────▼───────┐                           │
│  │  logger.py   │                           │
│  │              │                           │
│  │  Singleton   │                           │
│  │  Renkli      │──→ Console (ANSI)        │
│  │  Günlükçü    │──→ Dosya (Rotating)      │
│  └──────────────┘                           │
└────────────────────────────────────────────┘
```

---

## 3. Modül Tasarımları

### 3.1. BatteryData — Durum Makinesi

```
                   ┌─────────┐
                   │  BOŞALT │
                   │  (init) │
                   └────┬────┘
                        │ load_from_csv() / create_sample_data()
                        ▼
                   ┌─────────┐
                   │ HAM VERİ│
                   │ (loaded)│
                   └────┬────┘
                        │ validate_data()
                        ▼
                   ┌──────────┐
                   │DOĞRULANMIŞ│
                   │(validated)│
                   └────┬─────┘
                        │ clean_data()
                        ▼
                   ┌───────────┐
                   │TEMİZLENMİŞ│
                   │ (cleaned) │
                   └────┬──────┘
                        │ normalize_data()
                        ▼
                   ┌──────────────┐
                   │NORMALLEŞTİRİL│
                   │MİŞ (normalized)│
                   └──────┬───────┘
                          │ save_cleaned_data()
                          ▼
                   ┌─────────────┐
                   │  KAYDEDİLMİŞ│
                   └─────────────┘
```

### 3.2. SoHModel — Eğitim Akışı

```
  ┌──────────┐
  │ Başlangıç│
  └────┬─────┘
       │
       ▼
  ┌──────────────────┐
  │ prepare_data()   │
  │ • train_test_split│
  │ • StandardScaler  │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ model.fit()      │
  │ • RF/GB/Ridge/LR │
  └────────┬─────────┘
           │
     ┌─────┼─────┐
     │     │     │
     ▼     ▼     ▼
  ┌─────┐┌────┐┌─────┐
  │Train││Test││ CV  │
  │Pred ││Pred││5-Katlı│
  └──┬──┘└─┬──┘└──┬──┘
     │     │      │
     ▼     ▼      ▼
  ┌──────────────────┐
  │ Metrikler        │
  │ R², RMSE, MAE    │
  │ MAPE, CV±std     │
  └──────────────────┘
```

### 3.3. BMSController — Kontrol Döngüsü

```
  ┌─────────────────────────────┐
  │    update_batteries(data)   │ ← Sensör verileri
  └─────────────┬───────────────┘
                │
                ▼
  ┌─────────────────────────────┐
  │   execute_monitoring()      │
  │   ├─ monitor.monitor()      │ ← 7 kontrol
  │   ├─ Durum belirle          │ ← HEALTHY/WARNING/CRITICAL
  │   └─ Dengeleme gerekli mi?  │
  └─────────────┬───────────────┘
                │
        ┌───────┼───────┐
        │               │
        ▼               ▼
  ┌───────────┐  ┌──────────────┐
  │ Dengeleme │  │ İzin Kontrol │
  │ gereksi   │  │ allow_charge │
  │ ise çalıştır│ │allow_discharg│
  └───────────┘  └──────┬───────┘
                        │
                        ▼
                ┌───────────────┐
                │ estimate_soh()│
                └───────┬───────┘
                        │
                        ▼
                ┌───────────────┐
                │make_decision()│
                │  REUSE        │
                │  RECYCLE      │
                │  CRITICAL     │
                └───────┬───────┘
                        │
                        ▼
                ┌───────────────┐
                │generate_report│
                └───────────────┘
```

---

## 4. Veri Modelleri

### 4.1. Çekirdek Veri Yapıları

**CellData (Dataclass)**

```
CellData
├── cell_id: int          ─── Benzersiz hücre kimliği
├── voltage: float        ─── Anlık gerilim (V)
├── current: float        ─── Anlık akım (A)
├── temperature: float    ─── Anlık sıcaklık (°C)
├── soc: float            ─── Şarj durumu (%)
├── soh: float            ─── Sağlık durumu (%)
├── impedance: float      ─── İç direnç (mΩ)
└── timestamp: datetime   ─── Ölçüm zamanı
```

**BatteryCellAlert (Dataclass)**

```
BatteryCellAlert
├── cell_id: int          ─── Hücre ID (-1 = pack düzeyi)
├── alert_type: str       ─── OVERVOLTAGE, UNDERTEMP, IMBALANCE, ...
├── value: float          ─── Tetikleyen değer
├── threshold: float      ─── Eşik değer
├── severity: str         ─── info | warning | critical
├── message: str          ─── Açıklama metni
└── timestamp: datetime   ─── Oluşma zamanı
```

### 4.2. Durum Enumerasyonları

```
CellState                  ChargeState
├── NORMAL                 ├── IDLE
├── WARNING                ├── CHARGING
├── ALERT                  ├── DISCHARGING
├── CRITICAL               ├── BALANCING
└── SHUTDOWN               └── FAULT

SystemState                BatteryDecision
├── HEALTHY                ├── REUSE
├── WARNING                ├── RECYCLE
├── CRITICAL               └── CRITICAL
└── SHUTDOWN
```

### 4.3. Veri Akış Matrisi

| Kaynak | Hedef | Veri Tipi | Aktarım |
|--------|-------|-----------|---------|
| CSV/Excel | BatteryData | DataFrame | load_from_csv/excel |
| BatteryData | FeatureExtractor | DataFrame | extract_all_features(data) |
| FeatureExtractor | SoHModel | numpy array | train(X, y) / predict(X) |
| Sensörler | BatteryMonitor | CellData | update_cell_data() |
| BatteryMonitor | CellBalancer | voltage list | calculate_balancing_currents() |
| BatteryMonitor | BMSController | Alert list | execute_monitoring() |
| SoHModel | BMSController | float (SoH) | estimate_soh() |
| BMSController | API | dict (JSON) | generate_report() |
| API | Frontend | JSON | HTTP Response |

---

## 5. Entegrasyon Noktaları

### 5.1. Frontend ↔ Backend Entegrasyonu

```
Frontend (script.js)
        │
        │  fetch('/api/battery/BAT_001/monitor', { method: 'POST' })
        │
        ▼
API Sunucusu (Flask :5000)
        │
        │  controller.execute_monitoring()
        │
        ▼
BMSController
        │
        │  JSON yanıt
        │
        ▼
Frontend (DOM güncelleme)
```

**Entegrasyon Noktası:** `fetchProjectDataFromAPI()` (script.js içinde taslak olarak mevcut)

### 5.2. Modüler arası Entegrasyon

```python
# Tam entegrasyon akışı

# 1. Veri Katmanı
battery = BatteryData('BAT_001')
battery.load_from_csv('data.csv')
battery.clean_data()

# 2. Özellik Katmanı
extractor = FeatureExtractor()
features = extractor.extract_all_features(battery.data)

# 3. Model Katmanı
model = SoHModel('random_forest')
model.train(X, y)

# 4. BMS Katmanı (model entegrasyonu)
controller = BMSController('BAT_001', soh_model=model)
controller.update_batteries(sensor_data)
controller.execute_monitoring()

# 5. Karar
soh = controller.estimate_soh(features=new_features)
decision = controller.make_decision(soh['soh'])
```

---

## 6. Güvenlik Mimarisi

### 6.1. Mevcut Güvenlik Kontrolleri

| Katman | Kontrol | Durum |
|--------|---------|-------|
| API | Girdi doğrulama (temel) | Mevcut |
| API | Hata mesajı gizleme | Mevcut |
| BMS | Gerilim/Akım/Sıcaklık limitleri | Mevcut |
| BMS | Kritik durumda şarj/deşarj engelleme | Mevcut |
| Logger | Hassas veri olmadan günlükleme | Mevcut |
| Frontend | XSS koruması (Vanilla JS, innerHTML yerine textContent) | Kısmi |

### 6.2. Önerilen Güvenlik İyileştirmeleri

| Öncelik | Güvenlik Kontrolü | Açıklama |
|---------|-------------------|----------|
| Yüksek | JWT kimlik doğrulama | API erişim kontrolü |
| Yüksek | HTTPS | Şifreli iletişim |
| Orta | Hız sınırlama | DoS koruması |
| Orta | JSON Schema doğrulama | Girdi doğrulama güçlendirme |
| Düşük | CORS kısıtlama | Belirli originlerle sınırlama |
| Düşük | Erişim günlükleri | İzlenebilirlik |

---

## 7. Ölçeklenebilirlik

### 7.1. Mevcut Kapasite

| Bileşen | Mevcut Limit | Darboğaz |
|---------|-------------|----------|
| Batarya sayısı | Bellek ile sınırlı | Dict tabanlı bellek içi depo |
| Hücre sayısı | Sınırsız (parametre ile) | num_cells parametresi |
| Eşzamanlı istek | Flask varsayılanı (~10) | Tek thread |
| Veri boyutu | RAM ile sınırlı | Pandas DataFrame |
| Model sayısı | Sınırsız | Dict tabanlı depo |

### 7.2. Ölçekleme Stratejileri

**Dikey Ölçekleme (Mevcut):**
- Daha fazla RAM → daha büyük veri setleri
- Daha hızlı CPU → daha kısa eğitim süresi

**Yatay Ölçekleme (Gelecek):**
- Gunicorn/uWSGI ile çoklu worker
- Redis ile dağıtık önbellek
- PostgreSQL ile kalıcı depolama
- Celery ile asenkron görevler

---

## 8. Tasarım Kararları

### 8.1. Neden Vanilla JavaScript?

| Alternatif | Reddedilme Nedeni |
|------------|------------------|
| React | Gereksiz karmaşıklık, Netlify derleme gereksinimi |
| Vue | Ek öğrenme eğrisi, küçük proje için fazla |
| Angular | Aşırı mühendislik |
| **Vanilla JS** | **Seçilen: Sıfır bağımlılık, hızlı yüklenme, Netlify uyumlu** |

### 8.2. Neden Flask?

| Alternatif | Reddedilme Nedeni |
|------------|------------------|
| Django | Aşırı mühendislik, ORM gereksiz |
| FastAPI | Async gereksinimi yok, takım deneyimi |
| **Flask** | **Seçilen: Hafif, hızlı prototipleme, kolay entegrasyon** |

### 8.3. Neden Random Forest Varsayılan?

| Özellik | Random Forest | Gradient Boosting | Linear |
|---------|--------------|-------------------|--------|
| Doğruluk | Yüksek | En yüksek | Düşük |
| Hız | Orta | Yavaş | Hızlı |
| Aşırı öğrenme riski | Düşük | Orta | — |
| Yorumlanabilirlik | İyi | Orta | Çok iyi |
| Hiperparametre hassasiyeti | Düşük | Yüksek | — |
| **Genel uygunluk** | **En dengeli** | — | — |

### 8.4. Neden Bellek İçi Depolama?

Mevcut prototip aşamasında veritabanı kullanılmamıştır:
- Hızlı geliştirme ve test
- Dış bağımlılık yok
- Prototip için yeterli kapasite
- Gelecekte PostgreSQL/MongoDB eklenebilir

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — Sistem Mimarisi Dokümanı v1.0.0*
