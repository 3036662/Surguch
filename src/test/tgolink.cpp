#include "tgolink.hpp"

#include <QTest>

#include "core/utils.hpp"

TGolink::TGolink(QObject *parent) : QObject(parent) {}

void TGolink::BaseTest() {
    const std::string src_file = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";

    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    // doc
    int page_count = 0;
    fz_document *fzdoc = nullptr;
    fz_page* page = nullptr;

    auto pages_uri_cache_expected = std::make_unique<std::vector<core::utils::PagesUriCacheSinglePage>>();
    QVERIFY(pages_uri_cache_expected);

    core::utils::PagesUriCache pages_uri_cache = nullptr;

    fz_var(page_count);
    fz_var(page);
    fz_var(fzdoc);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, src_file.c_str());
        QVERIFY(fzdoc != nullptr);

        page_count = fz_count_pages(fzctx, fzdoc);
        QVERIFY(page_count > 0);

        for (int i = 0; i < page_count; ++i) {
            core::utils::PageUriList uri_list_expected;
            page = fz_load_page(fzctx, fzdoc, i);

            for (auto* page_uri = fz_load_links(fzctx, page); page_uri != nullptr; page_uri = page_uri->next) {
                if (auto* extracted_uri = page_uri->uri; strlen(extracted_uri) > 0) {
                    core::utils::PageUriData page_uri_data {
                        .uri_rect = page_uri->rect,
                        .uri = extracted_uri
                    };
                    uri_list_expected.push_back(page_uri_data);
                }
            }

            fz_drop_page(fzctx, page);
            pages_uri_cache_expected->emplace_back(page_count, uri_list_expected);
        }

        pages_uri_cache = core::utils::extractUriAllPages(fzctx, fzdoc);
    }
    fz_catch(fzctx) {
        fz_report_error(fzctx);
    }

    QVERIFY2(!pages_uri_cache->empty(), "pages_uri_cache is empty");
    QVERIFY2(!pages_uri_cache_expected->empty(), "pages_uri_cache_expected is empty");

    bool uri_lists_are_equal = true;
    for (auto [it1, it2] = std::tuple{pages_uri_cache->cbegin(), pages_uri_cache_expected->cbegin()};
         it1 != pages_uri_cache->cend();
         ++it1, ++it2) {

        auto page = it1->second;
        auto page_expected = it2->second;

        bool all_uri_page_are_equal = true;
        QVERIFY(page.size() == page_expected.size());
        for (auto [iit1, iit2] = std::tuple{page.cbegin(), page_expected.cbegin()};
                iit1 != page.cend();
                ++iit1, ++iit2) {
            if (strcmp(iit1->uri, iit2->uri) != 0      ||
                iit1->uri_rect.x0 != iit2->uri_rect.x0 ||
                iit1->uri_rect.x1 != iit2->uri_rect.x1 ||
                iit1->uri_rect.y0 != iit2->uri_rect.y0 ||
                iit1->uri_rect.y1 != iit2->uri_rect.y1) {
                all_uri_page_are_equal = false;
                break;
            }
        }

        if (!all_uri_page_are_equal) {
            uri_lists_are_equal = false;
            break;
        }
    }

    QVERIFY(uri_lists_are_equal);

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TGolink::ExtractAllUriPage() {
    const std::string src_file = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";

    // expected URIs from the 1st page of a document
    std::vector<std::string> expected_uris {
        "http://cryptoarm.ru",
        "http://trusted.ru"
    };

    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    // doc
    int page_count = 0;
    fz_document *fzdoc = nullptr;
    fz_page* page = nullptr;

    std::vector<std::string> extracted_uris;

    fz_var(fzdoc);
    fz_var(page);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, src_file.c_str());
        QVERIFY(fzdoc != nullptr);

        page_count = fz_count_pages(fzctx, fzdoc);
        QVERIFY(page_count >= 1);

        // extract URIs from the 1st page of a document
        auto extracted_uris_data = core::utils::extractAllUriPage(fzctx, fzdoc, 1);
        std::for_each(extracted_uris_data.cbegin(), extracted_uris_data.cend(),
            [&extracted_uris](auto const& uri_data) {
            extracted_uris.push_back(uri_data.uri);
        });
    }
    fz_always(fzctx) {
        fz_drop_page(fzctx, page);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    std::sort(extracted_uris.begin(), extracted_uris.end());
    QVERIFY2(std::equal(
        extracted_uris.begin(), extracted_uris.end(), expected_uris.begin()),
        "The extracted URIs don't match the expected onces");

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

using pages_uri_list_t = QList<QStringList>;

template <class ForwardIt1, class ForwardIt2>
bool isEqualUriLists(ForwardIt1 first1, ForwardIt1 last1,
                     ForwardIt2 first2) {
    if (std::distance(first1, last1) < 0) {
        return false;
    }

    bool uri_lists_are_equal = true;
    for (auto [it1, it2] = std::tuple{first1, first2};
         it1 != last1;
         ++it1, ++it2) {

        auto page = it1->second;

        if (page.size() != it2->size()) {
            uri_lists_are_equal = false;
            break;
        }

        bool all_uri_page_are_equal = true;
        for (auto [iit1, iit2] = std::tuple{page.cbegin(), it2->cbegin()};
                iit1 != page.cend();
                ++iit1, ++iit2) {
            if (!strlen(iit1->uri)) {
                continue;
            }

            if (QString::fromUtf8(iit1->uri)  != *iit2) {
                all_uri_page_are_equal = false;
                break;
            }
        }

        if (!all_uri_page_are_equal) {
            uri_lists_are_equal = false;
            break;
        }
    }

    return uri_lists_are_equal;
}

void TGolink::ExtractUriAllPages() {
    QFETCH(QString, filepath);
    QFETCH(pages_uri_list_t, expected);

    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    core::utils::PagesUriCache pages_uri_cache = nullptr;

    // doc
    int page_count = 0;
    fz_document *fzdoc = nullptr;
    fz_page* page = nullptr;
    fz_var(fzdoc);
    fz_var(page);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
        QVERIFY(fzdoc != nullptr);

        pages_uri_cache = core::utils::extractUriAllPages(fzctx, fzdoc);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    QVERIFY(pages_uri_cache->size() == expected.size());
    QVERIFY2(isEqualUriLists(pages_uri_cache->begin(), pages_uri_cache->end(), expected.begin()), "The lists with URIs aren't equal!");

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TGolink::ExtractUriAllPages_data() {
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<pages_uri_list_t>("expected");

    QDir dir(QString::fromStdString(test_files_dir_));
    QStringList test_files;
    for (auto const& filename : dir.entryList({"*.pdf"})) {
        QString filepath = dir.path() + QDir::separator() + filename;
        test_files << filepath;
    }

    std::for_each(test_files.cbegin(), test_files.cend(), [](auto const& filepath) {
        pages_uri_list_t pages_uri_list;

        // context
        fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
        QVERIFY(fzctx != nullptr);

        // handlers
        fz_register_document_handlers(fzctx);

        // doc
        int page_count = 0;
        fz_document *fzdoc = nullptr;
        fz_page* page = nullptr;

        fz_var(fzdoc);
        fz_var(page);
        fz_try(fzctx) {
            fz_set_aa_level(fzctx, 0);
            fz_register_document_handlers(fzctx);
            fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
            QVERIFY(fzdoc != nullptr);

            page_count = fz_count_pages(fzctx, fzdoc);
            if (!page_count) {
                return;
            }

            for (int i = 0; i < page_count; ++i) {
                page = fz_load_page(fzctx, fzdoc, i);
                QVERIFY(page != nullptr);

                QStringList extracted_uris;
                for (auto* page_uri = fz_load_links(fzctx, page); page_uri != nullptr; page_uri = page_uri->next) {
                    QVERIFY(page_uri != nullptr);
                    if (auto* extracted_uri = page_uri->uri; strlen(extracted_uri) > 0) {
                        extracted_uris.push_back(extracted_uri);
                    }
                }

                pages_uri_list.emplace_back(extracted_uris);

                fz_drop_page(fzctx, page);
            }
            auto const* test_filename = QFileInfo(filepath).fileName().toUtf8().constData();
            QTest::newRow(test_filename) << filepath << pages_uri_list;
        }
        fz_catch(fzctx) { fz_report_error(fzctx); }

        // cleanup
        fz_drop_document(fzctx, fzdoc);
        fz_drop_context(fzctx);
    });
}

void TGolink::CacheUri() {
    QFETCH(QString, filepath);
    QFETCH(QVector<size_t>, expected_num_of_uris);

    // context
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    core::utils::PagesUriCache pages_uri_cache = nullptr;

    // doc
    int page_count = 0;
    fz_document *fzdoc = nullptr;
    fz_page* page = nullptr;
    fz_var(fzdoc);
    fz_var(page);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
        QVERIFY(fzdoc != nullptr);

        pages_uri_cache = core::utils::extractUriAllPages(fzctx, fzdoc);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    QVERIFY(pages_uri_cache->size() == expected_num_of_uris.size());

    // compare the number of real uri and expected ones in a document
    bool areEquals = true;
    for (auto [it1, it2] = std::tuple{pages_uri_cache->cbegin(), expected_num_of_uris.cbegin()};
            it1 != pages_uri_cache->cend();
            ++it1, ++it2) {
        if (it1->second.size() != *it2) {
            areEquals = false;
            break;
        }
    }

    QVERIFY2(areEquals, "The actual number of URIs isn't equal to the expected once!");

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TGolink::CacheUri_data() {
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<QVector<size_t>>("expected_num_of_uris");

    QDir dir(QString::fromStdString(test_files_dir_));
    QStringList test_files;
    for (auto const& filename : dir.entryList({"*.pdf"})) {
        QString filepath = dir.path() + QDir::separator() + filename;
        test_files << filepath;
    }

    std::for_each(test_files.cbegin(), test_files.cend(), [](auto const& filepath) {
        // context
        fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
        QVERIFY(fzctx != nullptr);

        // handlers
        fz_register_document_handlers(fzctx);

        QVector<size_t> num_of_uris_page;

        // doc
        int page_count = 0;
        fz_document *fzdoc = nullptr;
        fz_page* page = nullptr;

        fz_var(fzdoc);
        fz_var(page);
        fz_try(fzctx) {
            fz_set_aa_level(fzctx, 0);
            fz_register_document_handlers(fzctx);
            fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
            QVERIFY(fzdoc != nullptr);

            page_count = fz_count_pages(fzctx, fzdoc);
            QVERIFY(page_count);

            num_of_uris_page.resize(page_count);
            for (int i = 0; i < page_count; ++i) {
                page = fz_load_page(fzctx, fzdoc, i);
                QVERIFY(page != nullptr);

                size_t uri_cnt = 0;
                for (auto* page_uri = fz_load_links(fzctx, page); page_uri != nullptr; page_uri = page_uri->next) {
                    if (strlen(page_uri->uri)) {
                        ++uri_cnt;
                    }
                }

                num_of_uris_page[i] = uri_cnt;
                fz_drop_page(fzctx, page);
            }
        }
        fz_catch(fzctx) { fz_report_error(fzctx); }

        auto const* test_filename = QFileInfo(filepath).fileName().toUtf8().constData();
        QTest::newRow(test_filename) << filepath << num_of_uris_page;

        // cleanup
        fz_drop_document(fzctx, fzdoc);
        fz_drop_context(fzctx);
    });
}
