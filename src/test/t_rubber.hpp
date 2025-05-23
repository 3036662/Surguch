#ifndef T_RUBBER_HPP
#define T_RUBBER_HPP

#include <QObject>
#include <QDir>


class TRubber: public QObject {
    Q_OBJECT
    public:
    explicit TRubber(QObject *parent = nullptr);

    signals:
    private slots:

    void createImages();

    private:

    const QString test_files_dir_ = TEST_FILES_DIR;

};



#endif //T_RUBBER_HPP
