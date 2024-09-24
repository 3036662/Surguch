#include "t_signature.hpp"
#include <QTest>

TSignature::TSignature(QObject *parent)
    : QObject{parent}
{

}

void TSignature::testTrue(){
     QVERIFY(true);
}
