/* File: sign_worker.hpp
Copyright (C) Basealt LLC,  2024
Author: Oleg Proskurin, <proskurinov@basealt.ru>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SIGNWORKER_HPP
#define SIGNWORKER_HPP

#include "pdf_csp_c.hpp"
#include <QDebug>
#include <QObject>
#include <clocale>
#include <vector>

namespace core {

using FlatByteRange = std::vector<uint64_t>;

/**
 * @brief Create a signature
 * @details Is supposed to run in a separate thread
 */
class SignWorker : public QObject {
  Q_OBJECT
public:
  // utility structure for storing parameters for library
  struct CSignParamsWrapper {
    QByteArray qb_logo_path;
    QByteArray qb_config_path;
    QByteArray qb_cert_serial;
    QByteArray qb_cert_serial_prefix;
    QByteArray qb_cert_subject;
    QByteArray qb_cert_subject_prefix;
    QByteArray qb_cert_time_validity;
    QByteArray qb_stamp_type;
    QByteArray qb_cades_type;
    QByteArray qb_file_to_sign_path;
    QString temp_dir;
    QByteArray qb_temp_dir;
    QByteArray qb_stamp_title;
    QByteArray qb_tsp_url;
    pdfcsp::pdf::CSignParams pod_params;
  };

  using SharedParamWrapper = std::shared_ptr<CSignParamsWrapper>;

  struct SignParams {
    int page_index = 0;
    qreal page_width = 0;
    qreal page_height = 0;
    qreal stamp_x = 0;
    qreal stamp_y = 0;
    qreal stamp_width = 0;
    qreal stamp_height = 0;
    QString logo_path;
    QString config_path;
    QString cert_serial;
    QString cert_subject;
    QString cert_time_validity;
    QString stamp_type;
    QString cades_type;
    QString file_to_sign_path;
    QString tsp_url;
    QString cert_serial_prefix;
    QString cert_subject_prefix;
    QString stamp_title;
  };

  struct SignResult {
    bool status = false;
    QString tmp_result_file;
    QString err_string;
  };

  struct AimResizeFactor {
    double x = 1;
    double y = 1;
  };

  explicit SignWorker(QObject *parent = nullptr);
  SignWorker(SignWorker &&) = delete;
  SignWorker(const SignWorker &) = delete;
  SignWorker &operator=(SignWorker &&) = delete;
  SignWorker &operator=(const SignWorker &) = delete;
  ~SignWorker() override;

  /// @brief perform signing
  void launchSign(SignParams sign_params);

  /// @brief calculate the actual stamp size for the given parameters
  void estimateStampSize(SignParams sign_params);

signals:

  /// @brief signinig process(launchSign) is completed
  void signCompleted(SignResult res);
  /// @brief estimateStampSize is completed
  void resizeStampCompleted(AimResizeFactor res);

private:
  /// @brief Go to library, execute pdfcsp::pdf::PrepareDoc
  [[nodiscard]] SignResult preparePdf();
  /// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
  [[nodiscard]] SharedParamWrapper createParams() const;

  SignParams params_;
  locale_t locale_ = nullptr;
};

} // namespace core

#endif // SIGNWORKER_HPP
