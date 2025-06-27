#ifndef T_RUBBER_HPP
#define T_RUBBER_HPP

#include <QDir>
#include <QObject>

class TRubber : public QObject {
    Q_OBJECT
   public:
    explicit TRubber(QObject *parent = nullptr);

   signals:
   private slots:

    void createRubber1();

    void createRubber2();

    void createRubber3();

   private:
    const QString test_files_dir_ = TEST_FILES_DIR;
    const QString file1_ = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";
    const QString file2_ = test_files_dir_ + "14_acrob_CADES-XLT1.pdf";
    const QString file3_ = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";
    const QString logo_ = test_files_dir_ + "profile_1_logo.jpg";
    const QString tag_ = test_files_dir_ + "tag_5_logo.jpg";
    const QString home_dir = TEST_DIR;
    const QString config_dir_ = home_dir + "/.config/csppdf";
    const QString profile_file_ = QString(TEST_FILES_DIR) + "/profiles.json";
    const QString stamps_file_ = QString(TEST_FILES_DIR) + "/stamps.json";
    const QString rubber_stamps_file_ =
        QString(TEST_FILES_DIR) + "/rubber_stamps.json";
};

#endif  // T_RUBBER_HPP
