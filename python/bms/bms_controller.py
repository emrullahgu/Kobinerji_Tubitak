"""
TUBITAK 7260634 - BMS Controller
Merkezi batarya yönetim kontrol sistemi
"""

import numpy as np
import pandas as pd
from datetime import datetime, timedelta
from enum import Enum
from typing import Dict, List, Tuple

from bms.battery_manager import BatteryMonitor, CellBalancer, ChargeState
from data_processing.battery_data import BatteryData
from models.soh_model import SoHModel
from utils.constants import (
    BMS_MAX_CHARGE_CURRENT, BMS_MAX_DISCHARGE_CURRENT,
    BMS_MAX_VOLTAGE, BMS_MIN_VOLTAGE,
    SOH_EXCELLENT, SOH_GOOD, SOH_FAIR, SOH_POOR, SOH_CRITICAL
)
from utils.logger import get_logger


# ============================================
# ENUMS
# ============================================

class SystemState(Enum):
    """Sistem durumları"""
    HEALTHY = "healthy"
    WARNING = "warning"
    CRITICAL = "critical"
    SHUTDOWN = "shutdown"


class BatteryDecision(Enum):
    """Batarya kararları"""
    REUSE = "reuse"
    RECYCLE = "recycle"
    CRITICAL = "critical"


# ============================================
# BMS CONTROLLER
# ============================================

