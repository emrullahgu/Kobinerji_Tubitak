// ============================================================
// TÜBİTAK 7260634 — PROJE YÖNETİM PANELİ
// Dashboard JavaScript — v3.0
// ============================================================

document.addEventListener('DOMContentLoaded', () => {
    dismissLoadingScreen();
    initNavigation();
    initScrollProgress();
    initBackToTop();
    renderWorkPackages();
    renderMilestones();
    renderTeam();
    updateDynamicDates();
    animateProgressBars();
    initScrollAnimations();
    initCounterAnimations();
    updateDate();
    initEditMode();
});

// ============ PROJECT DATA ============
const project = {
    number: '7260634',
    name: 'Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi',
    company: 'KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.',
    start: '2026-03-01',
    end: '2027-09-01',
    duration: 18,
    totalBudget: 3497730,
    spentBudget: 1220000,

    workPackages: [
        {
            id: 'İP-1', name: 'Batarya Veri Toplama Altyapısı',
            start: '2026-03-01', end: '2026-06-01', progress: 65,
            status: 'active', color: '#10b981',
            desc: 'Farklı tipte bataryalardan veri toplanmasına yönelik ölçüm altyapısının kurulması',
            activities: [
                'Batarya veri toplama altyapısının kurulması',
                'Ölçüm donanımlarının ve test düzeneklerinin hazırlanması',
                'Bataryaların kontrollü koşullarda test edilmesi',
                'Gerilim, akım, sıcaklık ve kapasite verilerinin toplanması',
                'İlk veri setinin oluşturulması ve kayıt altına alınması'
            ]
        },
        {
            id: 'İP-2', name: 'Veri Ön İşleme',
            start: '2026-06-01', end: '2026-09-01', progress: 0,
            status: 'upcoming', color: '#3b82f6',
            desc: 'Toplanan batarya verilerinin temizlenmesi ve yapay zeka modeline uygun hale getirilmesi',
            activities: [
                'Verilerin temizlenmesi ve düzenlenmesi',
                'Eksik ve hatalı verilerin giderilmesi',
                'Verilerin normalizasyonu ve standartlaştırılması',
                'Özellik çıkarımı (feature extraction) işlemleri',
                'Model geliştirmeye uygun veri setinin oluşturulması'
            ]
        },
        {
            id: 'İP-3', name: 'Yapay Zeka SoH Modeli',
            start: '2026-09-01', end: '2027-01-01', progress: 0,
            status: 'pending', color: '#ef4444',
            desc: 'Batarya sağlık durumunu (SoH) tahmin eden yapay zeka modelinin tasarım ve geliştirilmesi',
            target: 'Hedef: ≥%90 Doğruluk',
            activities: [
                'Yapay zeka modelinin tasarlanması',
                'Farklı algoritmaların (regresyon, ML) test edilmesi',
                'Modelin eğitilmesi ve doğrulanması',
                'Model performansının analiz edilmesi',
                'Hiperparametre optimizasyonunun yapılması'
            ]
        },
        {
            id: 'İP-4', name: 'Karar Destek Sistemi',
            start: '2027-01-01', end: '2027-03-01', progress: 0,
            status: 'pending', color: '#f59e0b',
            desc: 'Bataryaların yeniden kullanım/geri dönüşüm olarak sınıflandırma algoritması',
            target: 'Hedef: ≥%85 Doğruluk',
            activities: [
                'SoH model çıktılarının analiz edilmesi',
                'Bataryaların yeniden kullanım / geri dönüşüm sınıflandırması',
                'Karar algoritmasının geliştirilmesi',
                'Karar destek sisteminin oluşturulması',
                'Sistem entegrasyonunun sağlanması'
            ]
        },
        {
            id: 'İP-5', name: 'Mekanik Geri Kazanım',
            start: '2027-03-01', end: '2027-05-01', progress: 0,
            status: 'pending', color: '#8b5cf6',
            desc: 'Mekanik parçalama ve fiziksel ayrıştırma sistemi kurulumu',
            target: 'Hedef: +%10 Verim Artışı',
            activities: [
                'Bataryaların güvenli şekilde işleme alınması',
                'Mekanik parçalama (shredding) sürecinin kurulması',
                'Fiziksel ayrıştırma sistemlerinin oluşturulması',
                'Plastik, bakır ve alüminyum bileşenlerin ayrılması',
                'Kara kütle (black mass) elde edilmesi'
            ]
        },
        {
            id: 'İP-6', name: 'Sistem Entegrasyonu',
            start: '2027-05-01', end: '2027-07-01', progress: 0,
            status: 'pending', color: '#14b8a6',
            desc: 'Geliştirilen tüm sistemlerin entegre şekilde çalıştırılması',
            activities: [
                'Tüm sistemlerin entegre çalıştırılması',
                'Farklı batarya tipleri ile testler',
                'Model doğruluğu ve performans analizi',
                'Sistem hatalarının tespit edilmesi ve iyileştirilmesi',
                'Performans değerlendirme raporları'
            ]
        },
        {
            id: 'İP-7', name: 'Nihai Optimizasyon',
            start: '2027-07-01', end: '2027-09-01', progress: 0,
            status: 'pending', color: '#0f172a',
            desc: 'Proje çıktılarının raporlanması, dokümantasyon ve ticarileştirme hazırlıkları',
            activities: [
                'Nihai sistemin optimize edilmesi',
                'Proje çıktılarının raporlanması',
                'Teknik dokümantasyonun hazırlanması',
                'Ürün/sistem çıktılarının sunuma hazır hale getirilmesi',
                'Ticarileştirme ve iş modeli çalışmaları'
            ]
        }
    ],

    milestones: [
        { date: '01.06.2026', title: 'Batarya Veri Toplama Altyapısı', desc: 'Ölçüm donanımlarının kurulması ve ilk veri setinin oluşturulması', status: 'inprogress', criteria: ['Test düzeneği hazır', 'En az 50 batarya hücresi test edildi', 'Veri kalitesi doğrulandı'] },
        { date: '01.09.2026', title: 'İşlenmiş Veri Seti', desc: 'Temizleme, normalizasyon ve özellik çıkarımı tamamlanması', status: 'pending', criteria: ['Veri temizliği tamamlandı', '100+ özellik çıkartıldı', 'Eğitim-test seti ayrımı yapıldı'] },
        { date: '01.01.2027', title: 'SoH Tahmin Modeli (%90)', desc: 'AI SoH modelinin geliştirilmesi ve doğrulanması', status: 'pending', criteria: ['Model doğruluğu: ≥%90', 'Analiz süresi: ≤5 dakika', 'Validasyon R²: ≥0.85'] },
        { date: '01.03.2027', title: 'Karar Destek Sistemi', desc: 'Batarya sınıflandırma ve karar algoritması', status: 'pending', criteria: ['Sınıflandırma doğruluğu: ≥%85', '3 batarya tipine uyumlu', 'Karar hızı: Real-time'] },
        { date: '01.05.2027', title: 'Mekanik Geri Kazanım', desc: 'Parçalama sistemi ve Black Mass üretimi', status: 'pending', criteria: ['Parçalama sistemi operasyonel', 'Geri kazanım verimi: +%10', '5 ton black mass üretimi'] },
        { date: '01.07.2027', title: 'Sistem Entegrasyonu', desc: 'Tüm bileşenlerin entegre çalışması', status: 'pending', criteria: ['End-to-end workflow test edildi', 'Hata oranı: <5%', '10x batarya türü test edildi'] },
        { date: '01.09.2027', title: 'Nihai Sistem ve Dokümantasyon', desc: 'Proje tamamlanması ve ticarileştirme', status: 'pending', criteria: ['Teknik dokümantasyon tamamlandı', 'Kullanıcı el kitabı hazır', 'Ticarileştirme planı oluşturuldu'] }
    ],

    team: [
        { name: 'Cem Bülbül', initials: 'CB', role: 'Kurucu - Proje Yürütücüsü', edu: 'Lisans', tasks: ['Proje koordinasyonu', 'Genel yönetim', 'Paydaş yönetimi'] },
        { name: 'Alperen Helva', initials: 'AH', role: 'Elektrik-Elektronik Mühendisi', edu: 'Yüksek Lisans', tasks: ['Veri işleme', 'SoH model geliştirme', 'Model optimizasyonu'] },
        { name: 'Emrullah Günay', initials: 'EG', role: 'Elektrik-Elektronik Mühendisi', edu: 'Lisans', tasks: ['Veri analizi', 'Sistem geliştirme', 'Sistem testleri'] },
        { name: 'Görkem Tanır', initials: 'GT', role: 'Elektrik-Elektronik Mühendisi', edu: 'Lisans', tasks: ['Test altyapısı kurulum', 'Veri toplama', 'Batarya testleri'] },
        { name: 'İbrahim Çağdaş', initials: 'İÇ', role: 'Makina Mühendisi', edu: 'Lisans', tasks: ['Mekanik sistem kurulum', 'Geri kazanım sistemi', 'Test desteği'] }
    ]
};

