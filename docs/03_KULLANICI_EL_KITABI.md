# TÜBİTAK 7260634 — Kullanıcı El Kitabı

**Proje:** Yapay Zeka Destekli Lityum İyon Batarya Analizi  
**Sürüm:** 1.0.0  
**Tarih:** 19 Nisan 2026  
**Hedef Kitle:** Proje ekibi, araştırmacılar, Son kullanıcılar  

---

## İçindekiler

1. [Sistem Hakkında](#1-sistem-hakkında)
2. [Kurulum](#2-kurulum)
3. [Hızlı Başlangıç](#3-hızlı-başlangıç)
4. [Web Dashboard Kullanımı](#4-web-dashboard-kullanımı)
5. [Batarya Veri İşleme](#5-batarya-veri-işleme)
6. [SoH Modeli ile Çalışma](#6-soh-modeli-ile-çalışma)
7. [BMS Kullanımı](#7-bms-kullanımı)
8. [API Sunucusu](#8-api-sunucusu)
9. [Testleri Çalıştırma](#9-testleri-çalıştırma)
10. [Sık Sorulan Sorular](#10-sık-sorulan-sorular)
11. [Sorun Giderme](#11-sorun-giderme)

---

## 1. Sistem Hakkında

### 1.1. Ne Yapar?

Bu yazılım sistemi üç temel işlevi yerine getirir:

1. **Batarya Sağlık Tahmini (SoH):** Kullanılmış lityum iyon bataryaların kalan kapasitesini yapay zeka ile tahmin eder.

2. **Batarya Yönetim Sistemi (BMS):** Hücre gerilimi, akım, sıcaklık gibi parametreleri gerçek zamanlı izler; aşırı gerilim, aşırı sıcaklık, dengesizlik gibi tehlikeli durumları algılar.

3. **Karar Destek Sistemi:** Tahmin edilen SoH değerine göre bataryayı üç kategoriden birine yönlendirir:

| Kategori | SOH Aralığı | Anlam |
|----------|------------|-------|
| **REUSE** (Yeniden Kullan) | ≥ %85 | Enerji depolama, elektrikli araç gibi ikinci yaşam uygulamalarına uygun |
| **RECYCLE** (Geri Dönüştür) | %70–%85 | Mekanik geri dönüşümle malzeme geri kazanımı |
| **CRITICAL** (Kritik) | < %70 | Acil tehlikeli madde işleme |

### 1.2. Kimler İçin?

- **Araştırmacılar:** Batarya analiz algoritmaları geliştiren mühendisler
- **Operatörler:** BMS izleme panelini kullanan teknik personel
- **Proje Yöneticileri:** Dashboard üzerinden ilerlemeyi takip edenler
- **Yazılım Geliştiriciler:** API üzerinden entegrasyon yapanlar

---

## 2. Kurulum

### 2.1. Gereksinimler

| Bileşen | Minimum Sürüm |
|---------|---------------|
| Python | 3.8+ |
| pip | 21.0+ |
| Tarayıcı | Chrome 90+ / Firefox 88+ / Edge 90+ |
| İşletim Sistemi | Windows 10+, macOS 10.15+, Linux (Ubuntu 20.04+) |
| RAM | 4 GB (minimum), 8 GB (önerilen) |
| Disk | 500 MB boş alan |

### 2.2. Adım Adım Kurulum

**Adım 1: Proje Dosyalarını İndirin**

```powershell
# Proje dizinine gidin
cd C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak
```

**Adım 2: Python Bağımlılıklarını Kurun**

```powershell
pip install -r requirements.txt
```

**Adım 3: Kurulumu Doğrulayın**

```powershell
python -c "import numpy; import pandas; import sklearn; import flask; print('Kurulum başarılı!')"
```

Ekranda `Kurulum başarılı!` yazısını görmeniz gerekir.

**Adım 4 (Opsiyonel): Test Paketlerini Kurun**

```powershell
pip install pytest pytest-cov
```

### 2.3. Yapılandırma

Tüm sistem ayarları iki dosyada yönetilir:

| Dosya | İçerik | Ne Zaman Değiştirilir |
|-------|--------|-----------------------|
| `config.yaml` | Model parametreleri, BMS eşikleri, API ayarları | Sistem davranışını değiştirmek istediğinizde |
| `python/utils/constants.py` | Sabit değerler, fiziksel limitler | Nadiren (batarya tipi değişirse) |

---

## 3. Hızlı Başlangıç

### 3.1. En Hızlı Yol: Demo Çalıştırma

Sistemin tüm bileşenlerini 2 dakikada görmek için:

```powershell
cd C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak
python examples\demo_end_to_end.py
```

Bu komut sırasıyla çalıştırır:

```
STAGE 1: DATA PROCESSING      ← Sentetik veri oluşturma ve temizleme
STAGE 2: FEATURE EXTRACTION    ← 113 özellik çıkarma
STAGE 3: MODEL TRAINING        ← Random Forest eğitimi
STAGE 4: SOH PREDICTION        ← Tahmin ve sınıflandırma
STAGE 5: BMS CONTROL SYSTEM    ← İzleme, dengeleme, karar
STAGE 6: WEB API INTEGRATION   ← Endpoint listesi
```

### 3.2. Web Dashboard'u Açma

Tarayıcınızda `index.html` dosyasını açın:

```powershell
start index.html
```

veya yerel sunucu ile:

```powershell
python -m http.server 8000
# Tarayıcıda: http://localhost:8000
```

### 3.3. API Sunucusunu Başlatma

```powershell
cd python
python api\battery_api.py
```

Çıktı:

```
Starting Battery Management API Server...
 * Running on http://127.0.0.1:5000
```

Doğrulama:

```powershell
curl http://localhost:5000/api/health
```

---

## 4. Web Dashboard Kullanımı

### 4.1. Sekmeler

Dashboard 6 sekmeden oluşur. Her sekmeye üst menüdeki bağlantılardan veya klavye kısayollarıyla erişilir.

| Sekme | Kısayol | İçerik |
|-------|---------|--------|
| Dashboard | Alt+1 | Genel ilerleme, metrikler, riskler |
| Zaman Çizelgesi | Alt+2 | İş paketleri zaman çubuğu |
| Kilometre Taşları | Alt+3 | 7 hedef ve tamamlanma durumu |
| İş Paketleri | Alt+4 | Detaylı aktiviteler |
| Bütçe | Alt+5 | Toplam 3.497.730 TL dağılım |
| Ekip | Alt+6 | 5 takım üyesi bilgileri |

### 4.2. Dışa Aktarma

Dashboard verilerini dışa aktarmak için:

1. **JSON Olarak:** Sayfanın alt kısmındaki "JSON Dışa Aktar" düğmesine tıklayın
2. **CSV Olarak:** "CSV Dışa Aktar" düğmesine tıklayın
3. **Yazdırma:** `Ctrl+P` kısayolunu kullanın

### 4.3. Otomatik Kaydetme

Dashboard her 60 saniyede bir durumu tarayıcının LocalStorage'ına otomatik kaydeder. Sayfayı yenilediğinizde veriler korunur.

**Manuel kaydetme:** `Ctrl+S`

---

## 5. Batarya Veri İşleme

### 5.1. Veri Yükleme

**CSV Dosyasından:**

```python
from python.data_processing.battery_data import BatteryData

battery = BatteryData('BAT_001')
battery.load_from_csv('data/raw/battery_measurements.csv')
```

**Excel Dosyasından:**

```python
battery.load_from_excel('data/raw/measurements.xlsx', sheet_name='Sheet1')
```

**Sentetik Veri Oluşturma (Test İçin):**

```python
battery = BatteryData('BAT_TEST')
battery.create_sample_data(n_samples=2000, noise_level=0.05)
```

| Parametre | Açıklama | Önerilen Değer |
|-----------|----------|---------------|
| `n_samples` | Kaç veri noktası üretilecek | 1000–10000 |
| `noise_level` | Gürültü seviyesi (0=temiz, 1=çok gürültülü) | 0.03–0.10 |

### 5.2. CSV Dosya Formatı

Veri dosyanız aşağıdaki sütunları içermelidir:

| Sütun (Zorunlu) | Birim | Örnek |
|-----------------|-------|-------|
| `voltage` | V | 3.72 |
| `current` | A | 5.1 |
| `temperature` | °C | 25.3 |
| `capacity` | mAh | 2350 |

| Sütun (Opsiyonel) | Birim | Örnek |
|-------------------|-------|-------|
| `impedance` | mΩ | 52.1 |
| `soc` | % | 65.0 |
| `soh` | % | 93.5 |
| `timestamp` | ISO 8601 | 2026-04-19T12:30:00 |

### 5.3. Veri Temizleme

```python
# Otomatik temizleme (önerilen)
battery.clean_data()

# Adım adım temizleme
battery.validate_data()          # Zorunlu sütunları kontrol et
battery.remove_duplicates()      # Tekrar eden satırları sil
battery.handle_missing_values()  # Eksik değerleri enterpolasyonla doldur
battery.remove_outliers()        # Aykırı değerleri çıkar (IQR yöntemi)
```

### 5.4. Veri Normalleştirme

```python
# Standartlaştırma (ortalama=0, std=1) — Model eğitimi için önerilir
battery.normalize_data(method='standardize')

# Min-Max ölçekleme (0-1 arasına)
battery.normalize_data(method='minmax')
```

### 5.5. İstatistikleri Görüntüleme

```python
stats = battery.get_statistics()
print(f"Ortalama gerilim: {stats['voltage'][0]:.3f} V")
print(f"Min sıcaklık:     {stats['temperature'][2]:.1f} °C")
print(f"Toplam satır:      {battery.get_info()['rows']}")
```

### 5.6. Temiz Veriyi Kaydetme

```python
battery.save_cleaned_data(filepath='data/processed/clean_data.csv', format='csv')
battery.save_cleaned_data(filepath='data/processed/clean_data.parquet', format='parquet')
```

---

## 6. SoH Modeli ile Çalışma

### 6.1. Model Seçenekleri

| Model | Hız | Doğruluk | Yorumlanabilirlik | Ne Zaman Kullanılır |
|-------|-----|----------|--------------------|--------------------|
| Random Forest | Orta | Yüksek | Yüksek | **Varsayılan — çoğu durumda önerilir** |
| Gradient Boosting | Yavaş | En yüksek | Orta | Maksimum doğruluk gerektiğinde |
| Ridge | Hızlı | Düşük | Çok yüksek | Temel referans modeli olarak |
| Linear | En hızlı | En düşük | Çok yüksek | Hızlı prototipleme |

### 6.2. Model Eğitimi

```python
from python.data_processing.battery_data import BatteryData
from python.data_processing.feature_extraction import FeatureExtractor
from python.models.soh_model import SoHModel

# 1) Veri hazırla
battery = BatteryData('BAT_001')
battery.create_sample_data(n_samples=5000)
battery.clean_data()

# 2) Özellik çıkar
extractor = FeatureExtractor(window_size=100, window_stride=10)
features = extractor.extract_all_features(battery.data)
features_norm = extractor.normalize_features(features, fit=True)

# 3) Hedef değişkeni ayır
X = features_norm.drop('target_soh', axis=1).values  # Özellikler
y = features_norm['target_soh'].values                # SOH (%)

# 4) Modeli eğit
model = SoHModel('random_forest')
metrics = model.train(X, y)

# 5) Sonuçlar
print(f"R² skoru:   {metrics['test_r2']:.4f}")
print(f"RMSE:       {metrics['test_rmse']:.4f}")
print(f"CV skoru:   {metrics['cv_mean']:.4f} ± {metrics['cv_std']:.4f}")
print(f"Eğitim süresi: {metrics['training_time']:.2f} saniye")
```

### 6.3. Tahmin Yapma

**Toplu Tahmin:**

```python
predictions = model.predict(X_test)
for i, pred in enumerate(predictions[:5]):
    print(f"Batarya {i+1}: SOH = %{pred:.1f}")
```

**Tekil Tahmin + Karar:**

```python
result = model.predict_single(features_of_one_battery)
print(f"SOH:    %{result['soh']:.1f}")
print(f"Durum:  {result['status']}")       # REUSE / RECYCLE / CRITICAL
print(f"Öneri:  {result['recommendation']}")
```

### 6.4. Modeli Kaydetme ve Yükleme

```python
# Kaydet
model.save_model('models/soh_random_forest_v1.pkl')

# Yükle (başka bir oturumda)
model2 = SoHModel('random_forest')
model2.load_model('models/soh_random_forest_v1.pkl')
predictions = model2.predict(X_new)
```

### 6.5. Özellik Önemi

Hangi özelliklerin tahmin üzerinde en çok etkisi olduğunu görmek için:

```python
importance = model.get_feature_importance(top_n=10)
for idx, row in importance.iterrows():
    print(f"  {row['feature']}: {row['importance']:.4f}")
```

---

## 7. BMS Kullanımı

### 7.1. Batarya İzleme Başlatma

```python
from python.bms.bms_controller import BMSController
import pandas as pd

# 8 hücreli batarya paketi oluştur
controller = BMSController('BAT_001', num_cells=8)
```

### 7.2. Sensör Verilerini Gönderme

```python
import numpy as np

# Hücre verileri DataFrame olarak
cells = pd.DataFrame({
    'cell_id':     list(range(8)),
    'voltage':     [3.72, 3.68, 3.71, 3.70, 3.73, 3.69, 3.71, 3.70],
    'current':     [5.0] * 8,
    'temperature': [25.1, 24.8, 25.3, 24.9, 25.0, 25.2, 24.7, 25.1],
    'soc':         [52, 51, 53, 50, 52, 51, 50, 52],
    'soh':         [96, 95, 97, 93, 96, 94, 95, 96],
    'impedance':   [48, 50, 47, 53, 49, 51, 50, 48]
})

controller.update_batteries(cells)
```

### 7.3. İzleme Döngüsü

```python
result = controller.execute_monitoring()

print(f"Bulunan uyarılar: {result['alerts_found']}")
print(f"Sistem durumu:    {result['system_state']}")

# Uyarıları incele
for alert in controller.monitor.get_alerts():
    print(f"  [{alert.severity}] {alert.message}")
```

### 7.4. Şarj/Deşarj Kontrolü

```python
# Şarj izni var mı?
if controller.allow_charging():
    result = controller.process_charge_discharge(
        current_ma=5000,    # 5A şarj
        duration_sec=3600   # 1 saat
    )
    print(f"Durum: {result['status']}")
    if result['status'] == 'allowed':
        print(f"Transfer edilen enerji: {result['energy_kwh']:.3f} kWh")
else:
    print("Şarj izni yok! Sistem durumunu kontrol edin.")
```

### 7.5. Karar Alma

```python
# SOH'ı tahmin et
soh_info = controller.estimate_soh()
soh_value = soh_info['soh']

# Karar al
decision = controller.make_decision(soh_value)
print(f"SOH:         %{decision['soh']:.1f}")
print(f"Karar:       {decision['decision']}")
print(f"Gerekçe:     {decision['reason']}")
print(f"Yönlendirme: {decision['destination']}")
```

**Çıktı örneği:**

```
SOH:         93.2%
Karar:       reuse
Gerekçe:     Second-life applications (stationary storage, EVs)
Yönlendirme: SECOND_LIFE_PROGRAM
```

### 7.6. Sistem Raporu

```python
report = controller.generate_report()

print(f"Pack Gerilimi:  {report['health_metrics']['pack_voltage']:.2f} V")
print(f"Pack Akımı:     {report['health_metrics']['pack_current']:.2f} A")
print(f"Pack Sıcaklığı: {report['health_metrics']['pack_temperature']:.1f} °C")
print(f"Pack SOH:       %{report['health_metrics']['pack_soh']:.1f}")
print(f"V. Dengesizlik: {report['health_metrics']['voltage_imbalance']*1000:.1f} mV")
print(f"Toplam Uyarı:   {report['alerts_summary']['total']}")
print(f"  Kritik:       {report['alerts_summary']['critical']}")
print(f"  Uyarı:        {report['alerts_summary']['warning']}")
```

---

## 8. API Sunucusu

### 8.1. Sunucuyu Başlatma

```powershell
cd C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak\python
python api\battery_api.py
```

Varsayılan olarak `http://127.0.0.1:5000` adresinde çalışır.

### 8.2. Temel Endpointler

| İşlem | Komut |
|-------|-------|
| Sağlık kontrolü | `GET /api/health` |
| Batarya başlat | `POST /api/battery/init` |
| Veri güncelle | `POST /api/battery/<id>/update` |
| İzleme çalıştır | `POST /api/battery/<id>/monitor` |
| SOH tahmin et | `POST /api/battery/<id>/predict_soh` |
| Karar ver | `POST /api/battery/<id>/decide` |
| Durum raporu | `GET /api/battery/<id>/status` |

Detaylar için **API Referans Kılavuzu** dokümanına bakınız.

### 8.3. Hızlı Test

```powershell
# Sunucu çalışıyor mu?
curl http://localhost:5000/api/health

# Batarya başlat
curl -X POST http://localhost:5000/api/battery/init -H "Content-Type: application/json" -d "{\"battery_id\":\"BAT_001\",\"num_cells\":8}"
```

---

## 9. Testleri Çalıştırma

### 9.1. Tüm Testler

```powershell
cd C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak
pytest tests\test_suite.py -v
```

### 9.2. Belirli Test Sınıfı

```powershell
# Sadece model testleri
pytest tests\test_suite.py::TestSoHModel -v

# Sadece BMS testleri
pytest tests\test_suite.py::TestBMSController -v
```

### 9.3. Kapsam Raporu

```powershell
pytest tests\test_suite.py --cov=python --cov-report=html
# Rapor: htmlcov/index.html
```

### 9.4. Test Kategorileri

| Sınıf | Test Sayısı | Kapsam |
|-------|------------|--------|
| `TestBatteryData` | 5 | Veri yükleme, temizleme, normalleştirme |
| `TestFeatureExtraction` | 3 | Pencere oluşturma, özellik çıkarma |
| `TestSoHModel` | 4 | Eğitim, tahmin, kaydetme/yükleme |
| `TestBatteryMonitor` | 4 | İzleme, uyarı, pack metrikleri |
| `TestCellBalancer` | 2 | Dengeleme hesaplama |
| `TestBMSController` | 6 | Kontrolcü yaşam döngüsü |
| `TestConstants` | 3 | Sabit geçerlilik |

---

## 10. Sık Sorulan Sorular

### S: Kendi batarya verimi nasıl kullanırım?

CSV dosyanızı hazırlayın (zorunlu sütunlar: `voltage`, `current`, `temperature`, `capacity`), ardından:

```python
battery = BatteryData('MY_BAT')
battery.load_from_csv('path/to/my_data.csv')
battery.clean_data()
```

### S: Farklı bir batarya kimyası (LFP, NCA) kullanıyorum. Ne yapmalıyım?

`python/utils/constants.py` dosyasındaki aşağıdaki sabitleri güncelleyin:

```python
NOMINAL_VOLTAGE = 3.2    # LFP için
MIN_VOLTAGE = 2.0        # LFP için
MAX_VOLTAGE = 3.65       # LFP için
```

Ayrıca BMS eşiklerini de buna göre ayarlayın.

### S: Model yeni verilerle yeniden eğitilebilir mi?

Evet. Yeni verilerle `model.train(X_new, y_new)` çağrısı yaparak modeli yeniden eğitebilirsiniz. Eski model `save_model()` ile kaydedilmişse her zaman yüklenebilir.

### S: 8'den fazla hücre destekleniyor mu?

Evet. `BMSController('BAT_001', num_cells=16)` şeklinde istediğiniz hücre sayısını belirleyebilirsiniz.

### S: API sunucusunu farklı bir portta çalıştırabilir miyim?

`config.yaml` dosyasında:

```yaml
api:
  port: 8080
```

Veya doğrudan:

```python
api = BatteryAPIWrapper()
api.run(port=8080)
```

### S: Gerçek zamanlı izleme için ne kadar sık veri göndermem gerekir?

Varsayılan izleme aralığı 1 saniyedir (`BMS_MONITOR_INTERVAL = 1.0`). Daha seyrek izleme yapılabilir ancak güvenlik açısından 5 saniyeyi aşmamanız önerilir.

---

## 11. Sorun Giderme

### 11.1. ImportError: No module named 'utils'

**Neden:** Python modül yolunda proje dizini yok.

**Çözüm:**

```powershell
# Ortam değişkeni ekle
$env:PYTHONPATH = "C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak"

# Veya kodu güncelle
import sys
sys.path.insert(0, r'C:\Users\emrul\OneDrive\Masaüstü\Kobinerji_Tubitak')
```

### 11.2. Bağımlılık Hatası

```powershell
pip install --upgrade -r requirements.txt
```

### 11.3. API Bağlantı Reddedildi

**Neden:** Sunucu çalışmıyor.

**Çözüm:**

```powershell
cd python
python api\battery_api.py
```

### 11.4. Model Doğruluğu Düşük

**Olası Nedenler:**

1. Yetersiz eğitim verisi → `n_samples` değerini artırın (en az 1000)
2. Gürültülü veri → `noise_level` değerini düşürün veya temizleme parametrelerini ayarlayın
3. Yanlış model seçimi → `gradient_boosting` deneyebilirsiniz

### 11.5. Bellek Yetersizliği

Büyük veri setlerinde:

```python
# Pencere boyutunu küçültün
extractor = FeatureExtractor(window_size=50, window_stride=25)

# Veri alt kümesi kullanın
battery.data = battery.data.sample(frac=0.5, random_state=42)
```

### 11.6. Günlük Dosyaları Nerede?

```
python/logs/project_YYYYMMDD.log
```

Dosya boyutu 10 MB'ı aştığında otomatik dönüşüm yapılır (maksimum 5 yedek).

---

## Ek: Hızlı Komut Referansı

| İşlem | Komut |
|-------|-------|
| Demo çalıştır | `python examples\demo_end_to_end.py` |
| API başlat | `python python\api\battery_api.py` |
| Dashboard aç | `start index.html` |
| Testleri çalıştır | `pytest tests\test_suite.py -v` |
| Bağımlılık kur | `pip install -r requirements.txt` |
| Kapsam raporu | `pytest tests\test_suite.py --cov=python --cov-report=html` |

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — Kullanıcı El Kitabı v1.0.0*