class BMSController:
    """
    Merkezi batarya yönetim sistemi kontrolü
    Tüm izleme, koruma ve karar alma işlemlerini koordine eder
    """
    
    def __init__(self, battery_id, num_cells=8, soh_model=None):
        """
        Initialize BMSController
        
        Parameters:
        -----------
        battery_id : str
            Batarya ID'si
        num_cells : int
            Hücre sayısı
        soh_model : SoHModel, optional
            SoH öngörü modeli
        """
        self.battery_id = battery_id
        self.num_cells = num_cells
        self.logger = get_logger('bms_controller')
        
        # Bileşenler
        self.monitor = BatteryMonitor(battery_id, num_cells)
        self.balancer = CellBalancer(self.monitor)
        self.soh_model = soh_model
        
        # Sistem durumu
        self.system_state = SystemState.HEALTHY
        self.charge_state = ChargeState.IDLE
        
        # Kontrol bayrakları
        self.charging_enabled = True
        self.discharging_enabled = True
        
        # İstatistikler
        self.command_history = []
        self.decision_history = []
        self.charge_cycles = 0
        self.total_charge_energy = 0.0  # kWh
        self.total_discharge_energy = 0.0  # kWh
    
    def update_batteries(self, battery_data):
        """
        Batarya verilerini güncelle
        
        Parameters:
        -----------
        battery_data : DataFrame
            Batarya ölçüm verileri (voltaj, akım, sıcaklık, SOC, SOH)
        """
        # Veri doğrulaması
        required_cols = ['cell_id', 'voltage', 'current', 'temperature', 'soc', 'soh', 'impedance']
        if not all(col in battery_data.columns for col in required_cols):
            self.logger.error(f"Missing columns in battery data")
            return False
        
        # Her hücreyi güncelle
        for _, row in battery_data.iterrows():
            self.monitor.update_cell_data(
                cell_id=int(row['cell_id']),
                voltage=float(row['voltage']),
                current=float(row['current']),
                temperature=float(row['temperature']),
                soc=float(row['soc']),
                soh=float(row['soh']),
                impedance=float(row['impedance'])
            )
        
        return True
    
    def execute_monitoring(self):
        """
        İzlemeyi gerçekleştir ve sistem durumunu güncelle
        
        Returns:
        --------
        dict : İzleme sonuçları
        """
        # Tam izlemeyi çalıştır
        alerts_count = self.monitor.monitor()
        
        # Sistem durumunu belirle
        critical_alerts = len(self.monitor.get_alerts('critical'))
        warning_alerts = len(self.monitor.get_alerts('warning'))
        
        if critical_alerts > 0:
            self.system_state = SystemState.CRITICAL
        elif warning_alerts > 0:
            self.system_state = SystemState.WARNING
        else:
            self.system_state = SystemState.HEALTHY
        
        # Dengeleme gerekli mi?
        if self.balancer.is_balancing_needed():
            self.execute_balancing()
        
        return {
            'timestamp': datetime.now().isoformat(),
            'alerts_found': alerts_count,
            'system_state': self.system_state.value,
            'critical_alerts': critical_alerts,
            'warning_alerts': warning_alerts
        }
    
    def execute_balancing(self):
        """Dengelemeyi gerçekleştir"""
        if not self.balancer.execute_balancing():
            return False
        
        self.command_history.append({
            'timestamp': datetime.now(),
            'command': 'BALANCE',
            'status': 'executed'
        })
        
        self.logger.info("Balancing command executed")
        return True
    
    def allow_charging(self):
        """Şarj yapılabilir mi?"""
        # İzleme durumunu kontrol et
        alert_count = len(self.monitor.get_alerts())
        
        # Sistem kritik durumda mı?
        if self.system_state == SystemState.CRITICAL:
            self.charging_enabled = False
            return False
        
        # Aşırı voltaj var mı?
        for cell in self.monitor.cell_data:
            if cell is None:
                continue
            if cell.voltage > BMS_MAX_VOLTAGE:
                self.charging_enabled = False
                return False
        
        self.charging_enabled = True
        return True
    
    def allow_discharging(self):
        """Deşarj yapılabilir mi?"""
        # Sistem kritik durumda mı?
        if self.system_state == SystemState.CRITICAL:
            self.discharging_enabled = False
            return False
        
        # Düşük voltaj var mı?
        for cell in self.monitor.cell_data:
            if cell is None:
                continue
            if cell.voltage < BMS_MIN_VOLTAGE:
                self.discharging_enabled = False
                return False
        
        self.discharging_enabled = True
        return True
    
    def estimate_soh(self, features=None):
        """
        SOH'ı tahmin et
        
        Parameters:
        -----------
        features : array-like, optional
            Model özellikleri
        
        Returns:
        --------
        dict : SOH tahmini ve sınıflandırması
        """
        if self.soh_model is None or features is None:
            # Basit yaklaşım: Pack SOH'ı döndür
            return {
                'soh': self.monitor.get_pack_soh(),
                'method': 'direct_measurement'
            }
        
        try:
            soh_pred = self.soh_model.predict_single(features)
            return soh_pred
        except Exception as e:
            self.logger.error(f"SOH estimation failed: {e}")
            return {'soh': self.monitor.get_pack_soh(), 'error': str(e)}
    
    def make_decision(self, soh_value):
        """
        Batarya hakkında karar ver
        
        Parameters:
        -----------
        soh_value : float
            SOH değeri (0-100)
        
        Returns:
        --------
        dict : Karar ve gerekçe
        """
        decision_info = {
            'timestamp': datetime.now().isoformat(),
            'soh': soh_value,
            'alerts': len(self.monitor.alerts),
            'system_state': self.system_state.value
        }
        
        # SOH-based karar
        if soh_value >= 85:
            decision = BatteryDecision.REUSE
            reason = "Second-life applications (stationary storage, EVs)"
            destination = "SECOND_LIFE_PROGRAM"
        elif soh_value >= 70:
            decision = BatteryDecision.RECYCLE
            reason = "Material recovery (lithium, cobalt extraction)"
            destination = "MECHANICAL_RECYCLING"
        else:
            decision = BatteryDecision.CRITICAL
            reason = "Urgent recycling (safety risk)"
            destination = "HAZMAT_PROCESSING"
        
        # Uyarılar nedeniyle override?
        if self.system_state == SystemState.CRITICAL:
            decision = BatteryDecision.CRITICAL
            reason = "Safety alerts detected"
            destination = "HAZMAT_PROCESSING"
        
        decision_info['decision'] = decision.value
        decision_info['reason'] = reason
        decision_info['destination'] = destination
        
        self.decision_history.append(decision_info)
        self.logger.info(f"Decision: {decision.value} - {reason}")
        
        return decision_info
    
    def process_charge_discharge(self, current_ma, duration_sec):
        """
        Şarj/deşarj işlemini işle
        
        Parameters:
        -----------
        current_ma : float
            Akım (miliampere, negatif=deşarj)
        duration_sec : float
            Süre (saniye)
        
        Returns:
        --------
        dict : İşlem sonuçları
        """
        current_a = current_ma / 1000  # mA'den A'ye
        energy_kwh = abs(current_a) * (duration_sec / 3600) * (self.monitor.get_pack_voltage() / 1000)
        
        if current_a > 0:
            # Şarj
            if not self.allow_charging():
                return {'status': 'rejected', 'reason': 'Charging not allowed'}
            
            if abs(current_a) > BMS_MAX_CHARGE_CURRENT:
                return {'status': 'rejected', 'reason': 'Over-charge current'}
            
            self.charge_state = ChargeState.CHARGING
            self.total_charge_energy += energy_kwh
            action = "CHARGING"
        else:
            # Deşarj
            if not self.allow_discharging():
                return {'status': 'rejected', 'reason': 'Discharging not allowed'}
            
            if abs(current_a) > BMS_MAX_DISCHARGE_CURRENT:
                return {'status': 'rejected', 'reason': 'Over-discharge current'}
            
            self.charge_state = ChargeState.DISCHARGING
            self.total_discharge_energy += energy_kwh
            action = "DISCHARGING"
        
        return {
            'status': 'allowed',
            'action': action,
            'current_ma': current_ma,
            'duration_sec': duration_sec,
            'energy_kwh': energy_kwh
        }
    
    def get_system_health(self):
        """
        Sistem sağlığı raporu al
        
        Returns:
        --------
        dict : Sistem durumu ve metrikleri
        """
        status = self.monitor.get_status_summary()
        status.update({
            'system_state': self.system_state.value,
            'charge_state': self.charge_state.value,
            'charging_enabled': self.charging_enabled,
            'discharging_enabled': self.discharging_enabled,
            'charge_cycles': self.charge_cycles,
            'total_charge_energy_kwh': self.total_charge_energy,
            'total_discharge_energy_kwh': self.total_discharge_energy,
            'decisions_made': len(self.decision_history),
            'commands_executed': len(self.command_history)
        })
        
        return status
    
    def generate_report(self):
        """
        Kapsamlı sistem raporu oluştur
        
        Returns:
        --------
        dict : Tüm sistem metrikleri ve uyarıları
        """
        report = {
            'timestamp': datetime.now().isoformat(),
            'battery_id': self.battery_id,
            'system_health': self.get_system_health(),
            'recent_alerts': [
                {
                    'cell_id': alert.cell_id,
                    'type': alert.alert_type,
                    'value': alert.value,
                    'threshold': alert.threshold,
                    'severity': alert.severity,
                    'message': alert.message
                }
                for alert in self.monitor.get_alerts()[-10:]  # Son 10 uyarı
            ],
            'recent_decisions': self.decision_history[-5:],  # Son 5 karar
            'alerts_summary': {
                'total': len(self.monitor.alerts),
                'critical': len(self.monitor.get_alerts('critical')),
                'warning': len(self.monitor.get_alerts('warning')),
                'info': len(self.monitor.get_alerts('info'))
            },
            'health_metrics': {
                'pack_voltage': self.monitor.get_pack_voltage(),
                'pack_current': self.monitor.get_pack_current(),
                'pack_temperature': self.monitor.get_pack_temperature(),
                'pack_soc': self.monitor.get_pack_soc(),
                'pack_soh': self.monitor.get_pack_soh(),
                'voltage_imbalance': max([c.voltage for c in self.monitor.cell_data if c]) - 
                                     min([c.voltage for c in self.monitor.cell_data if c])
                                     if [c for c in self.monitor.cell_data if c] else 0
            }
        }
        
        return report


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    # Controller oluştur
    controller = BMSController('BAT_001', num_cells=8)
    
    print("=== BMS Controller Initialization ===")
    print(f"Battery ID: {controller.battery_id}")
    print(f"Number of cells: {controller.num_cells}")
    
    # Örnek batarya verileri oluştur
    cell_ids = list(range(8))
    voltages = [3.7 + np.random.normal(0, 0.05) for _ in cell_ids]
    currents = [5.0 + np.random.normal(0, 0.3) for _ in cell_ids]
    temperatures = [25 + np.random.normal(0, 1) for _ in cell_ids]
    socs = [50 + np.random.normal(0, 5) for _ in cell_ids]
    sohs = [95 + np.random.normal(0, 3) for _ in cell_ids]
    impedances = [50 + np.random.normal(0, 3) for _ in cell_ids]
    
    # DataFrame oluştur
    data = pd.DataFrame({
        'cell_id': cell_ids,
        'voltage': voltages,
        'current': currents,
        'temperature': temperatures,
        'soc': socs,
        'soh': sohs,
        'impedance': impedances
    })
    
    # Verileri güncelle
    print("\n=== Updating Battery Data ===")
    controller.update_batteries(data)
    
    # İzlemeyi gerçekleştir
    print("\n=== Executing Monitoring ===")
    monitoring_result = controller.execute_monitoring()
    print(f"Monitoring Result: {monitoring_result}")
    
    # Şarj izni kontrol et
    print("\n=== Checking Permissions ===")
    print(f"Charging allowed: {controller.allow_charging()}")
    print(f"Discharging allowed: {controller.allow_discharging()}")
    
    # Şarj işlemini işle
    print("\n=== Processing Charge ===")
    charge_result = controller.process_charge_discharge(current_ma=5000, duration_sec=3600)
    print(f"Charge Result: {charge_result}")
    
    # SOH tahminini al
    print("\n=== SOH Estimation ===")
    soh_estimate = controller.estimate_soh()
    print(f"SOH Estimate: {soh_estimate}")
    
    # Karar ver
    print("\n=== Making Decision ===")
    soh_value = controller.monitor.get_pack_soh()
    decision = controller.make_decision(soh_value)
    print(f"Decision: {decision}")
    
    # Sistem sağlığı raporu al
    print("\n=== System Health Report ===")
    report = controller.generate_report()
    print(f"Report: {report}")
