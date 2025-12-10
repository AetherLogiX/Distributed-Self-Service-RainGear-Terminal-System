#pragma once

#include <QMainWindow>
#include <QVector>
#include <memory>

#include "control/DatabaseManager.h"
#include "model/User.h"

class QStackedWidget;
class QWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QTextBrowser;
class SlotItem;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    enum class Page {
        Welcome,
        Identity,
        Login,
        ResetPwd,
        Dashboard,
        Borrow,
        Map,
        Profile,
        Instruction
    };

    enum class Role {
        Unknown,
        Student,
        Staff
    };

    void setupUi();
    QWidget* createWelcomePage();
    QWidget* createIdentityPage();
    QWidget* createLoginPage();
    QWidget* createResetPwdPage();
    QWidget* createDashboardPage();
    QWidget* createBorrowPage();
    QWidget* createMapPage();
    QWidget* createProfilePage();
    QWidget* createInstructionPage();
    void switchPage(Page page);
    void populateSlots(bool borrowMode);
    void updateRoleLabel();
    void updateProfileFromUser();
    bool performLogin(); // 会接入sha256密码校验

    QStackedWidget *m_stack { nullptr };
    QVector<SlotItem*> m_slots;
    QLabel *m_slotTitle { nullptr };
    bool m_borrowMode { true };

    // Login/role state (mock)
    Role m_currentRole { Role::Unknown };
    QLabel *m_loginRoleLabel { nullptr };
    QLineEdit *m_inputUser { nullptr };
    QLineEdit *m_inputName { nullptr };
    QLineEdit *m_inputPass { nullptr };
    std::unique_ptr<User> m_currentUser;

    // Profile mock labels
    QLabel *m_profileName { nullptr };
    QLabel *m_profileId { nullptr };
    QLabel *m_profileBalance { nullptr };
    QLabel *m_profileTitle { nullptr };

    QTextBrowser *m_instructionViewer { nullptr };
};

