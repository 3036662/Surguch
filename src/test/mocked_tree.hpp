#ifndef MOCKED_TREE_HPP
#define MOCKED_TREE_HPP

#include <QDebug>
#endif  // MOCKED_TREE_HPP

#include <optional>
#include <string>

#include "pod_structs.hpp"

class MocedTree {
   public:
    MocedTree() = default;

    std::optional<std::string> AddFilesJsonList(const std::string& json_list) {
        return std::nullopt;
    }

    std::optional<std::string> RemoveFilesJsonList(
        const std::string& json_list) {
        return std::nullopt;
    }
    bool ResetContext() { return true; }

    std::optional<std::string> BuildTree() { return std::nullopt; }

    const pdfcsp::c_bridge::CPodResult* GetCheckResultForNode(
        int sig_node_id, int signed_file_id) {
        return nullptr;
    };

    bool SignTree(const pdfcsp::c_bridge::BatchSignatureSettings& settings) {
        return false;
    }

    [[nodiscard]] std::optional<std::string> LastSignStatus() const {
        return std::nullopt;
    }
};
