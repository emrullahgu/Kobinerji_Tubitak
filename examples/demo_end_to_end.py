"""
TUBITAK 7260634 - End-to-End Demonstration
Tam sistem kullanım örneği ve workflow
"""

import numpy as np
import pandas as pd
from datetime import datetime, timedelta
import sys
import os

# Python path ekleme
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from python.data_processing.battery_data import BatteryData
from python.data_processing.feature_extraction import FeatureExtractor
from python.models.soh_model import SoHModel
from python.bms.bms_controller import BMSController
from python.api.battery_api import BatteryAPIWrapper
from python.utils.logger import get_logger
from python.utils.constants import *


# ============================================
# DEMONSTRATION: End-to-End Battery Analysis
# ============================================

def demo_data_processing():
    """
    Aşama 1: Veri Yükleme ve İşleme
    Stage 1: Data Loading and Processing
    """
    print("\n" + "="*60)
    print("STAGE 1: DATA PROCESSING")
    print("="*60)
    
    logger = get_logger('demo')
    
    # Örnek batarya verileri oluştur
    print("\n[1] Creating sample battery data...")
    battery = BatteryData('BAT_DEMO_001')
    battery.create_sample_data(n_samples=2000, noise_level=0.08)
    print(f"   ✓ Created {len(battery.data)} samples")
    
    # Verilerini doğrula
    print("\n[2] Validating data...")
    is_valid = battery.validate_data()
    print(f"   ✓ Data validation: {'PASSED' if is_valid else 'FAILED'}")
    
    # Verileri temizle
    print("\n[3] Cleaning data...")
    before = len(battery.data)
    battery.clean_data()
    after = len(battery.data)
    print(f"   ✓ Removed {before - after} invalid samples")
    
    # Verileri normalleştir
    print("\n[4] Normalizing data...")
    battery.normalize_data(method='standardize')
    print(f"   ✓ Data standardization complete")
    
    # İstatistikler al
    print("\n[5] Computing statistics...")
    stats = battery.get_statistics()
    print(f"   ✓ Mean voltage: {stats['voltage'][0]:.3f}V")
    print(f"   ✓ Mean current: {stats['current'][0]:.2f}A")
    print(f"   ✓ Mean temperature: {stats['temperature'][0]:.1f}°C")
    
    return battery


def demo_feature_extraction(battery):
    """
    Aşama 2: Özellik Çıkarma
    Stage 2: Feature Extraction
    """
    print("\n" + "="*60)
    print("STAGE 2: FEATURE EXTRACTION")
    print("="*60)
    
    # Feature extractor oluştur
    print("\n[1] Initializing feature extractor...")
    extractor = FeatureExtractor(window_size=100, window_stride=10)
    print(f"   ✓ Window size: {extractor.window_size}")
    print(f"   ✓ Stride: {extractor.window_stride}")
    
    # Özellikleri çıkar
    print("\n[2] Extracting features...")
    features = extractor.extract_all_features(battery.data)
    print(f"   ✓ Extracted {features.shape[0]} feature vectors")
    print(f"   ✓ Feature dimension: {features.shape[1]}")
    
    # Özellik normalleştir
    print("\n[3] Normalizing features...")
    features_norm = extractor.normalize_features(features, fit=True)
    print(f"   ✓ Feature normalization complete")
    
    # Örnek özellikleri göster
    print("\n[4] Sample features (first 10 dimensions):")
    for i, val in enumerate(features_norm[0, :10]):
        print(f"   Feature {i+1}: {val:.4f}")
    
    return features_norm


