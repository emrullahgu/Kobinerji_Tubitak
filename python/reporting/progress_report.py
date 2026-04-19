"""
TUBITAK 7260634 - Proje İlerleme Rapor Sistemi
Otomatik proje ilerleme raporları oluşturma
"""

import json
import os
from datetime import datetime, timedelta
from pathlib import Path


class ProjectProgressReport:
    """
    Proje ilerleme raporları oluşturma sınıfı.
    
    Kullanım:
        report = ProjectProgressReport('data.json')
        report.generate_full_report()
        report.export_report('rapor.md')
    """

    def __init__(self, data_path=None):
        self.project_data = None
        self.report_date = datetime.now()
        self.report_sections = []

        if data_path:
            self.load_project_data(data_path)

    def load_project_data(self, path):
        """Proje verilerini JSON dosyasından yükle."""
        with open(path, 'r', encoding='utf-8') as f:
            self.project_data = json.load(f)

    def load_from_dict(self, data):
        """Proje verilerini dict'ten yükle."""
        self.project_data = data

    # ─────────────────────────────────────────
    # Hesaplama Yardımcıları
    # ─────────────────────────────────────────

    def _parse_date(self, date_str):
        """Tarih string'ini datetime'a çevir."""
        for fmt in ('%Y-%m-%d', '%d.%m.%Y', '%Y-%m-%dT%H:%M:%S'):
            try:
                return datetime.strptime(date_str, fmt)
            except ValueError:
                continue
        return None

    def _days_between(self, start, end):
        """İki tarih arası gün farkı."""
        d1 = self._parse_date(start) if isinstance(start, str) else start
        d2 = self._parse_date(end) if isinstance(end, str) else end
        if d1 and d2:
            return (d2 - d1).days
        return 0

    def _get_status_emoji(self, status):
        """Durum emojisi döndür."""
        mapping = {
            'completed': '✅',
            'in-progress': '⏳',
            'pending': '⏸️',
            'delayed': '🔴',
            'at-risk': '⚠️'
        }
        return mapping.get(status, '❓')

    def _progress_bar(self, percent, width=20):
        """ASCII ilerleme çubuğu oluştur."""
        filled = int(width * percent / 100)
        empty = width - filled
        return f"[{'█' * filled}{'░' * empty}] %{percent}"

    # ─────────────────────────────────────────
    # Rapor Bölümleri
    # ─────────────────────────────────────────

    def calculate_schedule_variance(self):
        """Takvim sapma analizi hesapla."""
        if not self.project_data:
            return {}

        start = self._parse_date(self.project_data.get('startDate', ''))
        end = self._parse_date(self.project_data.get('endDate', ''))
        now = self.report_date

        if not start or not end:
            return {}

        total_days = (end - start).days
        elapsed_days = (now - start).days
        planned_progress = min(100, max(0, (elapsed_days / total_days) * 100))
        actual_progress = self.project_data.get('overallProgress', 0)
        variance = actual_progress - planned_progress

        return {
            'total_days': total_days,
            'elapsed_days': elapsed_days,
            'remaining_days': max(0, (end - now).days),
            'planned_progress': round(planned_progress, 1),
            'actual_progress': actual_progress,
            'variance': round(variance, 1),
            'status': 'on-track' if variance >= -5 else ('at-risk' if variance >= -15 else 'delayed')
        }

    def calculate_budget_status(self):
        """Bütçe durum analizi hesapla."""
        if not self.project_data:
            return {}

        total = self.project_data.get('totalBudget', 0)
        spent = self.project_data.get('spentBudget', 0)
        remaining = total - spent
        spent_pct = (spent / total * 100) if total > 0 else 0

        schedule = self.calculate_schedule_variance()
        planned_pct = schedule.get('planned_progress', 0)
        budget_variance = spent_pct - planned_pct

        return {
            'total': total,
            'spent': spent,
            'remaining': remaining,
            'spent_percentage': round(spent_pct, 1),
            'budget_variance': round(budget_variance, 1),
            'status': 'on-track' if abs(budget_variance) <= 10 else ('over-budget' if budget_variance > 0 else 'under-budget')
        }

    def analyze_work_packages(self):
        """İş paketleri analizi."""
        if not self.project_data:
            return []

        results = []
        for wp in self.project_data.get('workPackages', []):
            start = self._parse_date(wp.get('startDate', ''))
            end = self._parse_date(wp.get('endDate', ''))
            now = self.report_date

            if start and end:
                total_days = (end - start).days
                if now < start:
                    planned_pct = 0
                elif now > end:
                    planned_pct = 100
                else:
                    planned_pct = min(100, ((now - start).days / total_days) * 100)
            else:
                planned_pct = 0

            actual_pct = wp.get('progress', 0)
            variance = actual_pct - planned_pct

            results.append({
                'id': wp.get('id', ''),
                'name': wp.get('name', ''),
                'progress': actual_pct,
                'planned_progress': round(planned_pct, 1),
                'variance': round(variance, 1),
                'status': wp.get('status', 'pending'),
                'start': wp.get('startDate', ''),
                'end': wp.get('endDate', ''),
                'is_delayed': variance < -10
            })

        return results

    def analyze_milestones(self):
        """Kilometre taşları analizi."""
        if not self.project_data:
            return {'completed': 0, 'total': 0, 'upcoming': [], 'overdue': []}

        milestones = self.project_data.get('milestones', [])
        completed = sum(1 for m in milestones if m.get('status') == 'completed')
        total = len(milestones)

        upcoming = []
        overdue = []
        for m in milestones:
            if m.get('status') == 'completed':
                continue
            date = self._parse_date(m.get('date', ''))
            if date:
                if date < self.report_date:
                    overdue.append(m)
                elif (date - self.report_date).days <= 90:
                    upcoming.append(m)

        return {
            'completed': completed,
            'total': total,
            'completion_rate': round(completed / total * 100, 1) if total > 0 else 0,
            'upcoming': upcoming,
            'overdue': overdue
        }

    def analyze_team_workload(self):
        """Ekip iş yükü analizi."""
        if not self.project_data:
            return []

        team = self.project_data.get('team', [])
        results = []
        for member in team:
            responsibilities = member.get('responsibilities', [])
            results.append({
                'name': member.get('name', ''),
                'role': member.get('role', ''),
                'task_count': len(responsibilities),
                'responsibilities': responsibilities
            })

        return sorted(results, key=lambda x: x['task_count'], reverse=True)

    def calculate_risk_score(self):
        """Risk skoru hesaplama."""
        schedule = self.calculate_schedule_variance()
        budget = self.calculate_budget_status()

        score = 100
        # Takvim sapması risk etkisi
        variance = abs(schedule.get('variance', 0))
        if variance > 20:
            score -= 30
        elif variance > 10:
            score -= 15
        elif variance > 5:
            score -= 5

        # Bütçe sapması risk etkisi
        bv = abs(budget.get('budget_variance', 0))
        if bv > 20:
            score -= 25
        elif bv > 10:
            score -= 10
        elif bv > 5:
            score -= 5

        # Geciken milestone etkisi
        ms = self.analyze_milestones()
        score -= len(ms.get('overdue', [])) * 10

        return max(0, min(100, score))

    # ─────────────────────────────────────────
    # Rapor Oluşturma
    # ─────────────────────────────────────────

    def generate_full_report(self):
        """Tam ilerleme raporu oluştur."""
        self.report_sections = []

        self._add_header()
        self._add_executive_summary()
        self._add_schedule_section()
        self._add_budget_section()
        self._add_work_packages_section()
        self._add_milestones_section()
        self._add_team_section()
        self._add_risk_section()
        self._add_recommendations()

        return '\n'.join(self.report_sections)

    def _add_header(self):
        """Rapor başlığı."""
        name = self.project_data.get('projectName', 'Proje')
        number = self.project_data.get('projectNumber', '')
        company = self.project_data.get('company', '')
        date_str = self.report_date.strftime('%d.%m.%Y')

        self.report_sections.append(f"""# Proje İlerleme Raporu

**Proje:** {name}  
**Proje No:** {number}  
**Firma:** {company}  
**Rapor Tarihi:** {date_str}  
**Rapor Dönemi:** {self.report_date.strftime('%B %Y')}  

---
""")

    def _add_executive_summary(self):
        """Yönetici özeti."""
        schedule = self.calculate_schedule_variance()
        budget = self.calculate_budget_status()
        risk_score = self.calculate_risk_score()
        ms = self.analyze_milestones()

        status_map = {'on-track': '🟢 Yolunda', 'at-risk': '🟡 Risk Altında', 'delayed': '🔴 Gecikmeli'}
        schedule_status = status_map.get(schedule.get('status', ''), '❓')

        self.report_sections.append(f"""## 1. Yönetici Özeti

| Gösterge | Değer | Durum |
|----------|-------|-------|
| Genel İlerleme | {self._progress_bar(schedule.get('actual_progress', 0))} | {schedule_status} |
| Takvim Sapması | %{schedule.get('variance', 0):+.1f} | {'✅' if abs(schedule.get('variance', 0)) <= 5 else '⚠️'} |
| Bütçe Kullanımı | {budget.get('spent_percentage', 0):.1f}% ({budget.get('spent', 0):,.0f} TL) | {'✅' if budget.get('status') == 'on-track' else '⚠️'} |
| Kilometre Taşları | {ms['completed']}/{ms['total']} tamamlandı | {'✅' if not ms['overdue'] else '⚠️'} |
| Risk Skoru | {risk_score}/100 | {'✅' if risk_score >= 80 else ('⚠️' if risk_score >= 60 else '🔴')} |
| Kalan Süre | {schedule.get('remaining_days', 0)} gün | — |
""")

    def _add_schedule_section(self):
        """Takvim bölümü."""
        s = self.calculate_schedule_variance()

        self.report_sections.append(f"""## 2. Takvim Durumu

| Metrik | Değer |
|--------|-------|
| Toplam süre | {s.get('total_days', 0)} gün |
| Geçen süre | {s.get('elapsed_days', 0)} gün |
| Kalan süre | {s.get('remaining_days', 0)} gün |
| Planlanan ilerleme | %{s.get('planned_progress', 0)} |
| Gerçekleşen ilerleme | %{s.get('actual_progress', 0)} |
| Sapma | %{s.get('variance', 0):+.1f} |
""")

    def _add_budget_section(self):
        """Bütçe bölümü."""
        b = self.calculate_budget_status()

        self.report_sections.append(f"""## 3. Bütçe Durumu

| Metrik | Değer |
|--------|-------|
| Toplam bütçe | {b.get('total', 0):,.0f} TL |
| Harcanan | {b.get('spent', 0):,.0f} TL (%{b.get('spent_percentage', 0):.1f}) |
| Kalan | {b.get('remaining', 0):,.0f} TL |
| Bütçe sapması | %{b.get('budget_variance', 0):+.1f} |
""")

    def _add_work_packages_section(self):
        """İş paketleri bölümü."""
        wps = self.analyze_work_packages()

        lines = ["## 4. İş Paketleri Durumu\n"]
        lines.append("| İP | Ad | İlerleme | Planlanan | Sapma | Durum |")
        lines.append("|----|----|----------|-----------|-------|-------|")

        for wp in wps:
            emoji = self._get_status_emoji(wp['status'])
            delay_mark = ' 🔴' if wp['is_delayed'] else ''
            lines.append(
                f"| {wp['id']} | {wp['name']} | %{wp['progress']} | "
                f"%{wp['planned_progress']} | %{wp['variance']:+.1f}{delay_mark} | {emoji} |"
            )

        lines.append("")
        self.report_sections.append('\n'.join(lines))

    def _add_milestones_section(self):
        """Kilometre taşları bölümü."""
        ms = self.analyze_milestones()

        lines = [f"## 5. Kilometre Taşları\n"]
        lines.append(f"**Tamamlanan:** {ms['completed']}/{ms['total']} (%{ms['completion_rate']})\n")

        if ms['overdue']:
            lines.append("### ⚠️ Geciken Kilometre Taşları\n")
            for m in ms['overdue']:
                lines.append(f"- **{m.get('title', '')}** — Hedef: {m.get('date', '')}")
            lines.append("")

        if ms['upcoming']:
            lines.append("### 📅 Yaklaşan Kilometre Taşları (90 gün içi)\n")
            for m in ms['upcoming']:
                date = self._parse_date(m.get('date', ''))
                days_left = (date - self.report_date).days if date else '?'
                lines.append(f"- **{m.get('title', '')}** — {m.get('date', '')} ({days_left} gün kaldı)")
            lines.append("")

        self.report_sections.append('\n'.join(lines))

    def _add_team_section(self):
        """Ekip bölümü."""
        team = self.analyze_team_workload()

        lines = ["## 6. Ekip Durumu\n"]
        lines.append("| İsim | Rol | Görev Sayısı |")
        lines.append("|------|-----|-------------|")

        for m in team:
            lines.append(f"| {m['name']} | {m['role']} | {m['task_count']} |")

        lines.append("")
        self.report_sections.append('\n'.join(lines))

    def _add_risk_section(self):
        """Risk bölümü."""
        score = self.calculate_risk_score()

        if score >= 80:
            level = "DÜŞÜK"
            color = "🟢"
        elif score >= 60:
            level = "ORTA"
            color = "🟡"
        else:
            level = "YÜKSEK"
            color = "🔴"

        self.report_sections.append(f"""## 7. Risk Değerlendirmesi

**Genel Risk Seviyesi:** {color} {level} (Skor: {score}/100)

### Aktif Riskler

| Risk | Olasılık | Etki | Azaltma |
|------|----------|------|---------|
| Veri kalitesi yetersizliği | Orta | Yüksek | Çok aşamalı temizleme, doğrulama |
| Model doğruluğu hedef altı | Orta | Orta | Çoklu algoritma, hiperparametre opt. |
| Geri kazanım verimi düşük | Düşük | Orta | Proses optimizasyonu |
""")

    def _add_recommendations(self):
        """Öneriler bölümü."""
        schedule = self.calculate_schedule_variance()
        ms = self.analyze_milestones()

        recommendations = []

        if schedule.get('variance', 0) < -5:
            recommendations.append("Takvim gecikmesi tespit edildi; kaynak artırımı veya kapsam daralması değerlendirilmeli")

        if ms.get('overdue'):
            recommendations.append(f"{len(ms['overdue'])} geciken kilometre taşı var; acil eylem planı oluşturulmalı")

        wps = self.analyze_work_packages()
        delayed_wps = [wp for wp in wps if wp['is_delayed']]
        if delayed_wps:
            ids = ', '.join(wp['id'] for wp in delayed_wps)
            recommendations.append(f"Geciken iş paketleri ({ids}) için hızlandırma planı uygulanmalı")

        if not recommendations:
            recommendations.append("Proje planına uygun ilerliyor; mevcut hızı koruyun")

        lines = ["## 8. Öneriler\n"]
        for i, r in enumerate(recommendations, 1):
            lines.append(f"{i}. {r}")
        lines.append("")

        lines.append("---\n*Bu rapor otomatik olarak oluşturulmuştur.*")
        self.report_sections.append('\n'.join(lines))

    # ─────────────────────────────────────────
    # Dışa Aktarma
    # ─────────────────────────────────────────

    def export_report(self, filepath, format='md'):
        """Raporu dosyaya kaydet."""
        report = self.generate_full_report()

        path = Path(filepath)
        path.parent.mkdir(parents=True, exist_ok=True)

        with open(path, 'w', encoding='utf-8') as f:
            f.write(report)

        return str(path)

    def generate_period_comparison(self, previous_data):
        """İki dönem arası karşılaştırma raporu."""
        if not self.project_data or not previous_data:
            return "Karşılaştırma için yeterli veri yok."

        current_progress = self.project_data.get('overallProgress', 0)
        previous_progress = previous_data.get('overallProgress', 0)
        delta = current_progress - previous_progress

        current_budget = self.project_data.get('spentBudget', 0)
        previous_budget = previous_data.get('spentBudget', 0)
        budget_delta = current_budget - previous_budget

        report = f"""## Dönem Karşılaştırması

| Metrik | Önceki Dönem | Mevcut Dönem | Değişim |
|--------|-------------|-------------|---------|
| İlerleme | %{previous_progress} | %{current_progress} | %{delta:+.1f} |
| Harcama | {previous_budget:,.0f} TL | {current_budget:,.0f} TL | {budget_delta:+,.0f} TL |
"""
        return report


