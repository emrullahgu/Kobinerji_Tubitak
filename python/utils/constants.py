"""
TUBITAK 7260634 - Project Constants
Proje genelinde kullanılacak sabitler ve tanımlamalar
"""

import os
from pathlib import Path

# ============================================
# PROJECT PATHS
# ============================================

PROJECT_ROOT = Path(__file__).parent.parent
DATA_DIR = PROJECT_ROOT / "data"
RAW_DATA_DIR = DATA_DIR / "raw"
PROCESSED_DATA_DIR = DATA_DIR / "processed"
MODELS_DIR = PROJECT_ROOT / "models"
LOGS_DIR = PROJECT_ROOT / "logs"
REPORTS_DIR = PROJECT_ROOT / "reports"
PLOTS_DIR = PROJECT_ROOT / "plots"

# Create directories if they don't exist
for directory in [DATA_DIR, RAW_DATA_DIR, PROCESSED_DATA_DIR, MODELS_DIR, LOGS_DIR, REPORTS_DIR, PLOTS_DIR]:
    directory.mkdir(parents=True, exist_ok=True)

# ============================================
# BATTERY PARAMETERS
# ============================================

# Batarya elektriksel özellikleri
NOMINAL_VOLTAGE = 3.7  # V - Nominal cell voltage
MIN_VOLTAGE = 2.5      # V - Minimum safe voltage
MAX_VOLTAGE = 4.2      # V - Maximum safe voltage
NOMINAL_CAPACITY = 2500  # mAh - Nominal capacity
NOMINAL_ENERGY = 9.25  # Wh - Nominal energy (3.7V x 2.5Ah)

# Batarya karakteristiği
LI_ION_CHEMISTRY = "LiCoO2"  # Lithium Cobalt Oxide
CELL_IMPEDANCE_TYPICAL = 50  # mOhm - Typical internal impedance

# ============================================
# SoH (STATE OF HEALTH) DEFINITIONS
# ============================================

SOH_EXCELLENT = 1.0      # >95% kapasitesi
SOH_GOOD = 0.95          # 90-95%
SOH_FAIR = 0.85          # 80-90% - Second life için uygun
SOH_POOR = 0.70          # 70-80% - Recycling'e yönlendir
SOH_CRITICAL = 0.70      # <70% - Hemen recycle et

# SoH Kategorileri
SOH_CATEGORIES = {
    'excellent': {'min': 0.95, 'max': 1.0, 'recommendation': 'Aktif Kullanım'},
    'good': {'min': 0.90, 'max': 0.95, 'recommendation': 'Aktif Kullanım'},
    'fair': {'min': 0.80, 'max': 0.90, 'recommendation': 'İkinci Kullanım / Geri Dönüşüm'},
    'poor': {'min': 0.70, 'max': 0.80, 'recommendation': 'Geri Dönüşüm'},
    'critical': {'min': 0.0, 'max': 0.70, 'recommendation': 'Acil Geri Dönüşüm'}
}

# ============================================
# FEATURE ENGINEERING CONSTANTS
# ============================================

# Time-series window parameters
WINDOW_SIZE = 100       # Data points per window
WINDOW_STRIDE = 10      # Overlap/stride
SAMPLING_RATE = 1000    # Hz (1000 points per second)
NYQUIST_FREQUENCY = SAMPLING_RATE / 2

# Statistical features
STATISTICAL_FEATURES = [
    'mean', 'std', 'min', 'max', 'median',
    'q25', 'q75', 'skewness', 'kurtosis'
]

# Impedance features
IMPEDANCE_FEATURES = [
    'dcr',  # DC Resistance
    'acr_100hz',  # AC Resistance at 100Hz
    'acr_1khz',   # AC Resistance at 1kHz
    'phase_angle'  # Phase angle
]

# ============================================
# DATA PROCESSING CONSTANTS
# ============================================

# Normalization methods
NORMALIZE_STANDARDIZE = 'standardize'  # (x - mean) / std
NORMALIZE_MINMAX = 'minmax'            # (x - min) / (max - min)
NORMALIZE_LOG = 'log'                  # log transformation

# Missing data handling
MISSING_DATA_THRESHOLD = 0.20  # 20% hoşgörülü
MISSING_DATA_METHOD = 'interpolate'  # interpolate, forward_fill, drop

# Outlier detection
OUTLIER_METHOD = 'iqr'  # iqr, zscore
OUTLIER_IQR_MULTIPLIER = 1.5
OUTLIER_ZSCORE_THRESHOLD = 3.0

# ============================================
# MODEL PARAMETERS
# ============================================

# Random Forest
RF_N_ESTIMATORS = 100
RF_MAX_DEPTH = 20
RF_MIN_SAMPLES_SPLIT = 5
RF_MIN_SAMPLES_LEAF = 2
RF_RANDOM_STATE = 42

# XGBoost
XGB_N_ESTIMATORS = 100
XGB_MAX_DEPTH = 6
XGB_LEARNING_RATE = 0.1
XGB_RANDOM_STATE = 42

# Neural Network
NN_LAYERS = [128, 64, 32]
NN_DROPOUT = 0.3
NN_EPOCHS = 100
NN_BATCH_SIZE = 32
NN_LEARNING_RATE = 0.001
NN_EARLY_STOPPING_PATIENCE = 10

