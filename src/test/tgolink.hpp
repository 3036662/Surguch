#ifndef TGOLINK_HPP
#define TGOLINK_HPP

#include <QObject>

class TGolink : public QObject {
    Q_OBJECT
public:
    explicit TGolink(QObject *parent = nullptr);

private slots:
    void BaseTest();
    void ExtractAllUriPage();
    void ExtractUriAllPages();
    void CacheUri();

    void ExtractUriAllPages_data();
    void CacheUri_data();

private:
    const std::string test_files_dir_ = TEST_FILES_DIR;
};

#endif // TGOLINK_HPP
