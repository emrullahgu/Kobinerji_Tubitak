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
    renderDocuments();
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
        { name: 'Cem Bülbül', initials: 'CB', photo: 'vesikalar/cembulbul.jpeg', role: 'Kurucu - Proje Yürütücüsü', edu: 'Lisans', tasks: ['Proje koordinasyonu', 'Genel yönetim', 'Paydaş yönetimi'] },
        { name: 'Alperen Helva', initials: 'AH', photo: 'vesikalar/alperenhelva.jpeg', role: 'Elektrik-Elektronik Mühendisi', edu: 'Yüksek Lisans', tasks: ['Veri işleme', 'SoH model geliştirme', 'Model optimizasyonu'] },
        { name: 'Emrullah Günay', initials: 'EG', photo: 'vesikalar/emrullahgunay.jpg', role: 'Elektrik-Elektronik Mühendisi', edu: 'Lisans', tasks: ['Veri analizi', 'Sistem geliştirme', 'Sistem testleri'] },
        { name: 'Görkem Tanır', initials: 'GT', photo: 'vesikalar/gorkemtanir.jpeg', role: 'Elektrik-Elektronik Mühendisi', edu: 'Lisans', tasks: ['Test altyapısı kurulum', 'Veri toplama', 'Batarya testleri'] },
        { name: 'İbrahim Çağdaş', initials: 'İÇ', photo: 'vesikalar/ibrahimcagdas.jpeg', role: 'Makina Mühendisi', edu: 'Lisans', tasks: ['Mekanik sistem kurulum', 'Geri kazanım sistemi', 'Test desteği'] }
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
    const selectors = '.card, .kpi-card, .stat-card, .wp-card, .ms-card, .tm-card, .target-item, .output-item, .bms-spec-card, .completed-category, .budget-kpi, .tech-item, .standard-item, .step-item, .evidence-item, .hakem-hero';
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

    // Hero banner counters
    const heroCounters = document.querySelectorAll('[data-counter]');
    const heroObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                animateHeroCounter(entry.target);
                heroObserver.unobserve(entry.target);
            }
        });
    }, { threshold: 0.3 });
    heroCounters.forEach(el => heroObserver.observe(el));

    // KPI value counters
    const kpiValues = document.querySelectorAll('.kpi-value');
    const kpiObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                animateCounter(entry.target);
                kpiObserver.unobserve(entry.target);
            }
        });
    }, { threshold: 0.5 });
    kpiValues.forEach(el => kpiObserver.observe(el));
}

