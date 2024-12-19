/* File: signature_creator.hpp
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

#ifndef SIGNATURE_CREATOR_HPP
#define SIGNATURE_CREATOR_HPP

#include <QObject>
#include <QVariant>

#include "sign_worker.hpp"

namespace core {

/**
 * @brief Create a signature and signature's stamp
 */
class SignatureCreator : public QObject {
    Q_OBJECT
   public:
    explicit SignatureCreator(QObject *parent = nullptr);

    /// @brief Handle the result of the signature worker thread.
    void handleResult(const SignWorker::SignResult &res);

    /// @brief Recieve the estimated stamp size and send it to the frontend
    void handleStampResize(SignWorker::AimResizeFactor res);

    /**
     * @brief Create a signature (nonblocking)
     * @param QVariantMap, suitable for filling SignWorker::SignParams
     */
    Q_INVOKABLE bool createSignature(const QVariantMap &qvparams);

    /**
     * @brief estimate StampResizeFactor
     * @param qvparams from qml
     * @details In case text strings in certificates are very long, the stamp
     * size will be enlarged.
     */
    Q_INVOKABLE void estimateStampResizeFactor(const QVariantMap &qvparams);

   signals:

    /// @brief Signature is created
    void signCompleted(QVariantMap res);

    /// @brief Send the estimated stamp size to the frontend
    void stampSizeEstimated(QVariantMap res);

   private:
    /// @brief parse all parameters recieved from QML
    static SignWorker::SignParams createWorkerParams(
        const QVariantMap &qvparams);

    SignWorker *p_worker_resize_img_ = nullptr;
    SignWorker *p_worker_ = nullptr;
    QThread *p_sign_thread_ = nullptr;
    QThread *p_resize_img_thread_ = nullptr;
};

}  // namespace core

#endif  // SIGNATURE_CREATOR_HPP
