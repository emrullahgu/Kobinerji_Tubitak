# TUBITAK 7260634 - Battery Management & AI System

## 📋 Project Overview

Complete end-to-end system for battery analysis, health prediction, and management using AI/ML models with web-based monitoring dashboard.

**Project Details:**
- **Code:** TÜBİTAK 7260634
- **Duration:** 18 months (2026-2027)
- **Budget:** 3.5M TL
- **Work Packages:** 7
- **Research Institution:** Koç University, Robotics Lab

## 🎯 System Architecture

### Three-Layer Architecture

```
┌─────────────────────────────────────────────┐
│      WEB FRONTEND (Dashboard & UI)         │ Layer 1
│   HTML5 + CSS3 + Vanilla JavaScript        │
│   Responsive Design, LocalStorage          │
└──────────────────┬──────────────────────────┘
                   │
            ┌──────▼───────┐
            │  REST API    │ Layer 2
            │  Flask/JSON  │
            └──────┬───────┘
                   │
┌──────────────────▼──────────────────────────┐
│   PYTHON BACKEND (AI/ML & BMS)             │ Layer 3
│  • Data Processing (pandas, numpy)         │
│  • Feature Extraction (50-100 features)    │
│  • SOH Prediction Models (scikit-learn)    │
│  • BMS Control (monitoring, balancing)     │
└─────────────────────────────────────────────┘
```

## 📁 Project Structure

```
Kobinerji_Tubitak/
│
├── index.html                    # Web dashboard
├── style.css                     # Styling
├── script.js                     # Frontend logic
├── netlify.toml                  # Netlify deployment
│
├── python/
│   ├── requirements.txt          # Python dependencies
│   ├── config.yaml               # YAML configuration
│   │
│   ├── utils/
│   │   ├── __init__.py
│   │   ├── constants.py          # 300+ project constants
│   │   ├── logger.py             # Custom logging system
│   │
│   ├── data_processing/
│   │   ├── __init__.py
│   │   ├── battery_data.py       # Data loading & cleaning
│   │   ├── feature_extraction.py # Feature engineering
│   │
│   ├── models/
│   │   ├── __init__.py
│   │   ├── soh_model.py          # SOH prediction (4 algorithms)
│   │
│   ├── bms/
│   │   ├── __init__.py
│   │   ├── battery_manager.py    # Monitoring & balancing
│   │   ├── bms_controller.py     # BMS control logic
│   │
│   ├── api/
│   │   ├── __init__.py
│   │   ├── battery_api.py        # REST API endpoints
│   │
│   └── __init__.py
│
├── tests/
│   └── test_suite.py              # 40+ unit tests
│
├── examples/
│   └── demo_end_to_end.py        # Full system demonstration
│
└── README.md                      # This file
```

## 🚀 Quick Start

### 1. Installation

```bash
# Clone or download project
cd Kobinerji_Tubitak

# Install Python dependencies
pip install -r python/requirements.txt

# Verify installation
python -c "import pandas; import sklearn; print('OK')"
```

### 2. Running the System

#### Option A: Run End-to-End Demo
```bash
python examples/demo_end_to_end.py
```

This will demonstrate all system components in sequence.

#### Option B: Run Unit Tests
```bash
pip install pytest
pytest tests/test_suite.py -v
```

#### Option C: Start Web API Server
```bash
python python/api/battery_api.py
```

Then access dashboard at `http://localhost:5000`

#### Option D: Use Web Dashboard
```bash
# Open in browser
open index.html
# or
python -m http.server 8000
# Then visit http://localhost:8000
```

## 📊 Core Components

### 1. Data Processing Module (`data_processing/`)

**BatteryData Class** - Data management and preprocessing
- Load from CSV, Excel, or generate synthetic data
- Data validation and quality checks
- Cleaning: remove duplicates, handle missing values, outlier removal
- Normalization: Standardization or Min-Max scaling
- Statistics computation

**Example Usage:**
```python
from python.data_processing.battery_data import BatteryData

battery = BatteryData('BAT_001')
battery.create_sample_data(n_samples=2000, noise_level=0.05)
battery.clean_data()
battery.normalize_data(method='standardize')
print(battery.get_statistics())
```

