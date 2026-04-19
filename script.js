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
        { name: 'İbrahim Çağdaş', initials: 'İÇ', photo: 'vesikalar/ibrahimcagdas.jpg', role: 'Makina Mühendisi', edu: 'Lisans', tasks: ['Mekanik sistem kurulum', 'Geri kazanım sistemi', 'Test desteği'] }
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
    { id: '09', title: 'Hakem Ziyareti Durum Raporu', desc: 'Mevcut durum, ilerleme ve TÜBİTAK hakem sunumu', file: 'docs/09_HAKEM_ZIYARETI_DURUM_RAPORU.md', color: '#f97316', tags: ['Hakem', 'Durum', 'TÜBİTAK'] },
    { id: '10', title: 'Hakem Hazırlık Kontrol Listesi', desc: 'Hakem ziyareti öncesi idari, teknik ve içerik hazırlık listesi', file: 'docs/10_HAKEM_HAZIRLIK_KONTROL_LISTESI.md', color: '#0ea5e9', tags: ['Kontrol', 'Hazırlık', 'Checklist'] },
    { id: '10-F', title: 'VGX BMS Firmware Datasheet', desc: 'STM32F030C8 BMS gömülü yazılım teknik referansı', file: 'docs/10_VGX_BMS_FIRMWARE_DATASHEET.md', color: '#64748b', tags: ['Firmware', 'BMS', 'Gömülü'] },
    { id: '11', title: 'Sunum Taslağı', desc: 'TÜBİTAK hakem ziyareti sunum içeriği ve konuşmacı notları', file: 'docs/11_SUNUM_TASLAGI.md', color: '#d946ef', tags: ['Sunum', 'Hakem', 'Slayt'] }
];

function renderDocuments() {
    const grid = document.getElementById('docsGrid');
    if (!grid) return;
    grid.innerHTML = projectDocs.map(doc => `
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
        </div>
    `).join('');
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
            pdf.text(metaItems[i][0].toUpperCase(), mx + 4, my + 5);
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
                    const headers = token.header.map(h => stripInline(h.text || h));
                    const rows = token.rows.map(row => row.map(cell => stripInline(cell.text || cell)));
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
                    pdf.setFont('courier', 'normal');
                    pdf.setTextColor(226, 232, 240);
                    for (const cl of codeLines) {
                        if (y + 4 > PH - MB) break;
                        pdf.text(cl.substring(0, 100), ML + 4, y);
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

// ============ KEYBOARD SHORTCUTS ============
document.addEventListener('keydown', (e) => {
    if (e.altKey && e.key >= '1' && e.key <= '9') {
        const secs = ['overview','progress','roadmap','workpackages','milestones','budget','team','technical','documents'];
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