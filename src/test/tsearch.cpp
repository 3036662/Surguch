#include "tsearch.hpp"

#include <QChar>
#include <QString>
#include <QTest>
#include <iostream>
#include <text_extractor.hpp>

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
        for (block = stpage->first_block; block != nullptr;
             block = block->next) {
            if (block->type != FZ_STEXT_BLOCK_TEXT) {
                continue;
            }
            for (line = block->u.t.first_line; line != nullptr;
                 line = line->next) {
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

    std::cout << "cached pages count = " << cache->size() << "\n";
    const bool all_not_empty =
        std::all_of(cache->cbegin(), cache->cend(),
                    [](const std::pair<size_t, QString> &val) {
                        // std::cout << val.second.toStdString() << "\n";
                        return !val.second.isEmpty();
                    });
    const QString needle1 = "положения";
    QVERIFY(all_not_empty);
    {
        auto pages = core::utils::findPagesWithText(needle1, cache, true);
        QVERIFY(pages.size() == 5);
        std::vector<size_t> expected{1, 2, 7, 30, 31};
        QVERIFY(pages == expected);
    }

    size_t page_index = 1;
    core::utils::NeedleRectsOnPage needle_rects =
        core::utils::findNeedleRectsOnPage(needle1, 1, false, fzctx, fzdoc);
    QVERIFY(needle_rects);
    QVERIFY(!needle_rects->needle_rects.empty());
    std::cout << "page " << page_index
              << " needles found: " << needle_rects->needle_rects.size()
              << "\n";
    QVERIFY(needle_rects->needle_rects.size() == 1);
    const fz_rect &rect = needle_rects->needle_rects[0];
    std::cout << rect.x0 << " , " << rect.y0 << " -> " << rect.x1 << " , "
              << rect.y1 << "\n";
    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TSearch::SearchTest1() {
    const std::string src_file = test_files_dir_ + "gost-34.10-2012.pdf";
    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);
    // handlers
    fz_register_document_handlers(fzctx);
    // doc
    fz_document *fzdoc = nullptr;
    size_t total_needles = 0;
    QBENCHMARK {
        total_needles = 0;
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

        std::cout << "cached pages count = " << cache->size() << "\n";
        const bool all_not_empty =
            std::all_of(cache->cbegin(), cache->cend(),
                        [](const std::pair<size_t, QString> &val) {
                            // std::cout << val.second.toStdString() << "\n";
                            return !val.second.isEmpty();
                        });
        QVERIFY(all_not_empty);

        const QString needle1 = "ГОСТ";

        auto pages_with_needle =
            core::utils::findPagesWithText(needle1, cache, false);
        std::cout << "pages with needle : " << pages_with_needle.size() << "\n";
        QVERIFY(pages_with_needle.size() <= cache->size());
        std::for_each(
            pages_with_needle.cbegin(), pages_with_needle.cend(),
            [fzctx, fzdoc, &needle1, &total_needles](size_t page_index) {
                core::utils::NeedleRectsOnPage needle_rects =
                    core::utils::findNeedleRectsOnPage(needle1, page_index,
                                                       false, fzctx, fzdoc);
                QVERIFY(needle_rects);
                QVERIFY(!needle_rects->needle_rects.empty());
                total_needles += needle_rects->needle_rects.size();
            });
    }
    std::cout << "total needles found " << total_needles << " \n";
    QVERIFY(total_needles == 39);
    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TSearch::TextExtractorClass() {
    QEventLoop ev_loop;

    const std::string src_file = test_files_dir_ + "gost-34.10-2012.pdf";
    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);
    // handlers
    fz_register_document_handlers(fzctx);
    // doc
    fz_document *fzdoc = nullptr;

    fz_var(fzdoc);
    fz_try(fzctx) {
        fzdoc = fz_open_document(fzctx, src_file.c_str());
        QVERIFY(fzdoc != nullptr);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }
    auto extractor = std::make_unique<core::TextExtractor>(fzctx, fzdoc);
    extractor->updateCache();
    extractor->waitForCacheReady();
    QTest::qWait(100);
    extractor->updateCache();
    extractor->waitForCacheReady();
    QTest::qWait(100);
    extractor->performSearch("ГОСТ", false);
    extractor->waitForSearchReady();
    QTest::qWait(100);
    std::cout << "total needles: " << extractor->getNeedlesTotal();
    QVERIFY(extractor->getNeedlesTotal() == 39);
    const auto needles = extractor->getSearchContext();
    QVERIFY(needles);
    QVERIFY(!needles->empty());
    // QVERIFY(extractor->getCache());
    // QVERIFY(!extractor->getCache()->empty());

    //   cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}