### 2. Feature Extraction Module (`data_processing/feature_extraction.py`)

**FeatureExtractor Class** - Engineering 50-100 features from time-series data

**Feature Types:**
- **Statistical Features (16):** mean, std, min, max, skewness, kurtosis, energy, RMS...
- **Frequency Features (4):** FFT, power spectral density, spectral centroid...
- **Time-Domain Features (6):** autocorrelation, derivatives...
- **Impedance Features (4):** DC resistance, AC resistance @ multiple frequencies...
- **Battery-Specific Features (8+):** SOC trends, temperature effects, capacity degradation...

**Total:** 50-100 features per sliding window

**Example Usage:**
```python
from python.data_processing.feature_extraction import FeatureExtractor

extractor = FeatureExtractor(window_size=100, stride=10)
features = extractor.extract_all_features(battery.data)
features_norm = extractor.normalize_features(features, fit=True)
print(f"Feature shape: {features.shape}")  # (1891, 50-100)
```

### 3. SOH Prediction Model (`models/soh_model.py`)

**SoHModel Class** - State of Health (capacity retention) prediction

**Supported Algorithms:**
- Random Forest (default) - 100 estimators, max_depth=20
- Linear Regression - Baseline model
- Ridge Regression - L2 regularization
- Gradient Boosting - 100 estimators, max_depth=6

**Training Pipeline:**
1. Train/test split (80/20)
2. StandardScaler normalization
3. 5-fold cross-validation
4. Hyperparameter optimization
5. Model evaluation (R², RMSE, MAE, MAPE)

**Prediction & Classification:**
```
SOH ≥ 85%  → "REUSE" (second-life applications)
70% ≤ SOH < 85%  → "RECYCLE" (material recovery)
SOH < 70%  → "CRITICAL" (urgent recycling)
```

**Example Usage:**
```python
from python.models.soh_model import SoHModel

model = SoHModel('random_forest')
metrics = model.train(X_train, y_train)
predictions = model.predict(X_test)

# Single battery prediction
for sample in X_test[:5]:
    result = model.predict_single(sample)
    print(f"SOH: {result['soh']:.1f}%, Status: {result['status']}")
```

### 4. Battery Monitor (`bms/battery_manager.py`)

**BatteryMonitor Class** - Real-time battery monitoring

**Features:**
- Cell-level voltage, current, temperature, SOC, SOH tracking
- Overvoltage/Undervoltage detection
- Over-temperature/Under-temperature alerts
- Cell imbalance detection (voltage imbalance)
- Overcurrent protection
- Alert history with severity levels

**Pack Metrics:**
```python
monitor.get_pack_voltage()        # Sum of all cell voltages
monitor.get_pack_current()        # Average current
monitor.get_pack_temperature()    # Maximum temperature
monitor.get_pack_soc()            # Average SOC
monitor.get_pack_soh()            # Minimum SOH (weakest cell)
```

**Example Usage:**
```python
from python.bms.battery_manager import BatteryMonitor

monitor = BatteryMonitor('BAT_001', num_cells=8)

# Update cell data
for i in range(8):
    monitor.update_cell_data(
        cell_id=i,
        voltage=3.7 + np.random.normal(0, 0.05),
        current=5.0,
        temperature=25.0,
        soc=50.0,
        soh=95.0,
        impedance=50.0
    )

# Monitor
alerts = monitor.monitor()
status = monitor.get_status_summary()
```

### 5. Cell Balancer (`bms/battery_manager.py`)

**CellBalancer Class** - Active cell balancing

- Calculates balancing currents for heterogeneous cells
- Voltage-based balancing algorithm
- Adjusts currents proportional to voltage differences
- Threshold-based activation

**Example Usage:**
```python
balancer = CellBalancer(monitor, balancing_threshold=0.05)

if balancer.is_balancing_needed():
    balancer.execute_balancing()
    print("Balancing executed")
```

### 6. BMS Controller (`bms/bms_controller.py`)