# Model accuracy targets
TARGET_ACCURACY = 0.90  # 90% doğruluk hedefi
TARGET_R2 = 0.85        # R² score hedefi (0-1 arası)
TARGET_RMSE = 0.05      # Root Mean Square Error hedefi

# ============================================
# BMS PARAMETERS
# ============================================

# Battery Management System thresholds
BMS_MAX_VOLTAGE = 4.2          # V - Maximum charging voltage
BMS_MIN_VOLTAGE = 2.5          # V - Minimum discharging voltage
BMS_MAX_CHARGE_CURRENT = 5.0   # A
BMS_MAX_DISCHARGE_CURRENT = 10.0  # A

# Temperature limits
BMS_MAX_TEMPERATURE = 60        # °C - Maksimum işletme sıcaklığı
BMS_MIN_TEMPERATURE = 0         # °C - Minimum işletme sıcaklığı
BMS_SHUTDOWN_TEMPERATURE = 80   # °C - Şiddetli koşulda kapat

# Cell balancing
BMS_BALANCING_ENABLED = True
BMS_BALANCING_THRESHOLD = 0.05  # 50 mV (hücreler arasında max fark)
BMS_BALANCING_CURRENT = 0.5     # A - Dengeleme akımı
BMS_ALERT_THRESHOLD = 0.10      # 100 mV - Uyarı eşiği

# Monitor settings
BMS_MONITOR_INTERVAL = 1.0  # seconds - Kontrol aralığı
BMS_LOG_INTERVAL = 10.0     # seconds - Log interval

# ============================================
# CLASSIFICATION THRESHOLDS
# ============================================

# Battery classification
REUSE_THRESHOLD = 0.85      # >85% SoH - İkinci ömür kullanımı
RECYCLE_THRESHOLD = 0.70    # <70% SoH - Geri dönüşüm
RESEARCH_THRESHOLD = 0.50   # 50-70% SoH - Araştırma amaçlı

# Battery types
BATTERY_TYPES = {
    'NCM811': {'chemistry': 'LiCoO2', 'nominal_capacity': 2500},
    'NCA': {'chemistry': 'Lithium Nickel Cobalt Aluminum', 'nominal_capacity': 2600},
    'LFP': {'chemistry': 'Lithium Iron Phosphate', 'nominal_capacity': 2400}
}

# ============================================
# ANALYSIS PARAMETERS
# ============================================

# Time series analysis
MIN_DATA_POINTS = 50        # Minimum required data points
MAX_ANALYSIS_TIME = 5.0     # seconds - Maksimum analiz süresi
MIN_ANALYSIS_CYCLES = 10    # Minimum full cycles for reliability

# Statistical analysis
CONFIDENCE_LEVEL = 0.95     # 95% confidence
ALPHA = 0.05                # Significance level

# ============================================
# PERFORMANCE METRICS
# ============================================

# Target metrics for deliverables
METRIC_SOH_PREDICTION_ACCURACY = 0.90  # 90%
METRIC_CLASSIFICATION_ACCURACY = 0.85   # 85%
METRIC_ANALYSIS_TIME_TARGET = 5.0       # 5 minutes per battery
METRIC_RECOVERY_EFFICIENCY = 0.10       # 10% improvement

# ============================================
# FILE FORMATS
# ============================================

SUPPORTED_INPUT_FORMATS = ['csv', 'xlsx', 'json', 'parquet']
SUPPORTED_OUTPUT_FORMATS = ['csv', 'json', 'xlsx', 'html']

# File naming conventions
MODEL_FILE_FORMAT = "{model_type}_{timestamp}.pkl"
DATA_FILE_FORMAT = "{battery_id}_{timestamp}.csv"
REPORT_FILE_FORMAT = "report_{battery_id}_{timestamp}.html"

# ============================================
# LOGGING LEVELS
# ============================================

LOG_LEVELS = {
    'DEBUG': 10,
    'INFO': 20,
    'WARNING': 30,
    'ERROR': 40,
    'CRITICAL': 50
}

# ============================================
# MESSAGES
# ============================================

MESSAGES = {
    'safety_alert': '⚠️  SAFETYalert: {reason}',
    'success': '✅ Success: {message}',
    'error': '❌ Error: {message}',
    'info': 'ℹ️  Info: {message}',
    'warning': '⚠️  Warning: {message}'
}

# ============================================
# CONVERSION FACTORS
# ============================================

# Time conversions
SECONDS_TO_MINUTES = 1 / 60
MINUTES_TO_HOURS = 1 / 60
HOURS_TO_DAYS = 1 / 24

# Voltage conversions
MV_TO_V = 1 / 1000
MV_TO_MV = 1

# Current conversions
MA_TO_A = 1 / 1000
MA_TO_MA = 1

# Capacity
MAH_TO_AH = 1 / 1000
WH_TO_WH = 1

# ============================================
# CONSTANTS FOR CALCULATIONS
# ============================================

# Peukert's equation (battery discharge rate)
PEUKERT_EXPONENT = 1.2  # Typical for Li-ion

# Coulomb counting (charge/discharge integration)
COULOMB_EFFICIENCY = 0.95  # 95% efficiency

# Temperature coefficient
TEMPERATURE_COEFFICIENT = -0.003  # per °C
