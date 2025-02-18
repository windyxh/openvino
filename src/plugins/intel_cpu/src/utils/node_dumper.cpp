// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#ifdef CPU_DEBUG_CAPS

#include "node_dumper.h"

#include <node.h>
#include "ie_common.h"
#include "utils/blob_dump.h"
#include "memory_desc/cpu_memory_desc_utils.h"

#include <regex>
#include <sstream>
#include <string>

using namespace InferenceEngine;

namespace ov {
namespace intel_cpu {

static void formatNodeName(std::string& name) {
    std::replace(name.begin(), name.end(), '\\', '_');
    std::replace(name.begin(), name.end(), '/', '_');
    std::replace(name.begin(), name.end(), ' ', '_');
    std::replace(name.begin(), name.end(), ':', '-');
}

static bool shouldBeDumped(const NodePtr& node, const Config& config, const std::string& portsKind) {
    const auto& dumpFilters = config.blobDumpFilters;

    if (dumpFilters.empty())
        return false;

    if (dumpFilters.count(Config::FILTER::BY_PORTS)) { // filter by ports configured
        if (dumpFilters.at(Config::FILTER::BY_PORTS) != "ALL" &&
            portsKind != dumpFilters.at(Config::FILTER::BY_PORTS))
            return false;
    }

    if (dumpFilters.count(Config::FILTER::BY_EXEC_ID)) { // filter by exec id configured
        std::stringstream ss(dumpFilters.at(Config::FILTER::BY_EXEC_ID));
        int id;
        bool matched = false;

        while (ss >> id) {
            if (node->getExecIndex() == id) {// exec id matches
                matched = true;
                break;
            }
        }

        if (!matched)
            return false;
    }

    if (dumpFilters.count(Config::FILTER::BY_TYPE)) { // filter by type configured
        std::stringstream ss(dumpFilters.at(Config::FILTER::BY_TYPE));
        std::string type;
        bool matched = false;

        while (ss >> type) {
            if (NameFromType(node->getType()) == type) {// type does not match
                matched = true;
                break;
            }
        }

        if (!matched)
            return false;
    }

    if (dumpFilters.count(Config::FILTER::BY_NAME)) { // filter by name configured
        if (dumpFilters.at(Config::FILTER::BY_NAME) != "*" && // to have 'single char' option for matching all the names
            !std::regex_match(node->getName(), std::regex(dumpFilters.at(Config::FILTER::BY_NAME)))) // name does not match
            return false;
    }

    return true;
}

static void dump(const BlobDumper& bd, const std::string& file, const Config& config) {
    switch (config.blobDumpFormat) {
    case Config::FORMAT::BIN: {
        bd.dump(file);
        break;
    }
    case Config::FORMAT::TEXT: {
        bd.dumpAsTxt(file);
        break;
    }
    default:
        IE_THROW() << "NodeDumper: Unknown dump format";
    }
}

static void dumpInternalBlobs(const NodePtr& node, const Config& config) {
    std::string nodeName = node->getName();
    formatNodeName(nodeName);

    const auto& internalBlobs = node->getInternalBlobs();

    for (size_t i = 0; i < internalBlobs.size(); i++) {
        const auto& blb = internalBlobs[i];
        std::string file_name = NameFromType(node->getType()) + "_" + nodeName + "_blb" + std::to_string(i) + ".ieb";
        auto dump_file = config.blobDumpDir + "/#" + std::to_string(node->getExecIndex()) + "_" + file_name;

        TensorDesc desc = blb->getTensorDesc();
        if (desc.getPrecision() == Precision::BIN)
            continue;

        MemoryPtr memory = std::make_shared<Memory>(node->getEngine());
        memory->Create(MemoryDescUtils::convertToDnnlBlockedMemoryDesc(desc), blb->buffer());
        BlobDumper dumper(memory);
        dump(dumper, dump_file, config);
    }
}

void dumpInputBlobs(const NodePtr& node, const Config& config, int count) {
    if (!shouldBeDumped(node, config, "IN"))
        return;

    auto exec_order = std::to_string(node->getExecIndex());
    std::string nodeName = node->getName();
    formatNodeName(nodeName);

    auto num_ports = node->getSelectedPrimitiveDescriptor()->getConfig().inConfs.size();
    for (size_t i = 0; i < num_ports; i++) {
        auto prEdge = node->getParentEdgeAt(i);
        auto pr = prEdge->getParent();

        std::string file_name = NameFromType(node->getType()) + "_" + nodeName;
        if (count != -1)
            file_name += "_iter" + std::to_string(count);
        file_name += "_in" + std::to_string(i) + ".ieb";
        if (file_name.size() > 240)
            file_name = file_name.substr(file_name.size() - 240);

        auto dump_file = config.blobDumpDir + "/#" + exec_order + "_" + file_name;
        std::cout << "Dump inputs: " << dump_file << std::endl;

        auto& desc = prEdge->getMemory().getDesc();
        if (desc.getPrecision() == Precision::BIN)
            continue;

        BlobDumper dumper(prEdge->getMemoryPtr());
        dump(dumper, dump_file, config);
    }

    dumpInternalBlobs(node, config);
}

void dumpOutputBlobs(const NodePtr& node, const Config& config, int count) {
    if (!shouldBeDumped(node, config, "OUT"))
        return;

    auto exec_order = std::to_string(node->getExecIndex());
    std::string nodeName = node->getName();
    formatNodeName(nodeName);

    auto num_ports = node->getSelectedPrimitiveDescriptor()->getConfig().outConfs.size();
    for (size_t i = 0; i < num_ports; i++) {
        auto childEdge = node->getChildEdgeAt(i);

        std::string file_name = NameFromType(node->getType()) + "_" + nodeName;
        if (count != -1)
            file_name += "_iter" + std::to_string(count);
        file_name += "_out" + std::to_string(i) + ".ieb";
        if (file_name.size() > 240)
            file_name = file_name.substr(file_name.size() - 240);

        auto dump_file = config.blobDumpDir + "/#" + exec_order + "_" + file_name;
        std::cout << "Dump outputs:  " << dump_file << std::endl;

        auto& desc = childEdge->getMemory().getDesc();
        if (desc.getPrecision() == Precision::BIN)
            continue;

        BlobDumper dumper(childEdge->getMemoryPtr());
        dump(dumper, dump_file, config);
    }
}

}   // namespace intel_cpu
}   // namespace ov

#endif // CPU_DEBUG_CAPS
