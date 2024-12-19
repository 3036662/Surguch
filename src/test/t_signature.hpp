/* File: t_signature.hpp
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

#ifndef T_SIGNATURE_HPP
#define T_SIGNATURE_HPP

#include <QObject>

class TSignature : public QObject {
    Q_OBJECT
   public:
    explicit TSignature(QObject *parent = nullptr);

   signals:

   private slots:

    void testTrue();

    void testModelConstruct();

    void createProcessorFail();

    void createProcessor();

    void findSignatures();

    void signaturesNumber();

    void parseHexString();

    void cBridge();

    void cBridgeCertList();

    void OneSigCoverage();

   private:
    bool sigNumb(const QString &file, int sig_expected) const;

    const QString test_files_dir_ = TEST_FILES_DIR;
    const QString file1_ = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";
    const QString file2_ = test_files_dir_ + "14_acrob_CADES-XLT1.pdf";
    const QString file3_ = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";
};

#endif  // T_SIGNATURE_HPP
