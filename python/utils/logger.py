"""
TUBITAK 7260634 - Project Logger
Proje yönetimi için logging sistemi
"""

import logging
import logging.handlers
from pathlib import Path
from datetime import datetime
import json

from utils.constants import LOGS_DIR, LOG_LEVELS


class ColoredFormatter(logging.Formatter):
    """Renkli console output için formatter"""
    
    COLORS = {
        'DEBUG': '\033[36m',      # Cyan
        'INFO': '\033[32m',       # Green
        'WARNING': '\033[33m',    # Yellow
        'ERROR': '\033[31m',      # Red
        'CRITICAL': '\033[35m'    # Magenta
    }
    RESET = '\033[0m'
    
    def format(self, record):
        log_color = self.COLORS.get(record.levelname, self.RESET)
        record.levelname = f"{log_color}{record.levelname}{self.RESET}"
        return super().format(record)


def setup_logger(name, level='INFO', log_file=None):
    """
    Logger kurulumu
    
    Parameters:
    -----------
    name : str
        Logger adı
    level : str
        Log seviyesi (DEBUG, INFO, WARNING, ERROR, CRITICAL)
    log_file : str, optional
        Log dosya yolu
        
    Returns:
    --------
    logger : logging.Logger
        Konfigüre edilmiş logger
    """
    
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)
    
    # Console handler (colored output)
    console_handler = logging.StreamHandler()
    console_handler.setLevel(LOG_LEVELS.get(level, logging.INFO))
    
    console_formatter = ColoredFormatter(
        fmt='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )
    console_handler.setFormatter(console_formatter)
    logger.addHandler(console_handler)
    
    # File handler
    if log_file is None:
        log_file = LOGS_DIR / f"project_{datetime.now().strftime('%Y%m%d')}.log"
    else:
        log_file = Path(log_file)
    
    log_file.parent.mkdir(parents=True, exist_ok=True)
    
    file_handler = logging.handlers.RotatingFileHandler(
        log_file,
        maxBytes=10485760,  # 10 MB
        backupCount=5
    )
    file_handler.setLevel(logging.DEBUG)
    
    file_formatter = logging.Formatter(
        fmt='%(asctime)s - %(name)s - %(levelname)s - [%(filename)s:%(lineno)d] - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )
    file_handler.setFormatter(file_formatter)
    logger.addHandler(file_handler)
    
    return logger


class Logger:
    """
    Proje geneli logging sınıfı
    """
    
    _instance = None
    _loggers = {}
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance
    
    def __init__(self):
        self.main_logger = setup_logger('TUBITAK_7260634', level='INFO')
    
    def get_logger(self, module_name):
        """Modül için logger al"""
        if module_name not in self._loggers:
            self._loggers[module_name] = setup_logger(
                f'TUBITAK_7260634.{module_name}',
                level='INFO'
            )
        return self._loggers[module_name]
    
    def log_data_processing(self, battery_id, rows_processed, rows_cleaned, duration):
        """Veri işleme log'u"""
        logger = self.get_logger('data_processing')
        logger.info(
            f"Battery {battery_id}: "
            f"Processed={rows_processed}, "
            f"Cleaned={rows_cleaned}, "
            f"Duration={duration:.2f}s"
        )
    
    def log_model_training(self, model_name, accuracy, r2_score, training_time):
        """Model training log'u"""
        logger = self.get_logger('model')
        logger.info(
            f"Model {model_name}: "
            f"Accuracy={accuracy:.4f}, "
            f"R²={r2_score:.4f}, "
            f"Training_Time={training_time:.2f}s"
        )
    
    def log_prediction(self, battery_id, soh, confidence):
        """Tahmin log'u"""
        logger = self.get_logger('prediction')
        logger.info(
            f"Battery {battery_id}: "
            f"SoH={soh:.2%}, "
            f"Confidence={confidence:.2%}"
        )
    
    def log_bms_alert(self, alert_type, battery_id, value, threshold):
        """BMS uyarı log'u"""
        logger = self.get_logger('bms')
        logger.warning(
            f"BMS ALERT ({alert_type}): "
            f"Battery {battery_id}, "
            f"Value={value:.3f}, "
            f"Threshold={threshold:.3f}"
        )
    
    def log_error(self, module, error_message, exception=None):
        """Hata log'u"""
        logger = self.get_logger(module)
        if exception:
            logger.error(f"{error_message}", exc_info=True)
        else:
            logger.error(error_message)
    
    def log_performance(self, operation, duration, items_processed=None):
        """Performans log'u"""
        logger = self.get_logger('performance')
        msg = f"{operation}: {duration:.4f}s"
        if items_processed:
            msg += f" ({items_processed} items, {items_processed/duration:.0f} items/s)"
        logger.info(msg)


# Global logger instance
_global_logger = Logger()


def get_logger(module_name):
    """Modül için logger al"""
    return _global_logger.get_logger(module_name)


def log_json(data, filename='debug.json'):
    """Debug için JSON log"""
    log_file = LOGS_DIR / filename
    with open(log_file, 'w') as f:
        json.dump(data, f, indent=2, default=str)


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == '__main__':
    logger = get_logger('test_module')
    
    logger.info('Program başladı')
    logger.debug('Debug mesajı')
    logger.warning('Uyarı mesajı')
    logger.error('Hata oluştu')
    
    # Veri işleme log'u
    _global_logger.log_data_processing('BAT_001', 1000, 950, 2.5)
    
    # Model training log'u
    _global_logger.log_model_training('RandomForest', 0.92, 0.87, 45.3)
    
    # Tahmin log'u
    _global_logger.log_prediction('BAT_001', 0.88, 0.95)
    
    # BMS uyarı
    _global_logger.log_bms_alert('VOLTAGE_HIGH', 'BAT_001', 4.25, 4.20)
    
    # Performans
    _global_logger.log_performance('Feature Extraction', 3.2, items_processed=5000)
