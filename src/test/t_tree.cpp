/* File: t_tree.cpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

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

#include "t_tree.hpp"

#include <QDebug>
#include <QFile>
#include <QTest>

#include "models/file_tree_model.hpp"

TTree::TTree(QObject *parent) : QObject{parent} {}

void TTree::testEmpty() {
    FileTreeModel tree_;

    for (int i = 0; i < 50; ++i) {
        tree_.addNode({"a" + QString::number(i)});
    }

    {
        tree_.setupModelData({""}, tree_.root_item.get());
        tree_.getCertList(-1);
        tree_.getMrpaData(-10);
        tree_.signTree({});
    }

    {
        QFile file(test_json_file);
        QVERIFY(file.exists());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QVERIFY(file.isOpen());
        QByteArray file_data = file.readAll();
        file.close();
        QJsonParseError parse_error;
        QJsonDocument json_doc =
            QJsonDocument::fromJson(file_data, &parse_error);
        QVERIFY(parse_error.error == QJsonParseError::NoError);
        QJsonObject root = json_doc.object();
        QJsonArray data_ = root["children"].toArray();
        tree_.setupModelData(data_, tree_.root_item.get());
        tree_.getCertList(10);
        tree_.getMrpaData(10);
        tree_.signTree({});
    }

    {
        QFile file(test_broken_json_file);
        QVERIFY(file.exists());
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QVERIFY(file.isOpen());
        QByteArray file_data = file.readAll();
        file.close();
        QJsonParseError parse_error;
        QJsonDocument json_doc =
            QJsonDocument::fromJson(file_data, &parse_error);
        QVERIFY(parse_error.error != QJsonParseError::NoError);
        QJsonObject root = json_doc.object();
        QJsonArray data_ = root["children"].toArray();
        tree_.setupModelData(data_, tree_.root_item.get());
        tree_.getCertList(10);
        tree_.getMrpaData(10);
        tree_.signTree({});
    }

    {
        tree_.deleteTree();
        tree_.setupModelData({"blablabla"}, tree_.root_item.get());
        tree_.getCertList(-1);
        tree_.getMrpaData(-10);
        tree_.signTree({});
    }
}
