#include "sonarcontrolledview.h"
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <fstream>
#include <string>

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
    file.open("/dev/sonar");
    if (!file.is_open()) {
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
    if (file.is_open()) {
        file.close();
    }
}

void SonarControlledView::readSonarData()
{
    qWarning() << "began poll";
    
    // Close and reopen to reset kernel module's offset
    // The kernel module sets *offset after read, so we need to reset it
    if (file.is_open()) {
        file.close();
    }
    
    file.open("/dev/sonar");
    if (!file.is_open()) {
        qWarning() << "File not open";
        return;
    }
    
    // read one line (kernel module returns one line per read)
    std::string line;
    if (!std::getline(file, line)) {
        qWarning() << "Failed to read line";
        return;
    }
    
    if (line.empty()) {
        qWarning() << "Line is empty";
        return;
    }
    
    QString qLine = QString::fromStdString(line);

    qWarning() << "line : " << qLine;

    // Parse the string for 3 integers (format: "ECHO1: %d ECHO2: %d ECHO3: %d")
    // Extract numbers that come after colons and spaces
    QRegExp numberRegex(":\\s*(\\d+)");
    QStringList numbers;

    int pos = 0;
    while ((pos = numberRegex.indexIn(qLine, pos)) != -1) {
        numbers << numberRegex.cap(1);  // cap(1) is the captured group (the number)
        pos += numberRegex.matchedLength();
    }

    if (numbers.size() < 3) {
        qWarning() << "Not enough integers found in data, found: " << numbers.size();
        return;
    }

    bool ok1, ok2, ok3;
    int echo1, echo2, echo3;
    echo1 = numbers[0].toInt(&ok1);
    echo2 = numbers[1].toInt(&ok2);
    echo3 = numbers[2].toInt(&ok3);
    
    if (!ok1 || !ok2 || !ok3) {
        qWarning() << "Failed to parse integers";
        return; // Safety check
    }

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
