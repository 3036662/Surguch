#include "tgolink.hpp"

#include <QTest>

#include "core/utils.hpp"

TGolink::TGolink(QObject* parent) : QObject(parent) {}

void sortUriByDescendingArea(core::utils::PageUriList& page_uri_lst) {
    auto area = [](auto const& rect) {
        auto [x0, y0, x1, y1] = rect.uri_rect;
        return std::fabs(x1 - x0) * std::fabs(y1 - y0);
    };

    std::sort(page_uri_lst.begin(), page_uri_lst.end(),
              [&area = std::as_const(area)](auto const& lhs, auto const& rhs) {
                  return area(lhs) > area(rhs);
              });
}

void TGolink::BaseTest() {
    const std::string src_file = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";

    // context
    fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    // doc
    int page_count = 0;
    fz_document* fzdoc = nullptr;
    fz_page* page = nullptr;

    auto pages_uri_cache_expected =
        std::make_unique<std::vector<core::utils::PagesTextCacheSinglePage>>();
    QVERIFY(pages_uri_cache_expected);

    core::utils::PagesTextCache pages_uri_cache = nullptr;

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

            for (auto* page_uri = fz_load_links(fzctx, page);
                 page_uri != nullptr; page_uri = page_uri->next) {
                if (auto* extracted_uri = page_uri->uri;
                    strlen(extracted_uri) > 0) {
                    core::utils::PageUriData page_uri_data{
                        .uri_rect = page_uri->rect, .uri = extracted_uri};
                    uri_list_expected.push_back(page_uri_data);
                }
            }

            fz_drop_page(fzctx, page);
            sortUriByDescendingArea(uri_list_expected);
            core::utils::PagesTextCacheSinglePage page_cache{
                static_cast<size_t>(i), std::move(QString{}),
                std::move(uri_list_expected)};
            pages_uri_cache_expected->emplace_back(page_cache);
        }

        pages_uri_cache = core::utils::extractTextAllPages(fzctx, fzdoc);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    QVERIFY2(!pages_uri_cache->empty(), "pages_uri_cache is empty");
    QVERIFY2(!pages_uri_cache_expected->empty(),
             "pages_uri_cache_expected is empty");

    bool uri_lists_are_equal = true;
    for (auto [it1, it2] = std::tuple{pages_uri_cache->cbegin(),
                                      pages_uri_cache_expected->cbegin()};
         it1 != pages_uri_cache->cend(); ++it1, ++it2) {
        auto page = it1->page_uri_list;
        auto page_expected = it2->page_uri_list;

        bool all_uri_page_are_equal = true;
        QVERIFY(page.size() == page_expected.size());
        for (auto [iit1, iit2] =
                 std::tuple{page.cbegin(), page_expected.cbegin()};
             iit1 != page.cend(); ++iit1, ++iit2) {
            if (strcmp(iit1->uri, iit2->uri) != 0 ||
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
    const std::string src_file =
        test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";

    // expected URIs from the 1st page of a document
    QStringList expected_uris {
        "http://cryptoarm.ru",
        "http://trusted.ru" };

    // context
    fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    // doc
    int page_count = 0;
    fz_document* fzdoc = nullptr;
    fz_page* page = nullptr;

    QStringList extracted_uris;

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
        auto extracted_uris_data =
            core::utils::extractAllUriPage(fzctx, fzdoc, 1);
        std::for_each(extracted_uris_data.cbegin(), extracted_uris_data.cend(),
                      [&extracted_uris](auto const& uri_data) {
                          extracted_uris.push_back(uri_data.uri);
                      });
    }
    fz_always(fzctx) { fz_drop_page(fzctx, page); }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    std::sort(extracted_uris.begin(), extracted_uris.end());

    QVERIFY2(std::equal(extracted_uris.cbegin(), extracted_uris.cend(),
            expected_uris.cbegin()),
            "The extracted URIs don't match the expected onces");

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

using pages_uri_list_t = QVector<QStringList>;

template <class ForwardIt1, class ForwardIt2>
bool isEqualUriLists(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    if (std::distance(first1, last1) < 0) {
        return false;
    }

    bool uri_lists_are_equal = true;
    for (auto [it1, it2] = std::tuple{first1, first2}; it1 != last1;
         ++it1, ++it2) {
        if (it1->size() != it2->size()) {
            uri_lists_are_equal = false;
            break;
        }

        bool all_uri_it1_are_equal = true;
        for (auto [iit1, iit2] = std::tuple{it1->cbegin(), it2->cbegin()};
             iit1 != it1->cend(); ++iit1, ++iit2) {
            if (iit1->isEmpty()) {
                continue;
            }

            if (*iit1 != *iit2) {
                all_uri_it1_are_equal = false;
                break;
            }
        }

        if (!all_uri_it1_are_equal) {
            uri_lists_are_equal = false;
            break;
        }
    }

    return uri_lists_are_equal;
}

void TGolink::ExtractAllUriPageTest1() {
    QFETCH(QString, filepath);
    QFETCH(pages_uri_list_t, expected);

    // context
    fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    pages_uri_list_t pages_uri_list_extracted;

    // doc
    int page_count = 0;
    fz_document* fzdoc = nullptr;
    fz_page* page = nullptr;
    fz_var(fzdoc);
    fz_var(page);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
        QVERIFY(fzdoc != nullptr);

        page_count = fz_count_pages(fzctx, fzdoc);
        QVERIFY(page_count >= 0);

        for (int i = 0; i < page_count; ++i) {
            page = fz_load_page(fzctx, fzdoc, i);
            QVERIFY(page != nullptr);

            auto page_uri_list =
                core::utils::extractAllUriPage(fzctx, fzdoc, i);
            QStringList uri_list;
            std::for_each(page_uri_list.cbegin(), page_uri_list.cend(),
                          [&uri_list](auto const& page_uri_data) {
                              uri_list.emplace_back(page_uri_data.uri);
                          });

            pages_uri_list_extracted.emplace_back(uri_list);

            fz_drop_page(fzctx, page);
        }
    }
    fz_always(fzctx) { fz_drop_page(fzctx, page); }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    QVERIFY(pages_uri_list_extracted.size() == expected.size());
    bool areEqual = true;

    QVERIFY2(
        isEqualUriLists(pages_uri_list_extracted.cbegin(),
                        pages_uri_list_extracted.cend(), expected.cbegin()),
        "The list of extracted URIs doesn't match the expected one");

    // cleanup
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
}

void TGolink::ExtractAllUriPageTest1_data() {
    QTest::addColumn<QString>("filepath");
    QTest::addColumn<pages_uri_list_t>("expected");

    QDir dir(QString::fromStdString(test_files_dir_));
    QStringList test_files;
    for (auto const& filename : dir.entryList({"*.pdf"})) {
        QString filepath = dir.path() + QDir::separator() + filename;
        test_files << filepath;
    }

    std::for_each(
        test_files.cbegin(), test_files.cend(), [](auto const& filepath) {
            pages_uri_list_t pages_uri_list_extracted;

            // context
            fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
            QVERIFY(fzctx != nullptr);

            // handlers
            fz_register_document_handlers(fzctx);

            // doc
            int page_count = 0;
            fz_document* fzdoc = nullptr;
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

                    core::utils::PageUriList page_uri_list;
                    for (auto* page_uri = fz_load_links(fzctx, page);
                         page_uri != nullptr; page_uri = page_uri->next) {
                        QVERIFY(page_uri != nullptr);
                        if (auto* extracted_uri = page_uri->uri;
                            strlen(extracted_uri) > 0) {
                            core::utils::PageUriData page_uri_data{
                                .uri_rect = page_uri->rect,
                                .uri = extracted_uri};
                            page_uri_list.push_back(page_uri_data);
                        }
                    }

                    QStringList uri_list_extracted;
                    std::for_each(
                        page_uri_list.cbegin(), page_uri_list.cend(),
                        [&uri_list_extracted](auto const& page_uri_info) {
                            uri_list_extracted.emplace_back(page_uri_info.uri);
                        });
                    pages_uri_list_extracted.emplace_back(uri_list_extracted);

                    fz_drop_page(fzctx, page);
                }
                auto const* test_filename =
                    QFileInfo(filepath).fileName().toUtf8().constData();
                QTest::newRow(test_filename)
                    << filepath << pages_uri_list_extracted;
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
    fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
    QVERIFY(fzctx != nullptr);

    // handlers
    fz_register_document_handlers(fzctx);

    core::utils::PagesTextCache pages_uri_cache = nullptr;

    // doc
    int page_count = 0;
    fz_document* fzdoc = nullptr;
    fz_page* page = nullptr;
    fz_var(fzdoc);
    fz_var(page);
    fz_try(fzctx) {
        fz_set_aa_level(fzctx, 0);
        fz_register_document_handlers(fzctx);
        fzdoc = fz_open_document(fzctx, filepath.toUtf8().constData());
        QVERIFY(fzdoc != nullptr);

        pages_uri_cache = core::utils::extractTextAllPages(fzctx, fzdoc);
    }
    fz_catch(fzctx) { fz_report_error(fzctx); }

    QVERIFY(pages_uri_cache->size() == expected_num_of_uris.size());

    // compare the number of real uri and expected ones in a document
    bool areEquals = true;
    for (auto [it1, it2] = std::tuple{pages_uri_cache->cbegin(),
                                      expected_num_of_uris.cbegin()};
         it1 != pages_uri_cache->cend(); ++it1, ++it2) {
        if (it1->page_uri_list.size() != *it2) {
            areEquals = false;
            break;
        }
    }

    QVERIFY2(areEquals,
             "The actual number of URIs isn't equal to the expected once!");

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

    std::for_each(
        test_files.cbegin(), test_files.cend(), [](auto const& filepath) {
            // context
            fz_context* fzctx = fz_new_context(nullptr, nullptr, 500000000);
            QVERIFY(fzctx != nullptr);

            // handlers
            fz_register_document_handlers(fzctx);

            QVector<size_t> num_of_uris_page;

            // doc
            int page_count = 0;
            fz_document* fzdoc = nullptr;
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
                    for (auto* page_uri = fz_load_links(fzctx, page);
                         page_uri != nullptr; page_uri = page_uri->next) {
                        if (strlen(page_uri->uri)) {
                            ++uri_cnt;
                        }
                    }

                    num_of_uris_page[i] = uri_cnt;
                    fz_drop_page(fzctx, page);
                }
            }
            fz_catch(fzctx) { fz_report_error(fzctx); }

            auto const* test_filename =
                QFileInfo(filepath).fileName().toUtf8().constData();
            QTest::newRow(test_filename) << filepath << num_of_uris_page;

            // cleanup
            fz_drop_document(fzctx, fzdoc);
            fz_drop_context(fzctx);
        });
}

