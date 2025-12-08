#pragma once

#include <QWidget>

class QLabel;

class SlotItem : public QWidget {
    Q_OBJECT
public:
    enum class State {
        Available,   // 可借
        Empty,       // 可还
        Maintenance, // 故障
        Selected
    };

    explicit SlotItem(int index, QWidget *parent = nullptr);
    void setState(State state);
    State state() const { return m_state; }

signals:
    void clicked(int index, SlotItem::State state);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void refreshStyle();

    int m_index;
    State m_state { State::Available };
    QLabel *m_label;
};

