#ifndef SIGNWORKER_HPP
#define SIGNWORKER_HPP

#include <vector>
#include <QObject>
#include <QDebug>

namespace core{

using FlatByteRange=std::vector<uint64_t>;

class SignWorker : public QObject
{
    Q_OBJECT
public:

    struct SignParams{
        int page_index=0;
        qreal page_width=0;
        qreal page_height=0;
        qreal stamp_x=0;
        qreal stamp_y=0;
        qreal stamp_width=0;
        qreal stamp_height=0;
        QString logo_path;
        QString config_path;
        QString cert_serial;
        QString cert_subject;
        QString cert_time_validity;
        QString stamp_type;
        QString cades_type;
        QString file_to_sign_path;
        QString tsp_url;
    };

    struct SignResult{
        bool status=false;
        QString tmp_result_file;
        QString err_string;

    };

    explicit SignWorker(QObject *parent = nullptr);

    void launchSign(SignParams sign_params);

signals:
    void signCompleted(SignResult res);

private:
    SignParams params_;
    SignResult preparePdf();
};

} // namespace core

#endif // SIGNWORKER_HPP
