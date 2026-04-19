"""
TUBITAK 7260634 - Battery Management System (BMS)
Batarya yönetim sistemi, izleme ve koruması
"""

import numpy as np
import pandas as pd
from datetime import datetime, timedelta
from collections import deque
from dataclasses import dataclass
from enum import Enum

from utils.constants import (
    BMS_MAX_VOLTAGE, BMS_MIN_VOLTAGE,
    BMS_MAX_CHARGE_CURRENT, BMS_MAX_DISCHARGE_CURRENT,
    BMS_MAX_TEMPERATURE, BMS_MIN_TEMPERATURE, BMS_SHUTDOWN_TEMPERATURE,
    BMS_BALANCING_ENABLED, BMS_BALANCING_THRESHOLD, BMS_BALANCING_CURRENT,
    BMS_ALERT_THRESHOLD, BMS_MONITOR_INTERVAL,
    NOMINAL_VOLTAGE, NOMINAL_CAPACITY
)
from utils.logger import get_logger


# ============================================
# ENUMS
# ============================================

class CellState(Enum):
    """Hücre durumları"""
    NORMAL = "normal"
    WARNING = "warning"
    ALERT = "alert"
    CRITICAL = "critical"
    SHUTDOWN = "shutdown"


class ChargeState(Enum):
    """Şarj durumları"""
    IDLE = "idle"
    CHARGING = "charging"
    DISCHARGING = "discharging"
    BALANCING = "balancing"
    FAULT = "fault"


# ============================================
# DATA CLASSES
# ============================================

@dataclass
class CellData:
    """Batarya hücresi verisi"""
    cell_id: int
    voltage: float  # V
    current: float  # A
    temperature: float  # °C
    soc: float  # State of Charge (%)
    soh: float  # State of Health (%)
    impedance: float  # mOhm
    timestamp: datetime = None
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now()


@dataclass
class BatteryCellAlert:
    """Hücre uyarısı"""
    cell_id: int
    alert_type: str
    value: float
    threshold: float
    severity: str  # info, warning, critical
    message: str
    timestamp: datetime = None
    
    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now()


# ============================================
# BATTERY MONITOR
# ============================================

