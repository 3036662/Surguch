#ifndef T_SIGNATURE_HPP
#define T_SIGNATURE_HPP

#include <QObject>

class TSignature : public QObject
{
    Q_OBJECT
public:
    explicit TSignature(QObject *parent = nullptr);

signals:

private slots:

    void testTrue();

};

#endif // T_SIGNATURE_HPP
