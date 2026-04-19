# TÜBİTAK 7260634 — API Referans Kılavuzu

**Sürüm:** 1.0.0  
**Tarih:** 19 Nisan 2026  
**Base URL:** `http://localhost:5000/api`  
**Format:** JSON (Content-Type: application/json)  

---

## İçindekiler

1. [Genel Bilgiler](#1-genel-bilgiler)
2. [Kimlik Doğrulama](#2-kimlik-doğrulama)
3. [Hata Kodları](#3-hata-kodları)
4. [Sistem Endpointleri](#4-sistem-endpointleri)
5. [Batarya Yönetim Endpointleri](#5-batarya-yönetim-endpointleri)
6. [Özellik Çıkarma Endpointleri](#6-özellik-çıkarma-endpointleri)
7. [Model Endpointleri](#7-model-endpointleri)
8. [Kullanım Örnekleri](#8-kullanım-örnekleri)
9. [Hız Sınırları ve Güvenlik](#9-hız-sınırları-ve-güvenlik)

---

## 1. Genel Bilgiler

### 1.1. Sunucu Yapılandırması

| Ayar | Değer | Yapılandırma |
|------|-------|-------------|
| Host | `0.0.0.0` | `config.yaml → api.host` |
| Port | `5000` | `config.yaml → api.port` |
| Debug | `false` | `config.yaml → api.debug` |
| CORS | Enabled | `config.yaml → api.cors_enabled` |

### 1.2. İstek Formatı

Tüm POST istekleri JSON gövdesi gerektirir:

```http
POST /api/battery/init HTTP/1.1
Host: localhost:5000
Content-Type: application/json

{
    "battery_id": "BAT_001",
    "num_cells": 8
}
```

### 1.3. Yanıt Formatı

Tüm yanıtlar JSON formatındadır. Anahtarlar sıralanmaz (`JSON_SORT_KEYS = False`).

```json
{
    "status": "success",
    "data": { ... },
    "timestamp": "2026-04-19T12:30:00"
}
```

---

## 2. Kimlik Doğrulama

Mevcut sürümde kimlik doğrulama uygulanmamıştır. Üretim ortamında JWT veya API Key tabanlı doğrulama eklenmelidir.

---

## 3. Hata Kodları

| HTTP Kodu | Anlam | Örnek |
|-----------|-------|-------|
| 200 | Başarılı | İşlem tamamlandı |
| 201 | Oluşturuldu | Yeni batarya başlatıldı |
| 400 | Geçersiz İstek | Eksik parametre, hatalı JSON |
| 404 | Bulunamadı | Batarya ID'si veya model bulunamadı |
| 500 | Sunucu Hatası | Beklenmeyen iç hata |

**Hata Yanıt Formatı:**

```json
{
    "error": "Battery not found"
}
```

---

## 4. Sistem Endpointleri

### 4.1. Sağlık Kontrolü

```
GET /api/health
```

Sistemin çalışır durumda olduğunu doğrular.

**Parametreler:** Yok

**Başarılı Yanıt (200):**

```json
{
    "status": "healthy",
    "timestamp": "2026-04-19T12:30:00.123456",
    "controllers": 2,
    "models": 1
}
```

| Alan | Tür | Açıklama |
|------|-----|----------|
| `status` | string | Sistem durumu (her zaman "healthy") |
| `timestamp` | string | ISO 8601 formatında zaman damgası |
| `controllers` | int | Aktif BMSController sayısı |
| `models` | int | Yüklenmiş model sayısı |

---

## 5. Batarya Yönetim Endpointleri

### 5.1. Batarya Başlatma

```
POST /api/battery/init
```

Yeni bir batarya ve ilişkili BMSController oluşturur.

**İstek Gövdesi:**

```json
{
    "battery_id": "BAT_001",
    "num_cells": 8
}
```

| Alan | Tür | Zorunlu | Varsayılan | Açıklama |
|------|-----|---------|-----------|----------|
| `battery_id` | string | Hayır | Otomatik üretilir | Benzersiz batarya kimliği |
| `num_cells` | int | Hayır | 8 | Seri bağlı hücre sayısı |

**Başarılı Yanıt (201):**

```json
{
    "status": "initialized",
    "battery_id": "BAT_001",
    "num_cells": 8
}
```

**Not:** Aynı `battery_id` ile tekrar çağrılırsa mevcut controller korunur, yeni oluşturulmaz.

---

### 5.2. Batarya Verisi Güncelleme

```
POST /api/battery/<battery_id>/update
```

Hücre sensör verilerini günceller.

**URL Parametreleri:**

| Parametre | Tür | Açıklama |
|-----------|-----|----------|
| `battery_id` | string | Hedef batarya kimliği |

**İstek Gövdesi (JSON Dizisi):**

```json
[
    {
        "cell_id": 0,
        "voltage": 3.72,
        "current": 5.1,
        "temperature": 25.3,
        "soc": 52.0,
        "soh": 96.5,
        "impedance": 48.2
    },
    {
        "cell_id": 1,
        "voltage": 3.68,
        "current": 5.0,
        "temperature": 24.8,
        "soc": 51.0,
        "soh": 95.8,
        "impedance": 49.1
    }
]
```

| Alan | Tür | Birim | Geçerli Aralık | Açıklama |
|------|-----|-------|----------------|----------|
| `cell_id` | int | — | 0..N-1 | Hücre numarası |
| `voltage` | float | V | 2.0–4.5 | Hücre gerilimi |
| `current` | float | A | -30..+30 | Hücre akımı |
| `temperature` | float | °C | -20..+100 | Hücre sıcaklığı |
| `soc` | float | % | 0–100 | Şarj durumu |
| `soh` | float | % | 0–100 | Sağlık durumu |
| `impedance` | float | mΩ | 0–500 | İç direnç |

**Başarılı Yanıt (200):**

```json
{
    "status": "updated",
    "battery_id": "BAT_001",
    "records": 8
}
```

---

### 5.3. Batarya İzleme

```
POST /api/battery/<battery_id>/monitor
```

Tam izleme döngüsü çalıştırır: tüm koruma kontrollerini yapar, uyarıları oluşturur, gerekirse dengeleme başlatır.

**İstek Gövdesi:** Boş veya `{}`

**Başarılı Yanıt (200):**

```json
{
    "monitoring": {
        "timestamp": "2026-04-19T12:30:00.123456",
        "alerts_found": 2,
        "system_state": "warning",
        "critical_alerts": 0,
        "warning_alerts": 2
    },
    "health": {
        "battery_id": "BAT_001",
        "timestamp": "2026-04-19T12:30:00.234567",
        "pack_voltage": 29.6,
        "pack_current": 5.05,
        "pack_temperature": 26.3,
        "pack_soc": 51.5,
        "pack_soh": 93.2,
        "alerts_count": 2,
        "critical_alerts": 0,
        "monitor_count": 1,
        "system_state": "warning",
        "charge_state": "idle",
        "charging_enabled": true,
        "discharging_enabled": true,
        "charge_cycles": 0,
        "total_charge_energy_kwh": 0.0,
        "total_discharge_energy_kwh": 0.0,
        "decisions_made": 0,
        "commands_executed": 0
    }
}
```

**`system_state` Değerleri:**

| Değer | Açıklama |
|-------|----------|
| `"healthy"` | Uyarı yok |
| `"warning"` | Kritik olmayan uyarılar var |
| `"critical"` | Kritik uyarılar var, şarj/deşarj engelli |
| `"shutdown"` | Sistem kapatıldı |

---

### 5.4. SOH Tahmini

```
POST /api/battery/<battery_id>/predict_soh
```

Bataryanın sağlık durumunu (SoH) tahmin eder.

**İstek Gövdesi:**

```json
{
    "features": [0.12, -0.34, 1.56, ...]
}
```

| Alan | Tür | Zorunlu | Açıklama |
|------|-----|---------|----------|
| `features` | float[] | Hayır | Özellik vektörü. Verilmezse doğrudan pack SOH kullanılır |

**Başarılı Yanıt (200):**

```json
{
    "battery_id": "BAT_001",
    "soh_prediction": {
        "soh": 92.3,
        "status": "REUSE",
        "recommendation": "Second-life applications",
        "confidence": 0.87,
        "method": "model_prediction"
    }
}
```

**Model yoksa veya özellik verilmezse:**

```json
{
    "battery_id": "BAT_001",
    "soh_prediction": {
        "soh": 93.2,
        "method": "direct_measurement"
    }
}
```

---

### 5.5. Batarya Kararı

```
POST /api/battery/<battery_id>/decide
```

SOH değerine ve sistem durumuna göre batarya kararı verir.

**İstek Gövdesi:**

```json
{
    "soh": 88.5
}
```

| Alan | Tür | Varsayılan | Açıklama |
|------|-----|-----------|----------|
| `soh` | float | 50.0 | SOH değeri (0–100) |

**Başarılı Yanıt (200):**

```json
{
    "battery_id": "BAT_001",
    "decision": {
        "timestamp": "2026-04-19T12:30:00.123456",
        "soh": 88.5,
        "alerts": 0,
        "system_state": "healthy",
        "decision": "reuse",
        "reason": "Second-life applications (stationary storage, EVs)",
        "destination": "SECOND_LIFE_PROGRAM"
    }
}
```

**Karar Matrisi:**

| SOH | Karar | Yönlendirme |
|-----|-------|-------------|
| ≥ 85 | `reuse` | `SECOND_LIFE_PROGRAM` |
| 70–85 | `recycle` | `MECHANICAL_RECYCLING` |
| < 70 | `critical` | `HAZMAT_PROCESSING` |
| Herhangi (Sistem CRITICAL) | `critical` | `HAZMAT_PROCESSING` (override) |

---

### 5.6. Şarj İşlemi

```
POST /api/battery/<battery_id>/charge
```

Şarj operasyonunu işler. Güvenlik kontrolleri yapılır.

**İstek Gövdesi:**

```json
{
    "current_ma": 5000,
    "duration_sec": 3600
}
```

| Alan | Tür | Varsayılan | Birim | Açıklama |
|------|-----|-----------|-------|----------|
| `current_ma` | float | 5000 | mA | Şarj akımı (pozitif) |
| `duration_sec` | float | 3600 | s | Şarj süresi |

**Başarılı Yanıt (200):**

```json
{
    "battery_id": "BAT_001",
    "charge_result": {
        "status": "allowed",
        "action": "CHARGING",
        "current_ma": 5000,
        "duration_sec": 3600,
        "energy_kwh": 0.148
    }
}
```

**Reddedildiğinde:**

```json
{
    "battery_id": "BAT_001",
    "charge_result": {
        "status": "rejected",
        "reason": "Charging not allowed"
    }
}
```

**Ret Nedenleri:**

| Neden | Koşul |
|-------|-------|
| `"Charging not allowed"` | Sistem CRITICAL durumda veya hücre aşırı gerilim |
| `"Over-charge current"` | Akım > 5.0 A |

---

### 5.7. Deşarj İşlemi

```
POST /api/battery/<battery_id>/discharge
```

Deşarj operasyonunu işler.

**İstek Gövdesi:**

```json
{
    "current_ma": -5000,
    "duration_sec": 3600
}
```

| Alan | Tür | Varsayılan | Açıklama |
|------|-----|-----------|----------|
| `current_ma` | float | -5000 | Deşarj akımı (negatif) |
| `duration_sec` | float | 3600 | Süre (saniye) |

---

### 5.8. Batarya Durumu

```
GET /api/battery/<battery_id>/status
```

Kapsamlı sistem raporu döndürür.

**Başarılı Yanıt (200):**

```json
{
    "timestamp": "2026-04-19T12:30:00.123456",
    "battery_id": "BAT_001",
    "system_health": {
        "battery_id": "BAT_001",
        "pack_voltage": 29.6,
        "pack_current": 5.05,
        "pack_temperature": 26.3,
        "pack_soc": 51.5,
        "pack_soh": 93.2,
        "system_state": "healthy",
        "charge_state": "idle",
        "charging_enabled": true,
        "discharging_enabled": true,
        "charge_cycles": 0,
        "total_charge_energy_kwh": 0.0,
        "total_discharge_energy_kwh": 0.0
    },
    "recent_alerts": [
        {
            "cell_id": 3,
            "type": "OVERTEMPERATURE",
            "value": 62.1,
            "threshold": 60.0,
            "severity": "warning",
            "message": "Cell 3 overtemperature: 62.1°C"
        }
    ],
    "recent_decisions": [],
    "alerts_summary": {
        "total": 1,
        "critical": 0,
        "warning": 1,
        "info": 0
    },
    "health_metrics": {
        "pack_voltage": 29.6,
        "pack_current": 5.05,
        "pack_temperature": 26.3,
        "pack_soc": 51.5,
        "pack_soh": 93.2,
        "voltage_imbalance": 0.043
    }
}
```

---

## 6. Özellik Çıkarma Endpointleri

### 6.1. Özellik Çıkarımı

```
POST /api/features/extract
```

Ham batarya verisinden makine öğrenmesi özellikleri çıkarır.

**İstek Gövdesi:**

```json
{
    "battery_id": "BAT_001",
    "data": [
        {"voltage": 3.72, "current": 5.0, "temperature": 25.0, ...},
        {"voltage": 3.71, "current": 5.1, "temperature": 25.1, ...}
    ]
}
```

| Alan | Tür | Zorunlu | Açıklama |
|------|-----|---------|----------|
| `battery_id` | string | Hayır | İlişkilendirme kimliği |
| `data` | object[] | Evet | Ham ölçüm verileri |

**Başarılı Yanıt (200):**

```json
{
    "battery_id": "BAT_001",
    "features_shape": [19, 113],
    "features": [[0.12, -0.34, ...], ...]
}
```

---

## 7. Model Endpointleri

### 7.1. Model Eğitimi

```
POST /api/model/train
```

Yeni bir SoH tahmin modeli eğitir.

**İstek Gövdesi:**

```json
{
    "model_type": "random_forest",
    "X_train": [[0.12, -0.34, ...], ...],
    "y_train": [92.3, 88.1, ...]
}
```

| Alan | Tür | Varsayılan | Geçerli Değerler |
|------|-----|-----------|-----------------|
| `model_type` | string | `"random_forest"` | `random_forest`, `linear`, `ridge`, `gradient_boosting` |
| `X_train` | float[][] | — | Özellik matrisi |
| `y_train` | float[] | — | SOH hedef değerleri (0–100) |

**Başarılı Yanıt (200):**

```json
{
    "model_type": "random_forest",
    "training_metrics": {
        "train_r2": 0.98,
        "test_r2": 0.92,
        "train_rmse": 0.023,
        "test_rmse": 0.048,
        "train_mae": 0.018,
        "test_mae": 0.035,
        "cv_mean": 0.91,
        "cv_std": 0.03,
        "training_time": 2.45
    }
}
```

---

### 7.2. Model ile Tahmin

```
POST /api/model/<model_type>/predict
```

Eğitilmiş modelle toplu tahmin yapar.

**URL Parametreleri:**

| Parametre | Tür | Açıklama |
|-----------|-----|----------|
| `model_type` | string | Model türü (random_forest, linear, ...) |

**İstek Gövdesi:**

```json
{
    "features": [[0.12, -0.34, ...], ...]
}
```

**Başarılı Yanıt (200):**

```json
{
    "model_type": "random_forest",
    "predictions": [92.3, 88.1, 76.5, 94.2]
}
```

---

### 7.3. Model Bilgisi

```
GET /api/model/<model_type>/info
```

Eğitilmiş modelin meta bilgilerini döndürür.

**Başarılı Yanıt (200):**

```json
{
    "model_type": "random_forest",
    "is_trained": true,
    "model_algorithm": "RandomForestRegressor"
}
```

---

## 8. Kullanım Örnekleri

### 8.1. Python ile Tam İş Akışı

```python
import requests
import json

BASE = "http://localhost:5000/api"

# 1) Sağlık kontrolü
r = requests.get(f"{BASE}/health")
print(r.json())

# 2) Batarya başlat
r = requests.post(f"{BASE}/battery/init", json={
    "battery_id": "BAT_001",
    "num_cells": 8
})
bat_id = r.json()["battery_id"]

# 3) Hücre verilerini güncelle
cells = []
for i in range(8):
    cells.append({
        "cell_id": i,
        "voltage": 3.7,
        "current": 5.0,
        "temperature": 25.0,
        "soc": 50.0,
        "soh": 95.0,
        "impedance": 50.0
    })

r = requests.post(f"{BASE}/battery/{bat_id}/update", json=cells)
print(r.json())

# 4) İzleme çalıştır
r = requests.post(f"{BASE}/battery/{bat_id}/monitor")
monitoring = r.json()
print(f"Sistem durumu: {monitoring['monitoring']['system_state']}")

# 5) SOH tahmin et
r = requests.post(f"{BASE}/battery/{bat_id}/predict_soh", json={})
soh = r.json()["soh_prediction"]["soh"]
print(f"SOH: %{soh:.1f}")

# 6) Karar al
r = requests.post(f"{BASE}/battery/{bat_id}/decide", json={"soh": soh})
decision = r.json()["decision"]
print(f"Karar: {decision['decision']} → {decision['destination']}")

# 7) Durum raporu
r = requests.get(f"{BASE}/battery/{bat_id}/status")
report = r.json()
print(f"Pack gerilimi: {report['health_metrics']['pack_voltage']:.2f}V")
```

### 8.2. cURL Örnekleri

```bash
# Sağlık kontrolü
curl -s http://localhost:5000/api/health | python -m json.tool

# Batarya başlat
curl -X POST http://localhost:5000/api/battery/init \
  -H "Content-Type: application/json" \
  -d '{"battery_id":"BAT_001","num_cells":8}'

# İzleme çalıştır
curl -X POST http://localhost:5000/api/battery/BAT_001/monitor \
  -H "Content-Type: application/json"

# SOH tahmin et
curl -X POST http://localhost:5000/api/battery/BAT_001/predict_soh \
  -H "Content-Type: application/json" \
  -d '{}'

# Karar al
curl -X POST http://localhost:5000/api/battery/BAT_001/decide \
  -H "Content-Type: application/json" \
  -d '{"soh": 88.5}'

# Durum raporu
curl -s http://localhost:5000/api/battery/BAT_001/status | python -m json.tool
```

### 8.3. JavaScript (Frontend) Örneği

```javascript
const BASE = '/api';

// Batarya izleme
async function monitorBattery(batteryId) {
    const res = await fetch(`${BASE}/battery/${batteryId}/monitor`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' }
    });
    const data = await res.json();
    
    // Dashboard'u güncelle
    document.getElementById('systemState').textContent = data.monitoring.system_state;
    document.getElementById('packVoltage').textContent = data.health.pack_voltage.toFixed(2) + 'V';
    document.getElementById('packSoh').textContent = data.health.pack_soh.toFixed(1) + '%';
    
    return data;
}

// SOH Tahmini
async function predictSOH(batteryId) {
    const res = await fetch(`${BASE}/battery/${batteryId}/predict_soh`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({})
    });
    return await res.json();
}

// Karar alma
async function makeDecision(batteryId, sohValue) {
    const res = await fetch(`${BASE}/battery/${batteryId}/decide`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ soh: sohValue })
    });
    return await res.json();
}
```

---

## 9. Hız Sınırları ve Güvenlik

### 9.1. Mevcut Durum

| Özellik | Durum | Not |
|---------|-------|-----|
| Kimlik Doğrulama | Yok | Üretim için JWT/API Key önerilir |
| Hız Sınırı | Yok | Flask-Limiter ile eklenebilir |
| CORS | Etkin | Tüm originlere açık |
| HTTPS | Hayır | Üretim için zorunlu |
| Girdi Doğrulama | Kısmi | Temel kontroller mevcut |

### 9.2. Üretim Önerileri

1. **JWT tabanlı kimlik doğrulama** eklenmeli
2. **HTTPS** sertifikası yapılandırılmalı
3. **Flask-Limiter** ile hız sınırlaması (100 istek/dakika önerilir)
4. **Girdi doğrulama** JSON Schema ile güçlendirilmeli
5. **CORS** belirli originlerle kısıtlanmalı
6. **Günlükleme** erişim logları eklenmelidir

---

**Doküman Sonu**  
*TÜBİTAK 7260634 — API Referans Kılavuzu v1.0.0*
