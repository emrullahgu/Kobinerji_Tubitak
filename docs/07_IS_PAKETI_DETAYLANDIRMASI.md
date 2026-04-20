# TÜBİTAK 7260634 — İş Paketi Detaylandırması

**Proje:** Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi  
**Firma:** KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.  
**Süre:** 18 Ay (01.03.2026 – 01.09.2027)  
**Toplam Bütçe:** 3.497.730 TL  

---

## Genel Bakış

```
İP1 ████████████░░░░░░░░░░░░░░░░░░░░░░░░  Mar–Haz 2026  (65%)
İP2 ░░░░░░░░░░░░████████░░░░░░░░░░░░░░░░  Haz–Eyl 2026  (30%)
İP3 ░░░░░░░░░░░░░░░░░░░░████████████░░░░  Eyl 2026–Oca 2027
İP4 ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██████  Oca–Mar 2027
İP5 ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░████  Mar–May 2027
İP6 ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██  May–Tem 2027
İP7 ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░█  Tem–Eyl 2027
    ├─────┼─────┼─────┼─────┼─────┼─────┤
    Mar   Haz   Eyl   Oca   Mar   Haz   Eyl
    2026  2026  2026  2027  2027  2027  2027
```

---

## İP1 — Batarya Veri Toplama Altyapısı

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.03.2026 |
| **Bitiş** | 01.06.2026 |
| **Süre** | 3 Ay |
| **Durum** | Tamamlanma aşamasında (%65) |
| **Sorumlu** | Görkem Tanır (Test Altyapısı), Emrullah Günay (Sistem) |
| **Bağımlılık** | Yok (başlangıç paketi) |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 1.1 | Test ekipmanları tedarik ve kurulum | 4 hafta | G. Tanır | Kurulu test istasyonu | ✅ Tamamlandı |
| 1.2 | Batarya test protokolü oluşturma | 2 hafta | G. Tanır, A. Helva | Protokol dokümanı | ✅ Tamamlandı |
| 1.3 | Veri toplama yazılımı geliştirme | 3 hafta | E. Günay | Veri toplama modülü | ✅ Tamamlandı |
| 1.4 | Minimum 50 hücre testi | 4 hafta | G. Tanır | Ham veri seti | ⏳ Devam ediyor |
| 1.5 | Veri kalite doğrulama | 2 hafta | A. Helva | Kalite raporu | ⏳ Devam ediyor |
| 1.6 | Veri tabanı tasarımı ve kurulumu | 2 hafta | E. Günay | Çalışır DB | ✅ Tamamlandı |

### Kilometre Taşı Kriterleri

- [ ] 50+ hücre test edilmiş
- [x] Veri toplama altyapısı çalışır durumda
- [ ] Veri kalitesi doğrulanmış (≤%5 kayıp)
- [x] Test protokolü onaylanmış

### Kaynak Dağılımı

| Kaynak Türü | Planlanan | Harcanan |
|-------------|-----------|----------|
| Personel (adam-ay) | 6 | 4.2 |
| Ekipman | 450.000 TL | 380.000 TL |
| Malzeme (batarya hücreleri) | 80.000 TL | 65.000 TL |

### Riskler

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| Batarya tedarik gecikmesi | Orta | Yüksek | Alternatif tedarikçi listesi |
| Veri kalitesi yetersizliği | Düşük | Yüksek | Otomatik kalite kontrol |
| Ekipman arızası | Düşük | Orta | Yedek ekipman |

---

