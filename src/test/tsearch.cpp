#include "tsearch.hpp"

#include <QChar>
#include <QString>
#include <QTest>
#include <iostream>

#include "core/utils.hpp"

TSearch::TSearch(QObject *parent) : QObject{parent} {}

void TSearch::BaseTest() {
  const std::string src_file = test_files_dir_ + "resume_example.pdf";

  // context
  fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
  QVERIFY(fzctx != nullptr);

  // handlers
  fz_register_document_handlers(fzctx);

  // doc
  fz_document *fzdoc = nullptr;
  fz_try(fzctx) {
    fzdoc = fz_open_document(fzctx, src_file.c_str());
    QVERIFY(fzdoc != nullptr);

    //  structured text device
    fz_stext_page *stpage = fz_new_stext_page(fzctx, fz_empty_rect);
    QVERIFY(stpage != nullptr);
    // std::cout << fz_stext_options_usage << "\n";
    const fz_stext_options opts = {FZ_STEXT_DEHYPHENATE, 1.0f};
    fz_device *stext_dev = fz_new_stext_device(fzctx, stpage, &opts);
    QVERIFY(stext_dev != nullptr);

    const int page_number = 0;
    fz_page *page = fz_load_page(fzctx, fzdoc, page_number);
    QVERIFY(page != nullptr);
    fz_run_page(fzctx, page, stext_dev, fz_identity, nullptr);
    fz_close_device(fzctx, stext_dev);

    fz_stext_block *block = nullptr;
    fz_stext_line *line = nullptr;
    fz_stext_char *symbol = nullptr;

    QString extracted_string;
    for (block = stpage->first_block; block != nullptr; block = block->next) {
      if (block->type != FZ_STEXT_BLOCK_TEXT) {
        continue;
      }
      for (line = block->u.t.first_line; line != nullptr; line = line->next) {
        for (symbol = line->first_char; symbol != nullptr;
             symbol = symbol->next) {
          if (symbol->c <= 0xFFFF) {
            extracted_string.append(QChar(symbol->c));
          } else {
            auto arr = QChar::fromUcs4(symbol->c);
            std::for_each(arr.begin(), arr.end(),
                          [&extracted_string](char16_t ch) {
                            extracted_string.append(QChar(ch));
                          });
          }
        }
        extracted_string.append(QChar('\n'));
      }
      extracted_string.append(QChar('\n'));
    }

    fz_buffer *buffer = fz_new_buffer_from_page(fzctx, page, nullptr);
    QVERIFY(buffer != nullptr);
    const char *str_buffer = fz_string_from_buffer(fzctx, buffer);
    QVERIFY(str_buffer != nullptr);

    // use function
    auto extracted = core::utils::pageToQString(fzctx, fzdoc, page_number);
    QVERIFY(extracted == extracted_string);

    // std::cout << "\n\n";
    // std::cout << extracted_string.toStdString();
    // std::cout << "\n\n";
    // std::cout << str_buffer << "\n";
    // std::cout << "\n\n";

    QVERIFY(str_buffer == extracted_string.toStdString());
    QVERIFY(extracted_string == QString::fromUtf8(str_buffer, buffer->len));
    // cleanup
    fz_drop_buffer(fzctx, buffer);
    fz_drop_page(fzctx, page);
    fz_drop_device(fzctx, stext_dev);
    fz_drop_stext_page(fzctx, stpage);
  }
  fz_catch(fzctx) { fz_report_error(fzctx); }

  // cleanup
  fz_drop_document(fzctx, fzdoc);
  fz_drop_context(fzctx);
};

void TSearch::CacheText() {
  const std::string src_file = test_files_dir_ + "gost-34.10-2012.pdf";

  // context
  fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
  QVERIFY(fzctx != nullptr);
  // handlers
  fz_register_document_handlers(fzctx);
  // doc
  fz_document *fzdoc = nullptr;
  core::utils::PagesTextCache cache;
  fz_try(fzctx) {
    fzdoc = fz_open_document(fzctx, src_file.c_str());
    QVERIFY(fzdoc != nullptr);
    QVERIFY_THROWS_NO_EXCEPTION(
        cache = core::utils::extractTextAllPages(fzctx, fzdoc));
    QVERIFY(cache);
    QVERIFY(!cache->empty());
  }
  fz_catch(fzctx) { fz_report_error(fzctx); }

  // cleanup
  fz_drop_document(fzctx, fzdoc);
  fz_drop_context(fzctx);

  std::cout << "cached pages count = " << cache->size() << "\n";
  const bool all_not_empty =
      std::all_of(cache->cbegin(), cache->cend(),
                  [](const std::pair<size_t, QString> &val) {
                    // std::cout << val.second.toStdString() << "\n";
                    return !val.second.isEmpty();
                  });
  QVERIFY(all_not_empty);
}
