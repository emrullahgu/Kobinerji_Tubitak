# HAKEM ZİYARETİ ÖNCESİ MEVCUT DURUM RAPORU

## 7260634 No'lu TÜBİTAK 1507 KOBİ Ar-Ge Projesi

---

**Proje Adı:** Elektrikli Araç Bataryaları İçin Yapay Zeka Destekli Yeşil Dönüşüm ve Analiz Sistemi  
**Proje No:** 7260634  
**Yürütücü Kuruluş:** KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.  
**Proje Yürütücüsü:** Cem Bülbül  
**Rapor Tarihi:** 19 Nisan 2026  
**Rapor Hazırlayan:** Proje Ekibi  

---

## 1. PROJENİN KİMLİĞİ VE TEMEL ÇERÇEVESİ

### 1.1. Proje Tanımı

| Bilgi | Değer |
|-------|-------|
| Proje No | 7260634 |
| Program | TÜBİTAK 1507 — KOBİ Ar-Ge Başlangıç Destek Programı |
| Proje Adı | Elektrikli Araç Bataryaları İçin Yapay Zeka Destekli Yeşil Dönüşüm ve Analiz Sistemi |
| Yürütücü | KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş. |
| Başlangıç Tarihi | 01.03.2026 |
| Bitiş Tarihi | 01.09.2027 |
| Proje Süresi | 18 Ay |
| Toplam Bütçe | 3.497.730 TL |
| Teknoloji Alanı | Yapay Zeka, Enerji Depolama, Güç Elektroniği |
| Başlangıç THS | THS-3 (Deneysel Kavram Kanıtı) |
| Hedef THS | THS-7 (Prototip Gösterimi) |

### 1.2. Projenin Amacı

Projenin temel amacı; kullanım ömrünü tamamlamış veya tamamlamaya yaklaşmış elektrikli araç bataryalarının sağlık durumunu (State of Health — SoH) hızlı ve güvenilir biçimde belirleyen, uygun olanları ikinci ömür uygulamalarına yönlendiren, uygun olmayanları ise mekanik geri kazanım süreçlerine aktaran **yapay zeka destekli entegre bir analiz ve karar destek sistemi** geliştirmektir.

Proje çıktısı yalnızca analiz yazılımı değil; veri toplama, SoH tahmini, sınıflandırma, karar mekanizması ve mekanik geri kazanım tarafını kapsayan **bütünleşik bir sistemdir**.

### 1.3. Stratejik Hedefler ve Başarı Ölçütleri

| Hedef | Metrik | Ölçüt |
|-------|--------|-------|
| SoH tahmin doğruluğu | Accuracy / R² | ≥ %90 / R² ≥ 0,85 |
| Batarya sınıflandırma doğruluğu | F1-Score | ≥ %85 |
| Geri kazanım verim iyileştirmesi | Verim artış oranı | ≥ %10 |
| Batarya tipi uygulanabilirlik | Desteklenen kimya | ≥ 3 farklı tip |
| Analiz süresi | Batarya başına süre | ≤ 5 dakika |

Bu yönüyle proje, klasik şarj-deşarj tabanlı yavaş analiz yaklaşımından farklı olarak, **veri odaklı ve yapay zeka destekli bir değerlendirme sistemi** kurmayı hedeflemektedir. Proje ayrıca ikinci ömür ve geri dönüşüm süreçlerini tek çatı altında ele aldığı için teknik olarak bütüncül bir model sunmaktadır.

---

## 2. İÇİNDE BULUNDUĞUMUZ AŞAMA

### 2.1. Mevcut Durum Özeti

Bugünkü tarih (19.04.2026) itibarıyla proje, **İş Paketi 1 (İP1) — Batarya Veri Toplama Altyapısı** dönemindedir (01.03.2026 – 01.06.2026). Proje başlangıcından bu yana geçen süre **49 gün**, İP1'in tamamlanmasına kalan süre ise **43 gündür**.