def demo_model_training(features):
    """
    Aşama 3: Model Eğitimi
    Stage 3: Model Training
    """
    print("\n" + "="*60)
    print("STAGE 3: MODEL TRAINING")
    print("="*60)
    
    # Hedef değişken oluştur
    print("\n[1] Preparing training data...")
    np.random.seed(42)
    y = np.random.uniform(50, 100, len(features))
    
    # Train/test split
    from sklearn.model_selection import train_test_split
    X_train, X_test, y_train, y_test = train_test_split(
        features, y, test_size=0.2, random_state=42
    )
    print(f"   ✓ Training samples: {len(X_train)}")
    print(f"   ✓ Test samples: {len(X_test)}")
    
    # Model oluştur ve eğit
    print("\n[2] Training Random Forest model...")
    model = SoHModel('random_forest')
    metrics = model.train(X_train, y_train)
    
    print(f"   ✓ Model training complete")
    print(f"   ✓ R² Score: {metrics.get('r2', 'N/A'):.4f}") if 'r2' in metrics else None
    
    # Model değerlendirme
    print("\n[3] Evaluating model...")
    eval_metrics = model.evaluate(X_test, y_test)
    print(f"   ✓ Test R² Score: {eval_metrics.get('r2', 'N/A'):.4f}") if 'r2' in eval_metrics else None
    print(f"   ✓ RMSE: {eval_metrics.get('rmse', 'N/A'):.4f}") if 'rmse' in eval_metrics else None
    
    # Feature importance
    print("\n[4] Feature importance (top 5):")
    importance = model.get_feature_importance(top_n=5)
    for i, (feat, imp) in enumerate(importance, 1):
        print(f"   {i}. Feature {feat}: {imp:.4f}")
    
    return model, X_test, y_test


def demo_soh_prediction(model, features):
    """
    Aşama 4: SOH Tahmini
    Stage 4: SOH Prediction
    """
    print("\n" + "="*60)
    print("STAGE 4: SOH PREDICTION")
    print("="*60)
    
    # Toplu tahmin
    print("\n[1] Batch prediction...")
    predictions = model.predict(features[:10])
    print(f"   ✓ Predicted SOH values (first 10):")
    for i, pred in enumerate(predictions, 1):
        print(f"   Sample {i}: {pred:.2f}%")
    
    # Tek tahmin
    print("\n[2] Single battery prediction...")
    single_pred = model.predict_single(features[0])
    print(f"   ✓ SOH: {single_pred['soh']:.2f}%")
    print(f"   ✓ Status: {single_pred['status']}")
    print(f"   ✓ Confidence: {single_pred.get('confidence', 'N/A')}")
    
    return predictions


def demo_bms_control(battery_data):
    """
    Aşama 5: BMS Kontrol Sistemi
    Stage 5: BMS Control System
    """
    print("\n" + "="*60)
    print("STAGE 5: BMS CONTROL SYSTEM")
    print("="*60)
    
    # BMS Controller oluştur
    print("\n[1] Initializing BMS controller...")
    controller = BMSController('BAT_DEMO_001', num_cells=8)
    print(f"   ✓ Battery ID: {controller.battery_id}")
    print(f"   ✓ Number of cells: {controller.num_cells}")
    
    # Batarya veraileri hazırla
    print("\n[2] Preparing battery data...")
    cell_ids = list(range(8))
    voltages = [3.7 + np.random.normal(0, 0.05) for _ in cell_ids]
    currents = [5.0 + np.random.normal(0, 0.2) for _ in cell_ids]
    temperatures = [25 + np.random.normal(0, 1) for _ in cell_ids]
    socs = [50 + np.random.normal(0, 5) for _ in cell_ids]
    sohs = [95 + np.random.normal(0, 3) for _ in cell_ids]
    impedances = [50 + np.random.normal(0, 2) for _ in cell_ids]
    
    data = pd.DataFrame({
        'cell_id': cell_ids,
        'voltage': voltages,
        'current': currents,
        'temperature': temperatures,
        'soc': socs,
        'soh': sohs,
        'impedance': impedances
    })
    
    controller.update_batteries(data)
    print(f"   ✓ Battery data loaded ({len(data)} cells)")
    
    # İzleme
    print("\n[3] Executing monitoring...")
    monitor_result = controller.execute_monitoring()
    print(f"   ✓ Alerts found: {monitor_result['alerts_found']}")
    print(f"   ✓ System state: {monitor_result['system_state']}")
    
    # Permissionlar
    print("\n[4] Checking charge/discharge permissions...")
    charge_ok = controller.allow_charging()
    discharge_ok = controller.allow_discharging()
    print(f"   ✓ Charging allowed: {charge_ok}")
    print(f"   ✓ Discharging allowed: {discharge_ok}")
    
    # Şarj işlemi
    print("\n[5] Processing charge operation...")
    charge_result = controller.process_charge_discharge(5000, 3600)
    print(f"   ✓ Status: {charge_result['status']}")
    if charge_result['status'] == 'allowed':
        print(f"   ✓ Energy transferred: {charge_result['energy_kwh']:.2f} kWh")
    
    # SOH tahmini
    print("\n[6] Estimating SOH...")
    soh_estimate = controller.estimate_soh()
    print(f"   ✓ SOH: {soh_estimate['soh']:.2f}%")
    
    # Karar
    print("\n[7] Making decision...")
    decision = controller.make_decision(soh_estimate['soh'])
    print(f"   ✓ Decision: {decision['decision']}")
    print(f"   ✓ Reason: {decision['reason']}")
    print(f"   ✓ Destination: {decision['destination']}")
    
    # Sistem raporu
    print("\n[8] System health report...")
    report = controller.generate_report()
    print(f"   ✓ Pack voltage: {report['health_metrics']['pack_voltage']:.2f}V")
    print(f"   ✓ Pack current: {report['health_metrics']['pack_current']:.2f}A")
    print(f"   ✓ Pack temperature: {report['health_metrics']['pack_temperature']:.1f}°C")
    print(f"   ✓ Voltage imbalance: {report['health_metrics']['voltage_imbalance']*1000:.1f}mV")
    
    return controller


