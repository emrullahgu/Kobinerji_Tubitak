"""
TUBITAK 7260634 - SoH Prediction Model
State of Health (Sağlık Durumu) tahmin modelı
"""

import numpy as np
import pandas as pd
from sklearn.ensemble import RandomForestRegressor, GradientBoostingRegressor
from sklearn.linear_model import LinearRegression, Ridge
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import mean_squared_error, r2_score, mean_absolute_error
import joblib
from pathlib import Path
from datetime import datetime
import json

from utils.constants import (
    MODELS_DIR,
    RF_N_ESTIMATORS, RF_MAX_DEPTH, RF_MIN_SAMPLES_SPLIT,
    XGB_N_ESTIMATORS, XGB_MAX_DEPTH, XGB_LEARNING_RATE,
    TARGET_ACCURACY, TARGET_R2, TARGET_RMSE,
    REUSE_THRESHOLD, RECYCLE_THRESHOLD
)
from utils.logger import get_logger


class SoHModel:
    """
    State of Health (SoH) tahmin modeli
    Batarya sağlık durumunu %0-100 arasında tahmin eder
    """
    
    def __init__(self, model_type='random_forest', model_name=None):
        """
        Initialize SoH Model
        
        Parameters:
        -----------
        model_type : str
            Model türü: 'random_forest', 'linear', 'ridge', 'gradient_boosting'
        model_name : str, optional
            Model adı
        """
        self.model_type = model_type
        self.model_name = model_name or f"SoH_{model_type}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        self.logger = get_logger('soh_model')
        
        # Model initialization
        if model_type == 'random_forest':
            self.model = RandomForestRegressor(
                n_estimators=RF_N_ESTIMATORS,
                max_depth=RF_MAX_DEPTH,
                min_samples_split=RF_MIN_SAMPLES_SPLIT,
                random_state=42,
                n_jobs=-1
            )
        elif model_type == 'linear':
            self.model = LinearRegression()
        elif model_type == 'ridge':
            self.model = Ridge(alpha=1.0)
        elif model_type == 'gradient_boosting':
            self.model = GradientBoostingRegressor(
                n_estimators=XGB_N_ESTIMATORS,
                max_depth=XGB_MAX_DEPTH,
                learning_rate=XGB_LEARNING_RATE,
                random_state=42
            )
        else:
            raise ValueError(f"Unknown model type: {model_type}")
        
        self.scaler = StandardScaler()
        self.is_trained = False
        self.training_history = {}
        self.feature_names = None
        
        self.logger.info(f"Initialized {self.model_type} model: {self.model_name}")
    
    def prepare_data(self, X, y, test_size=0.2, random_state=42):
        """
        Verileri eğitim ve test setlerine böl
        
        Parameters:
        -----------
        X : pd.DataFrame or np.ndarray
            Özellikler
        y : pd.Series or np.ndarray
            Target (SoH hedef değerleri)
        test_size : float
            Test seti oranı
        random_state : int
            Randomness kontrolü
            
        Returns:
        --------
        X_train, X_test, y_train, y_test
        """
        # Feature names kaydet
        if isinstance(X, pd.DataFrame):
            self.feature_names = X.columns.tolist()
            X = X.values
        
        # Veriyi böl
        X_train, X_test, y_train, y_test = train_test_split(
            X, y,
            test_size=test_size,
            random_state=random_state
        )
        
        # Eğitim verisine göre scaler fit et
        X_train_scaled = self.scaler.fit_transform(X_train)
        X_test_scaled = self.scaler.transform(X_test)
        
        self.logger.info(
            f"Data prepared: "
            f"Train={len(X_train)}, Test={len(X_test)}, "
            f"Features={X_train.shape[1]}"
        )
        
        return X_train_scaled, X_test_scaled, y_train, y_test
    
    def train(self, X, y, test_size=0.2, verbose=True):
        """
        Modeli eğit
        
        Parameters:
        -----------
        X : pd.DataFrame or np.ndarray
            Özellikler
        y : pd.Series or np.ndarray
            Target değerleri (SoH: 0-100 veya 0-1)
        test_size : float
            Test seti oranı
        verbose : bool
            Detaylı çıktı
            
        Returns:
        --------
        dict : Eğitim sonuçları
        """
        self.logger.info(f"Starting training for {self.model_name}")
        
        # Verileri hazırla
        X_train, X_test, y_train, y_test = self.prepare_data(X, y, test_size=test_size)
        
        # Modeli eğit
        import time
        start_time = time.time()
        
        self.model.fit(X_train, y_train)
        
        training_time = time.time() - start_time
        
        # Tahmin yap
        y_train_pred = self.model.predict(X_train)
        y_test_pred = self.model.predict(X_test)
        
        # Metrikleri hesapla
        train_r2 = r2_score(y_train, y_train_pred)
        test_r2 = r2_score(y_test, y_test_pred)
        train_rmse = np.sqrt(mean_squared_error(y_train, y_train_pred))
        test_rmse = np.sqrt(mean_squared_error(y_test, y_test_pred))
        train_mae = mean_absolute_error(y_train, y_train_pred)
        test_mae = mean_absolute_error(y_test, y_test_pred)
        
        # Cross-validation
        cv_scores = cross_val_score(self.model, X_train, y_train, cv=5, scoring='r2')
        
        self.training_history = {
            'model_type': self.model_type,
            'training_time': training_time,
            'samples_train': len(X_train),
            'samples_test': len(X_test),
            'features': X_train.shape[1],
            'train_r2': float(train_r2),
            'test_r2': float(test_r2),
            'train_rmse': float(train_rmse),
            'test_rmse': float(test_rmse),
            'train_mae': float(train_mae),
            'test_mae': float(test_mae),
            'cv_mean': float(cv_scores.mean()),
            'cv_std': float(cv_scores.std()),
            'trained_at': datetime.now().isoformat()
        }
        
        self.is_trained = True
        
        if verbose:
            self.logger.info(
                f"Training completed in {training_time:.2f}s\n"
                f"  Train R² = {train_r2:.4f}, Test R² = {test_r2:.4f}\n"
                f"  Train RMSE = {train_rmse:.4f}, Test RMSE = {test_rmse:.4f}\n"
                f"  CV Mean R² = {cv_scores.mean():.4f} ± {cv_scores.std():.4f}"
            )
        
        return self.training_history
    
    def predict(self, X, confidence_interval=False):
        """
        SoH tahmin yap
        
        Parameters:
        -----------
        X : pd.DataFrame or np.ndarray
            Giriş özelikleri
        confidence_interval : bool
            Güven aralığı hesapla
            
        Returns:
        --------
        predictions : np.ndarray
            Tahmin edilen SoH değerleri (0-100)
        confidence : np.ndarray, optional
            Güven değerleri
        """
        if not self.is_trained:
            self.logger.warning("Model not trained yet!")
            return None
        
        # Veriyi normalleştir
        if isinstance(X, pd.DataFrame):
            X = X.values
        
        try:
            X_scaled = self.scaler.transform(X)
        except:
            self.logger.error("Error scaling data")
            return None
        
        # Tahmin yap
        predictions = self.model.predict(X_scaled)
        
        # SoH değerlerini 0-100 aralığına sıkıştır
        predictions = np.clip(predictions, 0, 100)
        
        if confidence_interval and hasattr(self.model, 'estimators_'):
            # Random Forest için güven aralığı
            predictions_all = np.array([
                tree.predict(X_scaled) for tree in self.model.estimators_
            ])
            confidence = np.std(predictions_all, axis=0) / np.mean(predictions_all, axis=0)
            return predictions, confidence
        
        return predictions
    
    def predict_single(self, features):
        """
        Tek bir batarya için tahmin yap
        
        Parameters:
        -----------
        features : array-like or dict
            Batarya özellikleri
            
        Returns:
        --------
        dict : Tahmin sonuçları
        """
        if not self.is_trained:
            self.logger.error("Model not trained!")
            return None
        
        # Tek örneği 2D diziye dönüştür
        if isinstance(features, dict):
            features = pd.DataFrame([features])
        else:
            features = np.array(features).reshape(1, -1)
        
        predictions = self.predict(features)
        soh = float(predictions[0]) if isinstance(predictions, np.ndarray) else predictions
        
        # SoH'ye göre sınıflama
        if soh >= REUSE_THRESHOLD * 100:
            recommendation = "İkinci Ömrü Kullananabilir"
            status = "REUSE"
        elif soh >= RECYCLE_THRESHOLD * 100:
            recommendation = "Geri Dönüşüme Yönlendir"
            status = "RECYCLE"
        else:
            recommendation = "Acil Geri Dönüşüm Gerekli"
            status = "CRITICAL"
        
        return {
            'soh': soh,
            'status': status,
            'recommendation': recommendation,
            'confidence': float(np.random.random() * 0.2 + 0.8)  # Mock confidence
        }
    
    def evaluate(self, X_test, y_test):
        """
        Modeli değerlendir
        
        Parameters:
        -----------
        X_test : np.ndarray
            Test özelikleri
        y_test : np.ndarray
            Test hedef değerleri
            
        Returns:
        --------
        dict : Değerlendirme metrikleri
        """
        if not self.is_trained:
            self.logger.error("Model not trained!")
            return None
        
        X_test_scaled = self.scaler.transform(X_test)
        y_pred = self.model.predict(X_test_scaled)
        y_pred = np.clip(y_pred, 0, 100)
        
        metrics = {
            'r2': float(r2_score(y_test, y_pred)),
            'rmse': float(np.sqrt(mean_squared_error(y_test, y_pred))),
            'mae': float(mean_absolute_error(y_test, y_pred)),
            'mape': float(np.mean(np.abs((y_test - y_pred) / y_test)) * 100),
            'median_error': float(np.median(np.abs(y_test - y_pred)))
        }
        
        self.logger.info(f"Evaluation metrics: {metrics}")
        
        return metrics
    
    def get_feature_importance(self, top_n=20):
        """
        Özellik önem sırasını al
        
        Parameters:
        -----------
        top_n : int
            En önemli n özellik
            
        Returns:
        --------
        pd.DataFrame : Özellik önem tablosu
        """
        if not hasattr(self.model, 'feature_importances_'):
            self.logger.warning("Model doesn't support feature importance")
            return None
        
        if self.feature_names is None:
            feature_names = [f"Feature_{i}" for i in range(len(self.model.feature_importances_))]
        else:
            feature_names = self.feature_names
        
        importance_df = pd.DataFrame({
            'feature': feature_names,
            'importance': self.model.feature_importances_
        }).sort_values('importance', ascending=False).head(top_n)
        
        return importance_df
    
    def save_model(self, filepath=None):
        """
        Modeli kaydet
        
        Parameters:
        -----------
        filepath : str, optional
            Kayıt yolu
            
        Returns:
        --------
        bool : Başarı durumu
        """
        if not self.is_trained:
            self.logger.warning("Model not trained yet!")
            return False
        
        if filepath is None:
            filepath = MODELS_DIR / f"{self.model_name}.pkl"
        else:
            filepath = Path(filepath)
        
        filepath.parent.mkdir(parents=True, exist_ok=True)
        
        try:
            model_data = {
                'model': self.model,
                'scaler': self.scaler,
                'feature_names': self.feature_names,
                'model_type': self.model_type,
                'training_history': self.training_history
            }
            joblib.dump(model_data, filepath)
            self.logger.info(f"Model saved to {filepath}")
            return True
        except Exception as e:
            self.logger.error(f"Error saving model: {e}")
            return False
    
    def load_model(self, filepath):
        """
        Modeli yükle
        
        Parameters:
        -----------
        filepath : str
            Model dosya yolu
            
        Returns:
        --------
        bool : Başarı durumu
        """
        try:
            model_data = joblib.load(filepath)
            self.model = model_data['model']
            self.scaler = model_data['scaler']
            self.feature_names = model_data['feature_names']
            self.training_history = model_data['training_history']
            self.is_trained = True
            self.logger.info(f"Model loaded from {filepath}")
            return True
        except Exception as e:
            self.logger.error(f"Error loading model: {e}")
            return False
    
    def get_model_info(self):
        """Model bilgilerini al"""
        return {
            'model_name': self.model_name,
            'model_type': self.model_type,
            'is_trained': self.is_trained,
            'training_history': self.training_history,
            'feature_names': self.feature_names
        }


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    from data_processing.battery_data import BatteryData
    from data_processing.feature_extraction import FeatureExtractor
    
    # Veri oluştur
    battery = BatteryData('BAT_001')
    battery.create_sample_data(n_samples=2000)
    battery.clean_data()
    
    # Özellikler çıkar
    extractor = FeatureExtractor()
    features = extractor.extract_all_features(battery.data)
    
    # Model oluştur ve eğit
    model = SoHModel(model_type='random_forest', model_name='SoH_RF_v1')
    
    X = features.drop('target_soh', axis=1)
    y = features['target_soh']
    
    history = model.train(X, y, test_size=0.2)
    print(f"Training history: {history}")
    
    # Tahmin yap
    prediction = model.predict_single(X.iloc[0])
    print(f"Prediction: {prediction}")
    
    # Modeli kaydet
    model.save_model()
