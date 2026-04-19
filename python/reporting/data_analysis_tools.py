"""
TUBITAK 7260634 - Veri Analiz Araçları
Batarya verisi analiz, görselleştirme ve raporlama araçları
"""

import numpy as np
import pandas as pd
from datetime import datetime
from pathlib import Path


class BatteryDataAnalyzer:
    """
    Batarya veri seti analiz aracı.
    
    Kullanım:
        analyzer = BatteryDataAnalyzer()
        analyzer.load_data('battery_data.csv')
        summary = analyzer.get_summary()
        report = analyzer.generate_analysis_report()
    """

    def __init__(self):
        self.data = None
        self.battery_id = None
        self.analysis_results = {}

    def load_data(self, filepath):
        """CSV dosyasından veri yükle."""
        path = Path(filepath)
        if path.suffix == '.csv':
            self.data = pd.read_csv(filepath)
        elif path.suffix in ('.xlsx', '.xls'):
            self.data = pd.read_excel(filepath)
        elif path.suffix == '.parquet':
            self.data = pd.read_parquet(filepath)
        elif path.suffix == '.json':
            self.data = pd.read_json(filepath)
        else:
            raise ValueError(f"Desteklenmeyen dosya formatı: {path.suffix}")

        return len(self.data)

    def load_from_dataframe(self, df, battery_id=None):
        """DataFrame'den veri yükle."""
        self.data = df.copy()
        self.battery_id = battery_id

    # ─────────────────────────────────────────
    # Temel İstatistik Analiz
    # ─────────────────────────────────────────

    def get_summary(self):
        """Veri kümesi özet istatistikleri."""
        if self.data is None:
            return {}

        numeric_cols = self.data.select_dtypes(include=[np.number]).columns.tolist()

        summary = {
            'row_count': len(self.data),
            'column_count': len(self.data.columns),
            'numeric_columns': len(numeric_cols),
            'missing_total': int(self.data.isnull().sum().sum()),
            'missing_percentage': round(self.data.isnull().sum().sum() / (len(self.data) * len(self.data.columns)) * 100, 2),
            'duplicate_rows': int(self.data.duplicated().sum()),
            'memory_mb': round(self.data.memory_usage(deep=True).sum() / 1024 / 1024, 2)
        }

        # Sütun bazında istatistikler
        column_stats = {}
        for col in numeric_cols:
            series = self.data[col].dropna()
            if len(series) == 0:
                continue
            column_stats[col] = {
                'count': int(series.count()),
                'mean': round(float(series.mean()), 4),
                'std': round(float(series.std()), 4),
                'min': round(float(series.min()), 4),
                'max': round(float(series.max()), 4),
                'median': round(float(series.median()), 4),
                'q25': round(float(series.quantile(0.25)), 4),
                'q75': round(float(series.quantile(0.75)), 4),
                'missing': int(self.data[col].isnull().sum()),
                'missing_pct': round(self.data[col].isnull().sum() / len(self.data) * 100, 2)
            }

        summary['columns'] = column_stats
        self.analysis_results['summary'] = summary
        return summary

    def detect_outliers(self, method='iqr', threshold=1.5):
        """Aykırı değer tespiti."""
        if self.data is None:
            return {}

        numeric_cols = self.data.select_dtypes(include=[np.number]).columns.tolist()
        outlier_report = {}

        for col in numeric_cols:
            series = self.data[col].dropna()
            if len(series) == 0:
                continue

            if method == 'iqr':
                q1 = series.quantile(0.25)
                q3 = series.quantile(0.75)
                iqr = q3 - q1
                lower = q1 - threshold * iqr
                upper = q3 + threshold * iqr
                outliers = series[(series < lower) | (series > upper)]
            elif method == 'zscore':
                mean = series.mean()
                std = series.std()
                if std == 0:
                    continue
                z_scores = np.abs((series - mean) / std)
                outliers = series[z_scores > threshold]
            else:
                continue

            if len(outliers) > 0:
                outlier_report[col] = {
                    'count': len(outliers),
                    'percentage': round(len(outliers) / len(series) * 100, 2),
                    'min_outlier': round(float(outliers.min()), 4),
                    'max_outlier': round(float(outliers.max()), 4),
                }

        self.analysis_results['outliers'] = outlier_report
        return outlier_report

    def check_data_quality(self):
        """Veri kalite raporu."""
        if self.data is None:
            return {}

        total_cells = len(self.data) * len(self.data.columns)
        missing = int(self.data.isnull().sum().sum())
        duplicates = int(self.data.duplicated().sum())

        # Fiziksel sınır kontrolleri
        violations = {}
        bounds = {
            'voltage': (2.0, 4.5),
            'current': (-25, 25),
            'temperature': (-10, 90),
            'soc': (0, 100),
            'soh': (0, 100),
            'impedance': (0, 1000)
        }

        for col, (low, high) in bounds.items():
            if col in self.data.columns:
                series = self.data[col].dropna()
                out_of_range = series[(series < low) | (series > high)]
                if len(out_of_range) > 0:
                    violations[col] = {
                        'count': len(out_of_range),
                        'percentage': round(len(out_of_range) / len(series) * 100, 2),
                        'range': f"{low} — {high}",
                        'actual_min': round(float(series.min()), 4),
                        'actual_max': round(float(series.max()), 4)
                    }

        # Kalite skoru
        completeness = 1 - (missing / total_cells) if total_cells > 0 else 1
        uniqueness = 1 - (duplicates / len(self.data)) if len(self.data) > 0 else 1
        validity = 1 - (sum(v['count'] for v in violations.values()) / len(self.data)) if violations and len(self.data) > 0 else 1

        quality_score = round((completeness * 0.4 + uniqueness * 0.2 + validity * 0.4) * 100, 1)

        report = {
            'total_records': len(self.data),
            'total_cells': total_cells,
            'missing_cells': missing,
            'completeness': round(completeness * 100, 1),
            'duplicate_rows': duplicates,
            'uniqueness': round(uniqueness * 100, 1),
            'violations': violations,
            'validity': round(validity * 100, 1),
            'quality_score': quality_score,
            'quality_grade': 'A' if quality_score >= 95 else ('B' if quality_score >= 85 else ('C' if quality_score >= 70 else 'D'))
        }

        self.analysis_results['quality'] = report
        return report

    # ─────────────────────────────────────────
    # Batarya Spesifik Analiz
    # ─────────────────────────────────────────

    def analyze_voltage_profile(self):
        """Gerilim profili analizi."""
        if self.data is None or 'voltage' not in self.data.columns:
            return {}

        v = self.data['voltage'].dropna()
        return {
            'mean_voltage': round(float(v.mean()), 3),
            'voltage_range': round(float(v.max() - v.min()), 3),
            'voltage_std': round(float(v.std()), 4),
            'below_min_count': int((v < 2.5).sum()),
            'above_max_count': int((v > 4.2).sum()),
            'normal_range_pct': round(float(((v >= 2.5) & (v <= 4.2)).sum() / len(v) * 100), 1)
        }

    def analyze_temperature_profile(self):
        """Sıcaklık profili analizi."""
        if self.data is None or 'temperature' not in self.data.columns:
            return {}

        t = self.data['temperature'].dropna()
        return {
            'mean_temp': round(float(t.mean()), 1),
            'max_temp': round(float(t.max()), 1),
            'min_temp': round(float(t.min()), 1),
            'temp_range': round(float(t.max() - t.min()), 1),
            'overtemp_count': int((t > 60).sum()),
            'undertemp_count': int((t < 0).sum()),
            'optimal_range_pct': round(float(((t >= 15) & (t <= 35)).sum() / len(t) * 100), 1)
        }

    def analyze_soh_distribution(self):
        """SoH dağılım analizi."""
        if self.data is None or 'soh' not in self.data.columns:
            return {}

        soh = self.data['soh'].dropna()
        return {
            'mean_soh': round(float(soh.mean()), 1),
            'std_soh': round(float(soh.std()), 1),
            'min_soh': round(float(soh.min()), 1),
            'max_soh': round(float(soh.max()), 1),
            'reuse_count': int((soh >= 85).sum()),
            'recycle_count': int(((soh >= 70) & (soh < 85)).sum()),
            'critical_count': int((soh < 70).sum()),
            'reuse_pct': round(float((soh >= 85).sum() / len(soh) * 100), 1),
            'recycle_pct': round(float(((soh >= 70) & (soh < 85)).sum() / len(soh) * 100), 1),
            'critical_pct': round(float((soh < 70).sum() / len(soh) * 100), 1)
        }

    def compute_correlation_matrix(self):
        """Sayısal sütunlar arası korelasyon matrisi."""
        if self.data is None:
            return None

        numeric_data = self.data.select_dtypes(include=[np.number])
        if numeric_data.empty:
            return None

        corr = numeric_data.corr()
        self.analysis_results['correlation'] = corr.to_dict()
        return corr

    def find_strong_correlations(self, threshold=0.7):
        """Güçlü korelasyonları bul."""
        corr = self.compute_correlation_matrix()
        if corr is None:
            return []

        strong = []
        cols = corr.columns.tolist()
        for i in range(len(cols)):
            for j in range(i + 1, len(cols)):
                val = abs(corr.iloc[i, j])
                if val >= threshold:
                    strong.append({
                        'feature_1': cols[i],
                        'feature_2': cols[j],
                        'correlation': round(float(corr.iloc[i, j]), 3),
                        'abs_correlation': round(val, 3)
                    })

        return sorted(strong, key=lambda x: x['abs_correlation'], reverse=True)

    # ─────────────────────────────────────────
    # Rapor Oluşturma
    # ─────────────────────────────────────────

    def generate_analysis_report(self):
        """Kapsamlı analiz raporu oluştur (Markdown formatında)."""
        if self.data is None:
            return "Veri yüklenmemiş."

        summary = self.get_summary()
        quality = self.check_data_quality()
        outliers = self.detect_outliers()
        voltage = self.analyze_voltage_profile()
        temp = self.analyze_temperature_profile()
        soh = self.analyze_soh_distribution()
        strong_corr = self.find_strong_correlations()

        report_lines = []

        # Başlık
        report_lines.append(f"# Batarya Veri Analiz Raporu")
        report_lines.append(f"\n**Batarya:** {self.battery_id or 'Bilinmiyor'}")
        report_lines.append(f"**Tarih:** {datetime.now().strftime('%d.%m.%Y %H:%M')}")
        report_lines.append(f"**Kayıt Sayısı:** {summary.get('row_count', 0):,}")
        report_lines.append(f"**Sütun Sayısı:** {summary.get('column_count', 0)}")
        report_lines.append("")

        # Veri Kalitesi
        report_lines.append("## 1. Veri Kalitesi\n")
        grade = quality.get('quality_grade', '?')
        score = quality.get('quality_score', 0)
        report_lines.append(f"**Kalite Skoru:** {score}/100 ({grade})\n")
        report_lines.append("| Metrik | Değer |")
        report_lines.append("|--------|-------|")
        report_lines.append(f"| Tamlık | %{quality.get('completeness', 0)} |")
        report_lines.append(f"| Benzersizlik | %{quality.get('uniqueness', 0)} |")
        report_lines.append(f"| Geçerlilik | %{quality.get('validity', 0)} |")
        report_lines.append(f"| Eksik hücre | {quality.get('missing_cells', 0)} |")
        report_lines.append(f"| Tekrarlanan satır | {quality.get('duplicate_rows', 0)} |")
        report_lines.append("")

        if quality.get('violations'):
            report_lines.append("### Fiziksel Sınır İhlalleri\n")
            report_lines.append("| Sütun | İhlal Sayısı | % | Beklenen Aralık |")
            report_lines.append("|-------|-----------:|---:|----------------|")
            for col, v in quality['violations'].items():
                report_lines.append(f"| {col} | {v['count']} | %{v['percentage']} | {v['range']} |")
            report_lines.append("")

        # Aykırı Değerler
        if outliers:
            report_lines.append("## 2. Aykırı Değer Analizi\n")
            report_lines.append("| Sütun | Aykırı Sayısı | % | Min | Max |")
            report_lines.append("|-------|-------------:|---:|----:|----:|")
            for col, info in outliers.items():
                report_lines.append(f"| {col} | {info['count']} | %{info['percentage']} | {info['min_outlier']} | {info['max_outlier']} |")
            report_lines.append("")

        # Gerilim Profili
        if voltage:
            report_lines.append("## 3. Gerilim Profili\n")
            report_lines.append(f"| Metrik | Değer |")
            report_lines.append(f"|--------|-------|")
            report_lines.append(f"| Ortalama | {voltage.get('mean_voltage', 0)} V |")
            report_lines.append(f"| Aralık | {voltage.get('voltage_range', 0)} V |")
            report_lines.append(f"| Alt sınır ihlali | {voltage.get('below_min_count', 0)} |")
            report_lines.append(f"| Üst sınır ihlali | {voltage.get('above_max_count', 0)} |")
            report_lines.append(f"| Normal aralık oranı | %{voltage.get('normal_range_pct', 0)} |")
            report_lines.append("")

        # Sıcaklık Profili
        if temp:
            report_lines.append("## 4. Sıcaklık Profili\n")
            report_lines.append(f"| Metrik | Değer |")
            report_lines.append(f"|--------|-------|")
            report_lines.append(f"| Ortalama | {temp.get('mean_temp', 0)} °C |")
            report_lines.append(f"| Maksimum | {temp.get('max_temp', 0)} °C |")
            report_lines.append(f"| Minimum | {temp.get('min_temp', 0)} °C |")
            report_lines.append(f"| Aşırı sıcaklık sayısı | {temp.get('overtemp_count', 0)} |")
            report_lines.append(f"| Optimum aralık oranı | %{temp.get('optimal_range_pct', 0)} |")
            report_lines.append("")

        # SoH Dağılımı
        if soh:
            report_lines.append("## 5. SoH Dağılımı\n")
            report_lines.append(f"| Kategori | Sayı | Oran |")
            report_lines.append(f"|----------|-----:|-----:|")
            report_lines.append(f"| REUSE (≥%85) | {soh.get('reuse_count', 0)} | %{soh.get('reuse_pct', 0)} |")
            report_lines.append(f"| RECYCLE (%70–85) | {soh.get('recycle_count', 0)} | %{soh.get('recycle_pct', 0)} |")
            report_lines.append(f"| CRITICAL (<%70) | {soh.get('critical_count', 0)} | %{soh.get('critical_pct', 0)} |")
            report_lines.append(f"\n**Ortalama SoH:** %{soh.get('mean_soh', 0)}")
            report_lines.append("")

        # Korelasyon
        if strong_corr:
            report_lines.append("## 6. Güçlü Korelasyonlar (|r| ≥ 0.7)\n")
            report_lines.append("| Özellik 1 | Özellik 2 | Korelasyon |")
            report_lines.append("|-----------|-----------|:----------:|")
            for c in strong_corr[:10]:
                report_lines.append(f"| {c['feature_1']} | {c['feature_2']} | {c['correlation']} |")
            report_lines.append("")

        report_lines.append("---\n*Bu rapor otomatik olarak oluşturulmuştur.*")

        return '\n'.join(report_lines)

    def export_analysis(self, filepath):
        """Analiz raporunu dosyaya kaydet."""
        report = self.generate_analysis_report()
        path = Path(filepath)
        path.parent.mkdir(parents=True, exist_ok=True)

        with open(path, 'w', encoding='utf-8') as f:
            f.write(report)

        return str(path)


