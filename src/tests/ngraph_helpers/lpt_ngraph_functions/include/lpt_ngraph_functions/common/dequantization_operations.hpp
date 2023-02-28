// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <ngraph/ngraph.hpp>
#include "fake_quantize_on_data.hpp"

namespace ngraph {
namespace builder {
namespace subgraph {

class DequantizationOperations {
public:
    class Convert {
    public:
        Convert();
        Convert(const ngraph::element::Type outPrecision, const bool toRemove = true);
        bool empty() const noexcept;
        bool equal(const DequantizationOperations::Convert& value) const noexcept;
        bool operator==(const Convert& value) const noexcept {
            return equal(value);
        }

        ngraph::element::Type outPrecision = element::undefined;
        bool addDequantizationAttribute = true;
    private:
        bool isEmpty;
    };

    class Subtract {
    public:
        Subtract();
        Subtract(const float value, const bool toRemove = true);
        Subtract(const std::vector<float>& values);
        Subtract(const std::vector<float>& values, const ngraph::element::Type outPrecision);
        Subtract(
            const std::vector<float>& values,
            const ngraph::element::Type outPrecision,
            const ngraph::Shape& constantShape,
            const bool toRemove = false,
            const size_t constantIndex = 1ul,
            const ngraph::element::Type constantPrecision = ngraph::element::undefined,
            const bool addConvert = false,
            const ov::Node::RTMap& attributes = {},
            const ov::Node::RTMap& convertAttributes = {});
        bool empty() const noexcept;
        bool equal(const DequantizationOperations::Subtract& value) const noexcept;
        bool operator==(const Subtract& value) const noexcept {
            return equal(value);
        }
        void erase() {
            isEmpty = true;
        }
        Subtract& setConstantPrecision(const ngraph::element::Type& precision);

        std::vector<float> values;
        ngraph::element::Type outPrecision = ngraph::element::undefined;
        ngraph::Shape constantShape;
        bool constantShapeIsDefined = false;
        size_t constantIndex = 1ul;
        ngraph::element::Type constantPrecision = ngraph::element::undefined;
        bool addConvert = false;
        ov::Node::RTMap attributes;
        ov::Node::RTMap convertAttributes;

    private:
        bool isEmpty;
    };

    class Multiply {
    public:
        Multiply();
        Multiply(const float value);
        Multiply(const std::vector<float>& values);
        Multiply(const std::vector<float>& values, const ngraph::element::Type outPrecision);
        Multiply(
            const std::vector<float>& values,
            const ngraph::element::Type outPrecision,
            const ngraph::Shape& constantShape,
            const bool toRemove = false,
            const size_t constantIndex = 1ul,
            const ngraph::element::Type constantPrecision = ngraph::element::undefined);
        bool empty() const noexcept;
        bool equal(const DequantizationOperations::Multiply& value) const noexcept;
        bool operator==(const Multiply& value) const noexcept {
            return equal(value);
        }
        Multiply& setConstantPrecision(const ngraph::element::Type& precision);

        std::vector<float> values;
        ngraph::element::Type outPrecision = ngraph::element::undefined;
        ngraph::Shape constantShape;
        bool constantShapeIsDefined = false;
        size_t constantIndex = 1ul;
        ngraph::element::Type constantPrecision = ngraph::element::undefined;

    private:
        bool isEmpty;
    };

    DequantizationOperations();

    DequantizationOperations(const Convert& convert, const Subtract& subtract, const Multiply& multiply);

    bool empty() const noexcept;
    bool equal(const DequantizationOperations& value) const noexcept;
    bool operator==(const DequantizationOperations& value) const noexcept {
        return equal(value);
    }
    void setPrecision(const ngraph::element::Type& type) noexcept;

    Convert convert;
    Subtract subtract;
    Multiply multiply;
};

inline std::ostream& operator<<(std::ostream& out, const DequantizationOperations::Convert& convert) {
    if (convert.empty()) {
        return out << "{}";
    }
    return out << "_" << (convert.outPrecision != element::undefined ? convert.outPrecision.get_type_name() : "");
}

inline std::ostream& operator<<(std::ostream& out, const DequantizationOperations::Subtract& subtract) {
    if (subtract.empty()) {
        return out << "{}";
    }
    return out << "_" <<
        subtract.values << "_" <<
        subtract.outPrecision << "_" <<
        subtract.constantShape << "_" <<
        subtract.constantShapeIsDefined << "_" <<
        subtract.constantIndex << "_" <<
        subtract.constantPrecision << "_" <<
        subtract.addConvert;
}

inline std::ostream& operator<<(std::ostream& out, const DequantizationOperations::Multiply& multiply) {
    if (multiply.empty()) {
        return out << "{}";
    }
    return out << "_" <<
        multiply.values << "_" <<
        multiply.outPrecision << "_" <<
        multiply.constantShape << "_" <<
        multiply.constantShapeIsDefined << "_" <<
        multiply.constantIndex << "_" <<
        multiply.constantPrecision;
}

inline std::ostream& operator<<(std::ostream& out, const DequantizationOperations& data) {
    if (data.empty()) {
        return out << "{}";
    }
    return out << "_" << data.convert << "_" << data.subtract << "_" << data.multiply;
}

}  // namespace subgraph
}  // namespace builder
}  // namespace ngraph
