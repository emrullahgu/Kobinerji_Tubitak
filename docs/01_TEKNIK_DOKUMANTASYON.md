# TÜBİTAK 7260634 — Teknik Dokümantasyon

**Proje Adı:** Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi  
**Kuruluş:** KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.  
**Proje Kodu:** TÜBİTAK 1507 — 7260634  
**Doküman Sürümü:** 1.0.0  
**Tarih:** 19 Nisan 2026  
**Hazırlayan:** Proje Teknik Ekibi  

---

## İçindekiler

1. [Giriş ve Kapsam](#1-giriş-ve-kapsam)
2. [Sistem Genel Görünümü](#2-sistem-genel-görünümü)
3. [Veri İşleme Katmanı](#3-veri-işleme-katmanı)
4. [Özellik Mühendisliği Katmanı](#4-özellik-mühendisliği-katmanı)
5. [Makine Öğrenmesi Modeli](#5-makine-öğrenmesi-modeli)
6. [Batarya Yönetim Sistemi (BMS)](#6-batarya-yönetim-sistemi-bms)
7. [REST API Katmanı](#7-rest-api-katmanı)
8. [Web Dashboard](#8-web-dashboard)
9. [Yapılandırma Sistemi](#9-yapılandırma-sistemi)
10. [Sabitler ve Eşik Değerler](#10-sabitler-ve-eşik-değerler)
11. [Günlükleme (Logging) Sistemi](#11-günlükleme-logging-sistemi)
12. [Veri Akış Diyagramları](#12-veri-akış-diyagramları)
13. [Hata Yönetimi](#13-hata-yönetimi)
14. [Performans Gereksinimleri](#14-performans-gereksinimleri)
15. [Sürüm Geçmişi](#15-sürüm-geçmişi)

---

## 1. Giriş ve Kapsam

### 1.1. Projenin Amacı

Bu proje, kullanılmış lityum iyon bataryaların sağlık durumunu (State of Health — SoH) yapay zeka yöntemleriyle tahmin eden, batarya yönetim sistemiyle (BMS) entegre çalışan ve karar destek mekanizması sunan bir yazılım sistemi geliştirmeyi amaçlamaktadır.

### 1.2. Teknik Dokümanın Kapsamı

Bu doküman aşağıdaki bileşenlerin teknik detaylarını içerir:

| Bileşen | Modül | Dosya |
|---------|-------|-------|
| Veri İşleme | `data_processing/battery_data.py` | CSV/Excel yükleme, temizleme, normalizasyon |
| Özellik Çıkarımı | `data_processing/feature_extraction.py` | 50-100 özellik, istatistiksel/frekans/zaman alanı |
| SoH Modeli | `models/soh_model.py` | LSTM, GRU, AST-LSTM (PyTorch), Ridge, Lineer |
| BMS İzleme | `bms/battery_manager.py` | Hücre izleme, dengeleme, koruma |
| BMS Kontrolü | `bms/bms_controller.py` | Merkezi koordinasyon, karar verme |
| REST API | `api/battery_api.py` | 13 endpoint, Flask tabanlı |
| Web Arayüz | `index.html`, `script.js`, `styles.css` | Dashboard, 6 bölüm |
| Yapılandırma | `config.yaml`, `utils/constants.py` | 300+ sabit, 10 YAML bölümü |
| Günlükleme | `utils/logger.py` | Renkli konsol, dönerli dosya |

### 1.3. Kullanılan Teknolojiler

| Katman | Teknoloji | Sürüm |
|--------|-----------|-------|
| Veri Bilimi | NumPy, Pandas, SciPy | ≥1.21, ≥1.3, ≥1.7 |
| Makine Öğrenmesi | scikit-learn, joblib | ≥1.0, ≥1.1 |
| Derin Öğrenme (opsiyonel) | TensorFlow, Keras | ≥2.10 |
| Görselleştirme | Matplotlib, Seaborn, Plotly | ≥3.4, ≥0.11, ≥5.0 |
| Web Backend | Flask, Flask-CORS | ≥2.0, ≥3.0 |
| Web Frontend | HTML5, CSS3, Vanilla JS | — |
| Test | pytest, pytest-cov | ≥6.2, ≥2.12 |
| Yapılandırma | PyYAML | ≥5.4 |
| Dağıtım | Netlify (FE), Flask (BE) | — |

### 1.4. Hedef Performans Metrikleri

| Metrik | Hedef Değer | Kaynak |
|--------|------------|--------|
| SoH Tahmin Doğruluğu | ≥ %90 | İP-3 çıktısı |
| R² Skoru | ≥ 0.85 | Model değerlendirme |
| RMSE | < 5.0 (yüzde ölçeğinde) | Model değerlendirme |
| Sınıflandırma Doğruluğu | ≥ %85 | İP-4 çıktısı |
| Analiz Süresi | < 5 saniye | Gerçek zamanlı gereksinim |
| Geri Kazanım Etkisi | +%10 iyileşme | Mekanik süreç |

---

## 2. Sistem Genel Görünümü

### 2.1. Üç Katmanlı Mimari

```
┌──────────────────────────────────────────────────────┐
│            KATMAN 1: WEB FRONTEND                    │
│  ┌─────────┐ ┌──────────┐ ┌───────────┐              │
│  │Dashboard│ │ Zaman    │ │  Bütçe    │              │
│  │ Özet    │ │ Çizelgesi│ │  Takibi   │              │
│  └─────────┘ └──────────┘ └───────────┘              │
│  HTML5 + CSS3 + Vanilla JavaScript                   │
│  Responsive Grid, LocalStorage, Klavye Kısayolları   │
└──────────────────────┬───────────────────────────────┘
                       │ HTTP/JSON
              ┌────────▼────────┐
              │  KATMAN 2: API  │
              │  Flask REST     │
              │  13 Endpoint    │
              │  CORS Enabled   │
              └────────┬────────┘
                       │
┌──────────────────────▼───────────────────────────────┐
│            KATMAN 3: PYTHON BACKEND                  │
│                                                      │
│  ┌───────────────┐  ┌────────────────┐               │
│  │ Veri İşleme   │  │ Özellik        │               │
│  │  battery_data │──│  Çıkarımı      │               │
│  │  .py          │  │  feature_ext.  │               │ 
│  └───────────────┘  └───────┬────────┘               │ 
│                             │                        │
│  ┌───────────────┐  ┌──────▼─────────┐               │
│  │ BMS Kontrol   │  │ SoH Tahmin     │               │
│  │  bms_control  │◄─│  Modeli        │               │
│  │  ler.py       │  │  soh_model.py  │               │
│  └───────────────┘  └────────────────┘               │
│                                                      │
│  ┌───────────────┐  ┌────────────────┐               │
│  │ BMS İzleme    │  │ Sabitler +     │               │
│  │  battery_mgr. │  │  Logger        │               │
│  └───────────────┘  └────────────────┘               │
└──────────────────────────────────────────────────────┘
```

### 2.2. Modüler Bağımlılık Grafiği

```
constants.py ◄── logger.py
     ▲               ▲
     │               │
battery_data.py ◄── feature_extraction.py
     ▲                       ▲
     │                       │
     └──── soh_model.py ─────┘
                ▲
                │
     battery_manager.py
           ▲
           │
     bms_controller.py
           ▲
           │
     battery_api.py
```

### 2.3. Dizin Yapısı

```
Kobinerji_Tubitak/
├── python/
│   ├── utils/
│   │   ├── constants.py        # 300+ proje sabiti
│   │   └── logger.py           # Tekil günlükleme sistemi
│   ├── data_processing/
│   │   ├── battery_data.py     # Veri yükleme ve temizleme
│   │   └── feature_extraction.py # Özellik mühendisliği
│   ├── models/
│   │   └── soh_model.py        # SoH tahmin modeli (4 algoritma)
│   ├── bms/
│   │   ├── battery_manager.py  # Gerçek zamanlı izleme + dengeleme
│   │   └── bms_controller.py   # Merkezi BMS koordinasyonu
│   └── api/
│       └── battery_api.py      # REST API (13 endpoint)
├── tests/
│   └── test_suite.py           # 25+ birim testi
├── examples/
│   └── demo_end_to_end.py      # Uçtan uca demo (6 aşama)
├── docs/                        # Bu dokümantasyon dizini
├── config.yaml                  # YAML yapılandırma
├── python/
│   └── requirements.txt         # Python bağımlılıkları
├── index.html                   # Web dashboard
├── script.js                    # Frontend mantığı
└── styles.css                   # Stil dosyası
```

---

## 3. Veri İşleme Katmanı

### 3.1. BatteryData Sınıfı

**Dosya:** `python/data_processing/battery_data.py`  
**Amaç:** Batarya ölçüm verilerinin yüklenmesi, doğrulanması, temizlenmesi ve dışa aktarılması.

#### 3.1.1. Sınıf Tanımı

```python
class BatteryData:
    def __init__(self, battery_id: str, data: pd.DataFrame = None)
```

**Öznitelikler:**

| Öznitelik | Tür | Açıklama |
|-----------|-----|----------|
| `battery_id` | `str` | Benzersiz batarya kimliği |
| `data` | `pd.DataFrame` | Ham veya işlenmiş veri çerçevesi |
| `metadata` | `dict` | İşlem geçmişi ve istatistikler |

#### 3.1.2. Yöntemler

**`load_from_csv(filepath) → bool`**

CSV dosyasından veri yükleme. Otomatik zaman damgası dönüşümü yapar.

| Parametre | Tür | Varsayılan | Açıklama |
|-----------|-----|-----------|----------|
| `filepath` | `str/Path` | — | CSV dosya yolu |

**`load_from_excel(filepath, sheet_name=0) → bool`**

Excel dosyasından veri yükleme. Çoklu sayfa desteği sağlar.

**`create_sample_data(n_samples=1000, noise_level=0.05) → pd.DataFrame`**

Gerçekçi sentetik batarya verisi üretir.

Üretilen sütunlar:

| Sütun | Formül / Mantık | Birim |
|-------|-----------------|-------|
| `voltage` | Sinüzoidal dalga + gürültü (2.5–4.2 V) | V |
| `current` | Sinüzoidal + gürültü (±20 A) | A |
| `temperature` | 25°C + self-heating etkisi + gürültü | °C |
| `capacity` | Zaman bağımlı bozunma (2500 mAh'dan düşüş) | mAh |
| `impedance` | Zamana bağlı artış (50 mΩ'dan yükselme) | mΩ |
| `soc` | 0–100 arası hesaplama | % |
| `soh` | Kapasite / nominal kapasite × 100 | % |
| `timestamp` | 1 saniyelik artışlar | datetime |

**`validate_data() → bool`**

Zorunlu sütunları kontrol eder: `voltage`, `current`, `temperature`, `capacity`.

**`clean_data(remove_dups=True, handle_missing=True, remove_outliers_flag=True) → bool`**

Entegre temizleme boru hattı:

```
validate_data()
     ↓
remove_duplicates() → Çift kayıtları sil
     ↓
handle_missing_values() → Eksik değerleri enterpolasyonla doldur
     ↓
remove_outliers() → IQR (1.5×) ile aykırı değerleri çıkar
```

**`remove_outliers(method, threshold) → int`**

| Yöntem | Parametre | Açıklama |
|--------|-----------|----------|
| `'iqr'` | `OUTLIER_IQR_MULTIPLIER = 1.5` | Q1 − 1.5×IQR ≤ x ≤ Q3 + 1.5×IQR |
| `'zscore'` | `OUTLIER_ZSCORE_THRESHOLD = 3.0` | \|z\| < 3.0 |

**`normalize_data(method, columns) → pd.DataFrame`**

| Yöntem | Formül |
|--------|--------|
| `'standardize'` | $z = \frac{x - \mu}{\sigma}$ |
| `'minmax'` | $x' = \frac{x - x_{min}}{x_{max} - x_{min}}$ |

Yeni `{sütun}_norm` sütunları oluşturulur. Orijinal veriler korunur.

**`save_cleaned_data(filepath, format) → bool`**

| Format | Açıklama |
|--------|----------|
| `'csv'` | Virgülle ayrılmış değerler |
| `'parquet'` | Sütun tabanlı sıkıştırılmış format |
| `'json'` | JSON kayıtları |

**`get_statistics() → dict`**

Her sayısal sütun için: ortalama, standart sapma, minimum, maksimum, medyan değerlerini döndürür.

---

## 4. Özellik Mühendisliği Katmanı

### 4.1. FeatureExtractor Sınıfı

**Dosya:** `python/data_processing/feature_extraction.py`  
**Amaç:** Zaman serisi batarya verilerinden makine öğrenmesi modeline girdi olacak özellik vektörlerinin çıkarılması.

#### 4.1.1. Sınıf Tanımı

```python
class FeatureExtractor:
    def __init__(self, window_size=100, window_stride=10)
```

| Parametre | Varsayılan | Açıklama |
|-----------|-----------|----------|
| `window_size` | 100 | Kayan pencere boyutu (örneklem sayısı) |
| `window_stride` | 10 | Pencere kayma adımı |

#### 4.1.2. Özellik Grupları

**A. İstatistiksel Özellikler (20 adet/kanal)**

| # | Özellik | Formül |
|---|---------|--------|
| 1 | Ortalama | $\bar{x} = \frac{1}{N}\sum_{i=1}^{N} x_i$ |
| 2 | Standart Sapma | $\sigma = \sqrt{\frac{1}{N-1}\sum(x_i - \bar{x})^2}$ |
| 3 | Minimum | $\min(x)$ |
| 4 | Maksimum | $\max(x)$ |
| 5 | Medyan | $\tilde{x}$ |
| 6 | Q1 (25. yüzdelik) | Alt çeyrek |
| 7 | Q3 (75. yüzdelik) | Üst çeyrek |
| 8 | Aralık | $\max(x) - \min(x)$ |
| 9 | IQR | $Q3 - Q1$ |
| 10 | Çarpıklık | $\frac{m_3}{m_2^{3/2}}$ |
| 11 | Basıklık | $\frac{m_4}{m_2^2} - 3$ |
| 12 | Enerji | $\sum x_i^2$ |
| 13 | Güç | $\frac{1}{N}\sum x_i^2$ |
| 14 | RMS | $\sqrt{\frac{1}{N}\sum x_i^2}$ |
| 15 | Varyans | $\sigma^2$ |
| 16 | Değişim Katsayısı | $\frac{\sigma}{\bar{x}}$ |
| 17 | Maks Mutlak | $\max(\|x\|)$ |
| 18 | Tepe-Tepe | $\max(x) - \min(x)$ |
| 19 | Sıfır Geçiş | Sıfır çizgisini geçme sayısı |
| 20 | Tepe Faktör | $\frac{\max(|x|)}{RMS}$ |

**B. Frekans Alanı Özellikleri (5 adet/kanal)**

| # | Özellik | Yöntem |
|---|---------|--------|
| 1 | FFT Ortalama | $\bar{\|F(x)\|}$ |
| 2 | FFT Maksimum | $\max(\|F(x)\|)$ |
| 3 | FFT Enerjisi | $\sum\|F(x)\|^2$ |
| 4 | Baskın Frekans | $\arg\max(\|F(x)\|)$ |
| 5 | Spektral Ağırlık Merkezi | $\frac{\sum f \cdot \|F(f)\|}{\sum \|F(f)\|}$ |

**C. Zaman Alanı Özellikleri (8 adet/kanal)**

| # | Özellik | Açıklama |
|---|---------|----------|
| 1 | Otokorelasyon (lag=1) | Kısa dönem bağımlılık |
| 2 | Otokorelasyon (lag=5) | Orta dönem bağımlılık |
| 3 | Otokorelasyon (lag=10) | Uzun dönem bağımlılık |
| 4 | Türev Ortalama | Birinci türev ortalaması |
| 5 | Türev Std | Birinci türev standart sapması |
| 6 | Türev Maks | Birinci türev maksimumu |
| 7 | İkinci Türev Ortalama | Hızlanma ortalaması |
| 8 | İkinci Türev Std | Hızlanma yayılımı |

**D. Empedans Özellikleri (4 adet)**

| # | Özellik | Hesaplama |
|---|---------|-----------|
| 1 | DC İç Direnç (DCR) | $R_{DC} = \frac{\Delta V}{\Delta I}$ |
| 2 | AC Direnç @100 Hz | 100 Hz frekans yanıtı |
| 3 | AC Direnç @1 kHz | 1 kHz frekans yanıtı |
| 4 | Faz Açısı | $\phi = \arctan\left(\frac{V_{imag}}{V_{real}}\right)$ |

**E. Bataryaya Özgü Özellikler (8+ adet)**

| # | Özellik | Açıklama |
|---|---------|----------|
| 1 | SOC Ortalama | Şarj durumu ortalaması |
| 2 | SOC Minimum | Minimum şarj durumu |
| 3 | SOC Maksimum | Maksimum şarj durumu |
| 4 | SOC Aralığı | Şarj durumu salınım genişliği |
| 5 | Sıcaklık Ortalama | Hücre sıcaklık ortalaması |
| 6 | Sıcaklık Maks | Termal tepe değer |
| 7 | Kapasite Ortalama | Mevcut kapasite |
| 8 | Kapasite Bozunma | Kapasite azalma eğimi |
| 9 | Empedans Ortalama | Ortalama iç direnç |
| 10 | Empedans Değişimi | Direnç artış eğimi |

#### 4.1.3. Toplam Özellik Sayısı

| Kanal | İstatistik | Frekans | Zaman | Toplam |
|-------|-----------|---------|-------|--------|
| Voltaj | 20 | 5 | 8 | 33 |
| Akım | 20 | 5 | 8 | 33 |
| Sıcaklık | 20 | 5 | 8 | 33 |
| Empedans | — | — | — | 4 |
| Batarya Özgü | — | — | — | 10 |
| **TOPLAM** | | | | **~113** |

#### 4.1.4. Normalleştirme

```python
normalize_features(fit=False) → pd.DataFrame
```

StandardScaler kullanılır. `fit=True` ise eğitim verisine uydurup dönüştürür; `fit=False` ise sadece dönüştürür. `target_soh` sütunu normalizasyondan hariç tutulur.

---

## 5. Makine Öğrenmesi Modeli

### 5.1. SoHModel Sınıfı

**Dosya:** `python/models/soh_model.py`  
**Amaç:** Batarya sağlık durumunu (SoH) %0–100 arasında tahmin etmek.

#### 5.1.1. Desteklenen Algoritmalar

| Algoritma | Sınıf | Varsayılan Parametreler |
|-----------|-------|------------------------|
| AST-LSTM | `ASTLSTMSoH` (PyTorch) | hidden_size=128, num_layers=2, dropout=0.2, attention gate |
| LSTM | `LSTMSoH` (PyTorch) | hidden_size=128, num_layers=2, dropout=0.2 |
| GRU | `GRUSoH` (PyTorch) | hidden_size=128, num_layers=2, dropout=0.2 |
| Transfer Öğrenme | `TransferLearningWrapper` | NMC→LFP/NCA, katman dondurma + fine-tune |

#### 5.1.2. Eğitim Boru Hattı

```
Girdi: X (özellikler), y (SoH hedefleri)
     ↓
prepare_data() → train_test_split (80/20) + StandardScaler
     ↓
model.fit(X_train, y_train) → Model eğitimi
     ↓
predict(X_test) → Test tahminleri
     ↓
5-katlı çapraz doğrulama → CV skorları
     ↓
Çıktı: Metrikler sözlüğü
```

#### 5.1.3. Değerlendirme Metrikleri

| Metrik | Formül | Hedef |
|--------|--------|-------|
| R² | $R^2 = 1 - \frac{SS_{res}}{SS_{tot}}$ | ≥ 0.85 |
| RMSE | $\sqrt{\frac{1}{n}\sum(y_i - \hat{y}_i)^2}$ | < 5.0 (SoH %0–100 ölçeğinde) |
| MAE | $\frac{1}{n}\sum\|y_i - \hat{y}_i\|$ | — |
| MAPE | $\frac{100}{n}\sum\left\|\frac{y_i - \hat{y}_i}{y_i}\right\|$ | — |
| Medyan Hata | $\text{median}(\|y_i - \hat{y}_i\|)$ | — |
| CV Ortalama | 5-katlı çapraz doğrulama ortalaması | ≥ 0.90 |
| CV Std | 5-katlı çapraz doğrulama standart sapması | — |

#### 5.1.4. Tahmin ve Sınıflandırma

**`predict(X, confidence_interval=False)`**

- Çıktı [0, 100] aralığına kırpılır
- AST-LSTM Attention mekanizması ile öznitelik önem skorları hesaplanır

**`predict_single(features) → dict`**

Tek bir batarya için SoH tahmini ve karar:

| SoH Aralığı | Durum | Öneri | Yönlendirme |
|-------------|-------|-------|-------------|
| ≥ %85 | `REUSE` | İkinci yaşam uygulamaları | SECOND_LIFE_PROGRAM |
| %70–%85 | `RECYCLE` | Malzeme geri kazanımı | MECHANICAL_RECYCLING |
| < %70 | `CRITICAL` | Acil geri dönüşüm | HAZMAT_PROCESSING |

#### 5.1.5. Model Kalıcılığı

```python
save_model(filepath) → bool    # joblib ile kaydet
load_model(filepath) → bool    # joblib ile yükle
```

Kaydedilen bileşenler: model nesnesi, StandardScaler, özellik isimleri, eğitim geçmişi.

---

## 6. Batarya Yönetim Sistemi (BMS)

### 6.1. BatteryMonitor Sınıfı

**Dosya:** `python/bms/battery_manager.py`  
**Amaç:** Gerçek zamanlı hücre izleme, anomali algılama ve koruma.

#### 6.1.1. Hücre Veri Yapısı

```python
@dataclass
class CellData:
    cell_id: int          # Hücre numarası (0..N-1)
    voltage: float        # Gerilim (V)
    current: float        # Akım (A)
    temperature: float    # Sıcaklık (°C)
    soc: float           # Şarj Durumu (%)
    soh: float           # Sağlık Durumu (%)
    impedance: float     # İç Direnç (mΩ)
    timestamp: datetime   # Zaman damgası
```

#### 6.1.2. Koruma Kontrolleri

| Kontrol | Yöntem | Eşik (Uyarı) | Eşik (Kritik) |
|---------|--------|-------------|---------------|
| Aşırı Gerilim | `check_overvoltage()` | > 4.2 V | > 4.4 V |
| Düşük Gerilim | `check_undervoltage()` | < 2.5 V | < 2.3 V |
| Aşırı Sıcaklık | `check_overtemperature()` | > 60°C | > 80°C |
| Düşük Sıcaklık | `check_undertemperature()` | < 0°C | — |
| Hücre Dengesizliği | `check_imbalance()` | > 100 mV | — |
| Aşırı Akım (Şarj) | `check_overcurrent()` | > 5 A | — |
| Aşırı Akım (Deşarj) | `check_overcurrent()` | > 10 A | — |

#### 6.1.3. Pack Metrikleri

| Metrik | Hesaplama | Açıklama |
|--------|-----------|----------|
| Pack Gerilimi | $V_{pack} = \sum V_{cell_i}$ | Seri bağlı hücre toplamı |
| Pack Akımı | $I_{pack} = \overline{I_{cell}}$ | Hücre akımı ortalaması |
| Pack Sıcaklığı | $T_{pack} = \max(T_{cell_i})$ | En sıcak hücre |
| Pack SOC | $SOC_{pack} = \overline{SOC_{cell}}$ | Ortalama şarj durumu |
| Pack SOH | $SOH_{pack} = \min(SOH_{cell_i})$ | En zayıf hücre (darboğaz) |

#### 6.1.4. Uyarı Yapısı

```python
@dataclass
class BatteryCellAlert:
    cell_id: int         # Hücre ID (-1 = pack seviyesi)
    alert_type: str      # OVERVOLTAGE, UNDERVOLTAGE, ...
    value: float         # Ölçülen değer
    threshold: float     # Eşik değer
    severity: str        # info, warning, critical
    message: str         # İnsan okunabilir mesaj
    timestamp: datetime  # Zaman damgası
```

### 6.2. CellBalancer Sınıfı

**Dosya:** `python/bms/battery_manager.py`  
**Amaç:** Heterojen hücreler için aktif dengeleme.

#### 6.2.1. Dengeleme Algoritması

```
1. Tüm hücrelerin ortalama gerilimi hesapla: V_ort
2. Her hücre için sapma hesapla: ΔV = V_cell - V_ort
3. Eğer |ΔV| > eşik (50 mV):
   - ΔV > 0: Deşarj akımı uygula (min(I_bal, ΔV × 2))
   - ΔV < 0: Şarj akımı uygula (max(-I_bal, ΔV × 2))
4. Gerilim ayarlaması: V_new = V_old - (I_bal × 0.01)
```

| Parametre | Değer | Açıklama |
|-----------|-------|----------|
| `balancing_threshold` | 0.05 V (50 mV) | Dengeleme başlatma eşiği |
| `balancing_current` | 0.5 A | Maksimum dengeleme akımı |

### 6.3. BMSController Sınıfı

**Dosya:** `python/bms/bms_controller.py`  
**Amaç:** Merkezi koordinasyon, karar verme ve operasyonel kontrol.

#### 6.3.1. Sistem Durumları

```
HEALTHY ──→ WARNING ──→ CRITICAL ──→ SHUTDOWN
   ↑          │            │
   └──────────┘            │
   (uyarılar temizlenirse)  │
                           ↓
                      Manual Reset
```

| Durum | Koşul | İzin verilen işlemler |
|-------|-------|-----------------------|
| `HEALTHY` | Uyarı yok | Şarj ✓, Deşarj ✓ |
| `WARNING` | Uyarı var, kritik yok | Şarj ✓, Deşarj ✓ |
| `CRITICAL` | Kritik uyarı var | Şarj ✗, Deşarj ✗ |
| `SHUTDOWN` | Sistem kapatıldı | Tüm işlemler ✗ |

#### 6.3.2. Karar Verme Matrisi

| Giriş SOH | Sistem Durumu | Karar | Yönlendirme |
|-----------|--------------|-------|-------------|
| ≥ %85 | HEALTHY/WARNING | `REUSE` | İkinci yaşam programı |
| %70–%85 | HEALTHY/WARNING | `RECYCLE` | Mekanik geri dönüşüm |
| < %70 | HEALTHY/WARNING | `CRITICAL` | Tehlikeli madde işleme |
| Herhangi | CRITICAL | `CRITICAL` | Tehlikeli madde işleme (override) |

#### 6.3.3. Şarj/Deşarj Kontrolü

```python
process_charge_discharge(current_ma, duration_sec) → dict
```

İşlem akışı:

```
1. Akım yönünü belirle (pozitif=şarj, negatif=deşarj)
2. İzin kontrolü: allow_charging() / allow_discharging()
3. Akım limiti kontrolü: |I| ≤ I_max
4. Enerji hesapla: E = |I| × t × V_pack (kWh)
5. Toplam enerji muhasebesi güncelle
```

---

## 7. REST API Katmanı

### 7.1. BatteryAPIWrapper Sınıfı

**Dosya:** `python/api/battery_api.py`  
**Amaç:** Tüm Python modüllerine web üzerinden erişim sağlayan RESTful API.

#### 7.1.1. Endpoint Özeti

| # | Yöntem | Yol | Açıklama | Durum Kodları |
|---|--------|-----|----------|--------------|
| 1 | GET | `/api/health` | Sistem sağlık kontrolü | 200 |
| 2 | POST | `/api/battery/init` | Yeni batarya başlat | 201, 400 |
| 3 | POST | `/api/battery/<id>/update` | Hücre verilerini güncelle | 200, 404, 400 |
| 4 | POST | `/api/battery/<id>/monitor` | İzleme döngüsü çalıştır | 200, 404, 400 |
| 5 | POST | `/api/battery/<id>/predict_soh` | SOH tahmini yap | 200, 404, 400 |
| 6 | POST | `/api/battery/<id>/decide` | Karar ver (REUSE/RECYCLE/CRITICAL) | 200, 404, 400 |
| 7 | POST | `/api/battery/<id>/charge` | Şarj işlemini işle | 200, 404, 400 |
| 8 | POST | `/api/battery/<id>/discharge` | Deşarj işlemini işle | 200, 404, 400 |
| 9 | GET | `/api/battery/<id>/status` | Kapsamlı durum raporu | 200, 404, 400 |
| 10 | POST | `/api/features/extract` | Özellik çıkarımı | 200, 400 |
| 11 | POST | `/api/model/train` | Model eğitimi | 200, 400 |
| 12 | POST | `/api/model/<type>/predict` | Model ile tahmin | 200, 404, 400 |
| 13 | GET | `/api/model/<type>/info` | Model bilgisi | 200, 404, 400 |

#### 7.1.2. İstek/Yanıt Formatları

Tüm endpointler JSON formatında iletişim kurar. Content-Type: `application/json`.

**Hata Yanıt Formatı:**
```json
{
    "error": "Battery not found"
}
```

**Başarı Yanıt Formatı:**
```json
{
    "status": "updated",
    "battery_id": "BAT_001",
    "records": 8
}
```

---

## 8. Web Dashboard

### 8.1. Yapı

6 sekmeli tek sayfa uygulaması (SPA):

| Sekme | İçerik | Veri Kaynağı |
|-------|--------|-------------|
| Dashboard | Özet metrikler, ilerleme, riskler | LocalStorage |
| Zaman Çizelgesi | 7 iş paketi ilerleme çubukları | projectData nesnesi |
| Kilometre Taşları | 7 hedef ve tarih | Sabit veri |
| İş Paketleri | Detaylı aktiviteler | Sabit veri |
| Bütçe | Toplam 3.497.730 TL dağılım | Sabit veri |
| Ekip | 5 takım üyesi | Sabit veri |

### 8.2. Klavye Kısayolları

| Kısayol | İşlev |
|---------|-------|
| Alt+1..6 | Sekme geçişi |
| Ctrl+P | Yazdır |
| Ctrl+S | Kaydet (LocalStorage'a) |

### 8.3. Dışa Aktarım

- `exportProjectDataAsJSON()` — JSON formatında indirme
- `exportProjectDataAsCSV()` — CSV formatında indirme
- 60 saniyelik otomatik kaydetme

---

## 9. Yapılandırma Sistemi

### 9.1. config.yaml Yapısı

| Bölüm | Anahtar Ayarlar |
|--------|----------------|
| `project` | name, version "1.0.0" |
| `data` | test_train_ratio=0.8, validation=0.1, seed=42 |
| `features` | window_size=100, stride=10, normalize="standardize" |
| `model` | type="random_forest", RF(100/20/5), XGB(100/6/0.1) |
| `training` | batch_size=32, epochs=100, lr=0.001, patience=10 |
| `bms` | balancing(enabled/0.05V/0.5A), monitor_interval=1.0s |
| `logging` | level=INFO, 10MB/5 yedek |
| `analysis` | soh_critical=0.70, reuse=0.85, recycle=0.70, max_time=5s |
| `output` | formats=[csv,json,excel], detailed_report=true |
| `api` | host="0.0.0.0", port=5000, cors_enabled=true |

---

## 10. Sabitler ve Eşik Değerler

### 10.1. Batarya Elektriksel Parametreleri

| Sabit | Değer | Birim | Açıklama |
|-------|-------|-------|----------|
| `NOMINAL_VOLTAGE` | 3.7 | V | Nominal hücre gerilimi |
| `MIN_VOLTAGE` | 2.5 | V | Minimum güvenli gerilim |
| `MAX_VOLTAGE` | 4.2 | V | Maksimum güvenli gerilim |
| `NOMINAL_CAPACITY` | 2500 | mAh | Nominal kapasite |
| `NOMINAL_ENERGY` | 9.25 | Wh | Nominal enerji |
| `CELL_IMPEDANCE_TYPICAL` | 50 | mΩ | Tipik iç direnç |
| `LI_ION_CHEMISTRY` | LiCoO2 | — | Kimya türü |

### 10.2. BMS Koruma Eşikleri

| Sabit | Değer | Birim |
|-------|-------|-------|
| `BMS_MAX_VOLTAGE` | 4.2 | V |
| `BMS_MIN_VOLTAGE` | 2.5 | V |
| `BMS_MAX_CHARGE_CURRENT` | 5.0 | A |
| `BMS_MAX_DISCHARGE_CURRENT` | 10.0 | A |
| `BMS_MAX_TEMPERATURE` | 60 | °C |
| `BMS_MIN_TEMPERATURE` | 0 | °C |
| `BMS_SHUTDOWN_TEMPERATURE` | 80 | °C |
| `BMS_BALANCING_THRESHOLD` | 0.05 | V |
| `BMS_BALANCING_CURRENT` | 0.5 | A |
| `BMS_ALERT_THRESHOLD` | 0.10 | V |
| `BMS_MONITOR_INTERVAL` | 1.0 | s |

### 10.3. Model Hedefleri

| Sabit | Değer |
|-------|-------|
| `TARGET_ACCURACY` | 0.90 |
| `TARGET_R2` | 0.85 |
| `TARGET_RMSE` | 5.0 |
| `METRIC_CLASSIFICATION_ACCURACY` | 0.85 |

### 10.4. Sınıflandırma Eşikleri

| Sabit | Değer | Karar |
|-------|-------|-------|
| `REUSE_THRESHOLD` | 0.85 | Yeniden kullanım |
| `RECYCLE_THRESHOLD` | 0.70 | Geri dönüşüm |
| `RESEARCH_THRESHOLD` | 0.50 | Araştırma |

---

## 11. Günlükleme (Logging) Sistemi

### 11.1. Mimari

```
                    ┌─────────────────┐
                    │    Logger       │
                    │  (Singleton)    │
                    └────┬───────┬────┘
                         │       │
              ┌──────────▼┐  ┌──▼──────────────┐
              │ Console   │  │ RotatingFile     │
              │ Handler   │  │ Handler          │
              │ (Renkli)  │  │ (10MB × 5 yedek) │
              └───────────┘  └──────────────────┘
```

### 11.2. Renk Kodları

| Seviye | Renk | ANSI Kodu |
|--------|------|-----------|
| DEBUG | Cyan | `\033[36m` |
| INFO | Yeşil | `\033[32m` |
| WARNING | Sarı | `\033[33m` |
| ERROR | Kırmızı | `\033[31m` |
| CRITICAL | Magenta | `\033[35m` |

### 11.3. Özelleştirilmiş Günlük Yöntemleri

| Yöntem | Parametreler | Kullanım |
|--------|-------------|----------|
| `log_data_processing()` | battery_id, rows_processed, rows_cleaned, duration | Veri temizleme sonrası |
| `log_model_training()` | model_name, accuracy, r2_score, training_time | Model eğitimi sonrası |
| `log_prediction()` | battery_id, soh, confidence | Tahmin sonrası |
| `log_bms_alert()` | alert_type, battery_id, value, threshold | BMS uyarısı tetiklendiğinde |
| `log_error()` | module, error_message, exception | Hata oluştuğunda |
| `log_performance()` | operation, duration, items_processed | Performans ölçümü |

---

## 12. Veri Akış Diyagramları

### 12.1. Ana Veri Akışı

```
[Ham Batarya Verisi]
        │
        ▼
┌─────────────────┐
│ BatteryData     │ ← CSV / Excel / Sentetik
│  .clean_data()  │
└────────┬────────┘
         │ Temizlenmiş DataFrame
         ▼
┌─────────────────┐
│ FeatureExtractor│
│  .extract_all() │ ← 113 özellik/pencere
└────────┬────────┘
         │ Özellik matrisi (N × 113)
         ▼
┌─────────────────┐
│ SoHModel        │
│  .train()       │ ← 80/20 bölme + 5-katlı CV
│  .predict()     │
└────────┬────────┘
         │ SoH tahminleri (%)
         ▼
┌─────────────────┐
│ BMSController   │
│  .make_decision │ ← REUSE / RECYCLE / CRITICAL
└────────┬────────┘
         │
         ▼
[Karar Çıktısı + Raporlama]
```

### 12.2. BMS Gerçek Zamanlı Döngüsü

```
[Sensör Verisi] ──→ update_cell_data()
                           │
                    ┌──────▼──────┐
                    │   monitor() │ ← 1 sn aralık
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
        ┌─────▼─────┐ ┌──▼───┐ ┌─────▼──────┐
        │ Overvolt  │ │Over │ │ Imbalance │
        │ Undervolt │ │Temp │ │ Overcurr  │
        └─────┬─────┘ └──┬───┘ └─────┬──────┘
              │          │           │
              └──────────┼───────────┘
                         │
                  ┌──────▼──────┐
                  │ Uyarı Listesi│
                  └──────┬──────┘
                         │
              ┌──────────┼──────────┐
              ▼          ▼          ▼
          [HEALTHY]  [WARNING]  [CRITICAL]
              │
              ▼
        Dengeleme gerekli mi?
          /        \
         Evet      Hayır
          │
    execute_balancing()
```

---

## 13. Hata Yönetimi

### 13.1. Hata Hiyerarşisi

| Seviye | Örnek | İşlem |
|--------|-------|-------|
| Bilgilendirme | Veri yüklendi | Günlüğe yaz |
| Uyarı | Eksik sütun | Varsayılan değer kullan |
| Hata | Dosya bulunamadı | False döndür, günlüğe yaz |
| Kritik | Bellek yetersiz | İşlemi durdur |

### 13.2. İstisna İşleme Deseni

Tüm modüllerde aşağıdaki desen kullanılır:

```python
try:
    # İşlem
    result = operation()
except SpecificException as e:
    logger.error(f"Operation failed: {e}")
    return False  # veya boş/varsayılan değer
except Exception as e:
    logger.error(f"Unexpected error: {e}")
    raise  # Beklenmeyen hataları yeniden fırlat
```

---

## 14. Performans Gereksinimleri

| Bileşen | Metrik | Hedef | Ölçüm Yöntemi |
|---------|--------|-------|---------------|
| Veri İşleme | İşlem süresi | < 2 s / 10K satır | `log_performance()` |
| Özellik Çıkarımı | Pencere başına süre | < 1 s | profiling |
| Model Eğitimi | Toplam süre | < 10 dk / veri seti | `log_model_training()` |
| Model Tahmini | Tekil tahmin | < 100 ms | API yanıt süresi |
| BMS İzleme | Döngü süresi | < 1 s | `monitor_interval` |
| API Yanıt | Ortalama latans | < 500 ms | Flask profiling |
| Dashboard | Sayfa yüklenme | < 3 s | Tarayıcı metrikleri |

---

## 15. Sürüm Geçmişi

| Sürüm | Tarih | Değişiklikler |
|-------|-------|---------------|
| 1.0.0 | 19.04.2026 | İlk sürüm — Tüm modüller tamamlandı |

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — KOBİNERJİ A.Ş.*
