# 🚀 TUBITAK 7260634 - Proje Yönetimi Dashboard

**Elektrikli Araç Bataryaları İçin Yapay Zeka Destekli Yeşil Dönüşüm ve Analiz Sistemi**

---

## 📋 Proje Özeti

Bu dashboard, TUBITAK 1507 Programı kapsamında yürütülen **"Elektrikli Araç Bataryaları İçin Yapay Zeka Destekli Yeşil Dönüşüm ve Analiz Sistemi"** projesinin yönetimini sağlar.

### 🎯 Proje Detayları
- **Proje Numarası:** 7260634
- **Yürütücü Kurum:** KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.
- **Başlangıç:** 01 Mart 2026
- **Bitiş:** 01 Eylül 2027
- **Süre:** 18 ay
- **Bütçe:** 3.497.730 TL
- **Teknoloji Hazırlık Seviyesi:** THS-3 → THS-7

---

## 🏗️ Proje Yapısı (7 İş Paketi)

| # | İş Paketi | Tarih | Durum | İlerleme |
|---|-----------|-------|-------|----------|
| 1 | Batarya Veri Toplama | Mar-Haz 2026 | ✅ Tamamlandı | 65% |
| 2 | Veri Ön İşleme | Haz-Eyl 2026 | ⧗ Devam Ediyor | 30% |
| 3 | AI SoH Modeli | Eyl 2026-Oca 2027 | ⧗ Beklemede | 0% |
| 4 | Karar Destek Sistemi | Oca-Mar 2027 | ⧗ Beklemede | 0% |
| 5 | Mekanik Geri Kazanım | Mar-May 2027 | ⧗ Beklemede | 0% |
| 6 | Sistem Entegrasyonu | May-Tem 2027 | ⧗ Beklemede | 0% |
| 7 | Nihai Optimizasyon | Tem-Eyl 2027 | ⧗ Beklemede | 0% |

---

## 🎯 Hemmed Başarı Kriterleri

✅ **SoH Tahmin Doğruluğu:** ≥ %90
✅ **Batarya Sınıflandırma:** ≥ %85
✅ **Analiz Süresi:** ≤ 5 dakika/batarya
✅ **Geri Kazanım Verimi:** +%10 iyileşme
✅ **Desteklenen Batarya Tipi:** 3+ tip

---

## 📊 Dashboard Özellikleri

### 1. **Ana Dashboard (Dashboard)**
- Genel proje ilerlemesi
- Bütçe ve kaynak görünümü
- Ekip size ve yapısı
- Proje riskleri ve yönetimi

### 2. **Zaman Çizelgesi (Timeline)**
- 7 İş Paketinin takvimi
- Gerçek zamanlı ilerleme takibi
- Alt görevlerin durum göstergesi

### 3. **Kilometre Taşları (Milestones)**
- Kritik teslimleme noktaları
- Başarı kriterleri kontrolü
- İlerleme doğrulaması

### 4. **İş Paketleri (Work Packages)**
- Detaylı IP açıklamaları
- Faaliyetler ve sorumluluklar
- Takvim ve hedefler

### 5. **Bütçe Yönetimi (Budget)**
- Dönemsel bütçe dağılımı
- Harcama takibi
- Kalan bütçe analizi

### 6. **Takım Yönetimi (Team)**
- Ekip üyeleri ve rolleri
- Sorumluluk matrisi
- İletişim bilgileri

---

## 🌐 Teknolojiler

- **Frontend:** HTML5, CSS3, JavaScript (Vanilla)
- **Stil:** Modern, Responsive Design
- **Uyumluluk:** Tüm tarayıcılar, Mobile-First
- **Hosting:** Netlify
- **Depolama:** LocalStorage (veri kalıcılığı)

---

## 📱 Kullanım

