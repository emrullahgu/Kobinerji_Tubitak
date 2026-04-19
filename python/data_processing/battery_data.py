"""
TUBITAK 7260634 - Battery Data Processing Module
Batarya verilerini yükleme, temizleme ve işleme
"""

import numpy as np
import pandas as pd
from pathlib import Path
from datetime import datetime
import warnings

from utils.constants import (
    RAW_DATA_DIR, PROCESSED_DATA_DIR,
    MISSING_DATA_THRESHOLD, MISSING_DATA_METHOD,
    OUTLIER_METHOD, OUTLIER_IQR_MULTIPLIER, OUTLIER_ZSCORE_THRESHOLD,
    NORMALIZE_STANDARDIZE, NORMALIZE_MINMAX,
    MIN_VOLTAGE, MAX_VOLTAGE, NOMINAL_CAPACITY
)
from utils.logger import get_logger

warnings.filterwarnings('ignore')


class BatteryData:
    """Batarya verileri sınıfı"""
    
    def __init__(self, battery_id, data=None):
        """
        Initialize BatteryData
        
        Parameters:
        -----------
        battery_id : str
            Batarya ID'si (e.g., 'BAT_001')
        data : pd.DataFrame, optional
            Batarya verisi
        """
        self.battery_id = battery_id
        self.data = data if data is not None else pd.DataFrame()
        self.logger = get_logger('battery_data')
        self.metadata = {
            'battery_id': battery_id,
            'created_at': datetime.now(),
            'samples_raw': 0,
            'samples_cleaned': 0,
            'samples_normalized': 0
        }
    
    def load_from_csv(self, filepath):
        """CSV dosyasından batarya verisi yükle"""
        try:
            self.data = pd.read_csv(filepath)
            self.metadata['samples_raw'] = len(self.data)
            self.logger.info(f"Loaded {len(self.data)} samples from {filepath}")
            
            # Zaman sütununu datetime'a dönüştür
            if 'timestamp' in self.data.columns:
                self.data['timestamp'] = pd.to_datetime(self.data['timestamp'])
            
            return True
        except Exception as e:
            self.logger.error(f"Error loading CSV: {e}")
            return False
    
    def load_from_excel(self, filepath, sheet_name=0):
        """Excel dosyasından batarya verisi yükle"""
        try:
            self.data = pd.read_excel(filepath, sheet_name=sheet_name)
            self.metadata['samples_raw'] = len(self.data)
            self.logger.info(f"Loaded {len(self.data)} samples from {filepath}")
            return True
        except Exception as e:
            self.logger.error(f"Error loading Excel: {e}")
            return False
    
    def create_sample_data(self, n_samples=1000, noise_level=0.05):
        """Test amacıyla örnek veri oluştur"""
        """
        Simulate battery cycling test data
        
        Parameters:
        -----------
        n_samples : int
            Veri noktası sayısı
        noise_level : float
            Gürültü seviyesi (0-1)
        """
        timestamps = pd.date_range(start='2026-01-01', periods=n_samples, freq='S')
        
        # Sinüsoidal pattern ile gerçekçi gerilim profili
        cycle_position = np.linspace(0, 4*np.pi, n_samples)
        voltage = 3.7 + 0.5*np.sin(cycle_position) + np.random.normal(0, noise_level*0.5, n_samples)
        voltage = np.clip(voltage, MIN_VOLTAGE, MAX_VOLTAGE)
        
        # Current (şarj/deşarj döngüleri)
        current = 2.0*np.sin(cycle_position) + np.random.normal(0, noise_level*0.2, n_samples)
        
        # Temperature (ambient + self-heating)
        temperature = 25 + 0.3*abs(current) + np.random.normal(0, noise_level, n_samples)
        temperature = np.clip(temperature, 0, 80)
        
        # Kapasitesi (time-dependent degradation)
        capacity = NOMINAL_CAPACITY * (1 - 0.0001*np.arange(n_samples) + np.random.normal(0, noise_level*10, n_samples))
        capacity = np.clip(capacity, 0, NOMINAL_CAPACITY)
        
        # İç direnç (degrades with time)
        impedance = 50 + 0.01*np.arange(n_samples) + np.random.normal(0, noise_level*2, n_samples)
        impedance = np.clip(impedance, 40, 200)
        
        self.data = pd.DataFrame({
            'timestamp': timestamps,
            'voltage': voltage,
            'current': current,
            'temperature': temperature,
            'capacity': capacity,
            'impedance': impedance,
            'soc': 50 + 40*np.sin(cycle_position) + np.random.normal(0, 5, n_samples)  # State of Charge
        })
        
        # Baseline SoH (decreases over time)
        self.data['soh'] = 100 - 0.01*np.arange(n_samples)
        self.data['soh'] = np.clip(self.data['soh'], 50, 100)
        
        self.metadata['samples_raw'] = len(self.data)
        self.logger.info(f"Created {n_samples} sample data points")
        
        return self.data
    
    def validate_data(self):
        """Veri geçerliliğini kontrol et"""
        if self.data.empty:
            self.logger.warning("Data is empty!")
            return False
        
        required_columns = ['voltage', 'current', 'temperature', 'capacity']
        missing_columns = [col for col in required_columns if col not in self.data.columns]
        
        if missing_columns:
            self.logger.warning(f"Missing columns: {missing_columns}")
            return False
        
        return True
    
    def remove_duplicates(self):
        """Kopya satırları kaldır"""
        initial_rows = len(self.data)
        self.data = self.data.drop_duplicates()
        removed = initial_rows - len(self.data)
        if removed > 0:
            self.logger.info(f"Removed {removed} duplicate rows")
        return removed
    
    def handle_missing_values(self, method=MISSING_DATA_METHOD):
        """Eksik verileri işle"""
        missing_count = self.data.isnull().sum().sum()
        missing_percentage = (missing_count / (len(self.data) * len(self.data.columns))) * 100
        
        if missing_percentage > MISSING_DATA_THRESHOLD * 100:
            self.logger.warning(f"High missing data: {missing_percentage:.2f}%")
            return False
        
        if missing_count == 0:
            self.logger.info("No missing values found")
            return True
        
        if method == 'interpolate':
            self.data = self.data.interpolate(method='linear', limit_direction='both')
        elif method == 'forward_fill':
            self.data = self.data.fillna(method='ffill').fillna(method='bfill')
        elif method == 'drop':
            self.data = self.data.dropna()
        
        self.logger.info(f"Handled {missing_count} missing values using {method}")
        return True
    
    def remove_outliers(self, method=OUTLIER_METHOD, threshold=None):
        """Aykırı değerleri kaldır"""
        initial_rows = len(self.data)
        
        numeric_columns = self.data.select_dtypes(include=[np.number]).columns
        
        for col in numeric_columns:
            if method == 'iqr':
                Q1 = self.data[col].quantile(0.25)
                Q3 = self.data[col].quantile(0.75)
                IQR = Q3 - Q1
                lower_bound = Q1 - OUTLIER_IQR_MULTIPLIER * IQR
                upper_bound = Q3 + OUTLIER_IQR_MULTIPLIER * IQR
                self.data = self.data[(self.data[col] >= lower_bound) & (self.data[col] <= upper_bound)]
            
            elif method == 'zscore':
                z_scores = np.abs((self.data[col] - self.data[col].mean()) / self.data[col].std())
                self.data = self.data[z_scores < OUTLIER_ZSCORE_THRESHOLD]
        
        removed = initial_rows - len(self.data)
        if removed > 0:
            self.logger.info(f"Removed {removed} outlier rows using {method}")
        
        return removed
    
    def normalize_data(self, method=NORMALIZE_STANDARDIZE, columns=None):
        """Verileri normalleştir"""
        if columns is None:
            columns = self.data.select_dtypes(include=[np.number]).columns
        
        for col in columns:
            if col not in self.data.columns:
                continue
            
            if method == 'standardize':
                # Z-score normalization
                mean = self.data[col].mean()
                std = self.data[col].std()
                if std > 0:
                    self.data[f'{col}_norm'] = (self.data[col] - mean) / std
            
            elif method == 'minmax':
                # Min-Max normalization (0-1)
                min_val = self.data[col].min()
                max_val = self.data[col].max()
                if max_val - min_val > 0:
                    self.data[f'{col}_norm'] = (self.data[col] - min_val) / (max_val - min_val)
        
        self.metadata['samples_normalized'] = len(self.data)
        self.logger.info(f"Normalized data using {method} method")
        return self.data
    
    def clean_data(self, remove_dups=True, handle_missing=True, remove_outliers_flag=True):
        """Birleştirilmiş veri temizleme işlemi"""
        self.logger.info(f"Starting data cleaning for {self.battery_id}")
        
        # Step 1: Validasyon
        if not self.validate_data():
            return False
        
        # Step 2: Duplikaları kaldır
        if remove_dups:
            self.remove_duplicates()
        
        # Step 3: Eksik verileri işle
        if handle_missing:
            self.handle_missing_values()
        
        # Step 4: Aykırı değerleri kaldır
        if remove_outliers_flag:
            self.remove_outliers()
        
        self.metadata['samples_cleaned'] = len(self.data)
        
        self.logger.info(
            f"Data cleaning completed: "
            f"{self.metadata['samples_raw']} → {self.metadata['samples_cleaned']} samples"
        )
        
        return True
    
    def save_cleaned_data(self, filepath=None, format='csv'):
        """Temizlenen veriyi kaydet"""
        if filepath is None:
            filepath = PROCESSED_DATA_DIR / f"{self.battery_id}_cleaned.csv"
        else:
            filepath = Path(filepath)
        
        try:
            filepath.parent.mkdir(parents=True, exist_ok=True)
            
            if format == 'csv':
                self.data.to_csv(filepath, index=False)
            elif format == 'parquet':
                self.data.to_parquet(filepath, index=False)
            elif format == 'json':
                self.data.to_json(filepath, orient='records')
            
            self.logger.info(f"Saved cleaned data to {filepath}")
            return True
        except Exception as e:
            self.logger.error(f"Error saving data: {e}")
            return False
    
    def get_statistics(self):
        """Veri istatistikleri al"""
        stats = {
            'battery_id': self.battery_id,
            'total_samples': len(self.data),
            'time_span': (self.data['timestamp'].max() - self.data['timestamp'].min()) if 'timestamp' in self.data.columns else None,
            'metadata': self.metadata,
            'column_stats': {}
        }
        
        for col in self.data.select_dtypes(include=[np.number]).columns:
            stats['column_stats'][col] = {
                'mean': self.data[col].mean(),
                'std': self.data[col].std(),
                'min': self.data[col].min(),
                'max': self.data[col].max(),
                'median': self.data[col].median()
            }
        
        return stats
    
    def get_info(self):
        """Batarya bilgilerini al"""
        return {
            'battery_id': self.battery_id,
            'rows': len(self.data),
            'columns': list(self.data.columns),
            'memory_usage_mb': self.data.memory_usage(deep=True).sum() / 1024**2
        }


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    # Örnek batarya verisi oluştur
    battery = BatteryData('BAT_001')
    battery.create_sample_data(n_samples=1000)
    
    # Verileri temizle
    battery.clean_data()
    
    # Verileri normalleştir
    battery.normalize_data(method='standardize')
    
    # İstatistikleri al
    stats = battery.get_statistics()
    print(f"Statistics: {stats}")
    
    # Verileri kaydet
    battery.save_cleaned_data()
    
    # Bilgi al
    info = battery.get_info()
    print(f"Battery Info: {info}")
