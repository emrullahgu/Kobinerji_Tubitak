# TÜBİTAK 7260634 — Kalite Kontrol Planı

**Proje:** Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi  
**Firma:** KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.  
**Sürüm:** 1.0.0  
**Tarih:** 19 Nisan 2026  

---

## İçindekiler

1. [Kalite Politikası](#1-kalite-politikası)
2. [Kalite Organizasyonu](#2-kalite-organizasyonu)
3. [Kalite Kontrol Noktaları](#3-kalite-kontrol-noktaları)
4. [Veri Kalite Yönetimi](#4-veri-kalite-yönetimi)
5. [Yazılım Kalite Güvencesi](#5-yazılım-kalite-güvencesi)
6. [Model Kalite Yönetimi](#6-model-kalite-yönetimi)
7. [Donanım ve Mekanik Kalite](#7-donanım-ve-mekanik-kalite)
8. [Süreç Kalite Ölçümleri](#8-süreç-kalite-ölçümleri)
9. [Uygunsuzluk Yönetimi](#9-uygunsuzluk-yönetimi)
10. [Gözden Geçirme ve Denetim](#10-gözden-geçirme-ve-denetim)

---

## 1. Kalite Politikası

### 1.1. Vizyon

Tüm proje çıktılarında TÜBİTAK 1507 programı gereksinimlerini karşılayan, tekrarlanabilir ve ölçülebilir kalite standartları sağlamak.

### 1.2. Temel İlkeler

| İlke | Açıklama |
|------|----------|
| **Ölçülebilirlik** | Her çıktı nicel metriklerle değerlendirilir |
| **Tekrarlanabilirlik** | Tüm deneyler ve testler tekrarlanabilir olmalıdır |
| **İzlenebilirlik** | Her veri noktası kaynağına kadar izlenebilir |
| **Sürekli İyileştirme** | Dönemsel gözden geçirme ile kalite artırılır |
| **Erken Tespit** | Sorunlar mümkün olan en erken aşamada tespit edilir |

### 1.3. Uygulanacak Standartlar

| Standart | Kapsam |
|----------|--------|
| ISO 9001:2015 | Genel kalite yönetim sistemi |
| IEC 62660 | Lityum-iyon batarya test prosedürleri |
| ISO/IEC 25010 | Yazılım kalite modeli |
| IEEE 730 | Yazılım kalite güvence planı |

---

## 2. Kalite Organizasyonu

### 2.1. Sorumluluk Matrisi (RACI)

| Faaliyet | C. Bülbül | A. Helva | E. Günay | G. Tanır | İ. Çağdaş |
|----------|-----------|----------|----------|----------|-----------|
| Kalite planı onayı | **A** | C | C | I | I |
| Veri kalite kontrolü | I | **R** | R | C | — |
| Yazılım test | I | C | **R** | — | — |
| Model doğrulama | C | **R** | R | — | — |
| Donanım kalitesi | I | — | — | **R** | R |
| Mekanik sistem kalitesi | I | — | — | C | **R** |
| Dönemsel kalite raporu | **A** | R | R | R | R |

> **R:** Sorumlu, **A:** Onaylayıcı, **C:** Danışılan, **I:** Bilgilendirilen

### 2.2. Kalite Gözden Geçirme Takvimi

| Periyot | Faaliyet | Katılımcılar | Çıktı |
|---------|----------|-------------|-------|
| Haftalık | Durum toplantısı | Tüm ekip | Haftalık rapor |
| 2-Haftalık | Teknik gözden geçirme | Teknik ekip | İnceleme notu |
| Aylık | Kalite durum raporu | Tüm ekip | Kalite metrikleri |
| 3-Aylık | TÜBİTAK dönem raporu | C. Bülbül | Dönem raporu |
| Kilometre taşı | Gate review | Tüm ekip + danışman | Go/No-Go kararı |

---

## 3. Kalite Kontrol Noktaları

Her iş paketinin tamamlanmasında bir **gate review** (kapı kontrolü) uygulanır:

### 3.1. Gate Review Süreci

```
İş Paketi Çıktısı
       │
       ▼
┌──────────────┐     Başarısız     ┌─────────────────┐
│ Teknik       │────────────────→ │ Düzeltici        │
│ İnceleme     │                  │ Faaliyet Planı   │
└──────┬───────┘                  └────────┬──────────┘
       │ Başarılı                          │
       ▼                                   │
┌──────────────┐                           │
│ Kalite       │◄──────────────────────────┘
│ Metrikleri   │
│ Kontrolü     │
└──────┬───────┘
       │ Geçti
       ▼
┌──────────────┐
│ Onay ve      │
│ Sonraki İP   │
│ Başlatma     │
└──────────────┘
```

### 3.2. İş Paketi Giriş/Çıkış Kriterleri

| İP | Giriş Kriteri | Çıkış Kriteri |
|----|---------------|---------------|
| İP1 | Bütçe onayı, ekipman siparişi | 50+ hücre, veri kalitesi ≥%95 |
| İP2 | Ham veri seti teslimi | 100+ özellik, ≤%1 eksik veri |
| İP3 | İşlenmiş veri seti | R² ≥ 0.85, doğruluk ≥%90 |
| İP4 | Eğitilmiş model | Sınıflandırma ≥%85, API çalışır |
| İP5 | Sınıflandırılmış bataryalar | %10 verimlilik artışı, 5 ton black mass |
| İP6 | Tüm alt sistemler hazır | E2E test ≥%95, hata <%5 |
| İP7 | Entegre sistem | Tüm dokümanlar, ticarileştirme planı |

---

## 4. Veri Kalite Yönetimi

### 4.1. Veri Kalite Boyutları

| Boyut | Tanım | Metrik | Hedef |
|-------|-------|--------|-------|
| **Tamlık** | Eksik veri oranı | `missing_rate = NaN / total` | ≤%5 |
| **Doğruluk** | Fiziksel sınırlar içinde | `valid_range_rate` | ≥%99 |
| **Tutarlılık** | Tekrarlanan testler arasında fark | `std / mean` (CV) | ≤%10 |
| **Güncellik** | Veri toplama zamanı | `max_age_days` | ≤30 gün |
| **Benzersizlik** | Tekrarlanan kayıt oranı | `duplicate_rate` | ≤%1 |

### 4.2. Otomatik Veri Doğrulama Kuralları

| Kural | Kontrol | Aksiyon |
|-------|---------|--------|
| Gerilim aralığı | 2.5V ≤ V ≤ 4.2V | Aralık dışı → uyarı, 2.0V veya 4.5V aşımı → red |
| Akım aralığı | -20A ≤ I ≤ 20A | Aşım → kritik uyarı |
| Sıcaklık aralığı | 0°C ≤ T ≤ 60°C | 80°C üstü → acil durdurma |
| Zaman sürekliliği | Δt ≤ 2 × normal | Boşluk → interpolasyon veya red |
| Empedans | 10mΩ ≤ Z ≤ 500mΩ | Aralık dışı → aykırı değer işaretleme |

### 4.3. Veri Temizleme Kalite Raporu

Her veri temizleme işlemi sonrası otomatik rapor:

```
┌─────────────────────────────────────┐
│ VERİ TEMİZLEME KALİTE RAPORU       │
├─────────────────────────────────────┤
│ Batarya ID:     BAT_001             │
│ Ham kayıt:      10,000              │
│ Temiz kayıt:    9,850               │
│ Silinen kayıt:  150 (%1.5)          │
│ Interpolasyon:  45 hücre            │
│ Aykırı değer:   87 kayıt           │
│ Duplicate:      18 kayıt            │
│ ─────────────────────────────────── │
│ Kalite skoru:   98.5%  ✅ KABUL    │
└─────────────────────────────────────┘
```

---

## 5. Yazılım Kalite Güvencesi

### 5.1. Kod Kalite Metrikleri

| Metrik | Araç | Hedef | Kabul |
|--------|------|-------|-------|
| Test kapsamı | pytest-cov | ≥%90 | ≥%80 |
| Birim test başarı oranı | pytest | %100 | ≥%95 |
| Kod karmaşıklığı (CC) | radon | ≤10 | ≤15 |
| Kod tekrarı | pylint | ≤%5 | ≤%10 |
| Dokümantasyon kapsamı | pydocstyle | ≥%80 | ≥%60 |
| Güvenlik açığı | bandit | 0 kritik | 0 kritik |

### 5.2. Kod İnceleme Kontrol Listesi

Her merge/commit öncesi kontrol:

- [ ] Birim testleri geçiyor mu?
- [ ] Yeni kod için test yazıldı mı?
- [ ] Fonksiyon ve parametre isimleri açıklayıcı mı?
- [ ] Hata yönetimi uygun mu?
- [ ] Güvenlik açığı var mı? (girdi doğrulama, vb.)
- [ ] Performans sorunu yaratabilir mi?
- [ ] constants.py kullanılıyor mu? (sabit gömülmemiş mi?)
- [ ] Logger kullanılıyor mu?

### 5.3. API Kalite Kontrolleri

| Kontrol | Metrik | Hedef |
|---------|--------|-------|
| Yanıt süresi | Ortalama latency | <500ms |
| Hata oranı | 5xx / toplam istek | <%1 |
| Erişilebilirlik | Uptime | ≥%99 |
| JSON uyumu | Schema doğrulama | %100 |
| CORS yapılandırma | Güvenlik taraması | Geçti |

---

## 6. Model Kalite Yönetimi

### 6.1. Model Geliştirme Yaşam Döngüsü Kontrolleri

```
Veri Hazırlama → Model Eğitim → Model Doğrulama → Model Onay → Dağıtım
     │                │               │               │           │
     ▼                ▼               ▼               ▼           ▼
 Veri kalite      Eğitim          5-katlı CV      Gate         A/B test
 raporu           metrikleri      sonuçları       review       sonuçları
```

### 6.2. Model Performans Kriterleri

| Algoritma | R² Hedef | RMSE Hedef | CV std Hedef | Kabul |
|-----------|----------|-----------|-------------|-------|
| Random Forest | ≥0.90 | ≤4.0 | ≤0.03 | ≥0.85 R² |
| Gradient Boosting | ≥0.92 | ≤3.5 | ≤0.03 | ≥0.87 R² |
| Neural Network | ≥0.93 | ≤3.0 | ≤0.05 | ≥0.88 R² |
| Ridge Regression | ≥0.80 | ≤6.0 | ≤0.02 | referans |

### 6.3. Aşırı Öğrenme Kontrolü

| Kontrol | Formül | Eşik |
|---------|--------|------|
| Train-Test farkı | `R²_train - R²_test` | ≤0.05 |
| CV varyansı | `std(CV_scores)` | ≤0.03 |
| Öğrenme eğrisi | Yakınsama analizi | Plato kontrolü |

### 6.4. Model Versiyon Yönetimi

| Alan | Kural |
|------|-------|
| Dosya adlandırma | `{algoritma}_{tarih}_{versiyon}.pkl` |
| Metrik kaydı | Her eğitimde JSON metrik raporu |
| Geri dönüş | Önceki en iyi modele geri dönüş imkânı |
| Karşılaştırma | Yeni model ≥ mevcut model performansı gerekir |

---

## 7. Donanım ve Mekanik Kalite

### 7.1. Test Ekipmanı Kalibrasyon Takvimi

| Ekipman | Kalibrasyon Periyodu | Sorumlu | Doğruluk |
|---------|---------------------|---------|----------|
| Voltmetre | 6 ayda bir | G. Tanır | ±0.01V |
| Amperemetre | 6 ayda bir | G. Tanır | ±0.1A |
| Termokupl | 12 ayda bir | G. Tanır | ±0.5°C |
| Empedans analizör | 6 ayda bir | G. Tanır | ±1mΩ |
| Yük hücresi (mekanik) | 12 ayda bir | İ. Çağdaş | ±0.1kg |

### 7.2. Batarya Test Kalite Kontrolleri

| Kontrol | Sıklık | Kriter |
|---------|--------|--------|
| Şarj/deşarj döngüsü tutarlılığı | Her test | CV ≤%5 |
| Sıcaklık kontrol doğruluğu | Her test | ±2°C |
| Gerilim ölçüm sapması | Günlük | ±10mV |
| Referans hücre doğrulaması | Haftalık | Bilinen SoH ±%2 |

### 7.3. Mekanik Geri Kazanım Kalitesi (İP5)

| Ölçüm | Metrik | Hedef |
|--------|--------|-------|
| Black mass saflığı | XRF analizi | ≥%95 |
| Geri kazanım oranı | Çıktı/girdi kütle | ≥%80 |
| Çevresel emisyon | Toz/gaz ölçümü | Yasal limit altı |
| İşçi güvenliği | Olay sayısı | 0 |

---

## 8. Süreç Kalite Ölçümleri

### 8.1. KPI (Anahtar Performans Göstergeleri)

| KPI | Hesaplama | Hedef | Ölçüm Periyodu |
|-----|-----------|-------|-----------------|
| Proje ilerleme sapması | `(Planlanan - Gerçekleşen) / Planlanan` | ≤%10 | Aylık |
| Bütçe sapması | `(Planlanan - Harcanan) / Planlanan` | ≤%15 | 3 aylık |
| Test başarı oranı | `Geçen / Toplam` | ≥%95 | Her test |
| Hata çözüm süresi | `Ortalama gün` | ≤5 gün | Haftalık |
| Doküman güncelliği | `Güncel / Toplam` | %100 | Aylık |
| Milestone zamanında teslim | `Zamanında / Toplam` | ≥%85 | Dönemlik |

### 8.2. Kalite Gösterge Tablosu

```
╔══════════════════════════════════════════════╗
║          KALİTE GÖSTERGE TABLOSU             ║
╠══════════════════════════════════════════════╣
║                                              ║
║  İlerleme Sapması:    ■■■■■■■░░░  8%   ✅   ║
║  Bütçe Sapması:       ■■■■■░░░░░  12%  ✅   ║
║  Test Başarı:         ■■■■■■■■■░  91%  ⚠️   ║
║  Hata Çözüm:          3.2 gün          ✅   ║
║  Doküman Güncelliği:  ■■■■■■■■■■ 100%  ✅   ║
║  Milestone Zamanında: ■■■■■■■■░░  85%  ✅   ║
║                                              ║
║  Genel Kalite Skoru:  92/100   ✅ İYİ        ║
╚══════════════════════════════════════════════╝
```

---

## 9. Uygunsuzluk Yönetimi

### 9.1. Uygunsuzluk Sınıflandırma

| Seviye | Tanım | Aksiyon | Süre |
|--------|-------|---------|------|
| **Kritik** | Güvenlik riski veya proje durdurma | Acil düzeltme, proje yürütücüsü onayı | 24 saat |
| **Majör** | Kalite hedefi kaçırılmış | Düzeltici faaliyet planı | 5 iş günü |
| **Minör** | Küçük sapma, trend riski | İyileştirme kaydı | 10 iş günü |
| **Gözlem** | Potansiyel iyileştirme alanı | Sonraki gözden geçirmede değerlendir | — |

### 9.2. Düzeltici Faaliyet Süreci

```
Tespit → Kayıt → Kök Neden Analizi → Düzeltici Faaliyet → Doğrulama → Kapanış
  │        │           │                    │                │          │
  ▼        ▼           ▼                    ▼                ▼          ▼
 QC log   NCR form  5 Neden/           Faaliyet          Test/        QC onay
          numarası  Balık kılçığı      planı             ölçüm
```

### 9.3. Mevcut Bilinen Uygunsuzluklar

| NCR# | Seviye | Açıklama | Durum |
|------|--------|----------|-------|
| NCR-001 | Minör | Test parametresi uyumsuzluğu (`stride` vs `window_stride`) | ✅ Düzeltildi |
| NCR-002 | Minör | Test parametresi uyumsuzluğu (`output_path` vs `filepath`) | ✅ Düzeltildi |

---

## 10. Gözden Geçirme ve Denetim

### 10.1. İç Denetim Takvimi

| Dönem | Tarih | Kapsam | Denetçi |
|-------|-------|--------|---------|
| Q2 2026 | Haziran 2026 | İP1 çıktıları, veri kalitesi | C. Bülbül |
| Q3 2026 | Eylül 2026 | İP2, yazılım kalitesi | A. Helva |
| Q4 2026 | Aralık 2026 | İP3 model kalitesi | C. Bülbül |
| Q1 2027 | Mart 2027 | İP4/İP5, sistem kalitesi | C. Bülbül |
| Q2 2027 | Haziran 2027 | İP6 entegrasyon | Tüm ekip |
| Q3 2027 | Ağustos 2027 | Nihai kalite denetimi | C. Bülbül + dış danışman |

### 10.2. TÜBİTAK Dönem Raporu Kontrol Listesi

Her dönem raporu öncesi:

- [ ] Tüm iş paketleri ilerleme güncellendi
- [ ] Harcama belgeleri toplandı
- [ ] Teknik çıktılar hazır ve doğrulandı
- [ ] Test sonuçları belgelendi
- [ ] Sapmalar ve düzeltici faaliyetler raporlandı
- [ ] Bir sonraki dönem planı güncellendi
- [ ] Ekip iş dağılımı kontrol edildi

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — Kalite Kontrol Planı v1.0.0*