void TGolink::RemoveAllCoveredUri() {
    core::utils::PageUriList page_uri_list;
    QVERIFY(page_uri_list.empty());

    auto processed_page_uri_list =
        core::utils::removeAllCoveredUri(page_uri_list);
    QVERIFY(page_uri_list.empty());

    fz_rect test_rect_bb1{1, 1, 25, 20};
    std::string test_uri1 = "https://ya.ru";
    page_uri_list.emplace_back(
        core::utils::PageUriData{test_rect_bb1, test_uri1.data()});
    QVERIFY(page_uri_list.size() == 1);

    processed_page_uri_list = core::utils::removeAllCoveredUri(page_uri_list);
    QVERIFY(processed_page_uri_list.size() == 1);

    fz_rect test_rect_bb2{5, 5, 35, 25};
    std::string test_uri2 = "https://altlinux.org";
    page_uri_list.emplace_back(
        core::utils::PageUriData{test_rect_bb2, test_uri2.data()});
    QVERIFY(page_uri_list.size() == 2);

    processed_page_uri_list = core::utils::removeAllCoveredUri(page_uri_list);
    QVERIFY(processed_page_uri_list.size() == 2);

    auto comp_bounding_box = [](auto const& lhs, auto const& rhs) {
        return lhs.x0 == rhs.x0 && lhs.y0 == rhs.y0 && lhs.x1 == rhs.x1 &&
               lhs.y1 == rhs.y1;
    };

    // the result is expected to be sorted by area in descending order
    auto obtained_rect_bb1 = processed_page_uri_list.at(0);
    QVERIFY(obtained_rect_bb1.uri == test_uri2 &&
            comp_bounding_box(obtained_rect_bb1.uri_rect, test_rect_bb2));

    auto obtained_rect_bb2 = processed_page_uri_list.at(1);
    QVERIFY(obtained_rect_bb2.uri == test_uri1 &&
            comp_bounding_box(obtained_rect_bb2.uri_rect, test_rect_bb1));

    // must be removed due to URI overlap with bounding box values of
    // test_rect_bb2
    fz_rect test_rect_bb3{10, 10, 30, 20};
    std::string test_uri3 = "https://packages.altlinux.org";
    page_uri_list.emplace_back(
        core::utils::PageUriData{test_rect_bb3, test_uri3.data()});
    QVERIFY(page_uri_list.size() == 3);
    processed_page_uri_list = core::utils::removeAllCoveredUri(page_uri_list);
    QVERIFY(processed_page_uri_list.size() == 2);

    obtained_rect_bb1 = processed_page_uri_list.at(0);
    QVERIFY(obtained_rect_bb1.uri == test_uri2 &&
            comp_bounding_box(obtained_rect_bb1.uri_rect, test_rect_bb2));

    obtained_rect_bb2 = processed_page_uri_list.at(1);
    QVERIFY(obtained_rect_bb2.uri == test_uri1 &&
            comp_bounding_box(obtained_rect_bb2.uri_rect, test_rect_bb1));
}

