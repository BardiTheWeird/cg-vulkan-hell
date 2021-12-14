#pragma once

#include "lve_model.hpp"

// std
#include <unordered_map>
#include <string>
#include <optional>

namespace lve {

    class ModelManager {
    public:
        ModelManager(const ModelManager&) = delete;
        ModelManager &operator=(const ModelManager&) = delete;

        ModelManager(LveDevice& lveDevice) : lveDevice{lveDevice} {}

        void addModel(std::string key, std::shared_ptr<LveModel> model) {
            modelMap.insert_or_assign(key, model);
        }

        void addModel(std::string key, std::string filepath) {
            addModel(key, LveModel::createModelFromFile(lveDevice, filepath));
        }

        const std::optional<std::shared_ptr<LveModel>> getModel(std::string key) const {
            if (modelMap.find(key) == modelMap.end()) {
                return std::nullopt;
            }
            return {modelMap.at(key)};
        }
        
    private:
        LveDevice& lveDevice;

        std::unordered_map<std::string, std::shared_ptr<LveModel>> modelMap{};
    };
}