## İP2 — Veri Ön İşleme

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.06.2026 |
| **Bitiş** | 01.09.2026 |
| **Süre** | 3 Ay |
| **Durum** | Devam ediyor (%30) |
| **Sorumlu** | Alperen Helva (Model), Emrullah Günay (Sistem) |
| **Bağımlılık** | İP1 (ham veri seti) |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 2.1 | Veri temizleme pipeline geliştirme | 3 hafta | A. Helva | `BatteryData` sınıfı | ✅ Tamamlandı |
| 2.2 | Eksik veri interpolasyonu | 2 hafta | A. Helva | Temiz veri seti | ✅ Tamamlandı |
| 2.3 | Aykırı değer tespiti (IQR/Z-score) | 2 hafta | E. Günay | Filtrelenmiş veri | ⏳ Devam ediyor |
| 2.4 | Özellik mühendisliği (113 özellik) | 4 hafta | A. Helva | `FeatureExtractor` | ⏳ Devam ediyor |
| 2.5 | İstatistiksel özellikler (20/kanal) | 2 hafta | A. Helva | Özellik matrisi | ⏳ Planlandı |
| 2.6 | Frekans özellikleri (FFT tabanlı) | 2 hafta | A. Helva | FFT özellikleri | ⏳ Planlandı |
| 2.7 | Empedans özellikleri | 1 hafta | E. Günay | DCR/ACR özellikleri | ⏳ Planlandı |
| 2.8 | Train/test veri ayrımı (%80/%20) | 1 hafta | A. Helva | Bölünmüş veri setleri | ⏳ Planlandı |

### Kilometre Taşı Kriterleri

- [ ] 100+ özellik çıkarılmış
- [ ] Train/test ayrımı yapılmış
- [ ] Veri normalizasyonu tamamlanmış
- [ ] Özellik korelasyon analizi raporlanmış

### Kaynak Dağılımı

| Kaynak Türü | Planlanan | Harcanan |
|-------------|-----------|----------|
| Personel (adam-ay) | 4 | 1.2 |
| Yazılım lisansları | 50.000 TL | 15.000 TL |
| Bulut hesaplama | 30.000 TL | 5.000 TL |

### Riskler

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| Düşük kaliteli ham veri | Orta | Yüksek | Çok aşamalı temizleme |
| Yetersiz özellik sayısı | Düşük | Orta | Wavelet/zaman alanı ekleme |
| Boyut laneti (çok özellik) | Düşük | Orta | PCA/feature selection |

---

## İP3 — Yapay Zeka SoH Modeli

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.09.2026 |
| **Bitiş** | 01.01.2027 |
| **Süre** | 4 Ay |
| **Durum** | Beklemede (%0) |
| **Sorumlu** | Alperen Helva (Baş Araştırmacı), E. Günay (Destek) |
| **Bağımlılık** | İP2 (işlenmiş veri seti) |
| **Hedef** | %90 doğruluk, R² ≥ 0.85 |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 3.1 | AST-LSTM model geliştirme (Attention-based) | 4 hafta | A. Helva | AST-LSTM model | ⏳ |
| 3.2 | LSTM / GRU model geliştirme | 3 hafta | A. Helva | LSTM, GRU modeller | ⏳ |
| 3.3 | SimpleRNN kıyaslama modeli | 1 hafta | A. Helva | RNN referans model | ⏳ |
| 3.4 | Transfer Öğrenme (NMC→LFP/NCA) | 2 hafta | E. Günay | TL-LSTM model | ⏳ |
| 3.5 | 5-katlı çapraz doğrulama | 2 hafta | A. Helva | CV metrikleri | ⏳ |
| 3.6 | Hiperparametre optimizasyonu | 3 hafta | A. Helva | Optimize parametreler | ⏳ |
| 3.7 | Model karşılaştırma ve seçim | 1 hafta | A. Helva, C. Bülbül | Karşılaştırma raporu | ⏳ |
| 3.8 | Model kaydetme/yükleme sistemi | 1 hafta | E. Günay | Serileştirme modülü | ⏳ |

### Başarı Metrikleri

| Metrik | Hedef | Kabul | Red |
|--------|-------|-------|-----|
| Doğruluk | ≥%90 | ≥%85 | <%80 |
| R² Skoru | ≥0.85 | ≥0.80 | <0.75 |
| RMSE | <5.0 | <7.0 | >10.0 |
| Tahmin süresi | <5 dk/batarya | <10 dk | >15 dk |

