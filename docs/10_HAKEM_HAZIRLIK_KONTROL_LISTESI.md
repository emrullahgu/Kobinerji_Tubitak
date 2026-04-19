# HAKEM ZİYARETİ HAZIRLIK KONTROL LİSTESİ

## 7260634 No'lu TÜBİTAK Projesi — KOBİNERJİ A.Ş.

---

> Bu kontrol listesi, hakem ziyareti öncesinde tüm hazırlıkların eksiksiz tamamlanmasını sağlamak amacıyla hazırlanmıştır. Her madde ziyaretten önce kontrol edilmeli ve işaretlenmelidir.

---

## A. YÖNETİMSEL HAZIRLIKLAR

### A1. Proje Dosyası ve Belgeler

- [ ] Proje özeti (1 sayfa) yazdırıldı mı?
- [ ] İş paketleri takvimi (Gantt) yazdırıldı mı?
- [ ] Kilometre taşı tablosu hazır mı?
- [ ] Ekip görev dağılımı sayfası hazır mı?
- [ ] Bütçe dökümü (planlanan vs. harcanan) çıktı alındı mı?
- [ ] Risk tablosu ve B planları dosyaya eklendi mi?
- [ ] Toplantı / ilerleme notları derlendi mi?
- [ ] İP1 faaliyet özet tablosu hazır mı?

### A2. Sunum Materyalleri

- [ ] Hakem sunum dosyası (PowerPoint / PDF) hazırlandı mı?
- [ ] Sunum 15-20 dakika süreli mi? (hakem soru süresine yer bırakılmalı)
- [ ] Sunum sırası belirlendi mi? (Kim neyi anlatacak?)
- [ ] Tüm ekip üyeleri sunum gününde hazır olacak mı?
- [ ] Yedek sunum cihazı / laptop hazır mı?

### A3. Mekân Hazırlığı

- [ ] Sunum yapılacak toplantı odası hazırlandı mı?
- [ ] Projeksiyon / ekran çalışıyor mu?
- [ ] Laboratuvar / test alanı temiz ve düzenli mi?
- [ ] Güvenlik ekipmanları (gözlük, eldiven vb.) görünür yerde mi?

---

## B. TEKNİK HAZIRLIKLAR

### B1. Donanım ve Test Altyapısı

- [ ] Test masası / test standı düzgün kurulmuş mu?
- [ ] Şarj-deşarj cihazı çalışır durumda mı?
- [ ] DAQ sistemi veri kaydediyor mu?
- [ ] Osiloskop ve ölçüm cihazları kalibreli mi?
- [ ] Termal kamera / sıcaklık sensörleri çalışıyor mu?
- [ ] Batarya hücre setleri hazır ve etiketli mi?
- [ ] En az bir aktif test döngüsü gösterilebilir mi?
- [ ] Ölçüm parametreleri (V, I, T, Z) ekranda görüntülenebilir mi?

### B2. Yazılım Gösterimleri

- [ ] Python ortamı çalışır durumda mı? (`python --version`)
- [ ] Tüm bağımlılıklar yüklü mü? (`pip install -r requirements.txt`)
- [ ] Demo script çalışıyor mu? (`python -m examples.demo_end_to_end`)
- [ ] Birim testler geçiyor mu? (`python -m tests.test_suite`)
- [ ] Web Dashboard erişilebilir mi? (`tubitak-7260634.netlify.app`)
- [ ] API sunucusu ayağa kalkıyor mu? (`python -m python.api.battery_api`)
- [ ] İlerleme raporu üretilebiliyor mu? (`python -m python.reporting.progress_report`)
- [ ] Veri analiz aracı çalışıyor mu? (`python -m python.reporting.data_analysis_tools`)

### B3. Veri ve Ölçüm Kanıtları

- [ ] İlk ham veri dosyaları mevcut mu?
- [ ] Örnek CSV/Excel veri dosyası gösterilebilir mi?
- [ ] Veri klasör yapısı tanımlı mı?
- [ ] Oluşturulmuş sentetik veri seti hazır mı? (`create_sample_data()`)
- [ ] Veri kalite raporu üretilebiliyor mu?
- [ ] En az 1 batarya hücresi için tam test kaydı var mı?

---

## C. İÇERİK HAZIRLIĞI

### C1. Hakemin Sorması Muhtemel Sorular

