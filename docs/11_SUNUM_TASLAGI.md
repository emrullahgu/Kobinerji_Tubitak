# HAKEM ZİYARETİ SUNUM TASLAĞI

## TÜBİTAK 1507 — Proje No: 7260634
### "Yapay Zekâ Tabanlı Batarya Sağlık Değerlendirme ve Geri Dönüşüm Karar Destek Sistemi"
### KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.

---

> **Not:** Bu dosya sunum (PowerPoint / PDF) hazırlanırken kullanılacak bir içerik taslağıdır. Her slayt için başlık, içerik önerileri ve konuşmacı notları verilmiştir.

---

## SLAYT 1 — KAPAK

**Başlık:**  
Yapay Zekâ Tabanlı Batarya Sağlık Değerlendirme ve Geri Dönüşüm Karar Destek Sistemi

**Alt Bilgi:**
- Proje No: 7260634
- Destek Programı: TÜBİTAK 1507 — KOBİ Ar-Ge Başlangıç Destek Programı
- Firma: KOBİNERJİ A.Ş.
- Tarih: [Ziyaret Tarihi]
- Sunan: Proje Ekibi

---

## SLAYT 2 — GÜNDEM

1. Problem ve Motivasyon
2. Çözüm Yaklaşımımız
3. Proje Hedefleri ve Başarı Ölçütleri
4. İP1 — Tamamlanan Çalışmalar
5. Canlı Demo
6. Yol Haritası (İP2-İP7)
7. Risk Yönetimi
8. Ekip ve Görev Dağılımı
9. Sorular

---

## SLAYT 3 — PROBLEM VE MOTİVASYON

**Başlık:** "Neden bu projeye ihtiyaç var?"

**İçerik:**
- Lityum-iyon batarya kullanımı her yıl katlanarak artıyor
- 2030'a kadar dünya genelinde yılda 11 milyon ton kullanılmış batarya bekleniyor
- Mevcut değerlendirme yöntemleri: yavaş, maliyetli, standart dışı
- İkinci ömür potansiyeli olan bataryalar gereksiz yere geri dönüşüme gönderiliyor
- Manuel test süreci: saatler süren şarj-deşarj döngüleri

**Konuşmacı Notu:**
> "Geleneksel yöntemlerle bir bataryayı değerlendirmek saatler alıyor. Biz bu süreyi dakikalara düşürmeyi hedefliyoruz."

---

## SLAYT 4 — ÇÖZÜM YAKLAŞIMI

**Başlık:** "AI Tabanlı Hızlı Değerlendirme"

**İçerik:**
- Yapay zekâ ile hızlı SoH (State of Health) tahmini
- Tek test döngüsünden çoklu parametre çıkarımı
- Otomatik sınıflandırma: ikinci ömür / geri dönüşüm / red
- Karar destek sistemi: operatöre net yönlendirme

**Görsel Öneri:**
- Akış diyagramı: Batarya → Test → AI Model → Karar (İkinci Ömür / Geri Dönüşüm)

**Konuşmacı Notu:**
> "Sistemimiz bir bataryayı 5 dakikadan kısa sürede test edip, sağlık durumunu tahmin ediyor ve otomatik olarak sınıflandırıyor."

---

## SLAYT 5 — TEKNİK HEDEFLER

**Başlık:** "Başarı Ölçütlerimiz"

| Hedef | Değer | Durum |
|-------|-------|-------|
| SoH tahmin doğruluğu | ≥ %90 | İP3'te doğrulanacak |
| Sınıflandırma doğruluğu | ≥ %85 | İP4'te doğrulanacak |
| Geri dönüşüm verimlilik artışı | ≥ %10 | İP5'te doğrulanacak |
| Desteklenen batarya tipi | ≥ 3 | İP1'de planlandı |
| Test süresi | ≤ 5 dk/batarya | İP3'te doğrulanacak |
| Teknoloji Hazırlık Seviyesi | THS 3 → THS 7 | Süreç boyunca |

**Konuşmacı Notu:**
> "Hedeflerimiz TÜBİTAK başvurusunda taahhüt ettiğimiz değerlerdir. Her biri belirli bir iş paketinde doğrulanacaktır."

---

## SLAYT 6 — İP1: MEVCUT DURUM

**Başlık:** "Batarya Veri Toplama Altyapısı — %65 Tamamlandı"

**İçerik:**
- ✅ Test düzeneği kurulumu (şarj-deşarj cihazı, DAQ, sıcaklık sensörleri)
- ✅ Yazılım altyapısı geliştirme (7 modül, 34 birim test)
- ✅ Veri toplama pipeline'ı tasarımı
- ✅ Web tabanlı izleme dashboard'u
- 🔄 İlk batarya testleri devam ediyor
- 🔄 Ham veri formatı standardizasyonu

