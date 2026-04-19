"""
TUBITAK 7260634 - API Wrapper
Web dashboard ve Python backend entegrasyonu
"""

import json
import numpy as np
import pandas as pd
from datetime import datetime
from typing import Dict, List, Any, Optional
from flask import Flask, request, jsonify, Blueprint
from functools import wraps

from bms.bms_controller import BMSController
from data_processing.battery_data import BatteryData
from data_processing.feature_extraction import FeatureExtractor
from models.soh_model import SoHModel
from utils.logger import get_logger


# ============================================
# API WRAPPER CLASS
# ============================================

class BatteryAPIWrapper:
    """
    Web API ara yüzü
    RESTful endpoints aracılığıyla BMS ve ML modellerine erişim
    """
    
    def __init__(self, debug=False):
        """
        Initialize BatteryAPIWrapper
        
        Parameters:
        -----------
        debug : bool
            Debug modu
        """
        self.app = Flask(__name__)
        self.app.config['JSON_SORT_KEYS'] = False
        self.debug = debug
        self.logger = get_logger('api_wrapper')
        
        # Sistem bileşenleri
        self.controllers = {}  # battery_id -> BMSController
        self.feature_extractors = {}  # battery_id -> FeatureExtractor
        self.soh_models = {}  # model_type -> SoHModel
        
        # Routes'ı kaydet
        self._register_routes()
        
        self.logger.info("BatteryAPIWrapper initialized")
    
    def _register_routes(self):
        """Tüm API routes'larını kaydet"""
        
        # Health check
        @self.app.route('/api/health', methods=['GET'])
        def health():
            return jsonify({
                'status': 'healthy',
                'timestamp': datetime.now().isoformat(),
                'controllers': len(self.controllers),
                'models': len(self.soh_models)
            }), 200
        
        # Battery initialization
        @self.app.route('/api/battery/init', methods=['POST'])
        def init_battery():
            """Batarya başlatma"""
            try:
                data = request.get_json()
                battery_id = data.get('battery_id', f'BAT_{datetime.now().timestamp()}')
                num_cells = data.get('num_cells', 8)
                
                # Controller oluştur
                if battery_id not in self.controllers:
                    self.controllers[battery_id] = BMSController(battery_id, num_cells)
                    self.feature_extractors[battery_id] = FeatureExtractor()
                
                return jsonify({
                    'status': 'initialized',
                    'battery_id': battery_id,
                    'num_cells': num_cells
                }), 201
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Update battery data
        @self.app.route('/api/battery/<battery_id>/update', methods=['POST'])
        def update_battery_data(battery_id):
            """Batarya verisini güncelle"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                data = request.get_json()
                
                # DataFrame oluştur
                battery_data = pd.DataFrame(data)
                
                # Güncelle
                controller = self.controllers[battery_id]
                controller.update_batteries(battery_data)
                
                return jsonify({
                    'status': 'updated',
                    'battery_id': battery_id,
                    'records': len(battery_data)
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Monitor battery
        @self.app.route('/api/battery/<battery_id>/monitor', methods=['POST'])
        def monitor_battery(battery_id):
            """Bataryayı izle"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                controller = self.controllers[battery_id]
                
                # İzlemeyi gerçekleştir
                monitoring_result = controller.execute_monitoring()
                
                # Sistem sağlığını al
                health = controller.get_system_health()
                
                return jsonify({
                    'monitoring': monitoring_result,
                    'health': health
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Predict SOH
        @self.app.route('/api/battery/<battery_id>/predict_soh', methods=['POST'])
        def predict_soh(battery_id):
            """SOH tahmini yap"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                data = request.get_json()
                features = data.get('features')
                
                controller = self.controllers[battery_id]
                
                # SOH tahmin et
                if features:
                    # Verilen özelliklerle tahmin
                    soh_result = controller.estimate_soh(features)
                else:
                    # Telemetri verilerinden tahmin
                    soh_result = controller.estimate_soh()
                
                return jsonify({
                    'battery_id': battery_id,
                    'soh_prediction': soh_result
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Make decision
        @self.app.route('/api/battery/<battery_id>/decide', methods=['POST'])
        def make_decision(battery_id):
            """Batarya kararı ver"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                data = request.get_json()
                soh_value = data.get('soh', 50.0)
                
                controller = self.controllers[battery_id]
                
                # Karar ver
                decision = controller.make_decision(soh_value)
                
                return jsonify({
                    'battery_id': battery_id,
                    'decision': decision
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Charge/Discharge
        @self.app.route('/api/battery/<battery_id>/charge', methods=['POST'])
        def charge_battery(battery_id):
            """Şarj işlemini işle"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                data = request.get_json()
                current_ma = data.get('current_ma', 5000)  # mA
                duration_sec = data.get('duration_sec', 3600)  # 1 saat
                
                controller = self.controllers[battery_id]
                
                # Şarj işlemini işle
                result = controller.process_charge_discharge(current_ma, duration_sec)
                
                return jsonify({
                    'battery_id': battery_id,
                    'charge_result': result
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Discharge
        @self.app.route('/api/battery/<battery_id>/discharge', methods=['POST'])
        def discharge_battery(battery_id):
            """Deşarj işlemini işle"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                data = request.get_json()
                current_ma = data.get('current_ma', -5000)  # mA (negatif)
                duration_sec = data.get('duration_sec', 3600)
                
                controller = self.controllers[battery_id]
                
                # Deşarj işlemini işle
                result = controller.process_charge_discharge(current_ma, duration_sec)
                
                return jsonify({
                    'battery_id': battery_id,
                    'discharge_result': result
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Get battery status
        @self.app.route('/api/battery/<battery_id>/status', methods=['GET'])
        def get_battery_status(battery_id):
            """Batarya durumunu al"""
            try:
                if battery_id not in self.controllers:
                    return jsonify({'error': 'Battery not found'}), 404
                
                controller = self.controllers[battery_id]
                
                # Reportu oluştur
                report = controller.generate_report()
                
                return jsonify(report), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Extract features
        @self.app.route('/api/features/extract', methods=['POST'])
        def extract_features():
            """Özellikleri çıkar"""
            try:
                data = request.get_json()
                battery_id = data.get('battery_id', 'default')
                
                if battery_id not in self.feature_extractors:
                    self.feature_extractors[battery_id] = FeatureExtractor()
                
                # DataFrame oluştur
                df = pd.DataFrame(data.get('data', []))
                
                if df.empty:
                    return jsonify({'error': 'No data provided'}), 400
                
                extractor = self.feature_extractors[battery_id]
                
                # Özellikleri çıkar
                features = extractor.extract_all_features(df)
                
                return jsonify({
                    'battery_id': battery_id,
                    'features_shape': features.shape if hasattr(features, 'shape') else len(features),
                    'features': features.tolist() if isinstance(features, np.ndarray) else features
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Train model
        @self.app.route('/api/model/train', methods=['POST'])
        def train_model():
            """Model eğit"""
            try:
                data = request.get_json()
                model_type = data.get('model_type', 'random_forest')
                X_train = np.array(data.get('X_train', []))
                y_train = np.array(data.get('y_train', []))
                
                if X_train.size == 0 or y_train.size == 0:
                    return jsonify({'error': 'Training data required'}), 400
                
                # Model oluştur ve eğit
                if model_type not in self.soh_models:
                    self.soh_models[model_type] = SoHModel(model_type)
                
                model = self.soh_models[model_type]
                metrics = model.train(X_train, y_train)
                
                return jsonify({
                    'model_type': model_type,
                    'training_metrics': metrics
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Predict with model
        @self.app.route('/api/model/<model_type>/predict', methods=['POST'])
        def predict_model(model_type):
            """Modelle tahmin yap"""
            try:
                if model_type not in self.soh_models:
                    return jsonify({'error': 'Model not found'}), 404
                
                data = request.get_json()
                X = np.array(data.get('features', []))
                
                if X.size == 0:
                    return jsonify({'error': 'Features required'}), 400
                
                model = self.soh_models[model_type]
                predictions = model.predict(X)
                
                return jsonify({
                    'model_type': model_type,
                    'predictions': predictions.tolist() if isinstance(predictions, np.ndarray) else predictions
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
        
        # Get model info
        @self.app.route('/api/model/<model_type>/info', methods=['GET'])
        def get_model_info(model_type):
            """Model bilgisini al"""
            try:
                if model_type not in self.soh_models:
                    return jsonify({'error': 'Model not found'}), 404
                
                model = self.soh_models[model_type]
                
                return jsonify({
                    'model_type': model_type,
                    'is_trained': model.model is not None,
                    'model_algorithm': str(type(model.model).__name__) if model.model else None
                }), 200
            
            except Exception as e:
                return jsonify({'error': str(e)}), 400
    
    def run(self, host='127.0.0.1', port=5000, debug=None):
        """
        API sunucusunu çalıştır
        
        Parameters:
        -----------
        host : str
            Server host
        port : int
            Server port
        debug : bool
            Debug modu
        """
        debug = debug or self.debug
        self.logger.info(f"Starting API server on {host}:{port}")
        self.app.run(host=host, port=port, debug=debug)


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    # API Wrapper oluştur
    api = BatteryAPIWrapper(debug=True)
    
    # Sunucuyu başlat
    print("Starting Battery Management API Server...")
    print("Available endpoints:")
    print("  GET  /api/health")
    print("  POST /api/battery/init")
    print("  POST /api/battery/<id>/update")
    print("  POST /api/battery/<id>/monitor")
    print("  POST /api/battery/<id>/predict_soh")
    print("  POST /api/battery/<id>/decide")
    print("  POST /api/battery/<id>/charge")
    print("  POST /api/battery/<id>/discharge")
    print("  GET  /api/battery/<id>/status")
    print("  POST /api/features/extract")
    print("  POST /api/model/train")
    print("  POST /api/model/<type>/predict")
    print("  GET  /api/model/<type>/info")
    
    api.run(port=5000)