// ============ LOADING SCREEN ============
function dismissLoadingScreen() {
    const screen = document.getElementById('loadingScreen');
    if (!screen) return;
    screen.classList.add('hidden');
    setTimeout(() => screen.remove(), 600);
}

// ============ SCROLL PROGRESS ============
function initScrollProgress() {
    const bar = document.getElementById('scrollProgress');
    if (!bar) return;
    window.addEventListener('scroll', () => {
        const scrollTop = document.documentElement.scrollTop || document.body.scrollTop;
        const scrollHeight = document.documentElement.scrollHeight - document.documentElement.clientHeight;
        const progress = scrollHeight > 0 ? (scrollTop / scrollHeight) * 100 : 0;
        bar.style.width = progress + '%';
    }, { passive: true });
}

// ============ BACK TO TOP ============
function initBackToTop() {
    const btn = document.getElementById('backToTop');
    if (!btn) return;
    window.addEventListener('scroll', () => {
        if (window.scrollY > 400) {
            btn.classList.add('visible');
        } else {
            btn.classList.remove('visible');
        }
    }, { passive: true });
    btn.addEventListener('click', () => {
        window.scrollTo({ top: 0, behavior: 'smooth' });
    });
}

// ============ DYNAMIC DATE CALCULATIONS ============
function updateDynamicDates() {
    const projectStart = new Date('2026-03-01');
    const projectEnd = new Date('2027-09-01');
    const ip1End = new Date('2026-06-01');
    const now = new Date();

    // Days elapsed since project start
    const daysElapsed = Math.max(0, Math.ceil((now - projectStart) / (1000 * 60 * 60 * 24)));
    const totalProjectDays = Math.ceil((projectEnd - projectStart) / (1000 * 60 * 60 * 24));
    const timePercent = Math.min(100, ((daysElapsed / totalProjectDays) * 100)).toFixed(1);

    const elapsedEl = document.getElementById('daysElapsed');
    if (elapsedEl) elapsedEl.textContent = daysElapsed + ' gün geçti';

    const timeBar = document.getElementById('timeBarFill');
    if (timeBar) timeBar.style.width = timePercent + '%';

    // Days remaining for İP-1
    const daysRemaining = Math.max(0, Math.ceil((ip1End - now) / (1000 * 60 * 60 * 24)));
    const remainEl = document.getElementById('daysRemaining');
    if (remainEl) remainEl.textContent = daysRemaining;
}

