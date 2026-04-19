# TÜBİTAK 7260634 — Kurulum ve Dağıtım Rehberi

**Sürüm:** 1.0.0  
**Tarih:** 19 Nisan 2026  

---

## İçindekiler

1. [Gereksinimler](#1-gereksinimler)
2. [Python Backend Kurulumu](#2-python-backend-kurulumu)
3. [Frontend Kurulumu](#3-frontend-kurulumu)
4. [Yapılandırma](#4-yapılandırma)
5. [Dağıtım — Netlify (Frontend)](#5-dağıtım--netlify-frontend)
6. [Dağıtım — Flask API (Backend)](#6-dağıtım--flask-api-backend)
7. [Doğrulama ve Test](#7-doğrulama-ve-test)
8. [Sorun Giderme](#8-sorun-giderme)

---

## 1. Gereksinimler

### 1.1. Donanım Gereksinimleri

| Bileşen | Minimum | Önerilen |
|---------|---------|----------|
| İşlemci | 2 çekirdek | 4+ çekirdek |
| RAM | 4 GB | 8 GB |
| Disk | 1 GB | 5 GB |
| Ağ | Internet bağlantısı | — |

### 1.2. Yazılım Gereksinimleri

| Yazılım | Sürüm | Amaç |
|---------|-------|------|
| Python | 3.8+ | Backend çalışma ortamı |
| pip | Son sürüm | Paket yönetimi |
| Git | 2.x+ | Sürüm kontrolü |
| Node.js | 16+ (opsiyonel) | Netlify CLI aracı |
| Web tarayıcı | Chrome/Firefox/Edge | Dashboard erişimi |

### 1.3. Python Paketleri

```
numpy>=1.21.0
pandas>=1.3.0
scikit-learn>=1.0.0
flask>=2.0.0
flask-cors>=3.0.0
scipy>=1.7.0
joblib>=1.1.0
pyyaml>=6.0
matplotlib>=3.5.0
colorama>=0.4.0
```

---

## 2. Python Backend Kurulumu

### 2.1. Projeyi İndirme

```bash
git clone <repo-url> Kobinerji_Tubitak
cd Kobinerji_Tubitak
```

### 2.2. Sanal Ortam Oluşturma

**Windows:**
```powershell
python -m venv venv
.\venv\Scripts\Activate.ps1
```

**Linux/macOS:**
```bash
python3 -m venv venv
source venv/bin/activate
```

### 2.3. Bağımlılıkları Yükleme

```bash
pip install -r requirements.txt
```

**Kurulumu Doğrulama:**

```bash
python -c "import numpy, pandas, sklearn, flask, scipy; print('Tüm paketler yüklü')"
```

### 2.4. Dizin Yapısı Doğrulama

Kurulum sonrası dizin yapısı şu şekilde olmalıdır:

```
Kobinerji_Tubitak/
├── python/
│   ├── api/
│   │   └── battery_api.py
│   ├── bms/
│   │   ├── battery_manager.py
│   │   └── bms_controller.py
│   ├── data_processing/
│   │   ├── battery_data.py
│   │   └── feature_extraction.py
│   ├── models/
│   │   └── soh_model.py
│   └── utils/
│       ├── constants.py
│       └── logger.py
├── tests/
│   └── test_suite.py
├── examples/
│   └── demo_end_to_end.py
├── docs/
│   ├── 01_TEKNIK_DOKUMANTASYON.md
│   ├── 02_API_REFERANS_KILAVUZU.md
│   ├── 03_KULLANICI_EL_KITABI.md
│   ├── 04_SISTEM_MIMARISI.md
│   ├── 05_KURULUM_DAGITIM_REHBERI.md
│   └── 06_TEST_RAPORU.md
├── config.yaml
├── requirements.txt
├── index.html
├── styles.css
├── script.js
├── netlify.toml
└── README.md
```

---

## 3. Frontend Kurulumu

Frontend tamamen statik dosyalardan oluşur; ek bir kurulum adımı gerektirmez.

### 3.1. Yerel Çalıştırma

**Yöntem 1 — Python HTTP sunucusu:**
```bash
cd Kobinerji_Tubitak
python -m http.server 8080
```
Tarayıcıda açın: `http://localhost:8080`

**Yöntem 2 — Doğrudan dosya:**
`index.html` dosyasını tarayıcıda açın.

> **Not:** Doğrudan dosya açımında `file://` protokolü nedeniyle LocalStorage çalışır, ancak API istekleri CORS hatası verebilir.

---

## 4. Yapılandırma

### 4.1. config.yaml

Ana yapılandırma dosyası `config.yaml` içinde 10 bölüm bulunur:

```yaml
# Proje bilgileri
project:
  name: "TÜBİTAK 7260634"
  company: "KOBİNERJİ A.Ş."
  version: "1.0.0"

# Veri işleme ayarları
data_processing:
  default_format: "csv"
  encoding: "utf-8"
  missing_value_strategy: "interpolate"
  outlier_method: "iqr"

# Model ayarları
model:
  default_algorithm: "random_forest"
  test_size: 0.2
  cv_folds: 5
  random_state: 42

# BMS ayarları
bms:
  max_voltage: 4.2
  min_voltage: 2.5
  max_temperature: 60.0
  max_charge_current: 5.0
  max_discharge_current: 10.0
  balancing_threshold: 0.05

# API ayarları
api:
  host: "0.0.0.0"
  port: 5000
  debug: false
  cors_enabled: true

# Günlükleme ayarları
logging:
  level: "INFO"
  file_enabled: true
  console_enabled: true
  log_dir: "logs"
  max_file_size: 10485760   # 10 MB
  backup_count: 5
```

### 4.2. constants.py Parametreleri

Değiştirilebilir kritik parametreler:

| Parametre | Varsayılan | Tanım |
|-----------|-----------|-------|
| `NOMINAL_VOLTAGE` | 3.7 V | Nominal hücre gerilimi |
| `MIN_VOLTAGE` | 2.5 V | Minimum güvenli gerilim |
| `MAX_VOLTAGE` | 4.2 V | Maksimum güvenli gerilim |
| `NOMINAL_CAPACITY` | 2500 mAh | Nominal kapasite |
| `MAX_TEMPERATURE` | 60 °C | Maksimum çalışma sıcaklığı |
| `TARGET_ACCURACY` | 0.90 | Minimum model doğruluğu |
| `TARGET_R2` | 0.85 | Minimum R² skoru |

---

## 5. Dağıtım — Netlify (Frontend)

### 5.1. Netlify Yapılandırması

`netlify.toml` dosyası zaten yapılandırılmıştır:

```toml
[build]
  publish = "."
  command = "echo 'Static site - no build needed'"

[[headers]]
  for = "/*"
  [headers.values]
    X-Frame-Options = "DENY"
    X-Content-Type-Options = "nosniff"
    Referrer-Policy = "strict-origin-when-cross-origin"
```

### 5.2. Netlify CLI ile Dağıtım

```bash
# Netlify CLI yükleme (bir kere)
npm install -g netlify-cli

# Giriş yapma
netlify login

# Yeni site oluşturma
netlify init

# Dağıtım
netlify deploy --prod
```

### 5.3. Git Entegrasyonu ile Otomatik Dağıtım

1. GitHub/GitLab deposuna bağlayın
2. Netlify → New Site → Import from Git
3. Publish directory: `.` (kök dizin)
4. Her `git push` otomatik dağıtım tetikler

---

## 6. Dağıtım — Flask API (Backend)

### 6.1. Geliştirme Sunucusu

```bash
cd Kobinerji_Tubitak
python -m python.api.battery_api
```

Sunucu `http://0.0.0.0:5000` adresinde başlar.

### 6.2. Üretim Sunucusu (Gunicorn)

```bash
pip install gunicorn

gunicorn -w 4 -b 0.0.0.0:5000 python.api.battery_api:app
```

> **Not:** Gunicorn yalnızca Linux/macOS destekler. Windows için `waitress` kullanın:

```bash
pip install waitress

waitress-serve --port=5000 python.api.battery_api:app
```

### 6.3. Systemd Servisi (Linux)

```ini
[Unit]
Description=KOBİNERJİ Battery API
After=network.target

[Service]
User=www-data
WorkingDirectory=/opt/Kobinerji_Tubitak
ExecStart=/opt/Kobinerji_Tubitak/venv/bin/gunicorn -w 4 -b 0.0.0.0:5000 python.api.battery_api:app
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable kobinerji-api
sudo systemctl start kobinerji-api
```

### 6.4. Docker ile Dağıtım (Opsiyonel)

**Dockerfile:**

```dockerfile
FROM python:3.10-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 5000

CMD ["gunicorn", "-w", "4", "-b", "0.0.0.0:5000", "python.api.battery_api:app"]
```

```bash
# İmaj oluşturma
docker build -t kobinerji-api .

# Çalıştırma
docker run -d -p 5000:5000 --name kobinerji kobinerji-api
```

---

## 7. Doğrulama ve Test

### 7.1. Kurulum Doğrulama

```bash
# 1. Python sürümü
python --version  # 3.8+

# 2. Paketler
pip list | grep -E "numpy|pandas|scikit|flask|scipy"

# 3. Modül içe aktarma
python -c "from python.utils.constants import PROJECT_NAME; print(PROJECT_NAME)"

# 4. API sağlık kontrolü
curl http://localhost:5000/api/health
```

**Beklenen yanıt:**
```json
{
  "status": "healthy",
  "service": "Battery Management API"
}
```

### 7.2. Birim Testleri

```bash
python -m tests.test_suite
```

### 7.3. Demo Çalıştırma

```bash
python -m examples.demo_end_to_end
```

---

## 8. Sorun Giderme

### 8.1. Sık Karşılaşılan Sorunlar

| Sorun | Çözüm |
|-------|-------|
| `ModuleNotFoundError` | `pip install -r requirements.txt` çalıştırın |
| `Address already in use` | `netstat -ano \| findstr :5000` ile PID bulup kapatın |
| CORS hatası | `flask-cors` yüklü mü kontrol edin |
| `PermissionError` (log) | `logs/` dizinini oluşturun ve yazma izni verin |
| `FileNotFoundError` (config) | Proje kök dizininden çalıştırın |
| Frontend verisi kaybolmuş | Tarayıcı LocalStorage temizlenmemiş mi kontrol edin |

### 8.2. Günlük Dosyaları

Günlük dosyaları `logs/` dizininde oluşturulur:

```
logs/
├── battery_system.log        # Ana günlük
├── battery_system.log.1      # Döndürülmüş (10 MB sonrası)
├── battery_system.log.2
└── ...                        # Maksimum 5 yedek
```

### 8.3. Ortam Değişkenleri

| Değişken | Varsayılan | Açıklama |
|----------|-----------|----------|
| `FLASK_ENV` | production | Çalışma ortamı |
| `FLASK_DEBUG` | 0 | Hata ayıklama modu |
| `PORT` | 5000 | API portu |

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — Kurulum ve Dağıtım Rehberi v1.0.0*