class PeriodicReportGenerator:
    """Dönemlik TÜBİTAK raporları oluşturma."""

    PERIODS = [
        {'name': 'I. Dönem 2026', 'start': '2026-03-01', 'end': '2026-06-30',
         'work_packages': ['İP1'], 'planned_budget': 1_190_000},
        {'name': 'II. Dönem 2026', 'start': '2026-07-01', 'end': '2026-09-30',
         'work_packages': ['İP2'], 'planned_budget': 1_240_000},
        {'name': 'I. Dönem 2027', 'start': '2027-01-01', 'end': '2027-06-30',
         'work_packages': ['İP3', 'İP4', 'İP5'], 'planned_budget': 550_000},
        {'name': 'II. Dönem 2027', 'start': '2027-07-01', 'end': '2027-09-01',
         'work_packages': ['İP6', 'İP7'], 'planned_budget': 517_730},
    ]

    def __init__(self, project_data):
        self.project_data = project_data
        self.report = ProjectProgressReport()
        self.report.load_from_dict(project_data)

    def get_current_period(self):
        """Mevcut dönemi tespit et."""
        now = datetime.now()
        for period in self.PERIODS:
            start = datetime.strptime(period['start'], '%Y-%m-%d')
            end = datetime.strptime(period['end'], '%Y-%m-%d')
            if start <= now <= end:
                return period
        return None

    def generate_period_report(self, period_name=None):
        """Dönem raporu oluştur."""
        if period_name:
            period = next((p for p in self.PERIODS if p['name'] == period_name), None)
        else:
            period = self.get_current_period()

        if not period:
            return "Dönem bulunamadı."

        full_report = self.report.generate_full_report()

        header = f"""# TÜBİTAK Dönem Raporu — {period['name']}

**Dönem:** {period['start']} — {period['end']}  
**İlgili İş Paketleri:** {', '.join(period['work_packages'])}  
**Planlanan Bütçe:** {period['planned_budget']:,.0f} TL  

---

"""
        return header + full_report


# ─────────────────────────────────────────────
# Komut satırı kullanımı
# ─────────────────────────────────────────────

if __name__ == '__main__':
    import sys

    # Varsayılan data.json yolunu bul
    project_root = Path(__file__).parent.parent.parent
    data_path = project_root / 'data.json'

    if not data_path.exists():
        print(f"HATA: {data_path} bulunamadı.")
        sys.exit(1)

    # Rapor oluştur
    report = ProjectProgressReport(str(data_path))
    output = report.generate_full_report()

    # Raporu kaydet
    reports_dir = project_root / 'reports'
    reports_dir.mkdir(exist_ok=True)
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    report_path = reports_dir / f'ilerleme_raporu_{timestamp}.md'

    report.export_report(str(report_path))
    print(f"Rapor oluşturuldu: {report_path}")
    print()
    print(output)