```
Proje Zaman Çizelgesi ve Mevcut Konum
══════════════════════════════════════════════════════════════

 İP1 [████████████░░░░░░░░]  %65         ◄── ŞU AN BURADAYIZ
     Mar 2026 ────────── Haz 2026

 İP2 [░░░░░░░░░░░░░░░░░░░░]                   
     Haz 2026 ────────── Eyl 2026

 İP3 [░░░░░░░░░░░░░░░░░░░░]
     Eyl 2026 ────────── Oca 2027

 İP4 [░░░░░░░░░░░░░░░░░░░░]
     Oca 2027 ────────── Mar 2027

 İP5 [░░░░░░░░░░░░░░░░░░░░]
     Mar 2027 ────────── May 2027

 İP6 [░░░░░░░░░░░░░░░░░░░░]
     May 2027 ────────── Tem 2027

 İP7 [░░░░░░░░░░░░░░░░░░░░]
     Tem 2027 ────────── Eyl 2027
══════════════════════════════════════════════════════════════
```

### 2.2. İP1 Kapsamında Tamamlanan Faaliyetler

| No | Faaliyet | Durum | Kanıt |
|----|----------|-------|-------|
| 1.1 | Batarya veri toplama altyapısının kurulması | ✅ Tamamlandı | Test istasyonu kurulu ve çalışır durumda |
| 1.2 | Ölçüm donanımlarının ve test düzeneklerinin hazırlanması | ✅ Tamamlandı | Cihaz listesi ve kalibrasyon kayıtları mevcut |
| 1.3 | Test protokolü oluşturma ve onaylama | ✅ Tamamlandı | IEC 62660 uyumlu test prosedürü hazır |
| 1.4 | Bataryaların kontrollü koşullarda test edilmesi | ⏳ Devam ediyor | İlk test döngüleri başlatıldı |
| 1.5 | Gerilim, akım, sıcaklık ve kapasite verilerinin toplanması | ⏳ Devam ediyor | İlk ham veri setleri oluşturulmakta |
| 1.6 | Veri kayıt sistemi ve veritabanı kurulumu | ✅ Tamamlandı | Veri klasör yapısı ve kayıt formatı tanımlı |

**İP1 Genel İlerleme: %65** — Proje takvimiyle uyumludur.

### 2.3. Kurulan Ar-Ge Altyapısı

Proje kapsamında aşağıdaki ölçüm ve test altyapısı kurulmuş/tedarik edilmiştir:

| Ekipman | Kullanım Amacı | Durum |
|---------|---------------|-------|
| Batarya Şarj/Deşarj Test Cihazı | Kontrollü şarj-deşarj döngüleri | ✅ Kurulu |
| Dijital Osiloskop | Gerilim/akım dalga formu analizi | ✅ Kurulu |
| Fonksiyon Jeneratörü | Test sinyal üretimi | ✅ Kurulu |
| Programlanabilir DC Güç Kaynağı | Kontrollü enerji besleme | ✅ Kurulu |
| Veri Toplama Sistemi (DAQ) | Çok kanallı veri kaydı | ✅ Kurulu |
| Yüksek İşlemcili Bilgisayar | Veri işleme ve model geliştirme | ✅ Kurulu |
| Batarya Test Standı | Hücre montajı ve ölçüm | ✅ Kurulu |
| Termal Kamera | Sıcaklık haritalama | ✅ Tedarik edildi |
| Battery Cycler | Hızlandırılmış yaşlandırma testi | ✅ Kurulu |
| Electronic Load | Kontrollü deşarj | ✅ Kurulu |
| İklimlendirme Test Kabini | Sıcaklık kontrollü test | ⏳ Kurulumda |
| BMS Geliştirme Platformu | Batarya yönetim sistemi testi | ⏳ Kurulumda |
| PCB Prototipleme Sistemi | Elektronik devre tasarımı | ✅ Tedarik edildi |

**Alet/Teçhizat/Yazılım Bütçesi:** 1.260.000 TL

### 2.4. Yazılım Altyapısı Durumu

Proje kapsamında geliştirilen yazılım modülleri:

| Modül | Açıklama | Durum |
|-------|----------|-------|
| `BatteryData` | Veri yükleme, temizleme, normalizasyon | ✅ Geliştirme tamamlandı |
| `FeatureExtractor` | 113 özellik çıkarımı (istatistik, frekans, zaman, empedans) | ✅ Geliştirme tamamlandı |
| `SoHModel` | 4 algoritma (RF, GB, Ridge, Linear) ile SoH tahmini | ✅ Geliştirme tamamlandı |
| `BatteryMonitor` | 7 koruma kontrolü, pack metrikleri | ✅ Geliştirme tamamlandı |
| `CellBalancer` | Hücre dengeleme algoritması | ✅ Geliştirme tamamlandı |
| `BMSController` | Üst düzey kontrol, karar verme, raporlama | ✅ Geliştirme tamamlandı |
| `BatteryAPI` | Flask REST API (13 endpoint) | ✅ Geliştirme tamamlandı |
| Web Dashboard | 6 sekmeli proje yönetim paneli | ✅ Yayında (Netlify) |
| Raporlama Sistemi | Otomatik ilerleme raporu ve veri analiz araçları | ✅ Geliştirme tamamlandı |
| Test Suite | 34 birim testi (6 test sınıfı) | ✅ %91 başarı oranı |

### 2.5. Proje Ekibi ve Görev Dağılımı

| İsim | Unvan | Eğitim | Sorumluluk Alanları | Aktif İP'ler |
|------|-------|--------|--------------------|----|
| **Cem Bülbül** | Kurucu — Proje Yürütücüsü | Lisans | Proje koordinasyonu, genel yönetim, paydaş yönetimi | Tümü |
| **Alperen Helva** | Elektrik-Elektronik Müh. | Yüksek Lisans | Veri işleme, SoH model geliştirme, model optimizasyonu | İP2, İP3 |
| **Emrullah Günay** | Elektrik-Elektronik Müh. | Lisans | Veri analizi, sistem geliştirme, sistem testleri | İP2, İP3, İP6 |
| **Görkem Tanır** | Elektrik-Elektronik Müh. | Lisans | Test altyapısı kurulum, veri toplama, batarya testleri | İP1 |
| **İbrahim Çağdaş** | Makina Mühendisi | Lisans | Mekanik sistem kurulum, geri kazanım sistemi, test desteği | İP5 |

### 2.6. Bütçe Durumu

| Kalem | Planlanan | Harcanan | Kalan | Oran |
|-------|-----------|----------|-------|------|
| Personel Giderleri | 1.982.730 TL | 693.400 TL | 1.289.330 TL | %35,0 |
| Alet/Teçhizat/Yazılım | 1.260.000 TL | 460.000 TL | 800.000 TL | %36,5 |
| Malzeme Alımları | 155.000 TL | 51.600 TL | 103.400 TL | %33,3 |
| Seyahat Giderleri | 100.000 TL | 15.000 TL | 85.000 TL | %15,0 |
| **TOPLAM** | **3.497.730 TL** | **1.220.000 TL** | **2.277.730 TL** | **%34,9** |

**Değerlendirme:** Bütçe kullanım oranı (%34,9) ile süre kullanım oranı (%8,9) arasında fark bulunmaktadır. Bu durum, İP1'deki ekipman alımlarının dönem başında gerçekleştirilmiş olmasından kaynaklanmakta olup planlanan bütçe takvimi ile uyumludur. I. Dönem 2026 (01.03–30.06.2026) planlanan bütçe: 1.190.000 TL.

---

## 3. ŞİMDİDEN SONRA NE YAPACAĞIZ?

### 3.1. Yol Haritası

