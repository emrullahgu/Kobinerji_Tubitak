"""
TUBITAK 7260634 - Test Suite
Tüm modüllerin birim testleri
"""

import pytest
import numpy as np
import pandas as pd
from datetime import datetime
import tempfile
import os

# İthalatlar
from python.utils.constants import *
from python.utils.logger import get_logger
from python.data_processing.battery_data import BatteryData
from python.data_processing.feature_extraction import FeatureExtractor
from python.models.soh_model import SoHModel
from python.bms.battery_manager import BatteryMonitor, CellBalancer
from python.bms.bms_controller import BMSController


# ============================================
# FIXTURES
# ============================================

@pytest.fixture
def sample_battery_data():
    """Örnek batarya verileri"""
    n_samples = 1000
    data = {
        'cell_id': np.repeat(range(8), n_samples // 8),
        'voltage': np.random.uniform(2.5, 4.2, n_samples),
        'current': np.random.uniform(-20, 20, n_samples),
        'temperature': np.random.uniform(0, 60, n_samples),
        'soc': np.random.uniform(10, 100, n_samples),
        'soh': np.random.uniform(50, 100, n_samples),
        'impedance': np.random.uniform(30, 100, n_samples),
        'timestamp': [datetime.now()] * n_samples
    }
    return pd.DataFrame(data)


@pytest.fixture
def battery_monitor():
    """Batarya izleme sistemi"""
    return BatteryMonitor('BAT_TEST', num_cells=8)


@pytest.fixture
def bms_controller():
    """BMS kontrolü"""
    return BMSController('BAT_TEST', num_cells=8)


# ============================================
# TEST: Battery Data Processing
# ============================================

class TestBatteryData:
    """Batarya veri işleme testleri"""
    
    def test_create_sample_data(self):
        """Örnek veri oluşturma testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=100, noise_level=0.05)
        
        assert battery.data is not None
        assert len(battery.data) == 100
        assert 'voltage' in battery.data.columns
    
    def test_validate_data(self):
        """Veri doğrulama testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=100)
        
        is_valid = battery.validate_data()
        assert is_valid is True
    
    def test_clean_data(self):
        """Veri temizleme testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=100)
        
        original_len = len(battery.data)
        battery.clean_data()
        
        assert battery.data is not None
        assert len(battery.data) <= original_len
    
    def test_normalize_data(self):
        """Veri normalleştirme testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=100)
        battery.clean_data()
        
        battery.normalize_data(method='standardize')
        
        # Standartlaştırılmış veriler 0 etrafında olmalı
        numeric_cols = battery.data.select_dtypes(include=[np.number]).columns
        for col in numeric_cols:
            if col.startswith('voltage') or col.startswith('current'):
                mean_val = battery.data[col].mean()
                assert abs(mean_val) < 2  # Makul tolerans
    
    def test_save_and_load(self):
        """Kaydetme ve yükleme testi"""
        with tempfile.TemporaryDirectory() as tmpdir:
            battery = BatteryData('BAT_001')
            battery.create_sample_data(n_samples=100)
            battery.clean_data()
            
            # CSV'ye kaydet
            csv_path = os.path.join(tmpdir, 'test.csv')
            battery.save_cleaned_data(filepath=csv_path, format='csv')
            
            assert os.path.exists(csv_path)
            
            # Yükle
            loaded = pd.read_csv(csv_path)
            assert len(loaded) == len(battery.data)


# ============================================
# TEST: Feature Extraction
# ============================================