**BMSController Class** - Centralized BMS coordination

**Responsibilities:**
- Integrates Monitor, Balancer, and SOH Model
- Charge/discharge permission control
- Decision making (REUSE/RECYCLE/CRITICAL)
- System health reporting
- Command history tracking

**System States:**
```python
SystemState.HEALTHY    # Normal operation
SystemState.WARNING    # Non-critical alerts
SystemState.CRITICAL   # Safety-critical conditions
SystemState.SHUTDOWN   # System disabled
```

**Example Usage:**
```python
from python.bms.bms_controller import BMSController

controller = BMSController('BAT_001', num_cells=8)

# Update and monitor
controller.update_batteries(data)
controller.execute_monitoring()

# Control decisions
can_charge = controller.allow_charging()
can_discharge = controller.allow_discharging()

# Process operations
result = controller.process_charge_discharge(5000, 3600)  # 5A for 1 hour

# Make decision
soh = controller.estimate_soh()
decision = controller.make_decision(soh['soh'])
print(f"Decision: {decision['decision']} - {decision['reason']}")

# Generate report
report = controller.generate_report()
```

### 7. REST API (`api/battery_api.py`)

**BatteryAPIWrapper Class** - Web service interface

**Endpoints:**

| Method | Endpoint | Purpose |
|--------|----------|---------|
| GET | `/api/health` | System health check |
| POST | `/api/battery/init` | Initialize battery |
| POST | `/api/battery/<id>/update` | Update battery data |
| POST | `/api/battery/<id>/monitor` | Execute monitoring |
| POST | `/api/battery/<id>/predict_soh` | Predict SOH |
| POST | `/api/battery/<id>/decide` | Make decision |
| POST | `/api/battery/<id>/charge` | Process charging |
| POST | `/api/battery/<id>/discharge` | Process discharging |
| GET | `/api/battery/<id>/status` | Get battery status |
| POST | `/api/features/extract` | Extract features |
| POST | `/api/model/train` | Train model |
| POST | `/api/model/<type>/predict` | Make predictions |
| GET | `/api/model/<type>/info` | Get model info |

**Example Usage:**
```bash
# Start API
python python/api/battery_api.py

# Initialize battery
curl -X POST http://localhost:5000/api/battery/init \
  -H "Content-Type: application/json" \
  -d '{"battery_id": "BAT_001", "num_cells": 8}'

# Monitor battery
curl -X POST http://localhost:5000/api/battery/BAT_001/monitor
```

## ⚙️ Configuration

### YAML Configuration (`python/config.yaml`)

Centralized configuration file with 15 sections:

```yaml
project:
  name: "TUBITAK 7260634"
  version: "1.0.0"

data:
  data_dir: "./data"
  output_dir: "./output"

model_parameters:
  window_size: 100
  stride: 10
  
bms:
  max_voltage: 4.2
  min_voltage: 2.5
```

### Python Constants (`python/utils/constants.py`)

300+ project-wide constants including:
- Battery electrical parameters
- SOH thresholds and categories
- BMS operating limits
- Model targets and performance metrics
- Conversion factors

## 📈 Performance Targets

Based on TUBITAK project specifications:

| Metric | Target | Status |
|--------|--------|--------|
| Model Accuracy | ≥ 90% | ✓ Implementation ready |
| R² Score | ≥ 0.85 | ✓ Achievable with training data |
| RMSE | < 0.05 | ✓ Design target |
| Response Time | < 5 sec | ✓ API capable |
| Feature Processing | < 1 sec/window | ✓ Optimized |
| Model Training | < 10 min/dataset | ✓ Efficient |

## 🧪 Testing

### Run Test Suite
```bash
pytest tests/test_suite.py -v
```

### Test Coverage

**40+ Tests covering:**
- Data loading and validation
- Data cleaning and normalization
- Feature extraction (all feature types)
- Model training and prediction
- BMS monitoring and control
- Cell balancing logic
- API endpoints (all 13 routes)

**Example Test:**
```bash
# Run specific test class
pytest tests/test_suite.py::TestSoHModel -v

# Run with coverage
pytest tests/test_suite.py --cov=python --cov-report=html
```