| İş Paketi | Dönem | Temel Faaliyet | Çıktı |
|-----------|-------|----------------|-------|
| **İP2** — Veri Ön İşleme | Haz–Eyl 2026 | Veri temizleme, normalizasyon, özellik çıkarımı | İşlenmiş veri seti (100+ özellik) |
| **İP3** — Yapay Zeka SoH Modeli | Eyl 2026–Oca 2027 | Model tasarım, eğitim, doğrulama, optimizasyon | %90 doğruluklu SoH modeli |
| **İP4** — Karar Destek Sistemi | Oca–Mar 2027 | Sınıflandırma algoritması, karar mekanizması | Yeniden kullanım/geri dönüşüm karar sistemi |
| **İP5** — Mekanik Geri Kazanım | Mar–May 2027 | Parçalama, fiziksel ayrıştırma, black mass üretimi | %10 verim artışlı geri kazanım hattı |
| **İP6** — Sistem Entegrasyonu | May–Tem 2027 | Uçtan uca test, çoklu batarya tipi doğrulama | Entegre çalışır sistem |
| **İP7** — Nihai Optimizasyon | Tem–Eyl 2027 | Dokümantasyon, ticarileştirme planı, final raporu | THS-7 seviyesinde prototip |

### 3.2. Kilometre Taşları

| # | Tarih | Kilometre Taşı | İlişkili İP | Durum |
|---|-------|----------------|-------------|-------|
| 1 | 01.06.2026 | Veri Toplama Altyapısı + İlk Veri Seti | İP1 | ⏳ Devam ediyor |
| 2 | 01.09.2026 | İşlenmiş Nihai Veri Seti | İP2 | ○ Beklemede |
| 3 | 01.01.2027 | SoH Tahmin Modeli (≥%90 doğruluk) | İP3 | ○ Beklemede |
| 4 | 01.03.2027 | Karar Destek Sistemi (≥%85 sınıflandırma) | İP4 | ○ Beklemede |
| 5 | 01.05.2027 | Mekanik Geri Kazanım Sistemi (≥%10 verim) | İP5 | ○ Beklemede |
| 6 | 01.07.2027 | Sistem Entegrasyonu (uçtan uca test) | İP6 | ○ Beklemede |
| 7 | 01.09.2027 | Nihai Sistem + Dokümantasyon + Ticarileştirme | İP7 | ○ Beklemede |

### 3.3. Ötelenen Faaliyet Bulunmamaktadır

Proje takviminde herhangi bir gecikme veya öteleme söz konusu değildir. Tüm faaliyetler başlangıçta tanımlanan takvime uygun şekilde yürütülmektedir.

---

## 4. RİSK DEĞERLENDİRMESİ VE B PLANLARI

### 4.1. Tanımlı Riskler

| # | Risk | Olasılık | Etki | Azaltma Stratejisi | B Planı |
|---|------|----------|------|--------------------|----|
| R1 | Yeterli ve kaliteli batarya verisinin toplanamaması | Orta | Yüksek | Farklı kaynaklardan veri toplama, test altyapısının genişletilmesi | Simülasyon ve literatür verileriyle veri setinin desteklenmesi |
| R2 | Yapay zeka modelinin istenen doğruluğa (≥%90) ulaşamaması | Orta | Orta | Farklı algoritmaların denenmesi, veri setinin zenginleştirilmesi | Model sadeleştirilerek kural tabanlı hibrit sistem kullanılması |
| R3 | Mekanik geri kazanım sürecinde verim düşüklüğü | Düşük | Orta | Proses optimizasyonu, parametre ayarlama | Alternatif fiziksel ayrıştırma yöntemlerinin denenmesi |

### 4.2. Risk Azaltma Faaliyetleri

**R1 için mevcut durum:** Test altyapısı kurulmuş ve ilk veri toplama başlatılmıştır. Veri kalite kontrol prosedürleri tanımlanmıştır. BatteryData sınıfı aracılığıyla otomatik veri doğrulama mekanizması geliştirilmiştir.

**R2 için mevcut durum:** Yazılım altyapısında 4 farklı algoritma (Random Forest, Gradient Boosting, Ridge Regression, Linear Regression) halihazırda uygulanmıştır. 5-katlı çapraz doğrulama altyapısı mevcuttur. Hiperparametre optimizasyon mekanizması hazırdır.

**R3 için mevcut durum:** Mekanik sistem tasarımı İP5'te gerçekleştirilecek olup şu an için erken aşamadadır.

---

## 5. REFERANS STANDARTLAR VE METODOLOJİ

### 5.1. Uygulanan Standartlar

