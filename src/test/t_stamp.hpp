#ifndef TSTAMP_HPP
#define TSTAMP_HPP

#include <QDir>
#include <QObject>

class TStamp : public QObject {
    Q_OBJECT
   public:
    explicit TStamp(QObject *parent = nullptr);

   signals:

   private slots:

    void checkParams();

    void createPreview();

    void createImage();
    void createImage2();
    void createImage3();
    void createImage4();

   private:
    const QString test_files_dir_ = TEST_FILES_DIR;
    const QString file1_ = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";
    const QString file2_ = test_files_dir_ + "14_acrob_CADES-XLT1.pdf";
    const QString file3_ = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";
    const QString logo_ = test_files_dir_ + "profile_1_logo.jpg";
    const QString home_dir = QDir::homePath();
    const QString config_dir_ = home_dir + "/.config/csppdf";
    const QString profile_file_ = config_dir_ + "/profiles.json";
    const QString stamps_file_ = config_dir_ + "/stamps.json";
};

#endif  // TSTAMP_HPP