// ============ SCROLL ANIMATIONS (IntersectionObserver) ============
function initScrollAnimations() {
    // Add animate-in class to key elements
    const selectors = '.card, .kpi-card, .stat-card, .wp-card, .ms-card, .tm-card, .target-item, .output-item, .bms-spec-card, .completed-category, .budget-kpi, .tech-item, .standard-item, .step-item';
    document.querySelectorAll(selectors).forEach(el => {
        el.classList.add('animate-in');
    });

    if (!('IntersectionObserver' in window)) {
        document.querySelectorAll('.animate-in').forEach(el => el.classList.add('visible'));
        return;
    }

    const observer = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('visible');
                observer.unobserve(entry.target);
            }
        });
    }, { threshold: 0.08, rootMargin: '0px 0px -40px 0px' });

    document.querySelectorAll('.animate-in').forEach(el => observer.observe(el));
}

// ============ ANIMATED COUNTERS ============
function initCounterAnimations() {
    const statElements = document.querySelectorAll('.stat-big');
    if (!('IntersectionObserver' in window)) return;

    const counterObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                animateCounter(entry.target);
                counterObserver.unobserve(entry.target);
            }
        });
    }, { threshold: 0.5 });

    statElements.forEach(el => counterObserver.observe(el));
}

function animateCounter(el) {
    const text = el.textContent.trim();
    const match = text.match(/^(\d+)/);
    if (!match) return;
    const target = parseInt(match[1], 10);
    const suffix = text.replace(/^\d+/, '');
    const duration = 1200;
    const start = performance.now();

    function step(now) {
        const elapsed = now - start;
        const progress = Math.min(elapsed / duration, 1);
        const eased = 1 - Math.pow(1 - progress, 3); // easeOutCubic
        const current = Math.round(eased * target);
        el.textContent = current + suffix;
        if (progress < 1) requestAnimationFrame(step);
    }
    requestAnimationFrame(step);
}

