#include "sonarcontrolledview.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

SonarControlledView::SonarControlledView(QGraphicsScene *scene, GameState *state)
    : QGraphicsView(scene)
    , state(state)
{
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    //set variables
    int pos_right = 300;
    int pos_center = 150
};

void SonarControlledView::sonarPoll()
{
    QFile file("/dev/sonar"); //get the file
        
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { //open the file
            qWarning() << "Failed to open /dev/sonar";
            return false;
    }
        
    QTextStream in(&file);
    QString line = in.readLine();
    file.close();
        
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
        return false;
    }
        
    bool ok1, ok2, ok3;
    echo1 = numbers[0].toInt(&ok1);
    echo2 = numbers[1].toInt(&ok2);
    echo3 = numbers[2].toInt(&ok3);

    int smallest;

    if (echo1 < echo2 && echo1 < echo3){
        smallest = 1;
    }else if(echo2 < echo1 && echo2 < echo3){
        smallest = 2;
    }else{
        smallest = 3;
    }

    // handle arrow key movement
    switch (smallest) {
    case 1:
        is_right = false;
        is_left = true;
        newPos.setX(pos_left);
        break;
    case 2: 
        is_right = false;
        is_left = false;
        newPos.setX(pos_center);
        break;
    case 3:
        is_right = true;
        is_left = false;
        newPos.setX(pos_right);
    }

    // update skier position
    state->guy->setPos(newPos);
}