**Takvim:** 01.03.2026 — 01.06.2026 (43 gün kaldı)

**Konuşmacı Notu:**
> "İP1 takviminde ilerliyoruz. Altyapı büyük ölçüde hazır, ilk veriler toplanmaya başladı."

---

## SLAYT 7 — CANLAMA DEMO PLANI

**Başlık:** "Canlı Gösterim"

**Demo 1 — Uçtan Uca Veri Akışı (Terminal)**
```
python -m examples.demo_end_to_end
```
- Sentetik veri oluşturma → Öznitelik çıkarma → Model eğitimi → SoH tahmini

**Demo 2 — Birim Testler (Terminal)**
```
python -m tests.test_suite
```
- 34 test, 7 modül kapsamı, tüm testlerin geçtiğini gösterme

**Demo 3 — Web Dashboard (Tarayıcı)**
- `tubitak-7260634.netlify.app`
- Proje ilerleme, iş paketleri, ekip, bütçe takibi

**Demo 4 — Raporlama Sistemi (Terminal)**
```
python -m python.reporting.progress_report
python -m python.reporting.data_analysis_tools
```
- Otomatik rapor oluşturma, veri analizi

**Konuşmacı Notu:**
> "Şimdi size yazılım altyapımızın çalışır durumda olduğunu canlı olarak göstereceğim."

---

## SLAYT 8 — YAZILIM MİMARİSİ

**Başlık:** "Modüler Yazılım Altyapısı"

**Modül Yapısı:**

```
┌─────────────────────────────────────────────┐
│              Web Dashboard (SPA)             │
│         HTML5 / CSS3 / JavaScript            │
├─────────────────────────────────────────────┤
│              Flask REST API                  │
│           13 Endpoint / 5 Kaynak             │
├──────────┬──────────┬──────────┬────────────┤
│  Veri    │ Öznitelik│   SoH    │    BMS     │
│ İşleme   │ Çıkarma  │  Model   │ Kontrolcü  │
├──────────┴──────────┴──────────┴────────────┤
│          Raporlama & Analiz Araçları         │
├─────────────────────────────────────────────┤
│     Python 3.8+ / NumPy / Pandas / sklearn   │
└─────────────────────────────────────────────┘
```

**Detay:**
- 7 ana modül, 3.000+ satır kod
- 34 birim test, tüm modüller kapsanmış
- Modüler mimari, her bileşen bağımsız test edilebilir

---

## SLAYT 9 — YOL HARİTASI

**Başlık:** "İP2-İP7 Planı"

```
2026                                    2027
MAR  HAZ  EYL  ARA  MAR  HAZ  EYL
 ├────┤                                  İP1: Veri Altyapısı (%65)
      ├────┤                             İP2: EIS ve Hızlı Test
           ├────┤                        İP3: AI Model Geliştirme
                ├────┤                   İP4: Sınıflandırma Sistemi
                     ├────┤              İP5: Geri Dönüşüm Optimizasyonu
                          ├────┤         İP6: Entegre Platform
                               ├────┤   İP7: Validasyon ve Raporlama
```

**Kilometre Taşları:**
1. ✅ Proje Başlangıç Toplantısı (01.04.2026)
2. ⏳ Veri Toplama Altyapısı Tamamlama (01.06.2026)
3. ○ EIS Test Protokolü Onayı (01.09.2026)
4. ○ İlk AI Model Prototipi (01.12.2026)
5. ○ Sınıflandırma Sistemi Beta (01.03.2027)
6. ○ Entegre Sistem Testi (01.06.2027)
7. ○ Final Rapor ve Kapanış (01.09.2027)

---

## SLAYT 10 — RİSK YÖNETİMİ

**Başlık:** "Riskler ve Önlemler"

| Risk | Olasılık | Etki | Önlem |
|------|----------|------|-------|
| 🔴 Veri kalitesi yetersiz | Orta | Yüksek | Çoklu sensör, kalite kontrol pipeline |
| 🟡 Model doğruluğu düşük | Düşük | Yüksek | 4 farklı algoritma, ensemble yöntemler |
| 🟡 Ekipman tedarik gecikmesi | Düşük | Orta | Erken sipariş, alternatif tedarikçi |

**Konuşmacı Notu:**
> "Her risk için hem ana önlem hem de B planımız tanımlanmıştır. Şu an hiçbir risk tetiklenmemiştir."

---

