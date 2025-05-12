#ifndef TSTAMP_HPP
#define TSTAMP_HPP

#include <QObject>

class TStamp : public QObject {
    Q_OBJECT
   public:
    explicit TStamp(QObject *parent = nullptr);

    void createPreview();

   signals:
};

#endif  // TSTAMP_HPP