// ============ NAVIGATION ============
function initNavigation() {
    const btns = document.querySelectorAll('.nav-btn[data-section]');
    const sections = document.querySelectorAll('.section');
    btns.forEach(btn => {
        btn.addEventListener('click', () => {
            const target = btn.dataset.section;
            if (!target) return;
            btns.forEach(b => b.classList.remove('active'));
            sections.forEach(s => s.classList.remove('active'));
            btn.classList.add('active');
            const targetSection = document.getElementById(target);
            if (targetSection) targetSection.classList.add('active');
            window.scrollTo({ top: 0, behavior: 'smooth' });
        });
    });
}

// ============ RENDER WORK PACKAGES ============
function renderWorkPackages() {
    const grid = document.getElementById('wpGrid');
    if (!grid) return;
    grid.innerHTML = project.workPackages.map(wp => {
        const statusLabel = wp.status === 'active' ? 'DEVAM EDİYOR' : wp.status === 'upcoming' ? 'SIRADA' : 'BEKLEMEDE';
        const startF = formatDateShort(wp.start);
        const endF = formatDateShort(wp.end);
        const days = daysBetween(wp.start, wp.end);
        return `
        <div class="wp-card">
            <div class="wp-header" style="background: linear-gradient(135deg, ${wp.color}, ${adjustColor(wp.color, -30)});">
                <span class="wp-number">${wp.id}</span>
                <span class="wp-status-badge">${statusLabel}</span>
            </div>
            <div class="wp-body">
                <h4>${wp.name}</h4>
                <div class="wp-meta">
                    <span>📅 ${startF} - ${endF}</span>
                    <span>⏱️ ${days} gün</span>
                </div>
                <p class="wp-desc">${wp.desc}</p>
                <div class="wp-progress">
                    <div class="wp-progress-bar"><div class="wp-progress-fill" style="width:${wp.progress}%;background:${wp.color};"></div></div>
                    <span class="wp-progress-text">${wp.progress}% tamamlandı</span>
                </div>
                <div class="wp-activities">
                    <strong>${wp.target || 'Faaliyetler'}</strong>
                    <ul>${wp.activities.map(a => `<li>${a}</li>`).join('')}</ul>
                </div>
            </div>
        </div>`;
    }).join('');
}

// ============ RENDER MILESTONES ============
function renderMilestones() {
    const grid = document.getElementById('milestonesGrid');
    if (!grid) return;
    grid.innerHTML = project.milestones.map((ms, i) => {
        const headerClass = ms.status === 'completed' ? 'ms-completed' : ms.status === 'inprogress' ? 'ms-inprogress' : 'ms-pending';
        const badgeText = ms.status === 'completed' ? '✓ TAMAMLANDI' : ms.status === 'inprogress' ? '⧗ DEVAM EDİYOR' : '○ BEKLEMEDE';
        const icon = ms.status === 'completed' ? '✓' : ms.status === 'inprogress' ? '→' : '○';
        return `
        <div class="ms-card">
            <div class="ms-header ${headerClass}">
                <span class="ms-date">${ms.date}</span>
                <span class="ms-badge">${badgeText}</span>
            </div>
            <div class="ms-body">
                <h4>${i + 1}. ${ms.title}</h4>
                <p>${ms.desc}</p>
                <div class="ms-criteria">
                    <strong>Başarı Kriterleri</strong>
                    <ul>${ms.criteria.map(c => `<li>${icon} ${c}</li>`).join('')}</ul>
                </div>
            </div>
        </div>`;
    }).join('');
}

// ============ RENDER TEAM ============
function renderTeam() {
    const grid = document.getElementById('teamGrid');
    if (!grid) return;
    grid.innerHTML = project.team.map(m => `
        <div class="tm-card">
            <div class="tm-avatar">${m.initials}</div>
            <div class="tm-name">${m.name}</div>
            <div class="tm-role">${m.role}</div>
            <div class="tm-edu">${m.edu}</div>
            <div class="tm-tasks">
                <strong>Sorumluluklar</strong>
                <ul>${m.tasks.map(t => `<li>${t}</li>`).join('')}</ul>
            </div>
        </div>
    `).join('');
}

