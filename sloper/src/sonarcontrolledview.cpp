#include "sonarcontrolledview.h"
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QTimer>

SonarControlledView::SonarControlledView(GameState *state)
    : state(state)
{
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    // set variables
    pos_left = 0;
    pos_center = 100;
    pos_right = 200;

    // if file is good, start the timer
    file.setFileName("/dev/sonar");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open /dev/sonar";
    } else {
        // 3. Setup the timer only if file opened successfully
        sonarClock = new QTimer(this); // 'this' sets the parent, preventing memory leaks
        connect(sonarClock, &QTimer::timeout, this, &SonarControlledView::readSonarData);
        sonarClock->start(250);
    }
};

// destructor
SonarControlledView::~SonarControlledView()
{
    if (file.isOpen()) {
        file.close();
    }
}

void SonarControlledView::readSonarData()
{
    qWarning() << "began poll";
    if (!file.isOpen() || !file.isReadable())
        qWarning() << "File not open/not readable";
        return;

    // read in the msot recent line
    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        QString temp = in.readLine();
        if (!temp.isEmpty())
            line = temp;
    }

    if (line.isEmpty())
        qWarning() << "Line is empty";
        return;

        qWarning() << "line : " << line;

        // Parse the string for 3 integers (extracts only numbers)
        QRegExp numberRegex("-?\\d+"); //
        QStringList numbers;

        int pos = 0;
        while ((pos = numberRegex.indexIn(line, pos)) != -1) {
            numbers << numberRegex.cap(0);
            pos += numberRegex.matchedLength();
        }

    if (numbers.size() < 3) {
        qWarning() << "Not enough integers found in data";
        return;
    }

    bool ok1, ok2, ok3;
    if (!ok1 || !ok2 || !ok3) {
        return; // Safety check
    }
    int echo1, echo2, echo3;
    echo1 = numbers[0].toInt(&ok1);
    echo2 = numbers[1].toInt(&ok2);
    echo3 = numbers[2].toInt(&ok3);

    qWarning() << echo1 << " " << echo2 << " " << echo3;

    int smallest;

    if (echo1 < echo2 && echo1 < echo3) {
        smallest = 1;
    } else if (echo2 < echo1 && echo2 < echo3) {
        smallest = 2;
    } else {
        smallest = 3;
    }

    // handle arrow key movement
    switch (smallest) {
    case 1:
        is_right = false;
        is_left = true;
        state->guy->setX(pos_left);
        break;
    case 2:
        is_right = false;
        is_left = false;
        state->guy->setX(pos_center);
        break;
    case 3:
        is_right = true;
        is_left = false;
        state->guy->setX(pos_right);
    }
}
