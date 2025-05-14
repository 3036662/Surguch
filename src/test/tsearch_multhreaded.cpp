#include <QChar>
#include <QCoreApplication>
#include <QString>
#include <QTimer>
#include <iostream>
#include <text_extractor.hpp>

#include "core/utils.hpp"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    fz_context *fzctx = fz_new_context(nullptr, nullptr, 500000000);
    fz_document *fzdoc = nullptr;
    QTimer::singleShot(0, [&fzctx, &fzdoc, &app]() {
        const std::string src_file =
            std::string(TEST_FILES_DIR) + "gost-34.10-2012.pdf";
        // context

        assert(fzctx != nullptr);
        // handlers
        fz_register_document_handlers(fzctx);
        // doc

        size_t total_needles = 0;

        total_needles = 0;
        fz_var(fzdoc);
        fz_try(fzctx) {
            fzdoc = fz_open_document(fzctx, src_file.c_str());
            assert(fzdoc != nullptr);
        }
        fz_catch(fzctx) { fz_report_error(fzctx); }
        auto extractor = std::make_unique<core::TextExtractor>(fzctx, fzdoc);
        extractor->updateCache();
        extractor->waitForCacheReady();
        QCoreApplication::processEvents();
        extractor->updateCache();
        extractor->waitForCacheReady();
        QCoreApplication::processEvents();
        extractor->performSearch("ГОСТ", false);
        extractor->waitForSearchReady();
        QCoreApplication::processEvents();
        std::cout << "total needles: " << extractor->getNeedlesTotal() << "\n";
        std::flush(std::cout);
        assert(extractor->getNeedlesTotal() == 39);
        const auto needles = extractor->getSearchContext();
        assert(needles);
        assert(!needles->empty());
        app.quit();
    });

    app.exec();
    fz_drop_document(fzctx, fzdoc);
    fz_drop_context(fzctx);
    //   cleanup
}