class ModelPerformanceAnalyzer:
    """Model performans karşılaştırma aracı."""

    def __init__(self):
        self.results = []

    def add_result(self, model_name, metrics):
        """Model sonucu ekle.
        
        Parameters:
            model_name: str — Model adı (ör. 'random_forest')
            metrics: dict — {'r2': float, 'rmse': float, 'mae': float, 'mape': float, 'cv_mean': float, 'cv_std': float}
        """
        self.results.append({
            'model': model_name,
            'timestamp': datetime.now().isoformat(),
            **metrics
        })

    def get_best_model(self, metric='r2'):
        """Belirtilen metriğe göre en iyi modeli döndür."""
        if not self.results:
            return None

        if metric in ('rmse', 'mae', 'mape'):
            # Düşük daha iyi
            return min(self.results, key=lambda x: x.get(metric, float('inf')))
        else:
            # Yüksek daha iyi
            return max(self.results, key=lambda x: x.get(metric, 0))

    def generate_comparison_report(self):
        """Model karşılaştırma raporu."""
        if not self.results:
            return "Karşılaştırılacak model sonucu yok."

        lines = ["# Model Performans Karşılaştırma Raporu\n"]
        lines.append(f"**Tarih:** {datetime.now().strftime('%d.%m.%Y')}")
        lines.append(f"**Model Sayısı:** {len(self.results)}\n")

        lines.append("| Model | R² | RMSE | MAE | MAPE | CV Ortalama | CV Std |")
        lines.append("|-------|----|------|-----|------|-------------|--------|")

        for r in self.results:
            lines.append(
                f"| {r['model']} "
                f"| {r.get('r2', '-'):.4f} " if isinstance(r.get('r2'), (int, float)) else f"| {r['model']} | — ",
            )

        # Basitleştirilmiş tablo
        lines = ["# Model Performans Karşılaştırma Raporu\n"]
        lines.append(f"**Tarih:** {datetime.now().strftime('%d.%m.%Y')}")
        lines.append(f"**Model Sayısı:** {len(self.results)}\n")

        lines.append("| Model | R² | RMSE | MAE |")
        lines.append("|-------|----|------|-----|")

        for r in self.results:
            r2 = f"{r['r2']:.4f}" if isinstance(r.get('r2'), (int, float)) else '—'
            rmse = f"{r['rmse']:.4f}" if isinstance(r.get('rmse'), (int, float)) else '—'
            mae = f"{r['mae']:.4f}" if isinstance(r.get('mae'), (int, float)) else '—'
            lines.append(f"| {r['model']} | {r2} | {rmse} | {mae} |")

        lines.append("")

        # En iyi model
        best = self.get_best_model('r2')
        if best:
            lines.append(f"**En İyi Model (R²):** {best['model']}")

        # Hedef kontrolü
        lines.append("\n### Hedef Karşılaştırma\n")
        lines.append("| Model | R² ≥ 0.85 | RMSE < 5.0 | Sonuç |")
        lines.append("|-------|-----------|-----------|-------|")
        for r in self.results:
            r2_ok = isinstance(r.get('r2'), (int, float)) and r['r2'] >= 0.85
            rmse_ok = isinstance(r.get('rmse'), (int, float)) and r['rmse'] < 5.0
            result = '✅ KABUL' if (r2_ok and rmse_ok) else '❌ RED'
            lines.append(f"| {r['model']} | {'✅' if r2_ok else '❌'} | {'✅' if rmse_ok else '❌'} | {result} |")

        lines.append("\n---\n*Bu rapor otomatik olarak oluşturulmuştur.*")
        return '\n'.join(lines)