def demo_api_integration():
    """
    Aşama 6: Web API Entegrasyonu
    Stage 6: Web API Integration
    """
    print("\n" + "="*60)
    print("STAGE 6: WEB API INTEGRATION")
    print("="*60)
    
    print("\n[1] Initializing API wrapper...")
    api = BatteryAPIWrapper(debug=False)
    print(f"   ✓ API wrapper initialized")
    
    print("\n[2] Available endpoints:")
    endpoints = [
        "GET  /api/health",
        "POST /api/battery/init",
        "POST /api/battery/<id>/update",
        "POST /api/battery/<id>/monitor",
        "POST /api/battery/<id>/predict_soh",
        "POST /api/battery/<id>/decide",
        "POST /api/battery/<id>/charge",
        "POST /api/battery/<id>/discharge",
        "GET  /api/battery/<id>/status",
        "POST /api/features/extract",
        "POST /api/model/train",
        "POST /api/model/<type>/predict",
        "GET  /api/model/<type>/info"
    ]
    
    for endpoint in endpoints:
        print(f"   • {endpoint}")
    
    print("\n[3] API is ready to run on http://localhost:5000")
    print("   Run: python api/battery_api.py")


# ============================================
# MAIN EXECUTION
# ============================================

if __name__ == '__main__':
    print("\n" + "="*60)
    print("TUBITAK 7260634 - END-TO-END DEMONSTRATION")
    print("Battery Analysis AI System")
    print("="*60)
    
    try:
        # Aşama 1: Veri İşleme
        battery = demo_data_processing()
        
        # Aşama 2: Özellik Çıkarma
        features = demo_feature_extraction(battery)
        
        # Aşama 3: Model Eğitimi
        model, X_test, y_test = demo_model_training(features)
        
        # Aşama 4: SOH Tahmini
        predictions = demo_soh_prediction(model, features)
        
        # Aşama 5: BMS Kontrol Sistemi
        controller = demo_bms_control(battery.data)
        
        # Aşama 6: Web API Entegrasyonu
        demo_api_integration()
        
        print("\n" + "="*60)
        print("DEMONSTRATION COMPLETE")
        print("="*60)
        print("\nNext steps:")
        print("1. Run test suite: pytest tests/test_suite.py")
        print("2. Start API server: python python/api/battery_api.py")
        print("3. Open web dashboard: index.html")
        print("="*60 + "\n")
        
    except Exception as e:
        print(f"\n❌ Error during demonstration: {e}")
        import traceback
        traceback.print_exc()
