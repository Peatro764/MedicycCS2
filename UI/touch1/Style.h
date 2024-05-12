#ifndef STYLE_H
#define STYLE_H

#include <QColor>
#include <QFont>

namespace ui_main {

namespace COLOR {
    const QColor BORDER = QRgb(0x209fdf);
};

namespace FONT {
    const QFont TITLE = QFont("Arial", 20);

    const QFont SMALL_HEADER = QFont("Arial", 7);
    const QFont MEDIUM_HEADER = QFont("Arial", 11);

    const QFont SMALL_LABEL = QFont("Arial", 9);
    const QFont MEDIUM_LABEL = QFont("Arial", 12);
    const QFont LARGE_LABEL = QFont("Arial", 15);
    const QFont XLARGE_LABEL = QFont("Arial", 18);
}

namespace BUTTON {
    const int SMALL_HEIGHT = 25;
    const int MEDIUM_HEIGHT = 40;
    const int LARGE_HEIGHT = 60;
}

}

#endif // STYLE_H