| Standart | Kapsam | Uygulandığı Aşama |
|----------|--------|-------------------|
| **IEC 62660** | Lityum-iyon batarya test prosedürleri | İP1 — Veri toplama |
| **IEC 62133** | Taşınabilir cihaz batarya güvenliği | İP1, İP5 — Güvenlik |
| **UN 38.3** | Tehlikeli madde taşıma testi | İP5 — Geri kazanım |
| **ISO 14001** | Çevre yönetim sistemi | İP5 — Geri kazanım süreci |
| **ISO/IEC 27001** | Bilgi güvenliği | Tüm İP'ler — Veri güvenliği |

### 5.2. Teknik Metodoloji

Proje, şarj-deşarj tabanlı geleneksel analiz yöntemlerinden farklı olarak **veri odaklı makine öğrenmesi** yaklaşımını benimsemektedir. Bu yaklaşımın seçilme nedenleri:

1. **Hız:** Geleneksel tam döngü testi saatler sürerken, AI tabanlı tahmin dakikalar içinde sonuç verir
2. **Ölçeklenebilirlik:** Model bir kez eğitildiğinde binlerce bataryaya uygulanabilir
3. **Çoklu parametre:** 113 özellik eşzamanlı olarak değerlendirilir (insan ile mümkün değil)
4. **Sürekli iyileştirme:** Yeni verilerle model performansı artırılabilir

---

## 6. HAKEM ZİYARETİNDE GÖSTERİLEBİLECEK KANITLAR

### 6.1. Yönetimsel Belgeler

| Belge | Durum | Konum |
|-------|-------|-------|
| Proje özeti ve amaç notu | ✅ Hazır | `README.md`, `data.json` |
| İş paketleri takvimi | ✅ Hazır | Dashboard (Zaman Çizelgesi sekmesi) |
| Kilometre taşı listesi | ✅ Hazır | Dashboard (Kilometre Taşları sekmesi) |
| Ekip görev dağılımı | ✅ Hazır | Dashboard (Ekip sekmesi) |
| Bütçe dağılımı ve kullanım | ✅ Hazır | Dashboard (Bütçe sekmesi) |
| İş Paketi detaylandırması | ✅ Hazır | `docs/07_IS_PAKETI_DETAYLANDIRMASI.md` |
| Kalite Kontrol Planı | ✅ Hazır | `docs/08_KALITE_KONTROL_PLANI.md` |
| Risk tablosu ve B planları | ✅ Hazır | `data.json` (risks bölümü) |

### 6.2. Teknik Belgeler

| Belge | Durum | Konum |
|-------|-------|-------|
| Teknik Dokümantasyon | ✅ Hazır | `docs/01_TEKNIK_DOKUMANTASYON.md` |
| API Referans Kılavuzu | ✅ Hazır | `docs/02_API_REFERANS_KILAVUZU.md` |
| Kullanıcı El Kitabı | ✅ Hazır | `docs/03_KULLANICI_EL_KITABI.md` |
| Sistem Mimarisi Dokümanı | ✅ Hazır | `docs/04_SISTEM_MIMARISI.md` |
| Kurulum ve Dağıtım Rehberi | ✅ Hazır | `docs/05_KURULUM_DAGITIM_REHBERI.md` |
| Test Raporu | ✅ Hazır | `docs/06_TEST_RAPORU.md` |
| Veri toplama prosedürü | ✅ Hazır | Test protokolü dokümanı |
| Ölçülecek parametre listesi | ✅ Hazır | Gerilim, akım, sıcaklık, SoC, SoH, empedans |

### 6.3. Yazılım Gösterimleri

| Gösterim | Açıklama | Komut |
|----------|----------|-------|
| Web Dashboard | 6 sekmeli proje yönetim paneli | `tubitak-7260634.netlify.app` |
| Demo — Uçtan Uca Akış | Veri→Temizleme→Özellik→Model→BMS→Karar | `python -m examples.demo_end_to_end` |
| Birim Testler | 34 test, 6 sınıf | `python -m tests.test_suite` |
| İlerleme Raporu | Otomatik proje raporu üretimi | `python -m python.reporting.progress_report` |
| Veri Analiz Aracı | Kalite skoru, aykırı değer, SoH dağılımı | `python -m python.reporting.data_analysis_tools` |
| API Sunucusu | 13 endpoint REST API | `python -m python.api.battery_api` |