// ============ ANIMATIONS ============
function animateProgressBars() {
    const fills = document.querySelectorAll('.kpi-bar-fill, .wp-progress-fill, .bb-bar, .gantt-fill');
    fills.forEach(el => {
        const w = el.style.width;
        el.style.width = '0%';
        setTimeout(() => {
            el.style.transition = 'width 1s cubic-bezier(0.4, 0, 0.2, 1)';
            el.style.width = w;
        }, 200);
    });
}

// ============ DATE ============
function updateDate() {
    const now = new Date();
    const opts = { day: 'numeric', month: 'long', year: 'numeric' };
    const dateStr = now.toLocaleDateString('tr-TR', opts);
    const el1 = document.getElementById('headerDate');
    const el2 = document.getElementById('footerDate');
    if (el1) el1.textContent = dateStr;
    if (el2) el2.textContent = dateStr;
}

// ============ UTILITIES ============
function formatDateShort(iso) {
    const d = new Date(iso);
    return d.toLocaleDateString('tr-TR', { day: '2-digit', month: '2-digit', year: 'numeric' });
}

function daysBetween(d1, d2) {
    return Math.ceil(Math.abs(new Date(d2) - new Date(d1)) / (1000 * 60 * 60 * 24));
}

function adjustColor(hex, amount) {
    hex = hex.replace('#', '');
    let r = Math.max(0, Math.min(255, parseInt(hex.substring(0, 2), 16) + amount));
    let g = Math.max(0, Math.min(255, parseInt(hex.substring(2, 4), 16) + amount));
    let b = Math.max(0, Math.min(255, parseInt(hex.substring(4, 6), 16) + amount));
    return `#${r.toString(16).padStart(2,'0')}${g.toString(16).padStart(2,'0')}${b.toString(16).padStart(2,'0')}`;
}

