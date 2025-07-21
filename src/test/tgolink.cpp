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
}

void TGolink::ExtractAllUriPage() {

}


void TGolink::ExtractUriAllPages() {

}

void TGolink::CacheUri() {

}