class TestFeatureExtraction:
    """Özellik çıkarma testleri"""
    
    def test_create_sliding_windows(self):
        """Kaydırmalı pencere oluşturma testi"""
        extractor = FeatureExtractor(window_size=100, window_stride=10)
        
        data = pd.DataFrame({
            'voltage': np.random.uniform(2.5, 4.2, 1000),
            'current': np.random.uniform(-20, 20, 1000),
            'temperature': np.random.uniform(0, 60, 1000)
        })
        
        windows = extractor.create_sliding_windows(data, 'voltage')
        
        assert len(windows) > 0
        assert windows[0].shape[0] == 100
    
    def test_extract_all_features(self):
        """Tüm özellikleri çıkarma testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=2000)
        battery.clean_data()
        
        extractor = FeatureExtractor()
        features = extractor.extract_all_features(battery.data)
        
        assert features is not None
        assert isinstance(features, (pd.DataFrame, np.ndarray))
    
    def test_normalize_features(self):
        """Özellik normalleştirme testi"""
        battery = BatteryData('BAT_001')
        battery.create_sample_data(n_samples=2000)
        
        extractor = FeatureExtractor()
        features = extractor.extract_all_features(battery.data)
        
        normalized = extractor.normalize_features(features, fit=True)
        
        assert normalized is not None
        assert normalized.shape == features.shape


# ============================================
# TEST: SOH Model
# ============================================

class TestSoHModel:
    """SOH model testleri"""
    
    def test_model_training(self):
        """Model eğitim testi"""
        # Eğitim verileri oluştur
        X_train = np.random.randn(500, 50)  # 500 örnekli, 50 özelliki
        y_train = np.random.uniform(50, 100, 500)
        
        # Model oluştur ve eğit
        model = SoHModel('random_forest')
        metrics = model.train(X_train, y_train)
        
        assert metrics is not None
        assert 'r2' in metrics or 'R2' in metrics
        assert model.model is not None
    
    def test_model_prediction(self):
        """Model tahmin testi"""
        # Eğitim verileri
        X_train = np.random.randn(500, 50)
        y_train = np.random.uniform(50, 100, 500)
        
        # Test verileri
        X_test = np.random.randn(100, 50)
        
        # Model oluştur ve eğit
        model = SoHModel('random_forest')
        model.train(X_train, y_train)
        
        # Tahmin yap
        predictions = model.predict(X_test)
        
        assert predictions is not None
        assert len(predictions) == 100
        assert all(50 <= p <= 100 for p in predictions)
    
    def test_single_prediction(self):
        """Tek tahmin testi"""
        # Eğitim
        X_train = np.random.randn(500, 50)
        y_train = np.random.uniform(50, 100, 500)
        
        model = SoHModel('random_forest')
        model.train(X_train, y_train)
        
        # Tek tahmin
        single_features = np.random.randn(1, 50)
        prediction = model.predict_single(single_features[0])
        
        assert prediction is not None
        assert 'soh' in prediction
        assert 'status' in prediction  # REUSE, RECYCLE, CRITICAL
        assert prediction['status'] in ['REUSE', 'RECYCLE', 'CRITICAL']
    
    def test_model_persistence(self):
        """Model kaydetme/yükleme testi"""
        with tempfile.TemporaryDirectory() as tmpdir:
            # Eğitim ve kaydetme
            X_train = np.random.randn(500, 50)
            y_train = np.random.uniform(50, 100, 500)
            
            model1 = SoHModel('random_forest')
            model1.train(X_train, y_train)
            
            model_path = os.path.join(tmpdir, 'model.pkl')
            model1.save_model(model_path)
            
            # Yükleme
            model2 = SoHModel('random_forest')
            model2.load_model(model_path)
            
            # Test tahminleri eşleşiyor mu?
            X_test = np.random.randn(10, 50)
            pred1 = model1.predict(X_test)
            pred2 = model2.predict(X_test)
            
            np.testing.assert_array_almost_equal(pred1, pred2, decimal=3)


# ============================================
# TEST: Battery Monitor
# ============================================

class TestBatteryMonitor:
    """Batarya izleme testleri"""
    
    def test_update_cell_data(self, battery_monitor):
        """Hücre verileri güncelleme testi"""
        result = battery_monitor.update_cell_data(
            cell_id=0,
            voltage=3.7,
            current=5.0,
            temperature=25.0,
            soc=50.0,
            soh=95.0,
            impedance=50.0
        )
        
        assert result is True
        assert battery_monitor.cell_data[0] is not None
    
    def test_overvoltage_detection(self, battery_monitor):
        """Aşırı gerilim algılaması testi"""
        battery_monitor.update_cell_data(
            cell_id=0,
            voltage=4.5,  # Aşırı
            current=5.0,
            temperature=25.0,
            soc=50.0,
            soh=95.0,
            impedance=50.0
        )
        
        result = battery_monitor.check_overvoltage(0)
        assert result is True
        assert len(battery_monitor.alerts) > 0
    
    def test_monitoring(self, battery_monitor):
        """Tam izleme testi"""
        # Verileri ekle
        for i in range(8):
            battery_monitor.update_cell_data(
                cell_id=i,
                voltage=3.7,
                current=5.0,
                temperature=25.0,
                soc=50.0,
                soh=95.0,
                impedance=50.0
            )
        
        # İzle
        battery_monitor.monitor()
        
        assert battery_monitor.monitor_count == 1
    
    def test_pack_metrics(self, battery_monitor):
        """Pack metrikleri testi"""
        for i in range(8):
            battery_monitor.update_cell_data(
                cell_id=i,
                voltage=3.7,
                current=5.0,
                temperature=25.0,
                soc=50.0,
                soh=95.0,
                impedance=50.0
            )
        
        assert battery_monitor.get_pack_voltage() > 0
        assert battery_monitor.get_pack_current() > 0
        assert battery_monitor.get_pack_temperature() > 0


# ============================================
# TEST: Cell Balancer
# ============================================

class TestCellBalancer:
    """Hücre dengeleyici testleri"""
    
    def test_balancing_calculation(self, battery_monitor):
        """Dengeleme hesaplaması testi"""
        # Dengesiz hücreler
        for i in range(8):
            voltage = 3.7 + (i * 0.05)  # Yükselen voltaj
            battery_monitor.update_cell_data(
                cell_id=i,
                voltage=voltage,
                current=5.0,
                temperature=25.0,
                soc=50.0,
                soh=95.0,
                impedance=50.0
            )
        
        balancer = CellBalancer(battery_monitor)
        
        # Dengeleme gerekli mi?
        assert balancer.is_balancing_needed() is True
    
    def test_balanced_system(self, battery_monitor):
        """Dengeli sistem testi"""
        # Dikkat: dengeleme gerekli değil
        for i in range(8):
            battery_monitor.update_cell_data(
                cell_id=i,
                voltage=3.7,
                current=5.0,
                temperature=25.0,
                soc=50.0,
                soh=95.0,
                impedance=50.0
            )
        
        balancer = CellBalancer(battery_monitor)
        
        # Dengeleme gerekmez
        assert balancer.is_balancing_needed() is False


# ============================================
# TEST: BMS Controller
# ============================================

class TestBMSController:
    """BMS kontrolü testleri"""
    
    def test_initialization(self, bms_controller):
        """İlk ayarlama testi"""
        assert bms_controller.battery_id == 'BAT_TEST'
        assert bms_controller.num_cells == 8
        assert bms_controller.monitor is not None
    
    def test_update_batteries(self, bms_controller, sample_battery_data):
        """Batarya güncelleme testi"""
        result = bms_controller.update_batteries(sample_battery_data)
        
        assert result is True
    
    def test_monitoring_execution(self, bms_controller, sample_battery_data):
        """İzleme yürütme testi"""
        bms_controller.update_batteries(sample_battery_data)
        
        result = bms_controller.execute_monitoring()
        
        assert result is not None
        assert 'alerts_found' in result
    
    def test_charge_discharge(self, bms_controller, sample_battery_data):
        """Şarj/deşarj testi"""
        bms_controller.update_batteries(sample_battery_data)
        
        # Şarj
        result = bms_controller.process_charge_discharge(5000, 3600)
        assert result['status'] in ['allowed', 'rejected']
    
    def test_system_health(self, bms_controller, sample_battery_data):
        """Sistem sağlığı testi"""
        bms_controller.update_batteries(sample_battery_data)
        
        health = bms_controller.get_system_health()
        
        assert health is not None
        assert 'pack_voltage' in health
        assert 'pack_soc' in health
        assert 'system_state' in health
    
    def test_generate_report(self, bms_controller, sample_battery_data):
        """Rapor oluşturma testi"""
        bms_controller.update_batteries(sample_battery_data)
        bms_controller.execute_monitoring()
        
        report = bms_controller.generate_report()
        
        assert report is not None
        assert 'system_health' in report
        assert 'alerts_summary' in report


# ============================================
# TEST: Constants
# ============================================

class TestConstants:
    """Sabit testleri"""
    
    def test_battery_parameters(self):
        """Batarya parametreleri testi"""
        assert NOMINAL_VOLTAGE > 0
        assert NOMINAL_CAPACITY > 0
        assert BMS_MAX_VOLTAGE > BMS_MIN_VOLTAGE
    
    def test_soh_thresholds(self):
        """SOH eşikleri testi"""
        assert SOH_EXCELLENT > SOH_GOOD > SOH_FAIR > SOH_POOR > SOH_CRITICAL
    
    def test_bms_limits(self):
        """BMS limitleri testi"""
        assert BMS_MAX_CHARGE_CURRENT > 0
        assert BMS_MAX_DISCHARGE_CURRENT > 0
        assert BMS_MAX_TEMPERATURE > BMS_MIN_TEMPERATURE


# ============================================
# RUN TESTS
# ============================================

if __name__ == '__main__':
    pytest.main([__file__, '-v', '--tb=short'])