### 6.4. Fiziksel Gösterim

| Ekipman | Gösterilecek |
|---------|-------------|
| Test masası / test standı | Batarya hücrelerinin bağlı olduğu kurulum |
| Şarj-deşarj cihazı | Aktif test döngüsü |
| DAQ sistemi | Gerçek zamanlı veri kaydı |
| Termal kamera / sıcaklık izleme | Sıcaklık ölçüm gösterimi |
| Batarya hücre setleri | Test numuneleri |
| Bilgisayar ve analiz ortamı | Python kodları ve Dashboard |

---

## 7. HAKEME YÖNELİK ÖNEMLİ NOTLAR

### 7.1. Doğru Durum Beyanı

Proje şu an için **nihai prototip aşamasında değil**, **altyapı kurulum ve ilk veri üretim aşamasındadır**. Bu durum olumsuz değil; tam tersine proje takvimiyle birebir uyumludur. İP1'in bitiş tarihi 01.06.2026 olup hâlâ 43 gün mevcuttur.

### 7.2. Tavsiye Edilen Anlatım Çerçevesi

> *"Projemiz 01.03.2026 itibarıyla başlamış olup şu an İş Paketi 1 aşamasındadır. Bu aşamada batarya veri toplama altyapısını kurduk, ölçüm donanımlarını hazırladık, kontrollü testleri başlattık ve ilk veri setini oluşturuyoruz. Yazılım altyapımız — veri işleme, özellik çıkarımı, SoH tahmin modeli, BMS kontrolcüsü ve REST API — halihazırda geliştirilmiş ve test edilmiş durumdadır. Proje planımıza göre bir sonraki aşamada bu verilerin temizlenmesi, özellik çıkarımı ve AI model eğitimi süreci başlayacaktır. Nihai hedefimiz; EV bataryalarının sağlık durumunu AI ile belirleyen, yeniden kullanım ve geri dönüşüm kararını veren, entegre ve ticarileştirilebilir bir sistem geliştirmektir."*

### 7.3. Kaçınılması Gereken Durumlar

| ❌ Yapılmamalı | ✅ Yapılmalı |
|---------------|------------|
| "Henüz veri yok" demek | İlk ham veri örneklerini göstermek |
| "Cihazları daha almadık" demek | Mevcut altyapıyı ve kurulum planını göstermek |
| "Kim ne yapıyor belli değil" görüntüsü | Kişi bazlı görev dağılımını net aktarmak |
| İP1 dışı sonuçları sanki elde edilmiş gibi sunmak | "Yazılım hazır, gerçek verilerle eğitim İP3'te yapılacak" demek |
| Henüz oluşmamış AI başarım sonuçlarını sunmak | Hazır olan altyapıyı ve sentetik veri ile demo'yu göstermek |

---

## 8. SONUÇ

Proje başlangıç tarihi olan 01.03.2026'dan bu yana geçen 49 günlük sürede:

- **Donanım altyapısı** büyük ölçüde kurulmuştur
- **Yazılım altyapısı** (veri işleme, model, BMS, API, dashboard) geliştirilmiştir
- **Test protokolleri** tanımlanmış ve ilk testler başlatılmıştır
- **İlk veri toplama** süreci devam etmektedir
- **Proje dokümantasyonu** kapsamlı şekilde hazırlanmıştır
- **Ekip görev dağılımı** net ve uygulamaya geçmiş durumdadır

Proje planıyla tam uyumlu şekilde ilerlemekte olup İP1 çıktılarının hedeflenen 01.06.2026 tarihinde tamamlanması öngörülmektedir.

---

*Bu rapor, TÜBİTAK 7260634 numaralı projenin hakem ziyareti öncesi mevcut durumunu özetlemek amacıyla hazırlanmıştır.*

**Onaylayan:** Cem Bülbül — Proje Yürütücüsü  
**Tarih:** 19.04.2026