void TGolink::RemoveAllCoveredUriTest() {
    QFETCH(core::utils::PageUriList, input);
    QFETCH(core::utils::PageUriList, expected);

    auto processedUriList = core::utils::removeAllCoveredUri(input);
    QVERIFY(processedUriList.size() == expected.size());

    bool isEqual = true;
    for (auto [it1, it2] =
             std::tuple{processedUriList.cbegin(), expected.cbegin()};
         it1 != processedUriList.cend(); ++it1, ++it2) {
        auto rect1 = it1->uri_rect;
        auto rect2 = it2->uri_rect;

        if (rect1.x0 != rect2.x0 || rect1.y0 != rect2.y0 ||
            rect1.x1 != rect2.x1 || rect1.y1 != rect2.y1) {
            isEqual = false;
            break;
        }
    }

    QVERIFY2(
        isEqual,
        "processedUriList and expected list of bounding boxes aren't equal");
}

void TGolink::RemoveAllCoveredUriTest_data() {
    QTest::addColumn<core::utils::PageUriList>("input");
    QTest::addColumn<core::utils::PageUriList>("expected");

    // test bounding boxes (area of the rectangle)
    QVector<fz_rect> test_bb_page1{
        {5, 5, 50, 50},    // 2025
        {4, 4, 51, 51},    // 2209
        {10, 10, 45, 45},  // 1225
        {0, 0, 50, 50},    // 2500
        {4, 4, 60, 50},    // 2576
        {4, 4, 57, 57},    // 2809
    };

    QVector<fz_rect> test_bb_page2{
        {5, 5, 10, 10},    // 25
        {10, 10, 30, 30},  // 400
        {7, 7, 14, 14},    // 49
    };

    // expected result sorted by area in descending order

    // expected bounding boxes
    QVector<fz_rect> expected_bb_page1{
        {4, 4, 57, 57},  // 2809
        {4, 4, 60, 50},  // 2576
        {0, 0, 50, 50},  // 2500

    };

    QVector<fz_rect> expected_bb_page2{
        {10, 10, 30, 30},  // 400
        {7, 7, 14, 14},    // 49
        {5, 5, 10, 10},    // 25
    };

    std::string test_uri{"https://ya.ru"};

    core::utils::PageUriList input_data1;

    std::for_each(test_bb_page1.cbegin(), test_bb_page1.cend(),
                  [&](auto const& bounding_box) {
                      input_data1.emplace_back(core::utils::PageUriData{
                          bounding_box, test_uri.data()});
                  });

    core::utils::PageUriList expected_data1;
    std::for_each(expected_bb_page1.cbegin(), expected_bb_page1.cend(),
                  [&](auto const& bounding_box) {
                      expected_data1.emplace_back(core::utils::PageUriData{
                          bounding_box, test_uri.data()});
                  });

    QTest::newRow("test_data_1") << input_data1 << expected_data1;

    core::utils::PageUriList input_data2;
    std::for_each(test_bb_page2.cbegin(), test_bb_page2.cend(),
                  [&](auto const& bounding_box) {
                      input_data2.emplace_back(core::utils::PageUriData{
                          bounding_box, test_uri.data()});
                  });

    core::utils::PageUriList expected_data2;
    std::for_each(expected_bb_page2.cbegin(), expected_bb_page2.cend(),
                  [&](auto const& bounding_box) {
                      expected_data2.emplace_back(core::utils::PageUriData{
                          bounding_box, test_uri.data()});
                  });

    QTest::newRow("test_data_2") << input_data2 << expected_data2;
}

