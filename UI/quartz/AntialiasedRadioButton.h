#ifndef ANTIALIASEDRADIOBUTTON_H
#define ANTIALIASEDRADIOBUTTON_H

#include <QRadioButton>

class AntialiasedRadioButton : public QRadioButton
{
public:
    AntialiasedRadioButton(QString label);
protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif // ANTIALIASEDRADIOBUTTON_H
