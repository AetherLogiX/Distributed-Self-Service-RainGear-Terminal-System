#include "MainWindow.h"
#include "SlotItem.h"

#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Welcome);
    setWindowTitle("RainHub Client");
    resize(900, 700);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createWelcomePage());     // 0
    m_stack->addWidget(createIdentityPage());    // 1
    m_stack->addWidget(createLoginPage());       // 2
    m_stack->addWidget(createResetPwdPage());    // 3
    m_stack->addWidget(createDashboardPage());   // 4
    m_stack->addWidget(createBorrowPage());      // 5
    m_stack->addWidget(createMapPage());         // 6
    m_stack->addWidget(createProfilePage());     // 7
    m_stack->addWidget(createInstructionPage()); // 8

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QWidget* MainWindow::createWelcomePage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel(tr("NUIST智能自助雨具系统"), page);
    title->setStyleSheet("font-size:28px; font-weight:700;");
    auto *subtitle = new QLabel(tr("欢迎您的使用"), page);
    subtitle->setStyleSheet("font-size:18px; color:#555;");

    auto *btnStart = new QPushButton(tr("开始使用"), page);
    btnStart->setFixedWidth(180);
    btnStart->setStyleSheet("padding:12px 18px; font-size:16px;");
    connect(btnStart, &QPushButton::clicked, this, [this] {
        switchPage(Page::Identity);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(btnStart, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createIdentityPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    auto *tip = new QLabel(tr("您的身份是？"), page);
    tip->setStyleSheet("font-size:20px; font-weight:600;");

    auto *row = new QHBoxLayout();
    row->setSpacing(20);
    auto *btnStaff = new QPushButton(tr("教职人员"), page);
    auto *btnStudent = new QPushButton(tr("学生"), page);
    btnStaff->setFixedSize(180, 70);
    btnStudent->setFixedSize(180, 70);
    connect(btnStaff, &QPushButton::clicked, this, [this] {
        m_currentRole = Role::Staff;
        updateRoleLabel();
        switchPage(Page::Login);
    });
    connect(btnStudent, &QPushButton::clicked, this, [this] {
        m_currentRole = Role::Student;
        updateRoleLabel();
        switchPage(Page::Login);
    });
    row->addWidget(btnStaff);
    row->addWidget(btnStudent);

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Welcome);
    });

    layout->addWidget(tip, 0, Qt::AlignCenter);
    layout->addLayout(row);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    m_loginRoleLabel = new QLabel(tr("请选择身份"), page);
    m_loginRoleLabel->setStyleSheet("font-size:18px; font-weight:600;");

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(12);
    m_inputUser = new QLineEdit(page);
    m_inputPass = new QLineEdit(page);
    m_inputPass->setEchoMode(QLineEdit::Password);
    form->addRow(tr("账号："), m_inputUser);
    form->addRow(tr("密码："), m_inputPass);

    auto *btnLogin = new QPushButton(tr("登录"), page);
    btnLogin->setFixedWidth(160);
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        // Mock success
        updateProfileMock();
        switchPage(Page::Dashboard);
    });

    auto *btnReset = new QPushButton(tr("修改密码"), page);
    btnReset->setFlat(true);
    connect(btnReset, &QPushButton::clicked, this, [this] {
        switchPage(Page::ResetPwd);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Identity);
    });

    layout->addWidget(m_loginRoleLabel, 0, Qt::AlignCenter);
    layout->addLayout(form);
    layout->addWidget(btnLogin, 0, Qt::AlignCenter);
    layout->addWidget(btnReset, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createResetPwdPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    auto *title = new QLabel(tr("重置密码"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");

    auto *inputOld = new QLineEdit(page);
    inputOld->setPlaceholderText(tr("默认密码"));
    auto *inputNew = new QLineEdit(page);
    inputNew->setPlaceholderText(tr("确认密码"));
    inputNew->setEchoMode(QLineEdit::Password);

    auto *btnSubmit = new QPushButton(tr("提交"), page);
    btnSubmit->setFixedWidth(140);
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        switchPage(Page::Login);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Login);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(inputOld);
    layout->addWidget(inputNew);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(18);

    auto *title = new QLabel(tr("NUIST智能雨具系统"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");

    auto *btnBorrow = new QPushButton(tr("我要借伞"), page);
    auto *btnReturn = new QPushButton(tr("我要还伞"), page);
    btnBorrow->setFixedSize(200, 80);
    btnReturn->setFixedSize(200, 80);
    btnBorrow->setStyleSheet("font-size:18px;");
    btnReturn->setStyleSheet("font-size:18px;");

    connect(btnBorrow, &QPushButton::clicked, this, [this] {
        m_borrowMode = true;
        populateSlots(true);
        switchPage(Page::Borrow);
    });
    connect(btnReturn, &QPushButton::clicked, this, [this] {
        m_borrowMode = false;
        populateSlots(false);
        switchPage(Page::Borrow);
    });

    auto *btnInstruction = new QPushButton(tr("使用说明"), page);
    btnInstruction->setFlat(true);
    connect(btnInstruction, &QPushButton::clicked, this, [this] {
        switchPage(Page::Instruction);
    });

    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(20, 0, 20, 0);
    bottom->setSpacing(20);
    auto *btnProfile = new QPushButton(tr("👤 个人中心"), page);
    btnProfile->setFixedWidth(150);
    auto *btnMap = new QPushButton(tr("🗺️ 查看地图"), page);
    btnMap->setFixedWidth(150);
    connect(btnProfile, &QPushButton::clicked, this, [this] {
        updateProfileMock();
        switchPage(Page::Profile);
    });
    connect(btnMap, &QPushButton::clicked, this, [this] {
        switchPage(Page::Map);
    });
    bottom->addWidget(btnProfile, 0, Qt::AlignLeft);
    bottom->addStretch();
    bottom->addWidget(btnMap, 0, Qt::AlignRight);

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(btnBorrow, 0, Qt::AlignCenter);
    layout->addWidget(btnReturn, 0, Qt::AlignCenter);
    layout->addWidget(btnInstruction, 0, Qt::AlignCenter);
    layout->addLayout(bottom);
    return page;
}

QWidget* MainWindow::createBorrowPage()
{
    auto *page = new QWidget(this);
    auto *outer = new QVBoxLayout(page);
    outer->setContentsMargins(16, 16, 16, 16);
    outer->setSpacing(12);

    auto *topBar = new QHBoxLayout();
    m_slotTitle = new QLabel(tr("借伞模式"), page);
    m_slotTitle->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    topBar->addWidget(m_slotTitle);
    topBar->addStretch();
    topBar->addWidget(btnBack);
    outer->addLayout(topBar);

    auto *grid = new QGridLayout();
    grid->setSpacing(10);

    // Create 12 slot widgets (3x4)
    for (int i = 0; i < 12; ++i) {
        auto *slot = new SlotItem(i, page);
        connect(slot, &SlotItem::clicked, this, [this](int index, SlotItem::State state) {
            // Simple selection toggle
            for (auto *s : m_slots) {
                if (s->state() == SlotItem::State::Selected && s != m_slots[index]) {
                    s->setState(m_borrowMode ? SlotItem::State::Available : SlotItem::State::Empty);
                }
            }
            auto target = m_slots[index];
            if (state == SlotItem::State::Selected) {
                target->setState(m_borrowMode ? SlotItem::State::Available : SlotItem::State::Empty);
            } else {
                target->setState(SlotItem::State::Selected);
            }
        });
        m_slots.push_back(slot);
        grid->addWidget(slot, i / 4, i % 4);
    }

    outer->addLayout(grid);
    return page;
}

QWidget* MainWindow::createMapPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("地图占位（后续接入真实散点）"), page);
    title->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

QWidget* MainWindow::createProfilePage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    m_profileTitle = new QLabel(tr("个人信息"), page);
    m_profileTitle->setStyleSheet("font-size:22px; font-weight:700;");
    m_profileName = new QLabel(page);
    m_profileId = new QLabel(page);
    m_profileBalance = new QLabel(page);
    m_profileName->setStyleSheet("font-size:18px;");
    m_profileId->setStyleSheet("font-size:16px; color:#444;");
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600;");

    auto *btnRefresh = new QPushButton(tr("刷新余额"), page);
    btnRefresh->setFixedWidth(140);
    connect(btnRefresh, &QPushButton::clicked, this, [this] {
        updateProfileMock();
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(m_profileTitle, 0, Qt::AlignLeft);
    layout->addWidget(m_profileName, 0, Qt::AlignLeft);
    layout->addWidget(m_profileId, 0, Qt::AlignLeft);
    layout->addWidget(m_profileBalance, 0, Qt::AlignLeft);
    layout->addSpacing(12);
    layout->addWidget(btnRefresh, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

QWidget* MainWindow::createInstructionPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *title = new QLabel(tr("使用说明 / 服务协议"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");

    m_instructionViewer = new QTextBrowser(page);
    m_instructionViewer->setOpenExternalLinks(false);
    m_instructionViewer->setHtml(
        "<h2 align=\"center\">NUIST 智能雨具系统服务协议</h2>"
        "<hr>"
        "<h3>一、 服务对象</h3>"
        "<p>本系统仅面向 NUIST 在校教职工与学生开放，登录需验证校园一卡通账户。</p>"
        "<h3>二、 借还规则</h3>"
        "<p><b>1. 借出：</b>账户余额需大于 <b>20.00元</b> 方可使用。借出时系统将冻结相应金额作为押金。</p>"
        "<p><b>2. 归还：</b>请将雨具插入任意站点的空闲卡槽，听到“咔哒”上锁声并看到屏幕提示“归还成功”后方可离开。</p>"
        "<h3>三、 资费标准 (自动扣款)</h3>"
        "<ul>"
        "<li><b>普通雨伞/一次性雨衣：</b> 押金 20元。</li>"
        "<li><b>高级抗风伞/加厚雨衣：</b> 押金 50元。</li>"
        "<li><b>免费时长：</b> 借出后 <b>24小时内</b> 归还免费。</li>"
        "<li><b>超时费用：</b> 超过24小时，按 <b>1元/12小时</b> 从余额扣除，直至扣完押金。</li>"
        "</ul>"
        "<h3>四、 遗失与损坏</h3>"
        "<p>若雨具遗失或严重损坏导致无法归还，系统将<b>扣除全额押金</b>用于赔偿。</p>"
        "<h3>五、 联系我们</h3>"
        "<p>如遇设备故障或扣费异常，请点击主页左侧的<b>【联系客服】</b>按钮，或致电校园服务中心：5873-6110。</p>"
    );

    auto *btnBack = new QPushButton(tr("我已阅读"), page);
    btnBack->setFixedWidth(140);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title);
    layout->addWidget(m_instructionViewer, 1);
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

void MainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    if (page == Page::Borrow && m_slotTitle) {
        m_slotTitle->setText(m_borrowMode ? tr("借伞模式") : tr("还伞模式"));
    }
}

void MainWindow::populateSlots(bool borrowMode)
{
    // Mock data: green for available (borrow), gray for empty (return), red for maintenance.
    // In real implementation, fill from server inventory.
    for (int i = 0; i < m_slots.size(); ++i) {
        auto *slot = m_slots[i];
        if (borrowMode) {
            if (i == 2) {
                slot->setState(SlotItem::State::Maintenance);
            } else if (i % 2 == 0) {
                slot->setState(SlotItem::State::Available);
            } else {
                slot->setState(SlotItem::State::Empty);
            }
        } else {
            if (i == 5) {
                slot->setState(SlotItem::State::Maintenance);
            } else if (i % 3 == 0) {
                slot->setState(SlotItem::State::Empty);
            } else {
                slot->setState(SlotItem::State::Available);
            }
        }
    }
}

void MainWindow::updateRoleLabel()
{
    if (!m_loginRoleLabel) return;
    QString roleText = tr("请选择身份");
    if (m_currentRole == Role::Student) roleText = tr("学生登录");
    else if (m_currentRole == Role::Staff) roleText = tr("教职人员登录");
    m_loginRoleLabel->setText(roleText);
}

void MainWindow::updateProfileMock()
{
    if (!m_profileName || !m_profileId || !m_profileBalance || !m_profileTitle) return;
    const bool isStudent = m_currentRole != Role::Staff;
    m_profileTitle->setText(tr("个人信息"));
    m_profileName->setText(tr("姓名：张三"));
    m_profileId->setText(isStudent ? tr("学号：2023xxxx") : tr("工号：T0001"));
    m_profileBalance->setText(tr("账户余额：￥50.00"));
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600; color:#2ecc71;");
}