bool compareRects(fz_rect const& lhs, fz_rect const& rhs) {
    return lhs.x0 == rhs.x0 && lhs.y0 == rhs.y0 && lhs.x1 == rhs.x1 &&
           lhs.y1 == rhs.y1;
}

void TGolink::FindAllUriPage() {
    size_t test_page_index = 0;
    core::utils::MousePos test_mouse_pos{.0F, .0F};
    core::utils::PagesTextCache test_page_text_cache;

    QVERIFY(core::utils::findAllUriPage(test_page_index, test_mouse_pos,
                                        test_page_text_cache) == nullptr);

    std::string test_uri = "https://ya.ru";
    core::utils::PageUriList page_uri_list;
    core::utils::PageUriData page_uri_data{.uri_rect = {55, 60, 80, 70},
                                           .uri = test_uri.data()};
    core::utils::PagesTextCacheSinglePage page_text_single_page{
        .page_index = test_page_index,
        .page_text = "clickme!",
        .page_uri_list = std::move(core::utils::PageUriList{page_uri_data}),
    };

    test_page_text_cache =
        std::make_unique<std::vector<core::utils::PagesTextCacheSinglePage>>();
    test_page_text_cache->push_back(page_text_single_page);
    QVERIFY(test_page_text_cache->size() == 1);

    // expected nullptr if page_index is >= page_uri_list.size()
    QVERIFY(core::utils::findAllUriPage(1, test_mouse_pos,
                                        test_page_text_cache) == nullptr);

    test_mouse_pos = {60, 65};
    auto result = core::utils::findAllUriPage(test_page_index, test_mouse_pos,
                                              test_page_text_cache);
    QVERIFY(result != nullptr);
    QVERIFY(!result->empty());

    // the found element is expected to contain the values from page_uri_data
    auto page_data = result->at(0);
    QVERIFY(compareRects(page_data.uri_rect, page_uri_data.uri_rect));
    QVERIFY(page_data.uri == test_uri);
}