// ============ EXPORT ============
function exportJSON() {
    const blob = new Blob([JSON.stringify(project, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `TUBITAK_7260634_${new Date().toISOString().split('T')[0]}.json`;
    a.click();
    URL.revokeObjectURL(url);
}

// ============ TOAST NOTIFICATIONS ============
function showToast(message, type = 'success') {
    const existing = document.querySelectorAll('.toast');
    existing.forEach(t => t.remove());
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    toast.innerHTML = message;
    document.body.appendChild(toast);
    setTimeout(() => toast.remove(), 3000);
}

// ============ EDIT MODE ============
let isEditMode = false;
let editSnapshot = '';

function initEditMode() {
    const btnEdit = document.getElementById('btnEdit');
    const btnSave = document.getElementById('btnSave');
    const btnCancel = document.getElementById('btnCancel');
    const btnPDF = document.getElementById('btnPDF');
    const btnPrint = document.getElementById('btnPrint');
    const btnJSON = document.getElementById('btnJSON');

    if (btnEdit) btnEdit.addEventListener('click', enterEditMode);
    if (btnSave) btnSave.addEventListener('click', saveEdits);
    if (btnCancel) btnCancel.addEventListener('click', cancelEdits);
    if (btnPDF) btnPDF.addEventListener('click', exportPDF);
    if (btnPrint) btnPrint.addEventListener('click', () => window.print());
    if (btnJSON) btnJSON.addEventListener('click', exportJSON);

    // Make specific elements editable
    makeEditable();
}

function makeEditable() {
    // Mark text elements as editable targets
    const selectors = [
        '.section-title h2',
        '.section-title p',
        '.card h3',
        '.kpi-value',
        '.kpi-sub',
        '.info-value',
        '.highlight-left h3',
        '.highlight-left p',
        '.check-list li',
        '.risk-detail strong',
        '.risk-detail p',
        '.risk-mitigation',
        '.target-value',
        '.target-label',
        '.step-content h4',
        '.step-content li',
        '.output-item h4',
        '.output-item p',
        '.budget-note',
        '.bms-spec-card h4',
        '.bms-spec-card li'
    ];
    document.querySelectorAll(selectors.join(',')).forEach(el => {
        el.classList.add('editable');
    });
}

function enterEditMode() {
    isEditMode = true;
    document.querySelector('.app').classList.add('edit-mode');
    editSnapshot = document.querySelector('.main').innerHTML;

    // Enable contentEditable on all .editable elements
    document.querySelectorAll('.editable').forEach(el => {
        el.setAttribute('contenteditable', 'true');
        el.setAttribute('spellcheck', 'true');
    });

    document.getElementById('btnEdit').style.display = 'none';
    document.getElementById('btnSave').style.display = 'inline-flex';
    document.getElementById('btnCancel').style.display = 'inline-flex';
    document.getElementById('tbStatus').innerHTML = '✏️ <strong style="color:#3b82f6">Düzenleme Modu</strong> — İçeriklere tıklayarak düzenleyebilirsiniz';

    showToast('✏️ Düzenleme modu açıldı — herhangi bir mavi çerçeveli alana tıklayıp yazabilirsiniz', 'info');
}

function saveEdits() {
    // Remove contentEditable
    document.querySelectorAll('.editable').forEach(el => {
        el.removeAttribute('contenteditable');
        el.removeAttribute('spellcheck');
    });

    document.querySelector('.app').classList.remove('edit-mode');
    isEditMode = false;

    document.getElementById('btnEdit').style.display = 'inline-flex';
    document.getElementById('btnSave').style.display = 'none';
    document.getElementById('btnCancel').style.display = 'none';
    document.getElementById('tbStatus').innerHTML = '📋 Rapor Görüntüleme Modu';

    // Save to localStorage
    const content = document.querySelector('.main').innerHTML;
    localStorage.setItem('tubitak_7260634_edited', content);
    localStorage.setItem('tubitak_7260634_editDate', new Date().toISOString());

    showToast('💾 Değişiklikler kaydedildi!', 'success');
}

function cancelEdits() {
    // Restore snapshot
    document.querySelector('.main').innerHTML = editSnapshot;
    document.querySelector('.app').classList.remove('edit-mode');
    isEditMode = false;

    document.getElementById('btnEdit').style.display = 'inline-flex';
    document.getElementById('btnSave').style.display = 'none';
    document.getElementById('btnCancel').style.display = 'none';
    document.getElementById('tbStatus').innerHTML = '📋 Rapor Görüntüleme Modu';

    // Re-render dynamic sections
    renderWorkPackages();
    renderMilestones();
    renderTeam();
    animateProgressBars();
    makeEditable();

    showToast('✖️ Düzenlemeler iptal edildi', 'error');
}

// Restore saved edits on load
function restoreSavedEdits() {
    const saved = localStorage.getItem('tubitak_7260634_edited');
    if (saved) {
        const editDate = localStorage.getItem('tubitak_7260634_editDate');
        const dateStr = editDate ? new Date(editDate).toLocaleDateString('tr-TR') : '';
        document.querySelector('.main').innerHTML = saved;
        makeEditable();
        showToast(`📂 Kaydedilmiş düzenlemeler yüklendi (${dateStr})`, 'info');
    }
}

// ============ PDF EXPORT ============
function exportPDF() {
    window.print();
}

// ============ KEYBOARD SHORTCUTS ============
document.addEventListener('keydown', (e) => {
    if (e.altKey && e.key >= '1' && e.key <= '8') {
        const secs = ['overview','progress','roadmap','workpackages','milestones','budget','team','technical'];
        const idx = parseInt(e.key) - 1;
        if (secs[idx]) document.querySelector(`[data-section="${secs[idx]}"]`).click();
    }
    if (e.ctrlKey && e.key === 'p') { e.preventDefault(); exportPDF(); }
    if (e.ctrlKey && e.key === 'e') { e.preventDefault(); if (!isEditMode) enterEditMode(); }
    if (e.ctrlKey && e.key === 's') {
        e.preventDefault();
        if (isEditMode) saveEdits();
    }
    if (e.key === 'Escape' && isEditMode) cancelEdits();
});

// ============ LOCAL STORAGE ============
function saveState() { localStorage.setItem('tubitak_7260634_v2', JSON.stringify({ lastVisit: new Date().toISOString() })); }
setInterval(saveState, 60000);
saveState();

console.log('%c✅ TÜBİTAK 7260634 Dashboard v3.0 yüklendi', 'color:#10b981;font-weight:700;font-size:13px;');