## SLAYT 11 — EKİP

**Başlık:** "Proje Ekibi"

| İsim | Rol | Sorumluluk |
|------|-----|------------|
| Cem Bülbül | Proje Yöneticisi | Genel koordinasyon, batarya test süreçleri |
| Alperen Helva | Kıdemli Araştırmacı | AI model geliştirme, veri analizi |
| Emrullah Günay | Yazılım Geliştirici | Python altyapısı, web dashboard, API |
| Görkem Tanır | Donanım Mühendisi | Test düzeneği, sensör entegrasyonu |
| İbrahim Çağdaş | Araştırma Asistanı | Veri toplama, literatür, test desteği |

---

## SLAYT 12 — BÜTÇE DURUMU

**Başlık:** "Bütçe Özeti"

| Kalem | Planlanan | Harcanan | Oran |
|-------|-----------|----------|------|
| Personel | 1.982.730 TL | 300.000 TL | %15 |
| Makine-Teçhizat | 1.260.000 TL | 850.000 TL | %67 |
| Malzeme-Sarf | 155.000 TL | 50.000 TL | %32 |
| Seyahat | 100.000 TL | 20.000 TL | %20 |
| **TOPLAM** | **3.497.730 TL** | **1.220.000 TL** | **%34.9** |

**Konuşmacı Notu:**
> "Makine-teçhizat harcaması yüksek görünüyor çünkü İP1'de tüm ana ekipmanlar tedarik edildi. Bu planlı bir harcamadır."

---

## SLAYT 13 — STANDARTLAR

**Başlık:** "Uyum Sağlanan Standartlar"

- **IEC 62660** — Lityum-iyon pil performans testleri
- **IEC 62133** — Taşınabilir batarya güvenlik gereksinimleri
- **UN 38.3** — Tehlikeli madde taşıma testleri
- **ISO 14001** — Çevre yönetim sistemi
- **ISO/IEC 27001** — Bilgi güvenliği (veri koruma)

---

## SLAYT 14 — YENİLİKÇİ YÖNLER

**Başlık:** "Projenin Yenilikçi Katkısı"

1. **Hız:** Saatler süren batarya değerlendirmesini dakikalara düşürme
2. **Doğruluk:** AI tabanlı tahmin ile insan hatasını minimuma indirme
3. **Bütünleşik Karar:** İkinci ömür + geri dönüşüm tek platformda
4. **Ölçeklenebilirlik:** Modüler mimari, farklı batarya tiplerine genişletilebilir
5. **Çevresel Katkı:** Gereksiz geri dönüşümü azaltma, kaynak verimliliği

**Konuşmacı Notu:**
> "Bu proje, Türkiye'de batarya ikinci ömür değerlendirmesini AI ile yapan ilk entegre sistemi geliştirmeyi hedeflemektedir."

---

## SLAYT 15 — KAPANIŞ

**Başlık:** "Özet ve Sonraki Adımlar"

**Mevcut Durum:**
- İP1 %65 tamamlandı, takvimde
- Altyapı (donanım + yazılım) büyük ölçüde hazır
- İlk veri toplama faaliyetleri başladı

**Sonraki 3 Ay:**
- İP1 tamamlama (Haziran 2026)
- İP2 başlangıç: EIS test protokolü geliştirme
- İlk model prototip çalışmaları

**Kapanış Mesajı:**
> "Proje planına uygun ilerliyor. Teknik altyapımız hazır, ekibimiz motivasyonlu. Sonraki dönemde veriye dayalı model geliştirme aşamasına geçiyoruz."

---

## TEKNİK NOTLAR

### Sunum Hazırlama İpuçları

- Toplam süre: 15-20 dakika (soru-cevap hariç)
- Her slaytta maksimum 4-5 madde
- Demo için yedek ekran görüntüleri hazırlayın (internet/yazılım sorunu olursa)
- Basılı sunum + proje dosyası hakeme verilecek
- Görseller: akış diyagramları, tablolar ve mimari şemalar tercih edin
- Animasyon ve gereksiz efektlerden kaçının — kurumsal ve sade tasarım

### Slayt Tasarım Önerileri

- **Renk paleti:** Koyu mavi (#1a237e), açık mavi (#42a5f5), beyaz, gri
- **Font:** Calibri veya Roboto, minimum 18pt
- **KOBİNERJİ logosu:** her slaytta sol üst
- **TÜBİTAK logosu:** her slaytta sağ üst
- **Sayfa numarası:** sağ alt

---

*Hazırlayan: Proje Ekibi*  
*Tarih: 19.04.2026*  
*Versiyon: 1.0*