class BatteryMonitor:
    """
    Batarya izleme sistemi
    Gerçek zamanlı hücre izlemesi ve anomali algılaması
    """
    
    def __init__(self, battery_id, num_cells=8, monitor_interval=BMS_MONITOR_INTERVAL):
        """
        Initialize BatteryMonitor
        
        Parameters:
        -----------
        battery_id : str
            Batarya ID'si
        num_cells : int
            Hücre sayısı (default 8S = 8 cell series)
        monitor_interval : float
            İzleme aralığı (saniye)
        """
        self.battery_id = battery_id
        self.num_cells = num_cells
        self.monitor_interval = monitor_interval
        self.logger = get_logger('bms_monitor')
        
        # Veri depoları (son 1000 okuma)
        self.voltage_history = deque(maxlen=1000)
        self.current_history = deque(maxlen=1000)
        self.temperature_history = deque(maxlen=1000)
        self.cell_data = [None] * num_cells
        self.alerts = []
        
        # İstatistikler
        self.last_monitor_time = datetime.now()
        self.monitor_count = 0
    
    def update_cell_data(self, cell_id, voltage, current, temperature, soc, soh, impedance):
        """
        Hücre verisini güncelle
        
        Parameters:
        -----------
        cell_id : int
            Hücre numarası (0-7)
        voltage : float
            Gerilim (V)
        current : float
            Akım (A)
        temperature : float
            Sıcaklık (°C)
        soc : float
            Şarj durumu (%)
        soh : float
            Sağlık durumu (%)
        impedance : float
            İç direnç (mOhm)
        """
        if cell_id >= self.num_cells:
            self.logger.error(f"Invalid cell ID: {cell_id}")
            return False
        
        cell_data = CellData(
            cell_id=cell_id,
            voltage=voltage,
            current=current,
            temperature=temperature,
            soc=soc,
            soh=soh,
            impedance=impedance
        )
        
        self.cell_data[cell_id] = cell_data
        
        # Geçmişe ekle
        self.voltage_history.append((cell_id, voltage))
        self.current_history.append((cell_id, current))
        self.temperature_history.append((cell_id, temperature))
        
        return True
    
    def check_overvoltage(self, cell_id):
        """Aşırı gerilim durumunu kontrol et"""
        if self.cell_data[cell_id] is None:
            return False
        
        voltage = self.cell_data[cell_id].voltage
        if voltage > BMS_MAX_VOLTAGE:
            severity = "critical" if voltage > BMS_MAX_VOLTAGE + 0.2 else "warning"
            alert = BatteryCellAlert(
                cell_id=cell_id,
                alert_type="OVERVOLTAGE",
                value=voltage,
                threshold=BMS_MAX_VOLTAGE,
                severity=severity,
                message=f"Cell {cell_id} overvoltage: {voltage:.3f}V"
            )
            self.alerts.append(alert)
            return True
        return False
    
    def check_undervoltage(self, cell_id):
        """Düşük gerilim durumunu kontrol et"""
        if self.cell_data[cell_id] is None:
            return False
        
        voltage = self.cell_data[cell_id].voltage
        if voltage < BMS_MIN_VOLTAGE:
            severity = "critical" if voltage < BMS_MIN_VOLTAGE - 0.2 else "warning"
            alert = BatteryCellAlert(
                cell_id=cell_id,
                alert_type="UNDERVOLTAGE",
                value=voltage,
                threshold=BMS_MIN_VOLTAGE,
                severity=severity,
                message=f"Cell {cell_id} undervoltage: {voltage:.3f}V"
            )
            self.alerts.append(alert)
            return True
        return False
    
    def check_overtemperature(self, cell_id):
        """Aşırı sıcaklık durumunu kontrol et"""
        if self.cell_data[cell_id] is None:
            return False
        
        temp = self.cell_data[cell_id].temperature
        if temp > BMS_MAX_TEMPERATURE:
            severity = "critical" if temp > BMS_SHUTDOWN_TEMPERATURE else "warning"
            alert = BatteryCellAlert(
                cell_id=cell_id,
                alert_type="OVERTEMPERATURE",
                value=temp,
                threshold=BMS_MAX_TEMPERATURE,
                severity=severity,
                message=f"Cell {cell_id} overtemperature: {temp:.1f}°C"
            )
            self.alerts.append(alert)
            return True
        return False
    
    def check_undertemperature(self, cell_id):
        """Düşük sıcaklık durumunu kontrol et"""
        if self.cell_data[cell_id] is None:
            return False
        
        temp = self.cell_data[cell_id].temperature
        if temp < BMS_MIN_TEMPERATURE:
            alert = BatteryCellAlert(
                cell_id=cell_id,
                alert_type="UNDERTEMPERATURE",
                value=temp,
                threshold=BMS_MIN_TEMPERATURE,
                severity="warning",
                message=f"Cell {cell_id} undertemperature: {temp:.1f}°C"
            )
            self.alerts.append(alert)
            return True
        return False
    
    def check_imbalance(self):
        """Hücre dengesizliğini kontrol et"""
        voltages = [cell.voltage for cell in self.cell_data if cell is not None]
        
        if len(voltages) < 2:
            return False
        
        voltage_diff = max(voltages) - min(voltages)
        
        if voltage_diff > BMS_ALERT_THRESHOLD:
            alert = BatteryCellAlert(
                cell_id=-1,  # Pack-level
                alert_type="IMBALANCE",
                value=voltage_diff,
                threshold=BMS_ALERT_THRESHOLD,
                severity="warning",
                message=f"Cell imbalance detected: {voltage_diff*1000:.1f}mV"
            )
            self.alerts.append(alert)
            return True
        
        return False
    
    def check_overcurrent(self, charge=True):
        """Aşırı akım durumunu kontrol et"""
        currents = [cell.current for cell in self.cell_data if cell is not None]
        
        if not currents:
            return False
        
        max_current = max(currents)
        threshold = BMS_MAX_CHARGE_CURRENT if charge else BMS_MAX_DISCHARGE_CURRENT
        
        if abs(max_current) > threshold:
            alert = BatteryCellAlert(
                cell_id=-1,
                alert_type="OVERCURRENT",
                value=max_current,
                threshold=threshold,
                severity="critical",
                message=f"Pack overcurrent: {max_current:.2f}A"
            )
            self.alerts.append(alert)
            return True
        
        return False
    
    def monitor(self):
        """Tam sistem izlemesi"""
        alerts_found = 0
        
        # Her hücreyi kontrol et
        for cell_id in range(self.num_cells):
            if self.check_overvoltage(cell_id):
                alerts_found += 1
            if self.check_undervoltage(cell_id):
                alerts_found += 1
            if self.check_overtemperature(cell_id):
                alerts_found += 1
            if self.check_undertemperature(cell_id):
                alerts_found += 1
        
        # Pack-level kontroller
        if self.check_imbalance():
            alerts_found += 1
        if self.check_overcurrent(charge=True):
            alerts_found += 1
        
        self.monitor_count += 1
        self.last_monitor_time = datetime.now()
        
        if alerts_found > 0:
            self.logger.warning(f"Found {alerts_found} alerts during monitoring")
        
        return alerts_found
    
    def get_alerts(self, severity=None):
        """Uyarıları al"""
        if severity is None:
            return self.alerts
        
        return [a for a in self.alerts if a.severity == severity]
    
    def clear_alerts(self):
        """Uyarıları temizle"""
        self.alerts.clear()
    
    def get_pack_voltage(self):
        """Pack gerilimini al (tüm hücrelerin toplamı)"""
        voltages = [cell.voltage for cell in self.cell_data if cell is not None]
        return sum(voltages) if voltages else 0
    
    def get_pack_current(self):
        """Pack akımını al (ortalama)"""
        currents = [cell.current for cell in self.cell_data if cell is not None]
        return np.mean(currents) if currents else 0
    
    def get_pack_temperature(self):
        """Pack sıcaklığını al (maksimum)"""
        temps = [cell.temperature for cell in self.cell_data if cell is not None]
        return max(temps) if temps else 0
    
    def get_pack_soc(self):
        """Pack SOC'ını al (ortalama)"""
        socs = [cell.soc for cell in self.cell_data if cell is not None]
        return np.mean(socs) if socs else 0
    
    def get_pack_soh(self):
        """Pack SOH'ını al (minimum)"""
        sohs = [cell.soh for cell in self.cell_data if cell is not None]
        return min(sohs) if sohs else 100
    
    def get_status_summary(self):
        """Durum özeti al"""
        return {
            'battery_id': self.battery_id,
            'timestamp': datetime.now().isoformat(),
            'pack_voltage': self.get_pack_voltage(),
            'pack_current': self.get_pack_current(),
            'pack_temperature': self.get_pack_temperature(),
            'pack_soc': self.get_pack_soc(),
            'pack_soh': self.get_pack_soh(),
            'alerts_count': len(self.alerts),
            'critical_alerts': len(self.get_alerts('critical')),
            'monitor_count': self.monitor_count
        }