### Kaynak Dağılımı

| Kaynak Türü | Planlanan |
|-------------|-----------|
| Personel (adam-ay) | 6 |
| GPU hesaplama | 80.000 TL |
| Yazılım/kütüphane | 20.000 TL |

---

## İP4 — Karar Destek Sistemi

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.01.2027 |
| **Bitiş** | 01.03.2027 |
| **Süre** | 2 Ay |
| **Durum** | Beklemede (%0) |
| **Sorumlu** | Emrullah Günay (Sistem), A. Helva (Model Entegrasyonu) |
| **Bağımlılık** | İP3 (eğitilmiş SoH modeli) |
| **Hedef** | %85 sınıflandırma doğruluğu |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 4.1 | BMS izleme modülü (`BatteryMonitor`) | 2 hafta | E. Günay | İzleme sistemi | ⏳ |
| 4.2 | Hücre dengeleme algoritması (`CellBalancer`) | 2 hafta | E. Günay | Dengeleme modülü | ⏳ |
| 4.3 | Karar motoru (`BMSController`) | 3 hafta | E. Günay, A. Helva | Karar sistemi | ⏳ |
| 4.4 | REUSE/RECYCLE/CRITICAL sınıflandırma | 1 hafta | A. Helva | Sınıflandırma kuralları | ⏳ |
| 4.5 | REST API geliştirme (13 endpoint) | 2 hafta | E. Günay | Flask API | ⏳ |
| 4.6 | Gerçek zamanlı izleme | 1 hafta | E. Günay | Anlık izleme | ⏳ |

### Karar Matrisi

| SoH Aralığı | Karar | Yönlendirme | Güven Eşiği |
|-------------|-------|-------------|-------------|
| ≥%85 | REUSE | İkinci ömür kullanımı | ≥%90 |
| %70–%85 | RECYCLE | Mekanik geri kazanım | ≥%85 |
| <%70 | CRITICAL | Tehlikeli madde protokolü | — |

### Kaynak Dağılımı

| Kaynak Türü | Planlanan |
|-------------|-----------|
| Personel (adam-ay) | 4 |
| Sunucu altyapısı | 40.000 TL |

---

## İP5 — Mekanik Geri Kazanım

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.03.2027 |
| **Bitiş** | 01.05.2027 |
| **Süre** | 2 Ay |
| **Durum** | Beklemede (%0) |
| **Sorumlu** | İbrahim Çağdaş (Mekanik), G. Tanır (Test) |
| **Bağımlılık** | İP4 (sınıflandırılmış bataryalar) |
| **Hedef** | %10 verimlilik artışı |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 5.1 | Geri kazanım hattı tasarımı | 2 hafta | İ. Çağdaş | Teknik çizimler | ⏳ |
| 5.2 | Söküm protokolü geliştirme | 2 hafta | İ. Çağdaş, G. Tanır | Söküm prosedürü | ⏳ |
| 5.3 | Mekanik parçalama sistemi | 3 hafta | İ. Çağdaş | Çalışır sistem | ⏳ |
| 5.4 | Black mass üretimi | 2 hafta | İ. Çağdaş | 5 ton black mass | ⏳ |
| 5.5 | Verimlilik ölçüm ve optimizasyon | 1 hafta | G. Tanır, İ. Çağdaş | Verimlilik raporu | ⏳ |

### Kaynak Dağılımı

| Kaynak Türü | Planlanan |
|-------------|-----------|
| Personel (adam-ay) | 4 |
| Mekanik ekipman | 350.000 TL |
| Hammadde | 75.000 TL |

---