# ─────────────────────────────────────────────
# Komut satırı kullanımı
# ─────────────────────────────────────────────

if __name__ == '__main__':
    print("=" * 60)
    print("TÜBİTAK 7260634 — Veri Analiz Araçları Demo")
    print("=" * 60)

    # Örnek veri oluştur
    np.random.seed(42)
    n = 1000

    sample_data = pd.DataFrame({
        'voltage': np.random.normal(3.7, 0.3, n),
        'current': np.random.normal(5.0, 3.0, n),
        'temperature': np.random.normal(30, 10, n),
        'soc': np.random.uniform(10, 100, n),
        'soh': np.random.normal(80, 12, n),
        'impedance': np.random.normal(55, 15, n)
    })

    # Analiz
    analyzer = BatteryDataAnalyzer()
    analyzer.load_from_dataframe(sample_data, battery_id='DEMO_BAT')

    report = analyzer.generate_analysis_report()
    print(report)

    # Model karşılaştırma demo
    print("\n" + "=" * 60)
    model_analyzer = ModelPerformanceAnalyzer()
    model_analyzer.add_result('random_forest', {'r2': 0.91, 'rmse': 3.2, 'mae': 2.1})
    model_analyzer.add_result('gradient_boosting', {'r2': 0.93, 'rmse': 2.8, 'mae': 1.8})
    model_analyzer.add_result('ridge_regression', {'r2': 0.78, 'rmse': 6.1, 'mae': 4.2})
    model_analyzer.add_result('neural_network', {'r2': 0.89, 'rmse': 3.5, 'mae': 2.4})

    comparison = model_analyzer.generate_comparison_report()
    print(comparison)
