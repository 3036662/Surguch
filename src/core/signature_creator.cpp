/* File: signature_creator.cpp
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

#include "signature_creator.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QVariant>

#include "sign_worker.hpp"

namespace core {

SignatureCreator::SignatureCreator(QObject *parent) : QObject{parent} {}

/// @brief parse all parameters recieved from QML
SignWorker::SignParams SignatureCreator::createWorkerParams(
    const QVariantMap &qvparams) {
    SignWorker::SignParams params{};
    if (qvparams.contains("page_index")) {
        params.page_index = qvparams.value("page_index").toInt();
    }
    if (qvparams.contains("page_width")) {
        params.page_width = qvparams.value("page_width").toReal();
    }
    if (qvparams.contains("page_height")) {
        params.page_height = qvparams.value("page_height").toReal();
    }
    if (qvparams.contains("stamp_x")) {
        params.stamp_x = qvparams.value("stamp_x").toReal();
    }
    if (qvparams.contains("stamp_y")) {
        params.stamp_y = qvparams.value("stamp_y").toReal();
    }
    if (qvparams.contains("stamp_width")) {
        params.stamp_width = qvparams.value("stamp_width").toReal();
    }
    if (qvparams.contains("stamp_height")) {
        params.stamp_height = qvparams.value("stamp_height").toReal();
    }
    if (qvparams.contains("logo_path")) {
        params.logo_path = qvparams.value("logo_path").toString();
    }
    if (qvparams.contains("config_path")) {
        params.config_path = qvparams.value("config_path").toString();
    }
    if (qvparams.contains("cert_serial")) {
        params.cert_serial = qvparams.value("cert_serial").toString();
    }
    if (qvparams.contains("cert_subject")) {
        params.cert_subject = qvparams.value("cert_subject").toString();
    }
    if (qvparams.contains("cert_time_validity")) {
        params.cert_time_validity =
            qvparams.value("cert_time_validity").toString();
    }
    if (qvparams.contains("stamp_type")) {
        params.stamp_type = qvparams.value("stamp_type").toString();
    }
    if (qvparams.contains("cades_type")) {
        params.cades_type = qvparams.value("cades_type").toString();
    }
    if (qvparams.contains("file_to_sign_path")) {
        params.file_to_sign_path =
            qvparams.value("file_to_sign_path").toString();
    }
    if (qvparams.contains("tsp_url")) {
        params.tsp_url = qvparams.value("tsp_url").toString();
    }
    if (qvparams.contains("cert_serial_prefix")) {
        params.cert_serial_prefix =
            qvparams.value("cert_serial_prefix").toString();
    }
    if (qvparams.contains("cert_subject_prefix")) {
        params.cert_subject_prefix =
            qvparams.value("cert_subject_prefix").toString();
    }
    if (qvparams.contains("stamp_title")) {
        params.stamp_title = qvparams.value("stamp_title").toString();
    }
    return params;
}

/**
 * @brief Create a signature (nonblocking)
 * @param QVariantMap, suitable for filling SignWorker::SignParams
 */
bool SignatureCreator::createSignature(const QVariantMap &qvparams) {
    qWarning() << "[SignatureCreator::CreateSignature]";
    auto params = createWorkerParams(qvparams);
    p_worker_ = new SignWorker();
    p_sign_thread_ = new QThread();
    p_worker_->moveToThread(p_sign_thread_);
    // start job
    QObject::connect(p_sign_thread_, &QThread::started,
                     [params = std::move(params), this]() mutable {
                         p_worker_->launchSign(std::move(params));
                     });
    // app closed
    QObject::connect(
        QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this]() {
            if (p_sign_thread_ != nullptr && p_sign_thread_->isRunning()) {
                p_sign_thread_->requestInterruption();
                p_sign_thread_->wait();
            }
        });
    // job is completed
    QObject::connect(p_worker_, &SignWorker::signCompleted,
                     [this](const SignWorker::SignResult &res) {
                         handleResult(res);
                         p_sign_thread_->quit();
                     });
    // thread is finished
    QObject::connect(p_sign_thread_, &QThread::finished, [this]() {
        p_worker_->deleteLater();
        p_sign_thread_->deleteLater();
        p_worker_ = nullptr;
        p_sign_thread_ = nullptr;
    });
    p_sign_thread_->start();
    ;
    return true;
}

/**
 * @brief estimate StampResizeFactor
 * @param qvparams from qml
 * @details In case text strings in certificates are very long, the stamp size
 * will be enlarged.
 */
void SignatureCreator::estimateStampResizeFactor(const QVariantMap &qvparams) {
    qWarning() << "[SignatureCreator::estimateStampResizeFactor]";
    auto params = createWorkerParams(qvparams);
    if (p_worker_resize_img_ != nullptr || p_resize_img_thread_ != nullptr) {
        qWarning() << "estimateStampResizeFactor is alreary running";
        return;
    }
    p_worker_resize_img_ = new SignWorker();
    p_resize_img_thread_ = new QThread();
    p_worker_resize_img_->moveToThread(p_resize_img_thread_);
    // start job
    QObject::connect(
        p_resize_img_thread_, &QThread::started,
        [params = std::move(params), this]() mutable {
            p_worker_resize_img_->estimateStampSize(std::move(params));
        });
    // app closed
    QObject::connect(QCoreApplication::instance(),
                     &QCoreApplication::aboutToQuit, [this]() {
                         if (p_resize_img_thread_ != nullptr &&
                             p_resize_img_thread_->isRunning()) {
                             p_resize_img_thread_->requestInterruption();
                             p_resize_img_thread_->wait();
                         }
                     });
    // job is completed
    QObject::connect(p_worker_resize_img_, &SignWorker::resizeStampCompleted,
                     [this](SignWorker::AimResizeFactor res) {
                         handleStampResize(res);
                         p_resize_img_thread_->quit();
                     });
    // thread is finished
    QObject::connect(p_resize_img_thread_, &QThread::finished, [this]() {
        p_worker_resize_img_->deleteLater();
        p_resize_img_thread_->deleteLater();
        p_worker_resize_img_ = nullptr;
        p_resize_img_thread_ = nullptr;
    });
    p_resize_img_thread_->start();
}

/// @brief Handle the result of the signature worker thread.
void SignatureCreator::handleResult(const SignWorker::SignResult &res) {
    QVariantMap js_result;
    js_result["status"] = res.status;
    js_result["tmp_file_path"] = res.tmp_result_file;
    js_result["err_string"] = res.err_string;
    emit signCompleted(js_result);
}

/// @brief Recieve the estimated stamp size and send it to the frontend
void SignatureCreator::handleStampResize(SignWorker::AimResizeFactor res) {
    QVariantMap js_result;
    js_result["x_resize"] = res.x;
    js_result["y_resize"] = res.y;
    emit stampSizeEstimated(js_result);
}

}  // namespace core