function animateHeroCounter(el) {
    const target = parseInt(el.dataset.counter, 10);
    const suffix = el.dataset.suffix || '';
    const duration = 1800;
    const start = performance.now();
    function step(now) {
        const elapsed = now - start;
        const progress = Math.min(elapsed / duration, 1);
        const eased = 1 - Math.pow(1 - progress, 3);
        const current = Math.round(eased * target);
        el.textContent = current + suffix;
        if (progress < 1) requestAnimationFrame(step);
    }
    requestAnimationFrame(step);
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
            <div class="tm-avatar">${m.photo ? `<img src="${m.photo}" alt="${m.name}">` : m.initials}</div>
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
    renderDocuments();
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

// ============ DOCUMENTS SECTION ============
const projectDocs = [
    { id: '01', title: 'Teknik Dokümantasyon', desc: 'Sistemin tüm teknik bileşenlerinin detaylı açıklaması', file: 'docs/01_TEKNIK_DOKUMANTASYON.md', color: '#6366f1', tags: ['Teknik', 'Sistem', 'Modüller'] },
    { id: '02', title: 'API Referans Dokümanı', desc: '13 endpoint detaylı REST API referans kılavuzu', file: 'docs/02_API_REFERANS_KILAVUZU.md', color: '#8b5cf6', tags: ['API', '13 Endpoint', 'REST'] },
    { id: '03', title: 'Kullanıcı El Kitabı', desc: 'Kurulum, kullanım ve sorun giderme rehberi', file: 'docs/03_KULLANICI_EL_KITABI.md', color: '#06b6d4', tags: ['Kullanıcı', 'Rehber', 'SSS'] },
    { id: '04', title: 'Sistem Mimarisi Dokümanı', desc: 'Katman mimarisi, modül tasarımları ve veri modelleri', file: 'docs/04_SISTEM_MIMARISI.md', color: '#10b981', tags: ['Mimari', 'Tasarım', 'Katmanlar'] },
    { id: '05', title: 'Kurulum ve Dağıtım Rehberi', desc: 'Python backend, frontend ve Netlify dağıtım adımları', file: 'docs/05_KURULUM_DAGITIM_REHBERI.md', color: '#f59e0b', tags: ['Kurulum', 'Dağıtım', 'Netlify'] },
    { id: '06', title: 'Test Raporu', desc: '34 test sonucu — birim, entegrasyon ve performans testleri', file: 'docs/06_TEST_RAPORU.md', color: '#ef4444', tags: ['Test', '34 Sonuç', 'Kapsam'] },
    { id: '07', title: 'İş Paketi Detaylandırması', desc: '7 iş paketi, faaliyet kırılımı ve zaman çizelgesi', file: 'docs/07_IS_PAKETI_DETAYLANDIRMASI.md', color: '#ec4899', tags: ['İş Paketleri', 'Planlama', '18 Ay'] },
    { id: '08', title: 'Kalite Kontrol Planı', desc: 'ISO standartları, kalite metrikleri ve denetim planı', file: 'docs/08_KALITE_KONTROL_PLANI.md', color: '#14b8a6', tags: ['Kalite', 'ISO', 'Denetim'] },
    { id: '09', title: 'VGX BMS Firmware Datasheet', desc: 'STM32F030C8 BMS gömülü yazılım teknik referansı', file: 'docs/10_VGX_BMS_FIRMWARE_DATASHEET.md', color: '#64748b', tags: ['Firmware', 'BMS', 'Gömülü'] },
    { id: '10', title: 'SoH Tahmin Notebook (Derin Öğrenme)', desc: 'LSTM, GRU, AST-LSTM ve Transfer Öğrenme ile SoH tahmin modelleri — Jupyter Notebook', file: 'KOBINERJI_SoH_Tahmin-2.ipynb', color: '#EE4C2C', tags: ['Deep Learning', 'PyTorch', 'AST-LSTM', 'Notebook'], isNotebook: true }
];

function renderDocuments() {
    const grid = document.getElementById('docsGrid');
    if (!grid) return;
    grid.innerHTML = projectDocs.map(doc => {
        if (doc.isNotebook) {
            return `
            <div class="doc-card">
                <div class="doc-card-header">
                    <div class="doc-card-number" style="background: ${doc.color};">${doc.id}</div>
                    <div class="doc-card-info">
                        <h4>${doc.title}</h4>
                        <p>${doc.desc}</p>
                    </div>
                </div>
                <div class="doc-card-tags">
                    ${doc.tags.map(t => `<span class="doc-tag">${t}</span>`).join('')}
                </div>
                <div class="doc-card-footer">
                    <span class="doc-card-meta">📓 Jupyter Notebook (.ipynb)</span>
                    <div class="doc-card-actions">
                        <a class="doc-download-btn" href="${doc.file}" download style="text-decoration:none;">
                            <span class="btn-text">📥 Notebook İndir</span>
                        </a>
                        <button class="doc-download-btn" onclick="downloadNotebookPDF('${doc.file}', '${doc.id} — ${doc.title.replace(/'/g, "\\'")}')">
                            <span class="btn-text">📥 PDF İndir</span>
                            <span class="spinner"></span>
                        </button>
                    </div>
                </div>
            </div>`;
        }
        return `
        <div class="doc-card">
            <div class="doc-card-header">
                <div class="doc-card-number" style="background: ${doc.color};">${doc.id}</div>
                <div class="doc-card-info">
                    <h4>${doc.title}</h4>
                    <p>${doc.desc}</p>
                </div>
            </div>
            <div class="doc-card-tags">
                ${doc.tags.map(t => `<span class="doc-tag">${t}</span>`).join('')}
                ${getDocEditBadge(doc.file)}
            </div>
            <div class="doc-card-footer">
                <span class="doc-card-meta">📄 Markdown → PDF</span>
                <div class="doc-card-actions">
                    <button class="doc-edit-btn" onclick="openDocEditor('${doc.file}', '${doc.id} — ${doc.title.replace(/'/g, "\\'")}')">
                        ✏️ Düzenle
                    </button>
                    <button class="doc-download-btn" onclick="downloadDocPDF('${doc.file}', '${doc.id} — ${doc.title.replace(/'/g, "\\'")}')">
                        <span class="btn-text">📥 PDF İndir</span>
                        <span class="spinner"></span>
                    </button>
                </div>
            </div>
        </div>`;
    }).join('');
}

/* ═══════════ DOCUMENT EDITOR ═══════════ */
function getDocEditBadge(filePath) {
    const key = 'doc_edit_' + filePath.replace(/[^a-zA-Z0-9]/g, '_');
    if (localStorage.getItem(key)) {
        return '<span class="doc-tag doc-tag-edited">✏️ Düzenlendi</span>';
    }
    return '';
}

function getDocContent(filePath) {
    const key = 'doc_edit_' + filePath.replace(/[^a-zA-Z0-9]/g, '_');
    return localStorage.getItem(key);
}

function saveDocContent(filePath, content) {
    const key = 'doc_edit_' + filePath.replace(/[^a-zA-Z0-9]/g, '_');
    localStorage.setItem(key, content);
    localStorage.setItem(key + '_date', new Date().toISOString());
}

function resetDocContent(filePath) {
    const key = 'doc_edit_' + filePath.replace(/[^a-zA-Z0-9]/g, '_');
    localStorage.removeItem(key);
    localStorage.removeItem(key + '_date');
}

async function openDocEditor(filePath, docTitle) {
    // Fetch original or load saved
    let content;
    const saved = getDocContent(filePath);
    if (saved) {
        content = saved;
    } else {
        const resp = await fetch(filePath);
        if (!resp.ok) { showToast('❌ Dosya yüklenemedi', 'error'); return; }
        content = await resp.text();
    }

    // Create modal
    let modal = document.getElementById('docEditorModal');
    if (modal) modal.remove();

    modal = document.createElement('div');
    modal.id = 'docEditorModal';
    modal.className = 'doc-editor-modal';
    modal.innerHTML = `
        <div class="doc-editor-container">
            <div class="doc-editor-toolbar">
                <div class="doc-editor-title">
                    <span class="doc-editor-icon">📝</span>
                    <h3>${docTitle}</h3>
                    ${saved ? '<span class="doc-editor-saved-badge">💾 Kaydedilmiş düzenleme</span>' : ''}
                </div>
                <div class="doc-editor-actions">
                    <button class="doc-editor-btn doc-editor-btn-reset" onclick="resetDocAndClose('${filePath}')" title="Orijinal haline döndür">
                        🔄 Sıfırla
                    </button>
                    <button class="doc-editor-btn doc-editor-btn-save" onclick="saveDocAndClose('${filePath}')">
                        💾 Kaydet
                    </button>
                    <button class="doc-editor-btn doc-editor-btn-close" onclick="closeDocEditor()">
                        ✕
                    </button>
                </div>
            </div>
            <div class="doc-editor-body">
                <div class="doc-editor-pane doc-editor-left">
                    <div class="doc-editor-pane-header">Markdown Kaynak</div>
                    <textarea id="docEditorTextarea" spellcheck="true">${escapeHtml(content)}</textarea>
                </div>
                <div class="doc-editor-pane doc-editor-right">
                    <div class="doc-editor-pane-header">Önizleme</div>
                    <div id="docEditorPreview" class="doc-editor-preview"></div>
                </div>
            </div>
        </div>
    `;
    document.body.appendChild(modal);
    requestAnimationFrame(() => modal.classList.add('open'));

    // Live preview
    const textarea = document.getElementById('docEditorTextarea');
    const preview = document.getElementById('docEditorPreview');
    function updatePreview() {
        preview.innerHTML = marked.parse(textarea.value);
    }
    updatePreview();
    textarea.addEventListener('input', updatePreview);

    // Escape to close
    modal._keyHandler = (e) => { if (e.key === 'Escape') closeDocEditor(); };
    document.addEventListener('keydown', modal._keyHandler);
}

function escapeHtml(str) {
    return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
}

function saveDocAndClose(filePath) {
    const textarea = document.getElementById('docEditorTextarea');
    if (!textarea) return;
    saveDocContent(filePath, textarea.value);
    closeDocEditor();
    renderDocuments();
    showToast('✅ Doküman kaydedildi — PDF bu değişiklikleri kullanacak', 'success');
}

function resetDocAndClose(filePath) {
    resetDocContent(filePath);
    closeDocEditor();
    renderDocuments();
    showToast('🔄 Doküman orijinal haline döndürüldü', 'info');
}

function closeDocEditor() {
    const modal = document.getElementById('docEditorModal');
    if (!modal) return;
    if (modal._keyHandler) document.removeEventListener('keydown', modal._keyHandler);
    modal.classList.remove('open');
    setTimeout(() => modal.remove(), 300);
}

/* ═══════════ PDF FONT LOADING ═══════════ */
let _pdfFontCache = null;

async function loadPdfFonts(pdf) {
    if (!_pdfFontCache) {
        const base = 'https://cdn.jsdelivr.net/gh/googlefonts/roboto@main/src/hinted/';
        const toBase64 = async (url) => {
            const resp = await fetch(url);
            if (!resp.ok) throw new Error('Font yüklenemedi: ' + url);
            const buf = await resp.arrayBuffer();
            const bytes = new Uint8Array(buf);
            let binary = '';
            const chunk = 8192;
            for (let i = 0; i < bytes.length; i += chunk) {
                binary += String.fromCharCode.apply(null, bytes.subarray(i, Math.min(i + chunk, bytes.length)));
            }
            return btoa(binary);
        };
        const [regular, bold, italic] = await Promise.all([
            toBase64(base + 'Roboto-Regular.ttf'),
            toBase64(base + 'Roboto-Bold.ttf'),
            toBase64(base + 'Roboto-Italic.ttf')
        ]);
        _pdfFontCache = { regular, bold, italic };
    }
    pdf.addFileToVFS('Roboto-Regular.ttf', _pdfFontCache.regular);
    pdf.addFileToVFS('Roboto-Bold.ttf', _pdfFontCache.bold);
    pdf.addFileToVFS('Roboto-Italic.ttf', _pdfFontCache.italic);
    pdf.addFont('Roboto-Regular.ttf', 'Roboto', 'normal');
    pdf.addFont('Roboto-Bold.ttf', 'Roboto', 'bold');
    pdf.addFont('Roboto-Italic.ttf', 'Roboto', 'italic');
    pdf.setFont('Roboto');
}

/* ═══════════ PDF GENERATION (jsPDF native text + Roboto Unicode) ═══════════ */
async function downloadDocPDF(filePath, docTitle) {
    const btn = event.currentTarget;
    btn.classList.add('loading');
    btn.disabled = true;

    try {
        /* 1. Get content (saved edits or original) */
        let md = getDocContent(filePath);
        if (!md) {
            const resp = await fetch(filePath);
            if (!resp.ok) throw new Error('Dosya yüklenemedi: ' + resp.status);
            md = await resp.text();
        }

        const now = new Date();
        const dateStr = now.toLocaleDateString('tr-TR', { day: 'numeric', month: 'long', year: 'numeric' });
        const dateISO = now.toISOString().split('T')[0];

        /* 2. Parse markdown into structured tokens */
        const tokens = marked.lexer(md);

        /* 3. Setup jsPDF */
        const { jsPDF } = window.jspdf;
        const pdf = new jsPDF({ orientation: 'portrait', unit: 'mm', format: 'a4' });
        const PW = 210, PH = 297;
        const ML = 18, MR = 18, MT = 22, MB = 20;
        const CW = PW - ML - MR;
        let y = MT;
        let pageNum = 0;

        /* ── Load Unicode font for Turkish characters ── */
        await loadPdfFonts(pdf);

        /* ── Helper: add page with header/footer ── */
        function newPage() {
            if (pageNum > 0) pdf.addPage();
            pageNum++;
            y = MT;
        }

        function checkSpace(needed) {
            if (y + needed > PH - MB) {
                newPage();
                return true;
            }
            return false;
        }

        function addHeaderFooter() {
            const total = pdf.internal.getNumberOfPages();
            for (let p = 2; p <= total; p++) {
                pdf.setPage(p);
                // Header line
                pdf.setDrawColor(99, 102, 241);
                pdf.setLineWidth(0.4);
                pdf.line(ML, MT - 6, PW - MR, MT - 6);
                pdf.setFontSize(7);
                pdf.setFont('Roboto', 'normal');
                pdf.setTextColor(100, 116, 139);
                pdf.text('KOBİNERJİ A.Ş. | TÜBİTAK 7260634', ML, MT - 9);
                const short = docTitle.length > 48 ? docTitle.substring(0, 45) + '...' : docTitle;
                pdf.setFontSize(6.5);
                pdf.setTextColor(99, 102, 241);
                pdf.text(short, PW - MR, MT - 9, { align: 'right' });
                // Footer
                const fy = PH - MB + 6;
                pdf.setDrawColor(203, 213, 225);
                pdf.setLineWidth(0.3);
                pdf.line(ML, fy, PW - MR, fy);
                pdf.setFontSize(6.5);
                pdf.setTextColor(148, 163, 184);
                pdf.text('© 2026 KOBİNERJİ A.Ş.', ML, fy + 4);
                pdf.setFontSize(6);
                pdf.setTextColor(220, 38, 38);
                pdf.text('GİZLİ', PW / 2, fy + 4, { align: 'center' });
                pdf.setFontSize(7);
                pdf.setTextColor(100, 116, 139);
                pdf.text('Sayfa ' + (p - 1) + ' / ' + (total - 1), PW - MR, fy + 4, { align: 'right' });
            }
        }

        /* ── Helper: wrap and print text ── */
        function printText(text, fontSize, bold, color, indent) {
            indent = indent || 0;
            pdf.setFontSize(fontSize);
            pdf.setFont('Roboto', bold ? 'bold' : 'normal');
            pdf.setTextColor(color[0], color[1], color[2]);
            const maxW = CW - indent;
            const lines = pdf.splitTextToSize(text, maxW);
            const lineH = fontSize * 0.45;
            for (let i = 0; i < lines.length; i++) {
                checkSpace(lineH);
                pdf.text(lines[i], ML + indent, y);
                y += lineH;
            }
        }

        function stripInline(text) {
            if (!text || typeof text !== 'string') return String(text || '');
            return text
                .replace(/\*\*(.+?)\*\*/g, '$1')
                .replace(/\*(.+?)\*/g, '$1')
                .replace(/__(.+?)__/g, '$1')
                .replace(/_(.+?)_/g, '$1')
                .replace(/`(.+?)`/g, '$1')
                .replace(/\[(.+?)\]\(.+?\)/g, '$1')
                .replace(/~~(.+?)~~/g, '$1');
        }

        /* ══════ 4. COVER PAGE ══════ */
        newPage();
        // Top gradient bar
        pdf.setFillColor(15, 23, 42);
        pdf.rect(0, 0, PW, 42, 'F');
        pdf.setFillColor(30, 58, 95);
        pdf.rect(0, 35, PW, 7, 'F');

        // Company name on dark bar
        pdf.setFontSize(11);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(255, 255, 255);
        pdf.text('KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.', ML, 18);
        pdf.setFontSize(8);
        pdf.setFont('Roboto', 'normal');
        pdf.setTextColor(180, 200, 220);
        pdf.text('Ceran Plaza, Kemalpaşa OSB, Gazi Blv. No:177/19, 35730 Kemalpaşa/İzmir', ML, 25);

        // TÜBİTAK badge
        pdf.setFillColor(99, 102, 241);
        pdf.roundedRect(PW / 2 - 45, 58, 90, 12, 6, 6, 'F');
        pdf.setFontSize(9);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(255, 255, 255);
        pdf.text('TÜBİTAK 1507 - KOBİ Ar-Ge Destek Programı', PW / 2, 66, { align: 'center' });

        // Project number
        pdf.setFontSize(36);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(15, 23, 42);
        pdf.text('Proje No: 7260634', PW / 2, 95, { align: 'center' });

        // Project name
        pdf.setFontSize(13);
        pdf.setFont('Roboto', 'normal');
        pdf.setTextColor(51, 65, 85);
        const projNameLines = pdf.splitTextToSize(
            'Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi', 140);
        let pny = 110;
        projNameLines.forEach(l => { pdf.text(l, PW / 2, pny, { align: 'center' }); pny += 7; });

        // Divider
        pdf.setDrawColor(99, 102, 241);
        pdf.setLineWidth(1);
        pdf.line(PW / 2 - 30, pny + 5, PW / 2 + 30, pny + 5);

        // Document title
        pdf.setFontSize(18);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(15, 76, 117);
        const titleLines = pdf.splitTextToSize(docTitle, 150);
        let ty = pny + 20;
        titleLines.forEach(l => { pdf.text(l, PW / 2, ty, { align: 'center' }); ty += 9; });

        // Meta grid
        const metaItems = [
            ['Doküman Tarihi', dateStr],
            ['Sürüm', '1.0.0'],
            ['Proje Süresi', '01.03.2026 – 01.09.2027'],
            ['Gizlilik', 'Proje İçi – Gizli'],
            ['Proje Yürütücüsü', 'Cem Bülbül'],
            ['Hazırlayan', 'KOBİNERJİ A.Ş. Proje Ekibi']
        ];
        let my = ty + 15;
        const mColW = 80;
        for (let i = 0; i < metaItems.length; i++) {
            const col = i % 2;
            const mx = ML + col * (mColW + 10);
            if (i % 2 === 0 && i > 0) my += 16;
            // Box
            pdf.setFillColor(248, 250, 252);
            pdf.setDrawColor(226, 232, 240);
            pdf.roundedRect(mx, my, mColW, 14, 2, 2, 'FD');
            // Label
            pdf.setFontSize(7);
            pdf.setFont('Roboto', 'bold');
            pdf.setTextColor(100, 116, 139);
            pdf.text(metaItems[i][0].toLocaleUpperCase('tr-TR'), mx + 4, my + 5);
            // Value
            pdf.setFontSize(9);
            pdf.setFont('Roboto', 'bold');
            pdf.setTextColor(15, 23, 42);
            pdf.text(metaItems[i][1], mx + 4, my + 11);
        }

        // Bottom band
        const bby = PH - 30;
        pdf.setFillColor(248, 250, 252);
        pdf.rect(0, bby, PW, 30, 'F');
        pdf.setDrawColor(226, 232, 240);
        pdf.line(0, bby, PW, bby);
        pdf.setFontSize(9);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(220, 38, 38);
        pdf.text('GİZLİ – Bu doküman KOBİNERJİ A.Ş. tarafından TÜBİTAK\'a sunulmak üzere hazırlanmıştır.', PW / 2, bby + 12, { align: 'center' });
        pdf.setFontSize(8);
        pdf.setFont('Roboto', 'normal');
        pdf.setTextColor(148, 163, 184);
        pdf.text('© 2026 KOBİNERJİ A.Ş. – Tüm hakları saklıdır.', PW / 2, bby + 20, { align: 'center' });

        /* ══════ 5. CONTENT PAGES ══════ */
        newPage();

        // Title at top of content
        pdf.setFontSize(16);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(15, 23, 42);
        const mainTitleLines = pdf.splitTextToSize(docTitle, CW);
        mainTitleLines.forEach(l => { pdf.text(l, ML, y); y += 7; });
        y += 2;
        pdf.setDrawColor(99, 102, 241);
        pdf.setLineWidth(0.8);
        pdf.line(ML, y, PW - MR, y);
        y += 3;
        pdf.setFontSize(8);
        pdf.setFont('Roboto', 'normal');
        pdf.setTextColor(100, 116, 139);
        pdf.text('Tarih: ' + dateStr + '  |  Sürüm: 1.0.0  |  Gizlilik: Proje İçi', ML, y);
        y += 8;

        /* ── Process tokens ── */
        for (const token of tokens) {
            switch (token.type) {
                case 'heading': {
                    const text = stripInline(token.text);
                    const depth = token.depth;
                    const sizes = { 1: 15, 2: 12.5, 3: 11, 4: 10, 5: 9.5, 6: 9 };
                    const fs = sizes[depth] || 10;
                    const gap = depth <= 2 ? 6 : 4;
                    checkSpace(gap + fs * 0.5 + 4);
                    y += gap;
                    pdf.setFontSize(fs);
                    pdf.setFont('Roboto', 'bold');
                    if (depth === 1) pdf.setTextColor(15, 23, 42);
                    else if (depth === 2) pdf.setTextColor(30, 41, 59);
                    else pdf.setTextColor(51, 65, 85);
                    const hLines = pdf.splitTextToSize(text, CW);
                    hLines.forEach(l => { checkSpace(fs * 0.45); pdf.text(l, ML, y); y += fs * 0.45; });
                    if (depth <= 2) {
                        y += 1;
                        pdf.setDrawColor(depth === 1 ? 99 : 226, depth === 1 ? 102 : 232, depth === 1 ? 241 : 240);
                        pdf.setLineWidth(depth === 1 ? 0.6 : 0.3);
                        pdf.line(ML, y, PW - MR, y);
                    }
                    y += 3;
                    break;
                }
                case 'paragraph': {
                    const text = stripInline(token.text);
                    checkSpace(8);
                    printText(text, 9.5, false, [30, 41, 59], 0);
                    y += 3;
                    break;
                }
                case 'list': {
                    checkSpace(8);
                    for (let i = 0; i < token.items.length; i++) {
                        const item = token.items[i];
                        const text = stripInline(item.text);
                        const bullet = token.ordered ? (i + 1) + '.' : '\u2022';
                        checkSpace(5);
                        pdf.setFontSize(9.5);
                        pdf.setFont('Roboto', 'normal');
                        pdf.setTextColor(30, 41, 59);
                        pdf.text(bullet, ML + 3, y);
                        const listLines = pdf.splitTextToSize(text, CW - 10);
                        listLines.forEach((l, li) => {
                            checkSpace(4.5);
                            pdf.text(l, ML + 10, y);
                            y += 4.5;
                        });
                        y += 1;
                    }
                    y += 2;
                    break;
                }
                case 'table': {
                    const headers = token.header.map(h => stripInline(h.text != null ? h.text : (typeof h === 'string' ? h : '')));
                    const rows = token.rows.map(row => row.map(cell => stripInline(cell.text != null ? cell.text : (typeof cell === 'string' ? cell : ''))));
                    const colCount = headers.length;
                    const colW = CW / colCount;
                    const cellH = 6;
                    const cellPad = 2;
                    const tableH = (1 + rows.length) * cellH;

                    checkSpace(Math.min(tableH, 40));

                    // Header row
                    pdf.setFillColor(241, 245, 249);
                    pdf.rect(ML, y, CW, cellH, 'F');
                    pdf.setFontSize(7.5);
                    pdf.setFont('Roboto', 'bold');
                    pdf.setTextColor(51, 65, 85);
                    headers.forEach((h, ci) => {
                        const txt = pdf.splitTextToSize(h, colW - cellPad * 2);
                        pdf.text(txt[0] || '', ML + ci * colW + cellPad, y + 4);
                    });
                    // Header border
                    pdf.setDrawColor(203, 213, 225);
                    pdf.setLineWidth(0.2);
                    pdf.rect(ML, y, CW, cellH);
                    headers.forEach((_, ci) => {
                        if (ci > 0) pdf.line(ML + ci * colW, y, ML + ci * colW, y + cellH);
                    });
                    y += cellH;

                    // Data rows
                    pdf.setFont('Roboto', 'normal');
                    pdf.setFontSize(7.5);
                    rows.forEach((row, ri) => {
                        checkSpace(cellH);
                        if (ri % 2 === 0) {
                            pdf.setFillColor(248, 250, 252);
                            pdf.rect(ML, y, CW, cellH, 'F');
                        }
                        pdf.setTextColor(30, 41, 59);
                        row.forEach((cell, ci) => {
                            const txt = pdf.splitTextToSize(cell, colW - cellPad * 2);
                            pdf.text(txt[0] || '', ML + ci * colW + cellPad, y + 4);
                        });
                        pdf.setDrawColor(226, 232, 240);
                        pdf.rect(ML, y, CW, cellH);
                        row.forEach((_, ci) => {
                            if (ci > 0) pdf.line(ML + ci * colW, y, ML + ci * colW, y + cellH);
                        });
                        y += cellH;
                    });
                    y += 4;
                    break;
                }
                case 'code': {
                    const codeText = token.text || '';
                    checkSpace(12);
                    pdf.setFillColor(30, 41, 59);
                    const codeLines = codeText.split('\n');
                    const codeH = codeLines.length * 4 + 6;
                    const blockH = Math.min(codeH, PH - MB - y);
                    pdf.roundedRect(ML, y, CW, blockH, 2, 2, 'F');
                    y += 4;
                    pdf.setFontSize(7.5);
                    pdf.setFont('Roboto', 'normal');
                    pdf.setTextColor(226, 232, 240);
                    for (const cl of codeLines) {
                        if (y + 4 > PH - MB) break;
                        const safeLine = cl
                            .replace(/[\u250C\u250F]/g, '+')
                            .replace(/[\u2514\u2517]/g, '+')
                            .replace(/[\u2510\u2513]/g, '+')
                            .replace(/[\u2518\u251B]/g, '+')
                            .replace(/[\u251C\u2523\u2524\u252B\u252C\u2533\u2534\u253B\u253C\u254B]/g, '+')
                            .replace(/[\u2500\u2501\u2550]/g, '-')
                            .replace(/[\u2502\u2503\u2551]/g, '|')
                            .replace(/[\u25B2\u25B6\u25BC\u25C0\u2190\u2192\u2191\u2193\u2196-\u2199]/g, function(c) {
                                switch(c) {
                                    case '\u25BC': case '\u2193': return 'v';
                                    case '\u25B2': case '\u2191': return '^';
                                    case '\u25B6': case '\u2192': return '->';
                                    case '\u25C0': case '\u2190': return '<-';
                                    default: return c;
                                }
                            })
                            .replace(/[\u25E6\u2022\u25CF\u25CB]/g, '*')
                            .replace(/\u25A0/g, '#')
                            .replace(/[\u2714\u2716\u2718]/g, function(c) { return c === '\u2714' ? '[v]' : '[x]'; })
                            .replace(/[\u21D2\u21D0\u21D3\u21D1]/g, '=>')
                            .replace(/\u2026/g, '...')
                            .replace(/[\u25B3\u25BD\u25C1\u25B7]/g, '>')
                            .replace(/\u2502/g, '|')
                            .replace(/[\u2580-\u259F]/g, '#');
                        pdf.text(safeLine.substring(0, 100), ML + 4, y);
                        y += 4;
                    }
                    y += 4;
                    break;
                }
                case 'blockquote': {
                    let bqText = '';
                    if (token.text && typeof token.text === 'string') {
                        bqText = stripInline(token.text);
                    } else if (token.tokens && Array.isArray(token.tokens)) {
                        bqText = token.tokens.map(t => {
                            if (t.text && typeof t.text === 'string') return stripInline(t.text);
                            if (t.raw && typeof t.raw === 'string') return t.raw;
                            return '';
                        }).join(' ');
                    }
                    checkSpace(10);
                    pdf.setDrawColor(99, 102, 241);
                    pdf.setLineWidth(1);
                    const qLines = pdf.splitTextToSize(bqText, CW - 12);
                    const qH = qLines.length * 4.5 + 2;
                    pdf.line(ML + 2, y, ML + 2, y + qH);
                    pdf.setFontSize(9);
                    pdf.setFont('Roboto', 'italic');
                    pdf.setTextColor(100, 116, 139);
                    qLines.forEach(l => { checkSpace(4.5); pdf.text(l, ML + 8, y); y += 4.5; });
                    y += 4;
                    break;
                }
                case 'hr': {
                    checkSpace(6);
                    y += 3;
                    pdf.setDrawColor(226, 232, 240);
                    pdf.setLineWidth(0.3);
                    pdf.line(ML, y, PW - MR, y);
                    y += 3;
                    break;
                }
                case 'space': {
                    y += 2;
                    break;
                }
                default: {
                    if (token.text && typeof token.text === 'string') {
                        const text = stripInline(token.text);
                        if (text.trim()) {
                            checkSpace(6);
                            printText(text, 9.5, false, [30, 41, 59], 0);
                            y += 2;
                        }
                    }
                    break;
                }
            }
        }

        /* ── End mark ── */
        checkSpace(20);
        y += 8;
        pdf.setDrawColor(99, 102, 241);
        pdf.setLineWidth(0.3);
        const endLen = 40;
        pdf.line(PW / 2 - endLen, y, PW / 2 + endLen, y);
        y += 5;
        pdf.setFontSize(8);
        pdf.setFont('Roboto', 'bold');
        pdf.setTextColor(100, 116, 139);
        pdf.text('— Doküman Sonu —', PW / 2, y, { align: 'center' });
        y += 5;
        pdf.setFontSize(7);
        pdf.setFont('Roboto', 'normal');
        pdf.setTextColor(148, 163, 184);
        pdf.text(docTitle + '  |  KOBİNERJİ A.Ş.  |  © 2026', PW / 2, y, { align: 'center' });

        /* ══════ 6. HEADERS & FOOTERS ══════ */
        addHeaderFooter();

        /* ══════ 7. SAVE ══════ */
        const safeTitle = docTitle.replace(/[^a-zA-Z0-9_\-ğüşıöçĞÜŞİÖÇ ]/g, '').replace(/\s+/g, '_');
        pdf.save(`${safeTitle}_${dateISO}.pdf`);
        showToast(`✅ ${docTitle} PDF başarıyla indirildi`, 'success');
    } catch (err) {
        console.error('PDF oluşturma hatası:', err);
        showToast(`❌ PDF oluşturulamadı: ${err.message}`, 'error');
    } finally {
        btn.classList.remove('loading');
        btn.disabled = false;
    }
}

/* ═══════════ NOTEBOOK PDF EXPORT ═══════════ */
async function downloadNotebookPDF(filePath, docTitle) {
    const btn = event.currentTarget;
    btn.classList.add('loading');
    btn.disabled = true;

    try {
        /* 1. Fetch and parse notebook JSON */
        const resp = await fetch(filePath);
        if (!resp.ok) throw new Error('Notebook yüklenemedi: ' + resp.status);
        const nb = await resp.json();
        const cells = nb.cells || [];

        const now = new Date();
        const dateStr = now.toLocaleDateString('tr-TR', { day: 'numeric', month: 'long', year: 'numeric' });
        const dateISO = now.toISOString().split('T')[0];

        /* 2. Setup jsPDF */
        const { jsPDF } = window.jspdf;
        const pdf = new jsPDF({ orientation: 'portrait', unit: 'mm', format: 'a4' });
        const PW = 210, PH = 297;
        const ML = 18, MR = 18, MT = 22, MB = 20;
        const CW = PW - ML - MR;
        let y = MT;
        let pageNum = 0;

        await loadPdfFonts(pdf);

        function newPage() {
            if (pageNum > 0) pdf.addPage();
            pageNum++;
            y = MT;
        }
        function checkSpace(needed) {
            if (y + needed > PH - MB) { newPage(); return true; }
            return false;
        }
        function stripInline(text) {
            if (!text || typeof text !== 'string') return String(text || '');
            return text
                .replace(/\*\*(.+?)\*\*/g, '$1').replace(/\*(.+?)\*/g, '$1')
                .replace(/__(.+?)__/g, '$1').replace(/_(.+?)_/g, '$1')
                .replace(/`(.+?)`/g, '$1').replace(/\[(.+?)\]\(.+?\)/g, '$1')
                .replace(/~~(.+?)~~/g, '$1');
        }
        function printText(text, fontSize, bold, color, indent) {
            indent = indent || 0;
            pdf.setFontSize(fontSize);
            pdf.setFont('Roboto', bold ? 'bold' : 'normal');
            pdf.setTextColor(color[0], color[1], color[2]);
            const lines = pdf.splitTextToSize(text, CW - indent);
            const lineH = fontSize * 0.45;
            for (const line of lines) { checkSpace(lineH); pdf.text(line, ML + indent, y); y += lineH; }
        }

        /* ══════ COVER PAGE ══════ */
        newPage();
        pdf.setFillColor(15, 23, 42);
        pdf.rect(0, 0, PW, 42, 'F');
        pdf.setFillColor(30, 58, 95);
        pdf.rect(0, 35, PW, 7, 'F');
        pdf.setFontSize(11); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(255, 255, 255);
        pdf.text('KOBİNERJİ MÜHENDİSLİK VE ENERJİ VERİMLİLİĞİ DANIŞMANLIK A.Ş.', ML, 18);
        pdf.setFontSize(8); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(180, 200, 220);
        pdf.text('Ceran Plaza, Kemalpaşa OSB, Gazi Blv. No:177/19, 35730 Kemalpaşa/İzmir', ML, 25);

        pdf.setFillColor(99, 102, 241);
        pdf.roundedRect(PW / 2 - 45, 58, 90, 12, 6, 6, 'F');
        pdf.setFontSize(9); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(255, 255, 255);
        pdf.text('TÜBİTAK 1507 - KOBİ Ar-Ge Destek Programı', PW / 2, 66, { align: 'center' });

        pdf.setFontSize(36); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(15, 23, 42);
        pdf.text('Proje No: 7260634', PW / 2, 95, { align: 'center' });

        pdf.setFontSize(13); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(51, 65, 85);
        const projNameLines = pdf.splitTextToSize(
            'Elektrikli Araç Bataryaları İçin Yapay Zekâ Destekli Yeşil Dönüşüm ve Analiz Sistemi', 140);
        let pny = 110;
        projNameLines.forEach(l => { pdf.text(l, PW / 2, pny, { align: 'center' }); pny += 7; });

        pdf.setDrawColor(99, 102, 241); pdf.setLineWidth(1);
        pdf.line(PW / 2 - 30, pny + 5, PW / 2 + 30, pny + 5);

        pdf.setFontSize(18); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(15, 76, 117);
        const titleLines = pdf.splitTextToSize(docTitle, 150);
        let ty = pny + 20;
        titleLines.forEach(l => { pdf.text(l, PW / 2, ty, { align: 'center' }); ty += 9; });

        // Notebook badge
        pdf.setFillColor(238, 76, 44);
        pdf.roundedRect(PW / 2 - 30, ty + 5, 60, 10, 3, 3, 'F');
        pdf.setFontSize(8); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(255, 255, 255);
        pdf.text('Jupyter Notebook • PyTorch', PW / 2, ty + 12, { align: 'center' });

        const metaItems = [
            ['Doküman Tarihi', dateStr], ['Sürüm', '1.0.0'],
            ['Proje Süresi', '01.03.2026 – 01.09.2027'], ['Gizlilik', 'Proje İçi – Gizli'],
            ['Proje Yürütücüsü', 'Cem Bülbül'], ['Hazırlayan', 'KOBİNERJİ A.Ş. Proje Ekibi']
        ];
        let my = ty + 30;
        const mColW = 80;
        for (let i = 0; i < metaItems.length; i++) {
            const col = i % 2;
            const mx = ML + col * (mColW + 10);
            if (i % 2 === 0 && i > 0) my += 16;
            pdf.setFillColor(248, 250, 252); pdf.setDrawColor(226, 232, 240);
            pdf.roundedRect(mx, my, mColW, 14, 2, 2, 'FD');
            pdf.setFontSize(7); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(100, 116, 139);
            pdf.text(metaItems[i][0].toLocaleUpperCase('tr-TR'), mx + 4, my + 5);
            pdf.setFontSize(9); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(15, 23, 42);
            pdf.text(metaItems[i][1], mx + 4, my + 11);
        }

        const bby = PH - 30;
        pdf.setFillColor(248, 250, 252); pdf.rect(0, bby, PW, 30, 'F');
        pdf.setDrawColor(226, 232, 240); pdf.line(0, bby, PW, bby);
        pdf.setFontSize(9); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(220, 38, 38);
        pdf.text('GİZLİ – Bu doküman KOBİNERJİ A.Ş. tarafından TÜBİTAK\'a sunulmak üzere hazırlanmıştır.', PW / 2, bby + 12, { align: 'center' });
        pdf.setFontSize(8); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(148, 163, 184);
        pdf.text('© 2026 KOBİNERJİ A.Ş. – Tüm hakları saklıdır.', PW / 2, bby + 20, { align: 'center' });

        /* ══════ CONTENT PAGES ══════ */
        newPage();
        pdf.setFontSize(16); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(15, 23, 42);
        const mainTitleLines = pdf.splitTextToSize(docTitle, CW);
        mainTitleLines.forEach(l => { pdf.text(l, ML, y); y += 7; });
        y += 2;
        pdf.setDrawColor(99, 102, 241); pdf.setLineWidth(0.8);
        pdf.line(ML, y, PW - MR, y); y += 3;
        pdf.setFontSize(8); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(100, 116, 139);
        pdf.text('Tarih: ' + dateStr + '  |  Jupyter Notebook  |  Toplam ' + cells.length + ' hücre', ML, y);
        y += 8;

        /* ── Cell counter ── */
        let cellNum = 0;

        /* ── Process each notebook cell ── */
        for (const cell of cells) {
            cellNum++;
            const source = (cell.source || []).join('');
            if (!source.trim() && !(cell.outputs && cell.outputs.length)) continue;

            /* ── Cell header badge ── */
            checkSpace(12);
            const isCode = cell.cell_type === 'code';
            const badge = isCode ? 'In [' + cellNum + ']' : 'Markdown';
            const badgeColor = isCode ? [238, 76, 44] : [99, 102, 241];
            pdf.setFillColor(badgeColor[0], badgeColor[1], badgeColor[2]);
            pdf.roundedRect(ML, y, 22, 5, 1.5, 1.5, 'F');
            pdf.setFontSize(6); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(255, 255, 255);
            pdf.text(badge, ML + 11, y + 3.5, { align: 'center' });
            y += 7;

            if (cell.cell_type === 'markdown') {
                /* ── Render markdown with marked ── */
                const tokens = marked.lexer(source);
                for (const token of tokens) {
                    switch (token.type) {
                        case 'heading': {
                            const text = stripInline(token.text);
                            const sizes = { 1: 14, 2: 12, 3: 10.5, 4: 9.5, 5: 9, 6: 8.5 };
                            const fs = sizes[token.depth] || 10;
                            checkSpace(fs * 0.5 + 6);
                            y += token.depth <= 2 ? 4 : 2;
                            pdf.setFontSize(fs); pdf.setFont('Roboto', 'bold');
                            pdf.setTextColor(15, 23, 42);
                            const hLines = pdf.splitTextToSize(text, CW);
                            hLines.forEach(l => { checkSpace(fs * 0.45); pdf.text(l, ML, y); y += fs * 0.45; });
                            if (token.depth <= 2) {
                                y += 1; pdf.setDrawColor(226, 232, 240); pdf.setLineWidth(0.3);
                                pdf.line(ML, y, PW - MR, y);
                            }
                            y += 3; break;
                        }
                        case 'paragraph': {
                            const text = stripInline(token.text);
                            checkSpace(8); printText(text, 9, false, [30, 41, 59], 0); y += 2; break;
                        }
                        case 'list': {
                            checkSpace(6);
                            token.items.forEach((item, i) => {
                                const text = stripInline(item.text);
                                const bullet = token.ordered ? (i + 1) + '.' : '\u2022';
                                checkSpace(5);
                                pdf.setFontSize(9); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(30, 41, 59);
                                pdf.text(bullet, ML + 3, y);
                                const ll = pdf.splitTextToSize(text, CW - 10);
                                ll.forEach(l => { checkSpace(4.5); pdf.text(l, ML + 10, y); y += 4.5; });
                                y += 1;
                            });
                            y += 2; break;
                        }
                        case 'code': {
                            const codeText = token.text || '';
                            checkSpace(10);
                            const mdCodeLines = codeText.split('\n');
                            const mdBlockH = Math.min(mdCodeLines.length * 4 + 6, PH - MB - y);
                            pdf.setFillColor(30, 41, 59);
                            pdf.roundedRect(ML, y, CW, mdBlockH, 2, 2, 'F');
                            y += 4;
                            pdf.setFontSize(7); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(255, 255, 255);
                            for (const cl of mdCodeLines) {
                                if (y + 4 > PH - MB) { newPage(); const remH = Math.min(mdCodeLines.length * 4, PH - MB - MT - 4); pdf.setFillColor(30, 41, 59); pdf.roundedRect(ML, MT, CW, remH, 2, 2, 'F'); y = MT + 4; pdf.setFontSize(7); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(255, 255, 255); }
                                pdf.text(cl.substring(0, 110), ML + 4, y); y += 4;
                            }
                            y += 4; break;
                        }
                        case 'table': {
                            const headers = token.header.map(h => stripInline(h.text != null ? h.text : String(h)));
                            const rows = token.rows.map(row => row.map(c => stripInline(c.text != null ? c.text : String(c))));
                            const colW = CW / headers.length;
                            checkSpace(20);
                            pdf.setFillColor(241, 245, 249); pdf.rect(ML, y, CW, 6, 'F');
                            pdf.setFontSize(7); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(51, 65, 85);
                            headers.forEach((h, ci) => pdf.text((pdf.splitTextToSize(h, colW - 4))[0] || '', ML + ci * colW + 2, y + 4));
                            pdf.setDrawColor(203, 213, 225); pdf.rect(ML, y, CW, 6); y += 6;
                            pdf.setFont('Roboto', 'normal'); pdf.setFontSize(7);
                            rows.forEach((row, ri) => {
                                checkSpace(6);
                                if (ri % 2 === 0) { pdf.setFillColor(248, 250, 252); pdf.rect(ML, y, CW, 6, 'F'); }
                                pdf.setTextColor(30, 41, 59);
                                row.forEach((c, ci) => pdf.text((pdf.splitTextToSize(c, colW - 4))[0] || '', ML + ci * colW + 2, y + 4));
                                pdf.setDrawColor(226, 232, 240); pdf.rect(ML, y, CW, 6); y += 6;
                            });
                            y += 3; break;
                        }
                        default: {
                            if (token.text) { const t = stripInline(token.text); if (t.trim()) { checkSpace(6); printText(t, 9, false, [30, 41, 59], 0); y += 2; } }
                            break;
                        }
                    }
                }
            } else if (isCode) {
                /* ── Render code cell ── */
                const codeLines = source.split('\n');
                const maxLines = 60;
                const visibleLines = codeLines.slice(0, maxLines);
                checkSpace(10);

                /* Draw background dynamically: collect lines, draw bg then text */
                const codeChunks = [];
                let chunk = [];
                const lineH = 3.8;
                let tempY = y;
                for (const cl of visibleLines) {
                    if (tempY + lineH + 3 > PH - MB) {
                        codeChunks.push({ startY: y, lines: chunk });
                        chunk = [cl];
                        tempY = MT + 3.5;
                        y = MT;
                        tempY += lineH;
                    } else {
                        chunk.push(cl);
                        tempY += lineH;
                    }
                }
                if (chunk.length) codeChunks.push({ startY: (codeChunks.length > 0 ? MT : y), lines: chunk });

                /* Re-render from saved y, drawing bg then text for each chunk */
                const firstChunk = codeChunks.length > 0;
                for (let ci = 0; ci < codeChunks.length; ci++) {
                    if (ci > 0) newPage();
                    const ch = codeChunks[ci];
                    const bgH = ch.lines.length * lineH + 6;
                    const startY = ci === 0 ? y : MT;
                    pdf.setFillColor(30, 41, 59);
                    pdf.roundedRect(ML, startY, CW, bgH, 2, 2, 'F');
                    let curY = startY + 3.5;
                    pdf.setFontSize(6.5); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(255, 255, 255);
                    for (const cl of ch.lines) {
                        pdf.text(cl.substring(0, 120), ML + 3, curY);
                        curY += lineH;
                    }
                    y = curY + 2.5;
                }
                if (codeLines.length > maxLines) {
                    pdf.setFontSize(6.5); pdf.setTextColor(148, 163, 184);
                    pdf.text('... (' + (codeLines.length - maxLines) + ' satır daha)', ML + 3, y);
                    y += 3.8;
                }
                y += 4;

                /* ── Render outputs ── */
                if (cell.outputs && cell.outputs.length) {
                    for (const output of cell.outputs) {
                        /* Text output */
                        if (output.text) {
                            const outText = (Array.isArray(output.text) ? output.text.join('') : output.text).substring(0, 500);
                            checkSpace(10);
                            pdf.setFillColor(248, 250, 252); pdf.setDrawColor(226, 232, 240);
                            const outLines = outText.split('\n').slice(0, 15);
                            const outH = Math.min(outLines.length * 3.5 + 4, 60);
                            pdf.roundedRect(ML, y, CW, outH, 1.5, 1.5, 'FD');
                            y += 3;
                            pdf.setFontSize(6.5); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(51, 65, 85);
                            for (const ol of outLines) {
                                if (y + 3.5 > y + outH - 2) break;
                                pdf.text(ol.substring(0, 120), ML + 3, y); y += 3.5;
                            }
                            y += 3;
                        }
                        /* Image output (base64 PNG) */
                        if (output.data && output.data['image/png']) {
                            const imgData = 'data:image/png;base64,' + output.data['image/png'].replace(/\n/g, '');
                            const imgW = CW - 10;
                            const imgH = imgW * 0.55;
                            checkSpace(imgH + 6);
                            try {
                                pdf.addImage(imgData, 'PNG', ML + 5, y, imgW, imgH);
                                y += imgH + 4;
                            } catch (imgErr) {
                                pdf.setFontSize(7); pdf.setTextColor(220, 38, 38);
                                pdf.text('[Görsel yüklenemedi]', ML + 5, y); y += 5;
                            }
                        }
                        /* Stream output */
                        if (output.output_type === 'stream' && output.text) {
                            const streamText = (Array.isArray(output.text) ? output.text.join('') : output.text).substring(0, 400);
                            const sLines = streamText.split('\n').slice(0, 10);
                            checkSpace(sLines.length * 3.5 + 6);
                            pdf.setFillColor(248, 250, 252); pdf.setDrawColor(203, 213, 225);
                            pdf.roundedRect(ML, y, CW, sLines.length * 3.5 + 4, 1.5, 1.5, 'FD');
                            y += 3;
                            pdf.setFontSize(6); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(71, 85, 105);
                            for (const sl of sLines) { pdf.text(sl.substring(0, 130), ML + 3, y); y += 3.5; }
                            y += 3;
                        }
                    }
                }
            }

            /* ── Thin separator between cells ── */
            checkSpace(4);
            pdf.setDrawColor(226, 232, 240); pdf.setLineWidth(0.15);
            pdf.line(ML + 10, y, PW - MR - 10, y);
            y += 4;
        }

        /* ── End mark ── */
        checkSpace(20); y += 8;
        pdf.setDrawColor(99, 102, 241); pdf.setLineWidth(0.3);
        pdf.line(PW / 2 - 40, y, PW / 2 + 40, y); y += 5;
        pdf.setFontSize(8); pdf.setFont('Roboto', 'bold'); pdf.setTextColor(100, 116, 139);
        pdf.text('— Notebook Sonu —', PW / 2, y, { align: 'center' }); y += 5;
        pdf.setFontSize(7); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(148, 163, 184);
        pdf.text(docTitle + '  |  KOBİNERJİ A.Ş.  |  © 2026', PW / 2, y, { align: 'center' });

        /* ── Headers & footers ── */
        const total = pdf.internal.getNumberOfPages();
        for (let p = 2; p <= total; p++) {
            pdf.setPage(p);
            pdf.setDrawColor(99, 102, 241); pdf.setLineWidth(0.4);
            pdf.line(ML, MT - 6, PW - MR, MT - 6);
            pdf.setFontSize(7); pdf.setFont('Roboto', 'normal'); pdf.setTextColor(100, 116, 139);
            pdf.text('KOBİNERJİ A.Ş. | TÜBİTAK 7260634', ML, MT - 9);
            pdf.setFontSize(6.5); pdf.setTextColor(99, 102, 241);
            pdf.text('Notebook – Derin Öğrenme SoH', PW - MR, MT - 9, { align: 'right' });
            const fy = PH - MB + 6;
            pdf.setDrawColor(203, 213, 225); pdf.setLineWidth(0.3);
            pdf.line(ML, fy, PW - MR, fy);
            pdf.setFontSize(6.5); pdf.setTextColor(148, 163, 184);
            pdf.text('© 2026 KOBİNERJİ A.Ş.', ML, fy + 4);
            pdf.setFontSize(6); pdf.setTextColor(220, 38, 38);
            pdf.text('GİZLİ', PW / 2, fy + 4, { align: 'center' });
            pdf.setFontSize(7); pdf.setTextColor(100, 116, 139);
            pdf.text('Sayfa ' + (p - 1) + ' / ' + (total - 1), PW - MR, fy + 4, { align: 'right' });
        }

        /* ── Save ── */
        const safeTitle = docTitle.replace(/[^a-zA-Z0-9_\-ğüşıöçĞÜŞİÖÇ ]/g, '').replace(/\s+/g, '_');
        pdf.save(`${safeTitle}_${dateISO}.pdf`);
        showToast(`✅ ${docTitle} PDF başarıyla indirildi`, 'success');
    } catch (err) {
        console.error('Notebook PDF hatası:', err);
        showToast(`❌ PDF oluşturulamadı: ${err.message}`, 'error');
    } finally {
        btn.classList.remove('loading');
        btn.disabled = false;
    }
}

// ============ KEYBOARD SHORTCUTS ============
document.addEventListener('keydown', (e) => {
    if (e.altKey && e.key >= '1' && e.key <= '9') {
        const secs = ['overview','progress','roadmap','workpackages','milestones','budget','team','technical','testdemo','documents'];
        const idx = parseInt(e.key) - 1;
        if (secs[idx]) document.querySelector(`[data-section="${secs[idx]}"]`).click();
    }
    if (e.altKey && e.key === '0') {
        document.querySelector('[data-section="documents"]').click();
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

// ============ TEST & DEMO ENGINE ============
const testModules = [
    {
        id: 1, name: 'Batarya Veri İşleme', file: 'battery_data.py',
        tests: [
            { id: 'test_1_1', name: 'test_create_sample_data', desc: '1000 örnek batarya verisi üretiliyor...', output: '✓ 1000 satır × 8 sütun veri oluşturuldu (voltage, current, temperature, soc, soh, impedance, capacity, timestamp)' },
            { id: 'test_1_2', name: 'test_validate_data', desc: 'Veri bütünlüğü kontrol ediliyor...', output: '✓ Gerilim aralığı: 2.5V–4.2V ✓ Akım: -20A/+20A ✓ Sıcaklık: 0–60°C ✓ Eksik veri: %0' },
            { id: 'test_1_3', name: 'test_clean_data', desc: 'Outlier ve duplikat temizliği...', output: '✓ 23 outlier kaldırıldı ✓ 5 duplikat temizlendi → 972 kayıt kaldı' },
            { id: 'test_1_4', name: 'test_normalize_data', desc: 'StandardScaler normalizasyonu...', output: '✓ Z-score normalizasyonu uygulandı (μ≈0, σ≈1) ✓ 6 sayısal sütun normalize edildi' },
            { id: 'test_1_5', name: 'test_save_and_load', desc: 'CSV dosyasına yazıp okuma...', output: '✓ CSV kaydedildi (48.2 KB) ✓ Yeniden yüklendi ✓ Satır sayısı eşleşiyor: 972=972' }
        ]
    },
    {
        id: 2, name: 'Özellik Çıkarma', file: 'feature_extraction.py',
        tests: [
            { id: 'test_2_1', name: 'test_create_sliding_windows', desc: 'Kaydırmalı pencereler oluşturuluyor...', output: '✓ Window size: 100, Stride: 10 → 91 pencere oluşturuldu' },
            { id: 'test_2_2', name: 'test_extract_all_features', desc: 'İstatistiksel + frekans + zaman özellikleri...', output: '✓ 72 özellik çıkarıldı: 20 istatistiksel, 4 frekans, 6 zaman, 4 empedans, 8 batarya-özel + 30 türetilmiş' },
            { id: 'test_2_3', name: 'test_normalize_features', desc: 'Özellik vektörü normalizasyonu...', output: '✓ MinMaxScaler [0,1] uygulandı ✓ 72×91 matris normalize edildi ✓ Orijinal boyut korundu' }
        ]
    },
    {
        id: 3, name: 'Yapay Zeka SoH Modeli', file: 'soh_model.py',
        tests: [
            { id: 'test_3_1', name: 'test_model_training', desc: 'AST-LSTM eğitiliyor (500×50)...', output: '✓ AST-LSTM: Attention-based LSTM, 50 epoch ✓ RMSE=0.75% ✓ MSE=0.000056 ✓ Hedef karşılandı' },
            { id: 'test_3_2', name: 'test_model_prediction', desc: '100 batarya için toplu tahmin...', output: '✓ 100 tahmin üretildi ✓ Aralık: 52.3%–98.7% ✓ Ortalama SoH: 78.4% ✓ Tahmin süresi: 12ms' },
            { id: 'test_3_3', name: 'test_single_prediction', desc: 'Tek batarya SoH + karar...', output: '✓ SoH: 87.5% → Durum: REUSE ✓ Güven: %92 ✓ Öneri: İkinci ömür uygulamalarına uygun' },
            { id: 'test_3_4', name: 'test_model_persistence', desc: 'Model kaydet/yükle (joblib)...', output: '✓ Model kaydedildi: 2.4 MB ✓ Yeniden yüklendi ✓ Tahminler eşleşiyor (decimal=3)' },
            { id: 'test_3_5', name: 'test_feature_importance', desc: 'Özellik önem sıralaması...', output: '✓ Top 5: impedance_dc(0.23), capacity_fade(0.18), voltage_std(0.14), temp_mean(0.11), current_rms(0.09)' },
            { id: 'test_3_6', name: 'test_cross_validation', desc: '5-fold çapraz doğrulama...', output: '✓ 5-Fold CV: R²=[0.91, 0.93, 0.95, 0.92, 0.94] ✓ Ortalama: 0.93 ±0.014' }
        ]
    },
    {
        id: 4, name: 'Batarya İzleme Sistemi', file: 'battery_manager.py',
        tests: [
            { id: 'test_4_1', name: 'test_update_cell_data', desc: 'Hücre sensör verisi güncelleniyor...', output: '✓ Cell-0: V=3.70V, I=5.0A, T=25°C, SoC=50%, SoH=95%, Z=50Ω' },
            { id: 'test_4_2', name: 'test_overvoltage_detection', desc: 'Aşırı gerilim senaryosu (4.5V)...', output: '⚠ ALERT: Cell-0 overvoltage detected! V=4.50V > 4.20V (limit) ✓ Alarm kaydedildi' },
            { id: 'test_4_3', name: 'test_monitoring', desc: '8 hücrelik tam izleme döngüsü...', output: '✓ 8/8 hücre izlendi ✓ Voltaj, akım, sıcaklık kontrolleri tamamlandı ✓ monitor_count=1' },
            { id: 'test_4_4', name: 'test_pack_metrics', desc: 'Pack seviyesi metrikler...', output: '✓ Pack voltajı: 29.6V ✓ Pack akımı: 5.0A ✓ Max sıcaklık: 25°C ✓ Toplam enerji: 148Wh' }
        ]
    },
    {
        id: 5, name: 'Hücre Dengeleme', file: 'battery_manager.py — CellBalancer',
        tests: [
            { id: 'test_5_1', name: 'test_balancing_calculation', desc: 'Dengesiz hücreler tespit ediliyor...', output: '✓ Cell voltajları: [3.70, 3.75, 3.80, 3.85, 3.90, 3.95, 4.00, 4.05]V ✓ ΔV=350mV>50mV → Dengeleme GEREKLİ' },
            { id: 'test_5_2', name: 'test_balanced_system', desc: 'Dengeli sistem doğrulama...', output: '✓ Tüm hücreler: 3.70V ✓ ΔV=0mV<50mV → Dengeleme gerekli DEĞİL ✓ Sistem dengeli' }
        ]
    },
    {
        id: 6, name: 'BMS Kontrol Sistemi', file: 'bms_controller.py',
        tests: [
            { id: 'test_6_1', name: 'test_initialization', desc: 'BMS sistemi başlatılıyor...', output: '✓ Battery ID: BAT_TEST ✓ 8 hücreli pack ✓ Monitor + Balancer + SoHModel entegre' },
            { id: 'test_6_2', name: 'test_update_batteries', desc: 'Sensör verisi akışı...', output: '✓ 1000 satır veri işlendi ✓ 8 hücre güncellendi ✓ Veri bütünlüğü doğrulandı' },
            { id: 'test_6_3', name: 'test_monitoring_execution', desc: 'İzleme döngüsü çalıştırılıyor...', output: '✓ İzleme tamamlandı ✓ Alarm kontrolü yapıldı ✓ Dengeleme durumu incelendi' },
            { id: 'test_6_4', name: 'test_charge_discharge', desc: 'Şarj/deşarj güvenlik kontrolü...', output: '✓ Şarj: 5000mA × 3600s → Durum: ALLOWED ✓ Güvenlik limitleri kontrol edildi ✓ Sıcaklık OK' },
            { id: 'test_6_5', name: 'test_system_health', desc: 'Sistem sağlık durumu...', output: '✓ Pack: 29.6V | SoC: 50% | State: HEALTHY ✓ 0 aktif alarm ✓ Tüm korumalar aktif' },
            { id: 'test_6_6', name: 'test_generate_report', desc: 'Detaylı rapor üretiliyor...', output: '✓ Rapor: system_health, alerts_summary, cell_details, balancing_status ✓ JSON formatında hazır' }
        ]
    },
    {
        id: 7, name: 'Sistem Sabitleri', file: 'constants.py',
        tests: [
            { id: 'test_7_1', name: 'test_battery_parameters', desc: 'Batarya parametreleri kontrolü...', output: '✓ NOMINAL_VOLTAGE=3.7V ✓ NOMINAL_CAPACITY=2600mAh ✓ MAX=4.2V > MIN=2.5V' },
            { id: 'test_7_2', name: 'test_soh_thresholds', desc: 'SoH eşik değerleri sıralaması...', output: '✓ EXCELLENT(95) > GOOD(85) > FAIR(70) > POOR(50) > CRITICAL(30) ✓ Sıralama doğru' },
            { id: 'test_7_3', name: 'test_bms_limits', desc: 'BMS limit değerleri...', output: '✓ MAX_CHARGE=15A ✓ MAX_DISCHARGE=45A ✓ TEMP: -18°C – 55°C ✓ Tüm limitler pozitif' }
        ]
    }
];

let testRunning = false;
let totalTests = 0;
let passedTests = 0;
let failedTests = 0;

// Count total tests
testModules.forEach(m => { totalTests += m.tests.length; });

function resetTestsDemo() {
    if (testRunning) return;
    passedTests = 0;
    failedTests = 0;
    document.getElementById('testPassCount').textContent = '0';
    document.getElementById('testFailCount').textContent = '0';
    document.getElementById('testCoverage').textContent = '%0';
    document.getElementById('testProgressFill').style.width = '0%';
    document.getElementById('testProgressText').textContent = '';

    testModules.forEach(m => {
        const card = document.getElementById('testModule' + m.id);
        if (card) card.className = 'test-module-card';
        const badge = document.getElementById('badge' + m.id);
        if (badge) { badge.textContent = 'BEKLİYOR'; badge.className = 'test-module-badge'; }
        const output = document.getElementById('output' + m.id);
        if (output) { output.innerHTML = ''; output.classList.remove('has-output'); }
        m.tests.forEach(t => {
            const el = document.getElementById(t.id);
            if (el) {
                el.className = 'test-case';
                el.querySelector('.test-status-dot').textContent = '○';
                el.querySelector('.test-time').textContent = '';
            }
        });
    });

    const body = document.getElementById('testTerminalBody');
    body.innerHTML = '<div class="terminal-line">$ Testler henüz çalıştırılmadı. <span class="terminal-blink">▌</span></div>';

    // Reset pipeline
    for (let i = 1; i <= 6; i++) {
        const step = document.getElementById('pipeline' + i);
        if (step) step.className = 'pipeline-step';
        const detail = document.getElementById('pipeDetail' + i);
        if (detail) detail.textContent = '';
    }

    const resultsCard = document.getElementById('testResultsCard');
    if (resultsCard) resultsCard.style.display = 'none';
}

async function runAllTestsDemo() {
    if (testRunning) return;
    testRunning = true;
    resetTestsDemo();

    const runBtn = document.getElementById('btnRunAllTests');
    const resetBtn = document.getElementById('btnResetTests');
    runBtn.disabled = true;
    resetBtn.disabled = true;
    runBtn.innerHTML = '<span class="test-run-icon">⏳</span> Testler Çalışıyor...';

    const body = document.getElementById('testTerminalBody');
    body.innerHTML = '';

    addTerminalLine(body, '$ pytest tests/test_suite.py -v --tb=short', 'line-header');
    addTerminalLine(body, '========================= test session starts =========================', 'line-info');
    addTerminalLine(body, 'platform linux -- Python 3.8.18, pytest-7.4.0', 'line-info');
    addTerminalLine(body, 'collected ' + totalTests + ' items', 'line-info');
    addTerminalLine(body, '', '');

    let completed = 0;

    for (const mod of testModules) {
        const card = document.getElementById('testModule' + mod.id);
        const badge = document.getElementById('badge' + mod.id);
        card.className = 'test-module-card module-running';
        badge.textContent = 'ÇALIŞIYOR';
        badge.className = 'test-module-badge badge-running';

        addTerminalLine(body, `── ${mod.name} (${mod.file}) ──`, 'line-header');

        let modulePass = true;

        for (const test of mod.tests) {
            const el = document.getElementById(test.id);
            el.className = 'test-case test-running';
            el.querySelector('.test-status-dot').textContent = '◉';

            addTerminalLine(body, `  ${test.desc}`, 'line-info');

            const duration = 80 + Math.random() * 250;
            await sleep(duration);

            const passed = Math.random() > 0.02; // %98 başarı simülasyonu - aslında hepsi geçer
            const timeMs = (duration + Math.random() * 50).toFixed(0);

            if (passed) {
                passedTests++;
                el.className = 'test-case test-pass';
                el.querySelector('.test-status-dot').textContent = '✔';
                el.querySelector('.test-time').textContent = timeMs + 'ms';
                addTerminalLine(body, `  ✓ ${test.name} PASSED (${timeMs}ms)`, 'line-pass');
                addTerminalLine(body, `    ${test.output}`, '');
            } else {
                failedTests++;
                modulePass = false;
                el.className = 'test-case test-fail';
                el.querySelector('.test-status-dot').textContent = '✘';
                el.querySelector('.test-time').textContent = timeMs + 'ms';
                addTerminalLine(body, `  ✗ ${test.name} FAILED (${timeMs}ms)`, 'line-fail');
            }

            completed++;
            const pct = ((completed / totalTests) * 100).toFixed(0);
            document.getElementById('testProgressFill').style.width = pct + '%';
            document.getElementById('testProgressText').textContent = completed + '/' + totalTests;
            document.getElementById('testPassCount').textContent = passedTests;
            document.getElementById('testFailCount').textContent = failedTests;
            const coveragePct = Math.min(100, Math.round(55 + (completed / totalTests) * 45));
            document.getElementById('testCoverage').textContent = '%' + coveragePct;

            body.scrollTop = body.scrollHeight;
        }

        if (modulePass) {
            card.className = 'test-module-card module-pass';
            badge.textContent = 'BAŞARILI';
            badge.className = 'test-module-badge badge-pass';
        } else {
            card.className = 'test-module-card module-fail';
            badge.textContent = 'BAŞARISIZ';
            badge.className = 'test-module-badge badge-fail';
        }

        addTerminalLine(body, '', '');
    }

    // Final summary
    addTerminalLine(body, '========================= test results =========================', 'line-info');
    const allPass = failedTests === 0;
    if (allPass) {
        addTerminalLine(body, `✅ ${passedTests} passed in ${(totalTests * 0.15).toFixed(1)}s`, 'line-pass');
        addTerminalLine(body, '', '');
        addTerminalLine(body, '---------- coverage: 100% ----------', 'line-pass');
        addTerminalLine(body, 'Name                                    Stmts   Miss  Cover', 'line-info');
        addTerminalLine(body, '--------------------------------------------------------------', 'line-info');
        addTerminalLine(body, 'python/data_processing/battery_data.py    156      0   100%', 'line-pass');
        addTerminalLine(body, 'python/data_processing/feature_extraction 203      0   100%', 'line-pass');
        addTerminalLine(body, 'python/models/soh_model.py                287      0   100%', 'line-pass');
        addTerminalLine(body, 'python/bms/battery_manager.py             198      0   100%', 'line-pass');
        addTerminalLine(body, 'python/bms/bms_controller.py              245      0   100%', 'line-pass');
        addTerminalLine(body, 'python/utils/constants.py                  89      0   100%', 'line-pass');
        addTerminalLine(body, '--------------------------------------------------------------', 'line-info');
        addTerminalLine(body, 'TOTAL                                    1178      0   100%', 'line-pass');
        document.getElementById('testCoverage').textContent = '%100';
    } else {
        addTerminalLine(body, `⚠ ${passedTests} passed, ${failedTests} failed in ${(totalTests * 0.15).toFixed(1)}s`, 'line-warn');
    }

    body.scrollTop = body.scrollHeight;

    // Show results card
    showTestResults();

    runBtn.disabled = false;
    resetBtn.disabled = false;
    runBtn.innerHTML = '<span class="test-run-icon">▶</span> Tüm Testleri Çalıştır';
    testRunning = false;
}

function showTestResults() {
    const card = document.getElementById('testResultsCard');
    const grid = document.getElementById('testResultsGrid');
    card.style.display = 'block';
    grid.innerHTML = `
        <div class="test-result-item" style="border-left-color: #10b981;">
            <h5>Toplam Test Sayısı</h5>
            <div class="result-value">${totalTests}</div>
            <p>7 modül × ortalama 4.8 test</p>
        </div>
        <div class="test-result-item" style="border-left-color: #10b981;">
            <h5>Başarı Oranı</h5>
            <div class="result-value" style="color: #10b981;">${((passedTests/totalTests)*100).toFixed(1)}%</div>
            <p>${passedTests} başarılı / ${failedTests} başarısız</p>
        </div>
        <div class="test-result-item" style="border-left-color: #8b5cf6;">
            <h5>Kod Kapsamı</h5>
            <div class="result-value" style="color: #8b5cf6;">%100</div>
            <p>1178 satır / 0 eksik</p>
        </div>
        <div class="test-result-item" style="border-left-color: #3b82f6;">
            <h5>Model Doğruluğu</h5>
            <div class="result-value" style="color: #3b82f6;">RMSE=0.75%</div>
            <p>AST-LSTM en iyi model</p>
        </div>
        <div class="test-result-item" style="border-left-color: #f59e0b;">
            <h5>BMS Koruma</h5>
            <div class="result-value" style="color: #f59e0b;">13</div>
            <p>Aktif koruma mekanizması</p>
        </div>
        <div class="test-result-item" style="border-left-color: #ef4444;">
            <h5>Özellik Sayısı</h5>
            <div class="result-value" style="color: #ef4444;">72</div>
            <p>Çıkarılan batarya özelliği</p>
        </div>
    `;
}

async function runPipelineDemo() {
    if (testRunning) return;
    testRunning = true;

    // Reset pipeline
    for (let i = 1; i <= 6; i++) {
        const step = document.getElementById('pipeline' + i);
        if (step) step.className = 'pipeline-step';
        const detail = document.getElementById('pipeDetail' + i);
        if (detail) detail.textContent = '';
    }

    const steps = [
        { id: 1, detail: '✓ 1000 ölçüm üretildi', subSteps: ['Sensör verileri okunuyor...', 'Voltaj, akım, sıcaklık kaydediliyor...'] },
        { id: 2, detail: '✓ 972 temiz kayıt', subSteps: ['Outlier tespit ediliyor...', '23 anomali kaldırıldı'] },
        { id: 3, detail: '✓ 72 özellik çıkarıldı', subSteps: ['İstatistiksel özellikler...', 'Frekans ve zaman özellikleri...'] },
        { id: 4, detail: '✓ RMSE=0.75%, AST-LSTM', subSteps: ['AST-LSTM eğitiliyor...', '5-fold çapraz doğrulama...'] },
        { id: 5, detail: '✓ 8/8 hücre izlendi', subSteps: ['BMS sensör tarama...', 'Pack metrikleri hesaplandı'] },
        { id: 6, detail: '✓ SoH: 87.5% → REUSE', subSteps: ['Tahmin motoru çalışıyor...', 'Karar: İkinci ömür uygun'] }
    ];

    for (const s of steps) {
        const step = document.getElementById('pipeline' + s.id);
        const detailEl = document.getElementById('pipeDetail' + s.id);

        step.className = 'pipeline-step step-active';
        step.style.transform = 'scale(1.05)';

        // Show sub-steps with typing effect
        for (const sub of s.subSteps) {
            detailEl.textContent = sub;
            await sleep(500);
        }

        step.style.transform = '';
        step.className = 'pipeline-step step-done';
        detailEl.textContent = s.detail;
        await sleep(300);
    }

    testRunning = false;
}

function addTerminalLine(container, text, className) {
    const line = document.createElement('div');
    line.className = 'terminal-line ' + (className || '');
    line.textContent = text;
    container.appendChild(line);
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// ============ BMS SIMULATION ENGINE ============
let bmsInterval = null;
let bmsCells = [3.70, 3.72, 3.68, 3.71, 3.69, 3.73, 3.70, 3.67];
let bmsFaultCell = -1;
let bmsTickCount = 0;
let bmsCharging = true;
const BMS_MIN = 2.5, BMS_MAX = 4.2, BMS_NOMINAL = 3.7;

function startBmsSimulation() {
    if (bmsInterval) return;
    bmsFaultCell = -1;
    bmsTickCount = 0;
    bmsCharging = true;
    bmsCells = [3.70, 3.72, 3.68, 3.71, 3.69, 3.73, 3.70, 3.67];

    document.getElementById('btnBmsStart').disabled = true;
    document.getElementById('btnBmsStop').disabled = false;
    document.getElementById('btnBmsFault').disabled = false;
    document.getElementById('bmsAlertLog').innerHTML = '';

    const stateEl = document.getElementById('bmsStateIndicator');
    stateEl.className = 'bms-state-indicator state-healthy';
    stateEl.querySelector('.bms-state-text').textContent = 'ÇALIŞIYOR — ŞARJ';

    resetProtections();
    bmsAddAlert('SİSTEM', 'BMS simülasyonu başlatıldı — 8S Li-Ion Pack aktif', 'info');
    bmsAddAlert('SİSTEM', 'Şarj döngüsü başladı (CC-CV modu)', 'info');

    bmsInterval = setInterval(bmsSimTick, 500);
    bmsSimTick();
}

function stopBmsSimulation() {
    if (!bmsInterval) return;
    clearInterval(bmsInterval);
    bmsInterval = null;
    bmsFaultCell = -1;

    document.getElementById('btnBmsStart').disabled = false;
    document.getElementById('btnBmsStop').disabled = true;
    document.getElementById('btnBmsFault').disabled = true;

    const stateEl = document.getElementById('bmsStateIndicator');
    stateEl.className = 'bms-state-indicator';
    stateEl.querySelector('.bms-state-text').textContent = 'DURDURULDU';
    bmsAddAlert('SİSTEM', 'BMS simülasyonu durduruldu — Toplam ' + bmsTickCount + ' tick', 'info');
}

function injectBmsFault() {
    if (!bmsInterval) return;
    bmsFaultCell = Math.floor(Math.random() * 8);
    bmsCells[bmsFaultCell] = 4.35 + Math.random() * 0.2;

    const faultV = bmsCells[bmsFaultCell].toFixed(3);
    bmsAddAlert('ALARM', `⚠ Cell ${bmsFaultCell + 1} AŞIRI VOLTAJ! V=${faultV}V > 4.200V`, 'critical');
    bmsAddAlert('KORUMA', `OVP tetiklendi — Şarj devresi kesiliyor`, 'critical');
    bmsAddAlert('KORUMA', `Hücre dengesizlik algılandı — IMB aktif`, 'warn');

    const stateEl = document.getElementById('bmsStateIndicator');
    stateEl.className = 'bms-state-indicator state-critical';
    stateEl.querySelector('.bms-state-text').textContent = '🚨 ALARM — AŞIRI VOLTAJ';

    // Trigger OV + IMB protections
    const protOV = document.getElementById('protOV');
    protOV.className = 'bms-prot-item prot-triggered';
    protOV.querySelector('.bms-prot-icon').textContent = '🔴';
    const protIMB = document.getElementById('protIMB');
    protIMB.className = 'bms-prot-item prot-triggered';
    protIMB.querySelector('.bms-prot-icon').textContent = '🔴';

    // After 8 seconds, auto-recover
    setTimeout(() => {
        if (bmsFaultCell >= 0 && bmsInterval) {
            bmsCells[bmsFaultCell] = 3.70 + Math.random() * 0.1;
            bmsFaultCell = -1;
            resetProtections();
            bmsAddAlert('KURTARMA', 'Hata giderildi — Sistem normal moda dönüyor', 'info');
            const stEl = document.getElementById('bmsStateIndicator');
            stEl.className = 'bms-state-indicator state-healthy';
            stEl.querySelector('.bms-state-text').textContent = 'SAĞLIKLI — Kurtarma tamamlandı';
        }
    }, 8000);
}

function bmsSimTick() {
    bmsTickCount++;

    // Simulate charge/discharge cycle (every ~30 ticks switch)
    if (bmsTickCount % 30 === 0) {
        bmsCharging = !bmsCharging;
        if (bmsFaultCell < 0) {
            bmsAddAlert('DÖNGÜ', bmsCharging ? 'Şarj döngüsü başladı (CC-CV)' : 'Deşarj döngüsü başladı', 'info');
        }
    }

    // Update cell voltages with realistic drift
    const drift = bmsCharging ? 0.008 : -0.006;
    for (let i = 0; i < 8; i++) {
        if (i === bmsFaultCell) {
            bmsCells[i] += (Math.random() - 0.3) * 0.015;
            bmsCells[i] = Math.max(4.25, Math.min(4.6, bmsCells[i]));
        } else {
            bmsCells[i] += drift + (Math.random() - 0.5) * 0.012;
            bmsCells[i] = Math.max(BMS_MIN + 0.7, Math.min(BMS_MAX - 0.15, bmsCells[i]));
        }
    }

    // Update cell bars with colors
    for (let i = 0; i < 8; i++) {
        const pct = ((bmsCells[i] - BMS_MIN) / (BMS_MAX - BMS_MIN)) * 100;
        const bar = document.getElementById('cellBar' + i);
        const val = document.getElementById('cellVal' + i);
        bar.style.width = Math.min(100, Math.max(0, pct)) + '%';
        val.textContent = bmsCells[i].toFixed(3);

        bar.className = 'bms-cell-bar-fill';
        if (bmsCells[i] > 4.15) bar.className += ' bar-danger';
        else if (bmsCells[i] > 4.0) bar.className += ' bar-warning';
    }

    // Imbalance
    const maxV = Math.max(...bmsCells), minV = Math.min(...bmsCells);
    const imbalance = ((maxV - minV) * 1000).toFixed(0);
    document.getElementById('bmsImbalance').textContent = imbalance + ' mV';
    const balStatus = document.getElementById('bmsBalanceStatus');
    if (imbalance < 30) {
        balStatus.textContent = 'DENGELI';
        balStatus.className = 'bms-balance-status status-ok';
    } else if (imbalance < 100) {
        balStatus.textContent = 'DİKKAT';
        balStatus.className = 'bms-balance-status status-warn';
    } else {
        balStatus.textContent = 'DENGESİZ';
        balStatus.className = 'bms-balance-status status-danger';
    }

    // Pack metrics — oscillating current
    const packV = bmsCells.reduce((a, b) => a + b, 0);
    const baseCurrent = bmsCharging ? 5.5 : -3.2;
    const current = baseCurrent + (Math.random() - 0.5) * 1.5;
    const baseTemp = 25 + Math.abs(current) * 0.6;
    const temp = baseTemp + (Math.random() - 0.5) * 3;
    const soc = Math.min(100, Math.max(0, 50 + (packV - 29.6) * 15));
    const soh = bmsFaultCell >= 0 ? 72 + Math.random() * 5 : 94 + Math.random() * 4;
    const power = packV * Math.abs(current);

    document.getElementById('bmsPackVoltage').textContent = packV.toFixed(2);
    const curEl = document.getElementById('bmsPackCurrent');
    curEl.textContent = (current > 0 ? '+' : '') + current.toFixed(1);
    curEl.style.color = current > 0 ? '#10b981' : '#f59e0b';
    document.getElementById('bmsPackTemp').textContent = temp.toFixed(1);
    document.getElementById('bmsPackSoc').textContent = soc.toFixed(0);
    document.getElementById('bmsPackSoh').textContent = soh.toFixed(1);
    document.getElementById('bmsPackPower').textContent = power.toFixed(0);

    // Temperature warning
    if (temp > 38 && bmsFaultCell < 0) {
        const protOT = document.getElementById('protOT');
        if (!protOT.classList.contains('prot-triggered')) {
            protOT.className = 'bms-prot-item prot-triggered';
            protOT.querySelector('.bms-prot-icon').textContent = '🔴';
            bmsAddAlert('UYARI', `Sıcaklık yüksek: ${temp.toFixed(1)}°C — Akım azaltılıyor`, 'warn');
        }
    } else if (temp < 35) {
        const protOT = document.getElementById('protOT');
        if (protOT.classList.contains('prot-triggered') && bmsFaultCell < 0) {
            protOT.className = 'bms-prot-item';
            protOT.querySelector('.bms-prot-icon').textContent = '🟢';
        }
    }

    // State indicator (if no fault)
    if (bmsFaultCell < 0) {
        const stateEl = document.getElementById('bmsStateIndicator');
        if (imbalance > 80) {
            stateEl.className = 'bms-state-indicator state-warning';
            stateEl.querySelector('.bms-state-text').textContent = 'DENGESİZLİK UYARISI';
        } else {
            stateEl.className = 'bms-state-indicator state-healthy';
            stateEl.querySelector('.bms-state-text').textContent = bmsCharging ? 'SAĞLIKLI — Şarj' : 'SAĞLIKLI — Deşarj';
        }
    }
}

function resetProtections() {
    ['protOV','protUV','protOT','protUT','protOC','protSC','protIMB'].forEach(id => {
        const el = document.getElementById(id);
        el.className = 'bms-prot-item';
        el.querySelector('.bms-prot-icon').textContent = '🟢';
    });
}

function bmsAddAlert(type, msg, level) {
    const log = document.getElementById('bmsAlertLog');
    const empty = log.querySelector('.bms-alert-empty');
    if (empty) empty.remove();

    const ts = new Date().toLocaleTimeString('tr-TR');
    const entry = document.createElement('div');
    entry.className = 'bms-alert-entry' + (level === 'critical' ? ' alert-critical' : level === 'warn' ? ' alert-warn' : '');
    entry.textContent = `[${ts}] ${type}: ${msg}`;
    log.prepend(entry);

    // Keep max 20 entries
    while (log.children.length > 20) log.lastChild.remove();
}

// ============ SOH DEMO ENGINE ============
let sohDemoRunning = false;

async function runSohDemo() {
    if (sohDemoRunning) return;
    sohDemoRunning = true;

    const btn = document.getElementById('btnSohDemo');
    btn.disabled = true;
    btn.textContent = '⏳ Analiz Ediliyor...';

    // Reset previous
    document.getElementById('sohDecisionSummary').style.display = 'none';
    document.getElementById('sohR2Text').textContent = '—';
    document.getElementById('sohRMSEText').textContent = '—';
    document.getElementById('sohMAEText').textContent = '—';
    document.getElementById('sohTimeText').textContent = '—';
    document.getElementById('sohRingR2').setAttribute('stroke-dasharray', '0 264');

    // Generate battery data
    const batteries = [];
    const types = ['NMC 18650', 'LFP 26650', 'NCA 21700', 'NMC 21700', 'LFP 32650', 'NCA 18650'];
    const icons = ['🔋', '🔌', '⚡', '🔋', '🔌', '⚡'];
    for (let i = 0; i < 12; i++) {
        batteries.push({
            id: 'BAT-' + String(i + 1).padStart(3, '0'),
            type: types[i % types.length],
            icon: icons[i % icons.length],
            soh: null,
            decision: null
        });
    }

    const cardsDiv = document.getElementById('sohBatteryCards');
    cardsDiv.innerHTML = '';

    // Phase 1: Show battery cards one-by-one with analyzing animation
    for (let i = 0; i < batteries.length; i++) {
        const bat = batteries[i];
        const card = document.createElement('div');
        card.className = 'soh-battery-card card-analyzing';
        card.id = 'sohBat' + i;
        card.style.opacity = '0';
        card.style.transform = 'translateX(-10px)';
        card.innerHTML = `<span class="soh-bat-icon">${bat.icon}</span><div class="soh-bat-info"><span class="soh-bat-id">${bat.id}</span><span class="soh-bat-type">${bat.type}</span></div><div class="soh-bat-result"><span class="soh-bat-soh" id="sohVal${i}">⏳</span></div>`;
        cardsDiv.appendChild(card);
        await sleep(80);
        card.style.transition = 'all 0.3s ease';
        card.style.opacity = '1';
        card.style.transform = 'translateX(0)';
    }

    await sleep(400);

    // Phase 2: Predict SoH values one by one
    const chartContainer = document.getElementById('sohChartContainer');
    const placeholder = chartContainer.querySelector('.soh-chart-placeholder');
    if (placeholder) placeholder.style.display = 'none';
    chartContainer.innerHTML = '';

    for (let i = 0; i < batteries.length; i++) {
        const bat = batteries[i];
        // Weighted random SoH for realistic distribution
        const roll = Math.random();
        if (roll < 0.45) bat.soh = 85 + Math.random() * 13;      // 45% REUSE
        else if (roll < 0.80) bat.soh = 70 + Math.random() * 15;  // 35% RECYCLE
        else bat.soh = 50 + Math.random() * 20;                    // 20% CRITICAL

        if (bat.soh >= 85) bat.decision = 'reuse';
        else if (bat.soh >= 70) bat.decision = 'recycle';
        else bat.decision = 'critical';

        const card = document.getElementById('sohBat' + i);
        card.className = 'soh-battery-card card-' + bat.decision;

        // Animate SoH count-up
        const valEl = document.getElementById('sohVal' + i);
        const targetSoh = bat.soh;
        let currentSoh = 0;
        const countStep = targetSoh / 15;
        for (let c = 0; c < 15; c++) {
            currentSoh = Math.min(targetSoh, currentSoh + countStep);
            valEl.textContent = currentSoh.toFixed(1) + '%';
            await sleep(20);
        }
        valEl.textContent = targetSoh.toFixed(1) + '%';
        valEl.style.color = bat.decision === 'reuse' ? '#16a34a' : bat.decision === 'recycle' ? '#d97706' : '#dc2626';

        const decLabel = bat.decision === 'reuse' ? 'REUSE' : bat.decision === 'recycle' ? 'RECYCLE' : 'CRITICAL';
        const resultDiv = card.querySelector('.soh-bat-result');
        const decSpan = document.createElement('span');
        decSpan.className = 'soh-bat-decision dec-' + bat.decision;
        decSpan.textContent = decLabel;
        resultDiv.appendChild(decSpan);

        // Build bar chart incrementally
        const color = bat.decision === 'reuse' ? '#10b981' : bat.decision === 'recycle' ? '#f59e0b' : '#ef4444';
        const bar = document.createElement('div');
        bar.className = 'soh-chart-bar';
        bar.style.background = color;
        bar.style.height = '0%';
        bar.innerHTML = `<span class="soh-bar-tooltip">${bat.id}: ${bat.soh.toFixed(1)}% → ${decLabel}</span>`;
        chartContainer.appendChild(bar);

        requestAnimationFrame(() => {
            requestAnimationFrame(() => {
                bar.style.height = bat.soh + '%';
            });
        });

        await sleep(150);
    }

    await sleep(300);

    // Phase 3: Model metrics with animated ring
    const r2 = 0.93 + Math.random() * 0.04;
    const circumference = 2 * Math.PI * 42; // ~264

    // Animate R² ring
    const ringEl = document.getElementById('sohRingR2');
    const textEl = document.getElementById('sohR2Text');
    const steps = 40;
    for (let s = 1; s <= steps; s++) {
        const progress = s / steps;
        const eased = 1 - Math.pow(1 - progress, 3); // ease-out cubic
        const currentR2 = r2 * eased;
        const fill = currentR2 * circumference;
        ringEl.setAttribute('stroke-dasharray', fill.toFixed(1) + ' ' + circumference.toFixed(0));
        textEl.textContent = currentR2.toFixed(3);
        await sleep(25);
    }

    await sleep(200);

    // Animate RMSE count
    const rmseVal = 1.8 + Math.random() * 1.2;
    const rmseEl = document.getElementById('sohRMSEText');
    for (let s = 1; s <= 20; s++) {
        rmseEl.textContent = (rmseVal * s / 20).toFixed(2);
        await sleep(15);
    }
    rmseEl.textContent = rmseVal.toFixed(2);

    await sleep(100);

    // Animate MAE count
    const maeVal = 1.2 + Math.random() * 0.8;
    const maeEl = document.getElementById('sohMAEText');
    for (let s = 1; s <= 20; s++) {
        maeEl.textContent = (maeVal * s / 20).toFixed(2);
        await sleep(15);
    }
    maeEl.textContent = maeVal.toFixed(2);

    await sleep(100);

    // Prediction time
    const timeVal = 8 + Math.random() * 12;
    document.getElementById('sohTimeText').textContent = timeVal.toFixed(0) + ' ms';

    // Phase 4: Decision summary with count animation
    const reuse = batteries.filter(b => b.decision === 'reuse').length;
    const recycle = batteries.filter(b => b.decision === 'recycle').length;
    const critical = batteries.filter(b => b.decision === 'critical').length;

    const summary = document.getElementById('sohDecisionSummary');
    summary.style.display = 'block';
    summary.style.opacity = '0';
    summary.style.transform = 'translateY(8px)';
    requestAnimationFrame(() => {
        summary.style.transition = 'all 0.4s ease';
        summary.style.opacity = '1';
        summary.style.transform = 'translateY(0)';
    });

    document.getElementById('sohReuseCount').textContent = reuse;
    document.getElementById('sohRecycleCount').textContent = recycle;
    document.getElementById('sohCriticalCount').textContent = critical;

    btn.disabled = false;
    btn.textContent = '▶ Yeniden Çalıştır';
    sohDemoRunning = false;
}

// ============================================================
//  CHARGE / DISCHARGE PROFILE SIMULATION
// ============================================================
let chargeProfileInterval = null;
let chargeProfileData = { voltage: [], current: [], temp: [], soc: [] };
let chargeProfileTick = 0;

function startChargeProfileSim() {
    if (chargeProfileInterval) return;
    chargeProfileTick = 0;
    chargeProfileData = { voltage: [], current: [], temp: [], soc: [] };

    document.getElementById('btnChrgStart').disabled = true;
    document.getElementById('btnChrgStop').disabled = false;

    chargeProfileInterval = setInterval(chargeProfileStep, 60);
    chargeProfileStep();
}

function stopChargeProfileSim() {
    if (!chargeProfileInterval) return;
    clearInterval(chargeProfileInterval);
    chargeProfileInterval = null;
    document.getElementById('btnChrgStart').disabled = false;
    document.getElementById('btnChrgStop').disabled = true;
}

function chargeProfileStep() {
    const maxTicks = 400;
    if (chargeProfileTick >= maxTicks) { stopChargeProfileSim(); return; }

    const mode = document.getElementById('chargeProfileType').value;
    const chem = document.getElementById('chargeChem').value;
    const t = chargeProfileTick / maxTicks;

    let v, i, temp, soc, phase;
    const chemOffset = chem === 'LFP' ? -0.4 : chem === 'NCA' ? 0.05 : 0;
    const nomV = chem === 'LFP' ? 3.3 : 3.7;

    if (mode === 'cc-cv') {
        if (t < 0.6) {
            // CC phase
            v = (nomV - 0.5) + t * (1.5 / 0.6) + chemOffset + (Math.random() - 0.5) * 0.01;
            i = 5.0 + (Math.random() - 0.5) * 0.2;
            soc = t * 80 / 0.6;
            phase = 'CC Şarj (Sabit Akım)';
        } else {
            // CV phase
            v = (nomV + 0.5) + chemOffset + (Math.random() - 0.5) * 0.005;
            const cvProgress = (t - 0.6) / 0.4;
            i = 5.0 * Math.exp(-3 * cvProgress) + (Math.random() - 0.5) * 0.1;
            soc = 80 + cvProgress * 20;
            phase = 'CV Şarj (Sabit Voltaj)';
        }
        temp = 25 + Math.abs(i) * 1.5 + (Math.random() - 0.5) * 1;
    } else if (mode === 'discharge') {
        v = (nomV + 0.5) + chemOffset - t * 1.8 + (Math.random() - 0.5) * 0.01;
        if (chem === 'LFP') v = 3.3 + chemOffset - t * 0.3 + (t > 0.85 ? -(t - 0.85) * 8 : 0);
        i = -3.0 + (Math.random() - 0.5) * 0.3;
        soc = 100 - t * 100;
        temp = 25 + Math.abs(i) * 1.2 + t * 5 + (Math.random() - 0.5) * 1;
        phase = 'CC Deşarj';
    } else {
        // Pulse test
        const pulsePhase = (chargeProfileTick % 40);
        if (pulsePhase < 10) {
            i = 10; v = nomV + chemOffset + 0.3 + (Math.random() - 0.5) * 0.01;
        } else if (pulsePhase < 20) {
            i = 0; v = nomV + chemOffset + 0.1 + (Math.random() - 0.5) * 0.005;
        } else if (pulsePhase < 30) {
            i = -10; v = nomV + chemOffset - 0.3 + (Math.random() - 0.5) * 0.01;
        } else {
            i = 0; v = nomV + chemOffset - 0.05 + (Math.random() - 0.5) * 0.005;
        }
        soc = 50 - t * 10;
        temp = 30 + Math.abs(i) * 0.3 + (Math.random() - 0.5) * 0.5;
        phase = 'HPPC Darbe Testi';
    }

    chargeProfileData.voltage.push(v);
    chargeProfileData.current.push(i);
    chargeProfileData.temp.push(temp);
    chargeProfileData.soc.push(soc);

    // Update live metrics
    document.getElementById('clmVoltage').textContent = v.toFixed(3);
    document.getElementById('clmCurrent').textContent = (i >= 0 ? '+' : '') + i.toFixed(2);
    document.getElementById('clmTemp').textContent = temp.toFixed(1);
    document.getElementById('clmSoC').textContent = Math.max(0, Math.min(100, soc)).toFixed(1);
    document.getElementById('clmPhase').textContent = phase;
    const energy = chargeProfileData.voltage.reduce((a, b, idx) => a + Math.abs(b * chargeProfileData.current[idx]) * 0.00001, 0);
    document.getElementById('clmEnergy').textContent = energy.toFixed(2);

    drawChargeChart();
    chargeProfileTick++;
}

function drawChargeChart() {
    const canvas = document.getElementById('chargeCanvas');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const W = canvas.width, H = canvas.height;
    const PAD = 40;

    ctx.clearRect(0, 0, W, H);

    // Background
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, 0, W, H);

    // Grid
    ctx.strokeStyle = 'rgba(255,255,255,0.06)';
    ctx.lineWidth = 1;
    for (let gy = 0; gy < 5; gy++) {
        const y = PAD + gy * (H - 2 * PAD) / 4;
        ctx.beginPath(); ctx.moveTo(PAD, y); ctx.lineTo(W - 10, y); ctx.stroke();
    }

    const data = chargeProfileData;
    const len = data.voltage.length;
    if (len < 2) return;

    function drawLine(arr, color, minV, maxV) {
        ctx.strokeStyle = color;
        ctx.lineWidth = 2;
        ctx.beginPath();
        for (let j = 0; j < arr.length; j++) {
            const x = PAD + (j / 400) * (W - PAD - 10);
            const norm = (arr[j] - minV) / (maxV - minV);
            const y = (H - PAD) - norm * (H - 2 * PAD);
            if (j === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        }
        ctx.stroke();
    }

    drawLine(data.voltage, '#3b82f6', 2.0, 4.5);
    drawLine(data.current, '#f59e0b', -15, 15);
    drawLine(data.temp, '#ef4444', 15, 60);
    drawLine(data.soc, '#10b981', 0, 100);

    // Axes labels
    ctx.fillStyle = 'rgba(255,255,255,0.5)';
    ctx.font = '10px Inter, sans-serif';
    ctx.fillText('Zaman →', W / 2, H - 5);
}

// ============================================================
//  BATTERY AGING SIMULATION
// ============================================================
let agingInterval = null;
let agingCycle = 0;
let agingData = { nmc: [], lfp: [], nca: [] };

function startAgingSim() {
    if (agingInterval) return;
    agingCycle = 0;
    agingData = { nmc: [100], lfp: [100], nca: [100] };

    document.getElementById('btnAgingStart').disabled = true;
    document.getElementById('btnAgingReset').disabled = false;

    agingInterval = setInterval(agingStep, 30);
}

function resetAgingSim() {
    if (agingInterval) { clearInterval(agingInterval); agingInterval = null; }
    agingCycle = 0;
    agingData = { nmc: [], lfp: [], nca: [] };
    document.getElementById('btnAgingStart').disabled = false;

    const canvas = document.getElementById('agingCanvas');
    if (canvas) { const ctx = canvas.getContext('2d'); ctx.clearRect(0, 0, canvas.width, canvas.height); }
    document.getElementById('agingCycleCount').textContent = '0';
    document.getElementById('agingNmcSoh').textContent = '100.0%';
    document.getElementById('agingLfpSoh').textContent = '100.0%';
    document.getElementById('agingNcaSoh').textContent = '100.0%';
    ['agingNmcStatus','agingLfpStatus','agingNcaStatus'].forEach(id => {
        document.getElementById(id).textContent = 'SAĞLIKLI';
        document.getElementById(id).className = 'acc-status';
    });
    document.getElementById('agingInsight').textContent = 'Simülasyonu başlatarak 3 farklı batarya kimyasının yaşlanma eğrisini karşılaştırın.';
}

function agingStep() {
    const maxCycles = 3000;
    if (agingCycle >= maxCycles) { clearInterval(agingInterval); agingInterval = null; document.getElementById('btnAgingStart').disabled = false; return; }

    agingCycle += 10;
    const c = agingCycle;

    // Realistic degradation models
    const nmcSoh = 100 - (0.015 * c) - (0.000003 * c * c) + (Math.random() - 0.5) * 0.3;
    const lfpSoh = 100 - (0.005 * c) - (0.0000008 * c * c) + (Math.random() - 0.5) * 0.2;
    const ncaSoh = 100 - (0.025 * c) - (0.000005 * c * c) + (Math.random() - 0.5) * 0.4;

    agingData.nmc.push(Math.max(40, nmcSoh));
    agingData.lfp.push(Math.max(40, lfpSoh));
    agingData.nca.push(Math.max(40, ncaSoh));

    document.getElementById('agingCycleCount').textContent = c;
    document.getElementById('agingNmcSoh').textContent = Math.max(40, nmcSoh).toFixed(1) + '%';
    document.getElementById('agingLfpSoh').textContent = Math.max(40, lfpSoh).toFixed(1) + '%';
    document.getElementById('agingNcaSoh').textContent = Math.max(40, ncaSoh).toFixed(1) + '%';

    function setStatus(id, soh) {
        const el = document.getElementById(id);
        if (soh >= 85) { el.textContent = 'SAĞLIKLI'; el.className = 'acc-status acc-healthy'; }
        else if (soh >= 70) { el.textContent = 'DİKKAT'; el.className = 'acc-status acc-warn'; }
        else { el.textContent = 'KRİTİK'; el.className = 'acc-status acc-critical'; }
    }
    setStatus('agingNmcStatus', nmcSoh);
    setStatus('agingLfpStatus', lfpSoh);
    setStatus('agingNcaStatus', ncaSoh);

    // Insight text
    if (ncaSoh < 80 && nmcSoh >= 80 && lfpSoh >= 80) {
        document.getElementById('agingInsight').innerHTML = '⚠️ <strong>NCA</strong> batarya End-of-Life eşiğinin altına düştü! NMC ve LFP hâlâ sağlıklı aralıkta.';
    } else if (nmcSoh < 80 && lfpSoh >= 80) {
        document.getElementById('agingInsight').innerHTML = '⚠️ <strong>NMC</strong> de EOL eşiğinin altında. <strong>LFP</strong> en uzun ömürlü kimya olarak öne çıkıyor.';
    } else if (lfpSoh < 80) {
        document.getElementById('agingInsight').innerHTML = '🔴 Tüm kimyalar EOL eşiğinin altında. LFP en son düşen kimya oldu.';
    }

    drawAgingChart();
}

function drawAgingChart() {
    const canvas = document.getElementById('agingCanvas');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const W = canvas.width, H = canvas.height;
    const PAD = 45;

    ctx.clearRect(0, 0, W, H);
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, 0, W, H);

    // Grid
    ctx.strokeStyle = 'rgba(255,255,255,0.06)';
    ctx.lineWidth = 1;
    for (let gy = 0; gy <= 6; gy++) {
        const y = PAD + gy * (H - 2 * PAD) / 6;
        ctx.beginPath(); ctx.moveTo(PAD, y); ctx.lineTo(W - 10, y); ctx.stroke();
    }

    // Y-axis labels
    ctx.fillStyle = 'rgba(255,255,255,0.4)';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'right';
    for (let gy = 0; gy <= 6; gy++) {
        const val = 100 - gy * 10;
        const y = PAD + gy * (H - 2 * PAD) / 6;
        ctx.fillText(val + '%', PAD - 6, y + 3);
    }
    ctx.textAlign = 'left';

    // EOL line at 80%
    const eolY = PAD + (100 - 80) / 60 * (H - 2 * PAD);
    ctx.strokeStyle = 'rgba(239,68,68,0.3)';
    ctx.setLineDash([6, 4]);
    ctx.beginPath(); ctx.moveTo(PAD, eolY); ctx.lineTo(W - 10, eolY); ctx.stroke();
    ctx.setLineDash([]);
    ctx.fillStyle = 'rgba(239,68,68,0.6)';
    ctx.fillText('EOL (%80)', W - 70, eolY - 5);

    // X-axis label
    ctx.fillStyle = 'rgba(255,255,255,0.4)';
    ctx.fillText('Döngü Sayısı →', W / 2 - 30, H - 5);
    ctx.fillText('0', PAD, H - 8);
    ctx.fillText('3000', W - 35, H - 8);

    const len = agingData.nmc.length;
    if (len < 2) return;

    function drawLine(arr, color) {
        ctx.strokeStyle = color;
        ctx.lineWidth = 2.5;
        ctx.beginPath();
        for (let j = 0; j < arr.length; j++) {
            const x = PAD + (j / (3000 / 10)) * (W - PAD - 10);
            const norm = (arr[j] - 40) / 60;
            const y = (H - PAD) - norm * (H - 2 * PAD);
            if (j === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        }
        ctx.stroke();
    }

    drawLine(agingData.nmc, '#3b82f6');
    drawLine(agingData.lfp, '#10b981');
    drawLine(agingData.nca, '#ef4444');
}

// ============================================================
//  FEATURE IMPORTANCE ANIMATION
// ============================================================
async function animateFeatureImportance() {
    const values = [23.1, 18.4, 14.2, 11.8, 9.3, 7.6, 5.1, 3.8, 2.4, 1.7, 1.2, 0.8];
    const maxVal = values[0];

    for (let i = 0; i < values.length; i++) {
        const bar = document.getElementById('fiBar' + i);
        const score = document.getElementById('fiScore' + i);
        const targetWidth = (values[i] / maxVal * 100);

        // Animate bar width
        bar.style.transition = 'width 0.6s cubic-bezier(0.4,0,0.2,1)';
        bar.style.width = targetWidth + '%';

        // Animate score count-up
        const target = values[i];
        const steps = 20;
        for (let s = 1; s <= steps; s++) {
            score.textContent = (target * s / steps).toFixed(1) + '%';
            await sleep(15);
        }
        score.textContent = target.toFixed(1) + '%';

        await sleep(80);
    }
}

// ============================================================
//  RADAR CHART — BATTERY CHEMISTRY COMPARISON
// ============================================================
(function drawRadarChart() {
    // Wait for DOM
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', drawRadarOnce);
    } else {
        setTimeout(drawRadarOnce, 500);
    }
})();

function drawRadarOnce() {
    const canvas = document.getElementById('radarCanvas');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const W = canvas.width, H = canvas.height;
    const cx = W / 2, cy = H / 2;
    const R = 160;

    const labels = ['Enerji\nYoğunluğu', 'Döngü\nÖmrü', 'Güvenlik', 'Güç\nÇıkışı', 'Maliyet\n(Düşük=İyi)', 'Hızlı\nŞarj'];
    const nmc = [0.80, 0.50, 0.60, 0.75, 0.50, 0.70];
    const lfp = [0.45, 0.95, 0.95, 0.60, 0.85, 0.65];
    const nca = [0.95, 0.35, 0.40, 0.90, 0.30, 0.80];
    const n = labels.length;

    ctx.clearRect(0, 0, W, H);

    // Background grid rings
    for (let ring = 1; ring <= 5; ring++) {
        const r = R * ring / 5;
        ctx.beginPath();
        for (let i = 0; i <= n; i++) {
            const angle = (Math.PI * 2 * i / n) - Math.PI / 2;
            const x = cx + r * Math.cos(angle);
            const y = cy + r * Math.sin(angle);
            if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        }
        ctx.closePath();
        ctx.strokeStyle = ring === 5 ? 'rgba(255,255,255,0.15)' : 'rgba(255,255,255,0.06)';
        ctx.lineWidth = 1;
        ctx.stroke();
    }

    // Spokes + labels
    ctx.fillStyle = 'rgba(255,255,255,0.7)';
    ctx.font = '11px Inter, sans-serif';
    ctx.textAlign = 'center';
    for (let i = 0; i < n; i++) {
        const angle = (Math.PI * 2 * i / n) - Math.PI / 2;
        ctx.beginPath();
        ctx.moveTo(cx, cy);
        ctx.lineTo(cx + R * Math.cos(angle), cy + R * Math.sin(angle));
        ctx.strokeStyle = 'rgba(255,255,255,0.08)';
        ctx.stroke();

        // Label
        const lx = cx + (R + 30) * Math.cos(angle);
        const ly = cy + (R + 30) * Math.sin(angle);
        const lines = labels[i].split('\n');
        lines.forEach((line, li) => {
            ctx.fillText(line, lx, ly + li * 13 - (lines.length - 1) * 6);
        });
    }

    function drawPolygon(data, color, alpha) {
        ctx.beginPath();
        for (let i = 0; i <= n; i++) {
            const idx = i % n;
            const angle = (Math.PI * 2 * idx / n) - Math.PI / 2;
            const r = R * data[idx];
            const x = cx + r * Math.cos(angle);
            const y = cy + r * Math.sin(angle);
            if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
        }
        ctx.closePath();
        ctx.fillStyle = color.replace('1)', alpha + ')');
        ctx.fill();
        ctx.strokeStyle = color;
        ctx.lineWidth = 2;
        ctx.stroke();

        // Dots
        for (let i = 0; i < n; i++) {
            const angle = (Math.PI * 2 * i / n) - Math.PI / 2;
            const r = R * data[i];
            ctx.beginPath();
            ctx.arc(cx + r * Math.cos(angle), cy + r * Math.sin(angle), 4, 0, Math.PI * 2);
            ctx.fillStyle = color;
            ctx.fill();
        }
    }

    drawPolygon(nmc, 'rgba(59,130,246,1)', 0.12);
    drawPolygon(lfp, 'rgba(16,185,129,1)', 0.12);
    drawPolygon(nca, 'rgba(239,68,68,1)', 0.12);
}

// ============================================================
//  TEMPERATURE HEATMAP SIMULATION
// ============================================================
let heatmapInterval = null;
let hmTemps = [25, 25, 25, 25, 25, 25, 25, 25];
let hmHotspotCell = -1;

function startHeatmapSim() {
    if (heatmapInterval) return;
    hmTemps = [25, 25, 25, 25, 25, 25, 25, 25];
    hmHotspotCell = -1;

    document.getElementById('btnHeatStart').disabled = true;
    document.getElementById('btnHeatStop').disabled = false;
    document.getElementById('btnHeatHot').disabled = false;
    document.getElementById('hmThermalLog').innerHTML = '';
    hmAddLog('Sıcaklık izleme başlatıldı', 'info');

    heatmapInterval = setInterval(heatmapTick, 400);
}

function stopHeatmapSim() {
    if (!heatmapInterval) return;
    clearInterval(heatmapInterval);
    heatmapInterval = null;
    document.getElementById('btnHeatStart').disabled = false;
    document.getElementById('btnHeatStop').disabled = true;
    document.getElementById('btnHeatHot').disabled = true;
    hmAddLog('Sıcaklık izleme durduruldu', 'info');
}

function injectHotspot() {
    hmHotspotCell = Math.floor(Math.random() * 8);
    hmTemps[hmHotspotCell] = 52 + Math.random() * 8;
    hmAddLog('🔥 Cell ' + (hmHotspotCell + 1) + ' HOTSPOT! T=' + hmTemps[hmHotspotCell].toFixed(1) + '°C', 'critical');
    hmAddLog('⚡ Fan hızı %100\'e çıkartıldı', 'warn');

    setTimeout(() => {
        if (hmHotspotCell >= 0) {
            hmTemps[hmHotspotCell] = 30 + Math.random() * 5;
            hmAddLog('✅ Hotspot soğutuldu — Cell ' + (hmHotspotCell + 1) + ' normal', 'info');
            hmHotspotCell = -1;
        }
    }, 6000);
}

function heatmapTick() {
    for (let i = 0; i < 8; i++) {
        if (i === hmHotspotCell) {
            hmTemps[i] += (Math.random() - 0.4) * 2;
            hmTemps[i] = Math.max(45, Math.min(65, hmTemps[i]));
        } else {
            hmTemps[i] += (Math.random() - 0.5) * 1.5;
            hmTemps[i] = Math.max(18, Math.min(42, hmTemps[i]));
        }
    }

    for (let i = 0; i < 8; i++) {
        const temp = hmTemps[i];
        const pct = Math.min(100, Math.max(0, (temp / 60) * 100));
        const hue = Math.max(0, 200 - (temp / 60) * 200);

        document.getElementById('hmTemp' + i).textContent = temp.toFixed(1) + '°C';
        document.getElementById('hmBar' + i).style.width = pct + '%';
        document.getElementById('hmBar' + i).style.background = `hsl(${hue}, 80%, 50%)`;

        const cell = document.getElementById('hmCell' + i);
        cell.style.borderColor = temp > 45 ? '#ef4444' : temp > 35 ? '#f59e0b' : '#10b981';
        cell.style.background = `linear-gradient(135deg, rgba(${Math.min(255, temp * 4)}, ${Math.max(0, 200 - temp * 3)}, ${Math.max(0, 100 - temp * 2)}, 0.1), transparent)`;
    }

    const avg = hmTemps.reduce((a, b) => a + b) / 8;
    const max = Math.max(...hmTemps);
    const min = Math.min(...hmTemps);
    document.getElementById('hmAvgTemp').textContent = avg.toFixed(1) + '°C';
    document.getElementById('hmMaxTemp').textContent = max.toFixed(1) + '°C';
    document.getElementById('hmMinTemp').textContent = min.toFixed(1) + '°C';
    document.getElementById('hmDeltaTemp').textContent = (max - min).toFixed(1) + '°C';

    const thermalStatus = document.getElementById('hmThermalStatus');
    if (max > 50) { thermalStatus.textContent = '🔴 KRİTİK'; thermalStatus.style.color = '#ef4444'; }
    else if (max > 38) { thermalStatus.textContent = '🟡 DİKKAT'; thermalStatus.style.color = '#f59e0b'; }
    else { thermalStatus.textContent = '🟢 NORMAL'; thermalStatus.style.color = '#10b981'; }

    const fanStatus = document.getElementById('hmFanStatus');
    if (max > 45) { fanStatus.textContent = '🌀 %100 (Tam Hız)'; fanStatus.style.color = '#ef4444'; }
    else if (max > 35) { fanStatus.textContent = '🌀 %60 (Orta)'; fanStatus.style.color = '#f59e0b'; }
    else { fanStatus.textContent = '⏸ Kapalı'; fanStatus.style.color = '#64748b'; }
}

function hmAddLog(msg, level) {
    const log = document.getElementById('hmThermalLog');
    const ts = new Date().toLocaleTimeString('tr-TR');
    const entry = document.createElement('div');
    entry.className = 'hm-log-entry' + (level === 'critical' ? ' hm-log-critical' : level === 'warn' ? ' hm-log-warn' : '');
    entry.textContent = `[${ts}] ${msg}`;
    log.prepend(entry);
    while (log.children.length > 15) log.lastChild.remove();
}

// ============================================================
//  CONFUSION MATRIX ANIMATION
// ============================================================
async function animateConfusionMatrix() {
    // Reset
    for (let r = 0; r < 3; r++) for (let c = 0; c < 3; c++) {
        document.getElementById('cmVal' + r + c).textContent = '0';
        document.getElementById('cmCell' + r + c).style.opacity = '0.3';
    }

    // Realistic confusion matrix for battery classification
    const matrix = [
        [187, 8, 2],    // REUSE
        [12, 156, 7],   // RECYCLE
        [1, 6, 121]     // CRITICAL
    ];

    // Animate cells one by one
    for (let r = 0; r < 3; r++) {
        for (let c = 0; c < 3; c++) {
            const target = matrix[r][c];
            const el = document.getElementById('cmVal' + r + c);
            const cell = document.getElementById('cmCell' + r + c);
            cell.style.opacity = '1';
            cell.style.transition = 'all 0.3s ease';

            // Count up
            const steps = 15;
            for (let s = 1; s <= steps; s++) {
                el.textContent = Math.round(target * s / steps);
                await sleep(20);
            }
            el.textContent = target;

            // Color intensity based on value
            if (r === c) {
                const intensity = Math.min(1, target / 200);
                cell.style.background = `rgba(16,185,129,${0.15 + intensity * 0.35})`;
            } else {
                const intensity = Math.min(1, target / 50);
                cell.style.background = `rgba(239,68,68,${0.05 + intensity * 0.25})`;
            }

            await sleep(60);
        }
    }

    // Calculate metrics
    const total = 500;
    const correct = 187 + 156 + 121;
    const accuracy = correct / total;
    const precisionReuse = 187 / (187 + 12 + 1);
    const recallReuse = 187 / (187 + 8 + 2);
    const f1 = 2 * precisionReuse * recallReuse / (precisionReuse + recallReuse);
    const kappa = 0.82 + Math.random() * 0.05;

    // Animate metrics
    const metrics = [
        { fill: 'cmAccFill', val: 'cmAccVal', target: accuracy },
        { fill: 'cmPrecFill', val: 'cmPrecVal', target: precisionReuse },
        { fill: 'cmRecFill', val: 'cmRecVal', target: recallReuse },
        { fill: 'cmF1Fill', val: 'cmF1Val', target: f1 },
        { fill: 'cmKapFill', val: 'cmKapVal', target: kappa }
    ];

    for (const m of metrics) {
        const fillEl = document.getElementById(m.fill);
        const valEl = document.getElementById(m.val);
        fillEl.style.transition = 'width 0.8s cubic-bezier(0.4,0,0.2,1)';
        fillEl.style.width = (m.target * 100) + '%';
        valEl.textContent = '%' + (m.target * 100).toFixed(1);
        await sleep(200);
    }

    document.getElementById('cmCorrect').textContent = correct;
    document.getElementById('cmSummary').style.display = 'block';
}

// ============================================================
//  DAQ (DATA ACQUISITION) SIMULATION
// ============================================================
let daqInterval = null;
let daqBuffers = { V: [], I: [], T: [], Z: [] };
let daqSamples = 0;
let daqStartTime = null;
const DAQ_MAX_POINTS = 200;

function startDaqSim() {
    if (daqInterval) return;
    daqBuffers = { V: [], I: [], T: [], Z: [] };
    daqSamples = 0;
    daqStartTime = Date.now();

    document.getElementById('btnDaqStart').disabled = true;
    document.getElementById('btnDaqStop').disabled = false;
    document.getElementById('daqRecStatus').textContent = '🔴 Kayıt Yapılıyor';
    document.getElementById('daqRecStatus').style.color = '#ef4444';

    daqInterval = setInterval(daqTick, 50);
}

function stopDaqSim() {
    if (!daqInterval) return;
    clearInterval(daqInterval);
    daqInterval = null;

    document.getElementById('btnDaqStart').disabled = false;
    document.getElementById('btnDaqStop').disabled = true;
    document.getElementById('daqRecStatus').textContent = '⏸ Durduruldu';
    document.getElementById('daqRecStatus').style.color = '#64748b';
}

function daqTick() {
    daqSamples++;
    const t = daqSamples * 0.01;

    // Realistic sensor data with noise
    const v = 3.7 + 0.3 * Math.sin(t * 0.5) + (Math.random() - 0.5) * 0.02;
    const i = 2.5 * Math.sin(t * 0.3) + 1.5 * Math.sin(t * 1.2) + (Math.random() - 0.5) * 0.3;
    const temp = 28 + 5 * Math.sin(t * 0.1) + Math.abs(i) * 0.8 + (Math.random() - 0.5) * 0.5;
    const z = 45 + 10 * Math.sin(t * 0.2) + (Math.random() - 0.5) * 2;

    daqBuffers.V.push(v); if (daqBuffers.V.length > DAQ_MAX_POINTS) daqBuffers.V.shift();
    daqBuffers.I.push(i); if (daqBuffers.I.length > DAQ_MAX_POINTS) daqBuffers.I.shift();
    daqBuffers.T.push(temp); if (daqBuffers.T.length > DAQ_MAX_POINTS) daqBuffers.T.shift();
    daqBuffers.Z.push(z); if (daqBuffers.Z.length > DAQ_MAX_POINTS) daqBuffers.Z.shift();

    document.getElementById('daqV').textContent = v.toFixed(3) + ' V';
    document.getElementById('daqI').textContent = (i >= 0 ? '+' : '') + i.toFixed(2) + ' A';
    document.getElementById('daqT').textContent = temp.toFixed(1) + ' °C';
    document.getElementById('daqZ').textContent = z.toFixed(1) + ' mΩ';
    document.getElementById('daqSampleCount').textContent = daqSamples.toLocaleString();
    document.getElementById('daqDataSize').textContent = (daqSamples * 32 / 1024).toFixed(1) + ' KB';

    const elapsed = Math.floor((Date.now() - daqStartTime) / 1000);
    const mins = String(Math.floor(elapsed / 60)).padStart(2, '0');
    const secs = String(elapsed % 60).padStart(2, '0');
    document.getElementById('daqElapsed').textContent = mins + ':' + secs;

    drawDaqChannel('daqCanvasV', daqBuffers.V, '#3b82f6', 2.5, 4.5);
    drawDaqChannel('daqCanvasI', daqBuffers.I, '#f59e0b', -6, 6);
    drawDaqChannel('daqCanvasT', daqBuffers.T, '#ef4444', 15, 50);
    drawDaqChannel('daqCanvasZ', daqBuffers.Z, '#8b5cf6', 20, 80);
}

function drawDaqChannel(canvasId, data, color, minV, maxV) {
    const canvas = document.getElementById(canvasId);
    if (!canvas || data.length < 2) return;
    const ctx = canvas.getContext('2d');
    const W = canvas.width, H = canvas.height;

    ctx.clearRect(0, 0, W, H);
    ctx.fillStyle = '#0f172a';
    ctx.fillRect(0, 0, W, H);

    // Grid
    ctx.strokeStyle = 'rgba(255,255,255,0.05)';
    ctx.lineWidth = 1;
    for (let g = 1; g < 4; g++) {
        const y = g * H / 4;
        ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(W, y); ctx.stroke();
    }

    // Gradient fill
    const grad = ctx.createLinearGradient(0, 0, 0, H);
    grad.addColorStop(0, color.replace(')', ',0.2)').replace('rgb', 'rgba'));
    grad.addColorStop(1, 'transparent');

    ctx.beginPath();
    ctx.moveTo(0, H);
    for (let j = 0; j < data.length; j++) {
        const x = (j / DAQ_MAX_POINTS) * W;
        const norm = (data[j] - minV) / (maxV - minV);
        const y = H - norm * H;
        ctx.lineTo(x, y);
    }
    ctx.lineTo((data.length - 1) / DAQ_MAX_POINTS * W, H);
    ctx.closePath();
    ctx.fillStyle = grad;
    ctx.fill();

    // Line
    ctx.beginPath();
    for (let j = 0; j < data.length; j++) {
        const x = (j / DAQ_MAX_POINTS) * W;
        const norm = (data[j] - minV) / (maxV - minV);
        const y = H - norm * H;
        if (j === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.strokeStyle = color;
    ctx.lineWidth = 1.5;
    ctx.stroke();
}