### Web'de Açmak
1. [Dashboard'u ziyaret edin](https://tubitak-7260634.netlify.app)
2. Navigasyon butonlarıyla bölümler arasında geçiş yapın
3. İş paketlerini takip edin

### Yerel Kullanım
```bash
# Dosyaları indirin
git clone <repo-url>
cd Kobinerji_Tubitak

# Tarayıcıda açın
start index.html
# veya
open index.html
```

### Veri Dışa Aktarımı
- **JSON:** Ctrl+Alt+J
- **CSV:** Ctrl+Alt+C
- **Print:** Ctrl+P
- **Save:** Ctrl+S

---

## ⌨️ Klavye Kısayolları

| Kısayol | İşlem |
|---------|-------|
| Alt + 1 | Dashboard |
| Alt + 2 | Timeline |
| Alt + 3 | Milestones |
| Alt + 4 | Work Packages |
| Alt + 5 | Budget |
| Alt + 6 | Team |
| Ctrl + P | Print |
| Ctrl + S | Save |

---

## 📊 Veri Yönetimi

### Otomatik Kayıt
- Veriler her dakika otomatik olarak kaydedilir
- Tarayıcıyı kapatmadan önce veriler kayıt edilir
- LocalStorage kullanılmaktadır

### Manuel Dışa Aktarımı
```javascript
// JSON olarak dışa aktar
exportProjectDataAsJSON();

// CSV olarak dışa aktar
exportProjectDataAsCSV();

// İş paketi ilerlemesini güncelle
updateWorkPackageProgress('IP1', 65);

// Kilometre taşı durumunu güncelle
completeMilestone(0);
```

---

## 🔄 Netlify Deployment

### Adımlar:
1. **GitHub Repository** oluşturun
2. Proje dosyalarını push edin
3. **Netlify**'ye bağlanın
4. Otomatik deployment seçin

### Environment Variables (İsteğe bağlı)
```
API_URL=https://your-api.com
PROJECT_ID=7260634
```

---

## 📈 API İntegrasyonu (Gelecek)

```javascript
// Gerçek zamanlı veri güncellemesi
async function fetchProjectDataFromAPI(projectId) {
    const response = await fetch(`/api/projects/${projectId}`);
    const data = await response.json();
    Object.assign(projectData, data);
}
```

---

## 🐛 Sorun Giderme

### Dashboard yüklenmiyor
- → Tarayıcı cache'ini temizleyin
- → JavaScript etkinleştirilmiş mi kontrol edin

### Veriler kayıt edilmiyor
- → LocalStorage etkinleştirilmiş mi kontrol edin
- → Tarayıcı geçmiş ayarlarını kontrol edin

### Mobil'de göstermişiyor
- → Cihazı döndürmeyi deneyin
- → Zoom seviyesini sıfırlayın (Ctrl+0)

---

## 📝 Notlar

- Dashboard tamamen **offline** çalışabilir
- Tüm veriler **LocalStorage**'da saklanır
- **Responsive** tasarım ile her cihazda çalışır
- **Dark mode** özelliği eklenebilir
- **Realtime** güncellemeler için WebSocket entegrasyonu yapılabilir

---

## 👥 Ekip

| Kişi | Rol | Sorumluluk |
|------|-----|-----------|
| Cem Bülbül | Proje Yürütücüsü | Koordinasyon, Yönetim |
| Alperen Helva | Elektrik Mühendisi | Veri İşleme, Model Dev. |
| Emrullah Günay | Elektrik Mühendisi | Veri Analizi, Sistem |
| Görkem Tanır | Elektrik Mühendisi | Test, Ölçüm |
| İbrahim Çağdaş | Makina Mühendisi | Geri Kazanım, Mekanik |

---

## 📚 Kaynaklar

- TUBITAK 1507 Rehberi
- Proje Destek Belgesi
- Teknik Dökümentasyon

---

## 📄 Lisans

Bu proje TUBITAK 1507 Programı kapsamında geliştirilmiştir.

---

## 📞 İletişim

**KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.**

📧 Email: info@kobinerji.com
📱 Tel: +90 232 504 5005
🌐 Web: www.kobinerji.com
📍 Adres: Kemalpaşa OSB Mah., Gazi Bulvarı No:177/19 - 35370 Kemalpaşa, İZMİR

---

**Son Güncelleme:** 19 Nisan 2026

⭐ Eğer projeyi beğendiyseniz, bir yıldız (⭐) verin!
