#ifndef __INFOPANE_H_Sparcle_2021_12_01
#define __INFOPANE_H_Sparcle_2021_12_01

#include <QFrame>
#include <QTimer>

class InfoPane : public QFrame
{
    Q_OBJECT
public:
    InfoPane(QWidget *parent);
    ~InfoPane();

    void startTimer();
    void stopTimer();

private slots:
    void onTimer();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    QTimer *m_timer;
    int m_timePassed;
};

#endif //__INFOPANE_H_Sparcle_2021_12_01