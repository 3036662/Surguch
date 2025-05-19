#ifndef TSEARCH_HPP
#define TSEARCH_HPP

#include <QObject>
#include <QString>

#include "mupdf/fitz.h"

class TSearch : public QObject {
    Q_OBJECT
   public:
    explicit TSearch(QObject *parent = nullptr);

   private slots:
    void BaseTest();
    void CacheText();
    void SearchTest1();
    void TextExtractorClass();

   private:
    const std::string test_files_dir_ = TEST_FILES_DIR;
};

#endif  // TSEARCH_HPP
