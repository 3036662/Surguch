#ifndef T_SIGNATURE_HPP
#define T_SIGNATURE_HPP

#include <QObject>

class TSignature : public QObject {
  Q_OBJECT
public:
  explicit TSignature(QObject *parent = nullptr);

signals:

private slots:

  void testTrue();

  void testModelConstruct();

  void createProcessorFail();

  void createProcessor();

  void findSignatures();

  void signaturesNumber();

  void parseHexString();

  void cBridge();

  void cBridgeCertList();

  void OneSigCoverage();

private:
  bool sigNumb(const QString &file, int sig_expected) const;

  const QString test_files_dir_ = TEST_FILES_DIR;
  const QString file1_ = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";
  const QString file2_ = test_files_dir_ + "14_acrob_CADES-XLT1.pdf";
  const QString file3_ = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";
};

#endif // T_SIGNATURE_HPP