| # | Muhtemel Soru | Hazırlık Notu |
|---|--------------|---------------|
| 1 | "Şu ana kadar somut olarak ne yaptınız?" | İP1 faaliyetleri: altyapı kuruldı, testler başladı, yazılım geliştirildi, ilk veriler toplandı |
| 2 | "Veri nerede? Gösterebilir misiniz?" | Ham veri örnekleri + sentetik veri ile demo |
| 3 | "Model doğruluğu hedefe ulaşacak mı?" | Yazılım hazır, 4 algoritma uygulandı; gerçek verilerle eğitim İP3'te |
| 4 | "Ekipte kim ne yapıyor?" | 5 kişilik ekip, kişi bazlı iş paketi atamaları |
| 5 | "Bütçe neden hızlı harcandı?" | İP1'de ekipman tedariki yapıldı (1.260.000 TL bütçelenmiş) |
| 6 | "Riskleriniz neler?" | 3 ana risk tanımlı, her birine B planı mevcut |
| 7 | "Hangi standartları kullanıyorsunuz?" | IEC 62660, IEC 62133, UN 38.3, ISO 14001 |
| 8 | "Neden bu yöntemi seçtiniz?" | Geleneksel şarj-deşarj analizi yavaş; AI ile dakikalar içinde sonuç |
| 9 | "Piyasadan farkınız ne?" | İkinci ömür + geri dönüşüm tek çatı altında, AI destekli |
| 10 | "Takvimde gecikme var mı?" | Hayır, İP1 takviminde ilerleniyor (%65, bitiş: 01.06.2026) |

### C2. Anlatım Sırası Önerisi

```
1. Proje tanıtımı ve amaç (3 dk)
   → Problem, çözüm yaklaşımı, neden AI gerekli

2. Teknik hedefler ve başarı ölçütleri (2 dk)
   → %90 SoH, %85 sınıflandırma, %10 verimlilik

3. İP1 — Şu ana kadar yapılanlar (5 dk)
   → Altyapı kurulumu, test düzeneği, ilk veriler

4. Yazılım altyapısı ve demo (5 dk)
   → Canlı Demo: end-to-end akış gösterimi

5. Yol haritası — bundan sonrası (3 dk)
   → İP2-İP7 özet, kilometre taşları

6. Risk yönetimi ve B planları (2 dk)
   → 3 risk ve çözüm stratejileri

7. Soru-cevap
```

### C3. Kritik Mesajlar

Hakem ziyaretinde iletilmesi gereken **3 ana mesaj**:

1. **"Proje fikirden ibaret değil, sahada çalışan bir altyapısı var."**
   → Kurulu cihazlar, çalışır yazılım, ilk veriler

2. **"Takvimle uyumluyuz, İP1 çıktıları oluşmakta."**
   → %65 ilerleme, 43 gün kaldı, gecikme yok

3. **"Sonraki adımlar net planlanmış, ekip hazır."**
   → 7 İP, 7 kilometre taşı, kişi bazlı atamalar

---

## D. ZİYARET GÜNÜ KONTROL

### Ziyaretten 1 Gün Önce

- [ ] Tüm cihazlar açılıp test edildi mi?
- [ ] Demo script son kez çalıştırıldı mı?
- [ ] Dashboard güncel veri gösteriyor mu?
- [ ] Sunum son hali onaylandı mı?
- [ ] Basılı dosyalar hazır mı? (3 kopya önerilir)
- [ ] Laboratuvar düzenlendi mi?

### Ziyaret Sabahı

- [ ] Tüm bilgisayarlar açık ve hazır mı?
- [ ] İnternet bağlantısı çalışıyor mu?
- [ ] Projeksiyon / ekran test edildi mi?
- [ ] Çay/kahve/su ikramı hazır mı?
- [ ] Ziyaretçi karşılama planı netleşti mi?
- [ ] Ekip üyeleri yerinde mi?

---

## E. SONRASI

### Ziyaret Sonrası Yapılacaklar

- [ ] Hakemin sorduğu soruları not edin
- [ ] Eksik kalan noktaları listeleyin
- [ ] Varsa ek belge talebi → 1 hafta içinde gönderin
- [ ] Ekip değerlendirme toplantısı yapın
- [ ] Ziyaret özet notunu dosyaya ekleyin

---

*Hazırlayan: Proje Ekibi*  
*Tarih: 19.04.2026*