# ============================================
# CELL BALANCER
# ============================================

class CellBalancer:
    """
    Aktif hücre dengeleme sistemi
    Heterojen hücreler için çift yönlü dengeleme
    """
    
    def __init__(self, battery_monitor, balancing_threshold=BMS_BALANCING_THRESHOLD,
                 balancing_current=BMS_BALANCING_CURRENT):
        """
        Initialize CellBalancer
        
        Parameters:
        -----------
        battery_monitor : BatteryMonitor
            İzleme sistemi referansı
        balancing_threshold : float
            Dengeleme eşiği (V)
        balancing_current : float
            Dengeleme akımı (A)
        """
        self.monitor = battery_monitor
        self.threshold = balancing_threshold
        self.balancing_current = balancing_current
        self.logger = get_logger('balancer')
        self.balancing_active = False
    
    def calculate_balancing_currents(self):
        """
        Dengeleme akımlarını hesapla
        
        Returns:
        --------
        dict : Hücre ID'leri ve dengeleme akımları
        """
        cells = self.monitor.cell_data
        voltages = [cell.voltage for cell in cells if cell is not None]
        
        if len(voltages) < 2:
            return {}
        
        mean_voltage = np.mean(voltages)
        balancing_currents = {}
        
        for cell_id, cell in enumerate(cells):
            if cell is None:
                continue
            
            voltage_diff = cell.voltage - mean_voltage
            
            # Artış ayarlaması (aşırı voltaj)
            if voltage_diff > self.threshold:
                balancing_currents[cell_id] = min(self.balancing_current, voltage_diff * 2)
            # Düşüş ayarlaması (düşük voltaj)
            elif voltage_diff < -self.threshold:
                balancing_currents[cell_id] = max(-self.balancing_current, voltage_diff * 2)
        
        return balancing_currents
    
    def execute_balancing(self):
        """Dengelemeyi gerçekleştir"""
        balancing_currents = self.calculate_balancing_currents()
        
        if not balancing_currents:
            self.balancing_active = False
            return False
        
        self.balancing_active = True
        
        # Simülasyon: Gerilim farkını azalt
        for cell_id, current in balancing_currents.items():
            if self.monitor.cell_data[cell_id] is not None:
                # Voltaj değişimini simüle et
                voltage_adjustment = current * 0.01  # Basit model
                self.monitor.cell_data[cell_id].voltage -= voltage_adjustment
        
        self.logger.info(f"Balancing executed: {len(balancing_currents)} cells")
        return True
    
    def is_balancing_needed(self):
        """Dengeleme gerekli mi?"""
        cells = self.monitor.cell_data
        voltages = [cell.voltage for cell in cells if cell is not None]
        
        if len(voltages) < 2:
            return False
        
        voltage_diff = max(voltages) - min(voltages)
        return voltage_diff > self.threshold


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    # Monitor oluştur
    monitor = BatteryMonitor('BAT_001', num_cells=8)
    
    # Örnek hücre verisi ekle
    for cell_id in range(8):
        voltage = 3.7 + np.random.normal(0, 0.05)
        current = 5.0 + np.random.normal(0, 0.5)
        temperature = 25 + np.random.normal(0, 2)
        
        monitor.update_cell_data(
            cell_id=cell_id,
            voltage=voltage,
            current=current,
            temperature=temperature,
            soc=50 + np.random.normal(0, 10),
            soh=95 + np.random.normal(0, 5),
            impedance=50 + np.random.normal(0, 5)
        )
    
    # İzlemeyi çalıştır
    monitor.monitor()
    
    # Durum özeti al
    status = monitor.get_status_summary()
    print(f"Battery Status: {status}")
    
    # Balanceı oluştur
    balancer = CellBalancer(monitor)
    if balancer.is_balancing_needed():
        balancer.execute_balancing()
        print("Balancing executed!")