## İP6 — Sistem Entegrasyonu

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.05.2027 |
| **Bitiş** | 01.07.2027 |
| **Süre** | 2 Ay |
| **Durum** | Beklemede (%0) |
| **Sorumlu** | C. Bülbül (Koordinasyon), E. Günay (Teknik) |
| **Bağımlılık** | İP3, İP4, İP5 |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 6.1 | Alt sistem entegrasyonu | 3 hafta | E. Günay | Entegre sistem | ⏳ |
| 6.2 | Uçtan uca test senaryoları | 2 hafta | G. Tanır | Test raporu | ⏳ |
| 6.3 | Hata oranı doğrulama (<%5) | 1 hafta | A. Helva | Doğrulama raporu | ⏳ |
| 6.4 | 10+ batarya tipi desteği | 2 hafta | E. Günay, A. Helva | Genişletilmiş sistem | ⏳ |
| 6.5 | Performans optimizasyonu | 1 hafta | A. Helva | Optimize sistem | ⏳ |

### Kabul Kriterleri

| Kriter | Hedef |
|--------|-------|
| Uçtan uca test başarısı | ≥%95 |
| Hata oranı | <%5 |
| Desteklenen batarya tipi | ≥10 |
| Yanıt süresi (API) | <500ms |

---

## İP7 — Nihai Optimizasyon

| Özellik | Değer |
|---------|-------|
| **Başlangıç** | 01.07.2027 |
| **Bitiş** | 01.09.2027 |
| **Süre** | 2 Ay |
| **Durum** | Beklemede (%0) |
| **Sorumlu** | C. Bülbül (Doküman), Tüm Ekip |
| **Bağımlılık** | İP6 |

### Faaliyet Kırılımı

| No | Faaliyet | Süre | Sorumlu | Çıktı | Durum |
|----|----------|------|---------|-------|-------|
| 7.1 | Teknik dokümantasyon tamamlama | 2 hafta | E. Günay | Teknik dokümanlar | ⏳ |
| 7.2 | Kullanıcı el kitabı | 1 hafta | E. Günay | El kitabı | ⏳ |
| 7.3 | Ticarileştirme planı | 2 hafta | C. Bülbül | İş planı | ⏳ |
| 7.4 | Patent başvurusu hazırlığı | 3 hafta | C. Bülbül, A. Helva | Patent taslağı | ⏳ |
| 7.5 | Nihai proje raporu | 2 hafta | Tüm Ekip | TÜBİTAK raporu | ⏳ |
| 7.6 | Sunum ve demo hazırlığı | 1 hafta | Tüm Ekip | Demo paketi | ⏳ |

### Ticarileştirme Hedefleri

| Yıl | Hedef Gelir | Kanal |
|-----|------------|-------|
| 1. Yıl | 2.000.000 TL | Yazılım lisansı |
| 3. Yıl | 10.000.000 TL | Lisans + analiz hizmeti |
| 5. Yıl | 25.000.000 TL | Lisans + hizmet + black mass |

---

## Bağımlılık Matrisi

```
     İP1  İP2  İP3  İP4  İP5  İP6  İP7
İP1   —    →    ·    ·    ·    ·    ·
İP2   ←    —    →    ·    ·    ·    ·
İP3   ·    ←    —    →    ·    →    ·
İP4   ·    ·    ←    —    →    →    ·
İP5   ·    ·    ·    ←    —    →    ·
İP6   ·    ·    ←    ←    ←    —    →
İP7   ·    ·    ·    ·    ·    ←    —

→ = çıktı verir     ← = girdi alır     · = bağımsız
```

## Kritik Yol

```
İP1 → İP2 → İP3 → İP4 → İP5 → İP6 → İP7
 3 ay   3 ay   4 ay   2 ay   2 ay   2 ay   2 ay  = 18 ay
```

Projedeki tüm iş paketleri seri bağımlılık zincirinde olduğundan, herhangi bir gecikme proje bitiş tarihini doğrudan etkiler. **İP3 (4 ay)** en uzun paket olarak kritik yolun darboğazıdır.

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — İş Paketi Detaylandırması v1.0.0*
