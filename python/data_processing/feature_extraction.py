"""
TUBITAK 7260634 - Feature Extraction Module
Batarya verilerinden özellikleri çıkar
"""

import numpy as np
import pandas as pd
from scipy import stats, signal
from sklearn.preprocessing import StandardScaler
import warnings

from utils.constants import (
    WINDOW_SIZE, WINDOW_STRIDE,
    STATISTICAL_FEATURES, IMPEDANCE_FEATURES,
    SAMPLING_RATE, NYQUIST_FREQUENCY
)
from utils.logger import get_logger

warnings.filterwarnings('ignore')


class FeatureExtractor:
    """Batarya verilerinden özellikleri çıkaran sınıf"""
    
    def __init__(self, window_size=WINDOW_SIZE, window_stride=WINDOW_STRIDE):
        """
        Initialize FeatureExtractor
        
        Parameters:
        -----------
        window_size : int
            Time-series penceresi boyutu
        window_stride : int
            Pencere stride'ı
        """
        self.window_size = window_size
        self.window_stride = window_stride
        self.logger = get_logger('feature_extraction')
        self.features = None
        self.scaler = StandardScaler()
    
    def create_sliding_windows(self, data, column):
        """Kaydırmalı pencere oluştur"""
        windows = []
        for i in range(0, len(data) - self.window_size + 1, self.window_stride):
            window = data[column].iloc[i:i+self.window_size].values
            windows.append(window)
        
        return np.array(windows)
    
    def extract_statistical_features(self, data, window):
        """İstatistiksel özellikler çıkar"""
        features_dict = {}
        
        # Temel istatistikler
        features_dict['mean'] = np.mean(window)
        features_dict['std'] = np.std(window)
        features_dict['min'] = np.min(window)
        features_dict['max'] = np.max(window)
        features_dict['median'] = np.median(window)
        features_dict['q25'] = np.percentile(window, 25)
        features_dict['q75'] = np.percentile(window, 75)
        features_dict['range'] = np.max(window) - np.min(window)
        features_dict['iqr'] = features_dict['q75'] - features_dict['q25']
        
        # Çarpıklık ve basıklık (Shape)
        features_dict['skewness'] = stats.skew(window)
        features_dict['kurtosis'] = stats.kurtosis(window)
        
        # Energy ve Power
        features_dict['energy'] = np.sum(window ** 2)
        features_dict['power'] = features_dict['energy'] / len(window)
        features_dict['rms'] = np.sqrt(np.mean(window ** 2))
        
        # Varyans ve Standart sapması
        features_dict['variance'] = np.var(window)
        features_dict['coeff_variation'] = (features_dict['std'] / features_dict['mean']) if features_dict['mean'] != 0 else 0
        
        # Peak ve Valley
        features_dict['max_abs'] = np.max(np.abs(window))
        features_dict['peak_to_peak'] = np.max(window) - np.min(window)
        
        # Zero Crossing Rate (geçişler)
        features_dict['zero_crossings'] = np.sum(np.abs(np.diff(np.sign(window - np.mean(window))))) / 2
        
        return features_dict
    
    def extract_frequency_features(self, window):
        """Frekans domain özellikler çıkar"""
        features_dict = {}
        
        # FFT (Fast Fourier Transform)
        fft = np.fft.fft(window)
        magnitude = np.abs(fft[:len(fft)//2])
        frequencies = np.fft.fftfreq(len(window), 1/SAMPLING_RATE)[:len(fft)//2]
        
        # Power Spectral Density
        psd = magnitude ** 2
        features_dict['fft_mean'] = np.mean(magnitude)
        features_dict['fft_max'] = np.max(magnitude)
        features_dict['fft_energy'] = np.sum(psd)
        
        # Dominant frequency
        if len(frequencies) > 0:
            dominant_idx = np.argmax(psd)
            features_dict['dominant_frequency'] = frequencies[dominant_idx]
        
        # Spectral centroid
        features_dict['spectral_centroid'] = np.sum(frequencies * psd) / np.sum(psd) if np.sum(psd) > 0 else 0
        
        return features_dict
    
    def extract_wavelet_features(self, window):
        """Dalgacık (Wavelet) özellikler çıkar"""
        features_dict = {}
        
        # Approximation ve detail coefficients (Daubechies)
        # Using simple implementation without external library
        try:
            # Simple discrete wavelet-like features
            features_dict['energy_band_low'] = np.sum((signal.butter(2, 0.1, output='ba')[0] * window) ** 2)
            features_dict['energy_band_mid'] = np.sum((signal.butter(2, 0.5, output='ba')[0] * window) ** 2)
            features_dict['energy_band_high'] = np.sum((signal.butter(2, 0.9, output='ba')[0] * window) ** 2)
        except:
            features_dict['energy_band_low'] = 0
            features_dict['energy_band_mid'] = 0
            features_dict['energy_band_high'] = 0
        
        return features_dict
    
    def extract_time_domain_features(self, window):
        """Zaman domain özellikleri çıkar"""
        features_dict = {}
        
        # Autocorrelation
        if len(window) > 1:
            autocorr = np.correlate(window - np.mean(window), window - np.mean(window), mode='full')
            autocorr = autocorr[len(autocorr)//2:]
            autocorr = autocorr / autocorr[0]
            
            features_dict['autocorr_lag1'] = autocorr[1] if len(autocorr) > 1 else 0
            features_dict['autocorr_lag5'] = autocorr[5] if len(autocorr) > 5 else 0
            features_dict['autocorr_lag10'] = autocorr[10] if len(autocorr) > 10 else 0
        
        # First derivative (rate of change)
        diff = np.diff(window)
        features_dict['diff_mean'] = np.mean(diff)
        features_dict['diff_std'] = np.std(diff)
        features_dict['diff_max'] = np.max(np.abs(diff))
        
        # Second derivative (acceleration)
        if len(diff) > 1:
            ddiff = np.diff(diff)
            features_dict['ddiff_mean'] = np.mean(ddiff)
            features_dict['ddiff_std'] = np.std(ddiff)
        
        return features_dict
    
    def extract_impedance_features(self, voltage, current):
        """İmpedans tabanlı özellikler çıkar"""
        features_dict = {}
        
        # DC Resistance
        if len(current) > 0 and np.max(np.abs(current)) > 0.1:
            voltage_change = np.max(voltage) - np.min(voltage)
            current_change = np.max(current) - np.min(current)
            if current_change != 0:
                features_dict['dcr'] = voltage_change / current_change
            else:
                features_dict['dcr'] = 0
        else:
            features_dict['dcr'] = 0
        
        # AC Resistance at 100Hz
        if len(voltage) > 100:
            # Simple high-frequency component extraction
            high_freq = np.fft.fft(voltage)
            features_dict['acr_100hz'] = np.abs(high_freq[10]) if len(high_freq) > 10 else 0
            features_dict['acr_1khz'] = np.abs(high_freq[100]) if len(high_freq) > 100 else 0
        else:
            features_dict['acr_100hz'] = 0
            features_dict['acr_1khz'] = 0
        
        # Phase angle
        if len(voltage) > 0 and np.std(voltage) > 0:
            phase = np.angle(np.fft.fft(voltage)[1]) if len(np.fft.fft(voltage)) > 1 else 0
            features_dict['phase_angle'] = phase
        else:
            features_dict['phase_angle'] = 0
        
        return features_dict
    
    def extract_battery_specific_features(self, data, window_idx):
        """Bataryaya özgü özellikler çıkar"""
        features_dict = {}
        start_idx = window_idx * self.window_stride
        end_idx = start_idx + self.window_size
        
        if 'soc' in data.columns:
            soc_window = data['soc'].iloc[start_idx:end_idx].values
            features_dict['soc_mean'] = np.mean(soc_window)
            features_dict['soc_min'] = np.min(soc_window)
            features_dict['soc_max'] = np.max(soc_window)
            features_dict['soc_range'] = features_dict['soc_max'] - features_dict['soc_min']
        
        if 'temperature' in data.columns:
            temp_window = data['temperature'].iloc[start_idx:end_idx].values
            features_dict['temp_mean'] = np.mean(temp_window)
            features_dict['temp_max'] = np.max(temp_window)
            features_dict['temp_min'] = np.min(temp_window)
            features_dict['temp_variance'] = np.var(temp_window)
        
        if 'capacity' in data.columns:
            cap_window = data['capacity'].iloc[start_idx:end_idx].values
            features_dict['capacity_mean'] = np.mean(cap_window)
            features_dict['capacity_degradation'] = cap_window[0] - cap_window[-1]
        
        if 'impedance' in data.columns:
            imp_window = data['impedance'].iloc[start_idx:end_idx].values
            features_dict['impedance_mean'] = np.mean(imp_window)
            features_dict['impedance_change'] = imp_window[-1] - imp_window[0]
        
        return features_dict
    
    def extract_all_features(self, data):
        """Tüm özelliklerini çıkar"""
        self.logger.info(f"Extracting features from {len(data)} samples")
        
        all_features = []
        n_windows = (len(data) - self.window_size) // self.window_stride + 1
        
        for window_idx in range(n_windows):
            start_idx = window_idx * self.window_stride
            end_idx = start_idx + self.window_size
            
            features_dict = {}
            
            # Voltage features
            if 'voltage' in data.columns:
                voltage_window = data['voltage'].iloc[start_idx:end_idx].values
                features_dict.update(self._get_feature_prefix('voltage', voltage_window))
            
            # Current features
            if 'current' in data.columns:
                current_window = data['current'].iloc[start_idx:end_idx].values
                features_dict.update(self._get_feature_prefix('current', current_window))
            
            # Temperature features
            if 'temperature' in data.columns:
                temp_window = data['temperature'].iloc[start_idx:end_idx].values
                features_dict.update(self._get_feature_prefix('temperature', temp_window))
            
            # Impedance features
            if 'voltage' in data.columns and 'current' in data.columns:
                voltage_window = data['voltage'].iloc[start_idx:end_idx].values
                current_window = data['current'].iloc[start_idx:end_idx].values
                imp_features = self.extract_impedance_features(voltage_window, current_window)
                features_dict.update(imp_features)
            
            # Battery-specific features
            battery_features = self.extract_battery_specific_features(data, window_idx)
            features_dict.update(battery_features)
            
            # Target (if available)
            if 'soh' in data.columns:
                features_dict['target_soh'] = data['soh'].iloc[end_idx-1] if end_idx-1 < len(data) else data['soh'].iloc[-1]
            
            all_features.append(features_dict)
        
        self.features = pd.DataFrame(all_features)
        self.logger.info(f"Extracted {len(self.features)} feature vectors with {len(self.features.columns)} features")
        
        return self.features
    
    def _get_feature_prefix(self, prefix, window):
        """Bir sütun için tüm özellikleri çıkar (prefix ile)"""
        features = {}
        
        stat_features = self.extract_statistical_features(None, window)
        freq_features = self.extract_frequency_features(window)
        time_features = self.extract_time_domain_features(window)
        
        for key, val in stat_features.items():
            features[f'{prefix}_{key}'] = val
        for key, val in freq_features.items():
            features[f'{prefix}_{key}'] = val
        for key, val in time_features.items():
            features[f'{prefix}_{key}'] = val
        
        return features
    
    def normalize_features(self, fit=False):
        """Özellikleri normalleştir"""
        if self.features is None:
            self.logger.warning("No features to normalize")
            return None
        
        # Target sütunu ayır
        if 'target_soh' in self.features.columns:
            target = self.features['target_soh']
            features_df = self.features.drop('target_soh', axis=1)
        else:
            target = None
            features_df = self.features
        
        # Normalleştir
        if fit:
            normalized = self.scaler.fit_transform(features_df)
        else:
            normalized = self.scaler.transform(features_df)
        
        normalized_df = pd.DataFrame(normalized, columns=features_df.columns)
        
        # Target ekle
        if target is not None:
            normalized_df['target_soh'] = target.values
        
        self.logger.info("Features normalized")
        return normalized_df
    
    def get_feature_names(self):
        """Özellik adlarını al"""
        if self.features is not None:
            return list(self.features.columns)
        return []
    
    def get_feature_importance(self, feature_names, importance_scores):
        """Özellik önem sırasını al"""
        return pd.DataFrame({
            'feature': feature_names,
            'importance': importance_scores
        }).sort_values('importance', ascending=False)


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    from data_processing.battery_data import BatteryData
    
    # Batarya verisi oluştur
    battery = BatteryData('BAT_001')
    battery.create_sample_data(n_samples=5000)
    battery.clean_data()
    
    # Özellik çıkar
    extractor = FeatureExtractor(window_size=100, window_stride=10)
    features = extractor.extract_all_features(battery.data)
    
    print(f"Extracted {len(features)} feature vectors")
    print(f"Features: {features.shape}")
    print(f"\nFirst 5 rows:\n{features.head()}")
    
    # Normalleştir
    normalized = extractor.normalize_features(fit=True)
    print(f"\nNormalized features shape: {normalized.shape}")