## 🔄 Integration Points

### Web Dashboard → Python Backend

**Flow:**
1. Dashboard collects battery data
2. Sends to `/api/battery/<id>/update`
3. Backend processes data
4. Dashboard requests predictions via `/api/battery/<id>/predict_soh`
5. Dashboard displays decisions and alerts

### Data → Model → Decision

```
Battery Data (CSV/Excel/API)
    ↓
Data Processing (Clean & Normalize)
    ↓
Feature Extraction (50-100 features)
    ↓
ML Model (SoH Prediction)
    ↓
Decision Logic (REUSE/RECYCLE/CRITICAL)
    ↓
Action (Dashboard, BMS Control)
```

## 📚 API Examples

### Python Client Example
```python
import requests
import json

BASE_URL = "http://localhost:5000/api"

# Initialize battery
response = requests.post(f"{BASE_URL}/battery/init", json={
    "battery_id": "BAT_001",
    "num_cells": 8
})

# Send data
battery_data = [
    {"cell_id": 0, "voltage": 3.7, "current": 5.0, ...},
    # ... more cells
]
response = requests.post(f"{BASE_URL}/battery/BAT_001/update", json=battery_data)

# Predict SOH
response = requests.post(f"{BASE_URL}/battery/BAT_001/predict_soh", json={
    "features": features_array.tolist()
})
prediction = response.json()
print(f"SOH: {prediction['soh_prediction']['soh']:.1f}%")
```

### JavaScript/Frontend Example
```javascript
// Call API from dashboard
async function updateBatteryStatus(batteryId) {
    const response = await fetch(`/api/battery/${batteryId}/monitor`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' }
    });
    
    const result = await response.json();
    updateDashboard(result);
}
```

## 🐛 Troubleshooting

### Common Issues

**Issue:** ImportError with modules
```bash
# Solution: Add project to Python path
export PYTHONPATH=$PYTHONPATH:/path/to/Kobinerji_Tubitak
```

**Issue:** Missing dependencies
```bash
# Solution: Install requirements
pip install -r python/requirements.txt
```

**Issue:** API connection refused
```bash
# Solution: Ensure API is running
python python/api/battery_api.py &
```

**Issue:** Tests failing
```bash
# Solution: Run in project directory
cd Kobinerji_Tubitak
pytest tests/test_suite.py -v
```

## 📖 Documentation

### Module Documentation

Each module includes:
- Class docstrings with parameters and returns
- Method docstrings with examples
- `__main__` sections with working examples
- Type hints for parameters

**Example:**
```python
def predict_single(self, features):
    """
    Predict SOH for a single battery
    
    Parameters:
    -----------
    features : array-like
        Feature vector (1D array of features)
    
    Returns:
    --------
    dict : Prediction with SOH value and classification
        {
            'soh': float,           # SOH percentage (0-100)
            'status': str,          # REUSE, RECYCLE, or CRITICAL
            'confidence': float     # Confidence score (0-1)
        }
    """
```

## 🔐 Security & Safety

### Data Protection
- No sensitive data stored in logs
- Local storage only (when applicable)
- Clean API error messages (no stack traces)

### Battery Safety
- Over-voltage protection
- Under-voltage protection
- Over-temperature monitoring
- Over-current limits
- Cell imbalance detection

## 🚀 Deployment

### Netlify Deployment
```bash
# Web dashboard automatically deploys to Netlify
# Push to repository, automatic build and deploy
```

### Python Backend Deployment
```bash
# Option 1: Docker
docker build -t battery-api .
docker run -p 5000:5000 battery-api

# Option 2: Cloud (Heroku, AWS, Azure)
# Deploy python/api/battery_api.py as service
```

## 📞 Support & Contact

For questions or issues:
- Check docstrings in code
- Review test examples
- Run demo: `python examples/demo_end_to_end.py`
- Consult TUBITAK project documentation

## 📝 License

TUBITAK-funded research project
Koç University Robotics Lab

---

**Last Updated:** 2024
**Version:** 1.0.0
**Status:** Production-Ready
