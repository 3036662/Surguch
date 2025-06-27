#ifndef TAG_CREATOR_HPP
#define TAG_CREATOR_HPP

#include <QObject>
#include <pdf_csp_c.hpp>

namespace core {

/**
 * @brief Create a tag  stamp
 */
class TagCreator : public QObject {
    Q_OBJECT
   public:
    explicit TagCreator(QObject *parent = nullptr);

    /// @brief emdbed annotations into pdf
    static Q_INVOKABLE QString
    embedAnnot(const std::vector<pdfcsp::pdf::CAnnotParams> &params,
               const QString &file_path);
};

}  //  namespace core

#endif  // TAG_CREATOR_HPP
