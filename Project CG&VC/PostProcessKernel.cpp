#include "PostProcessKernel.h"

PostProcessKernel::PostProcessKernel(Type type) : m_type(type) {
    switch (type) {
    case Type::Gaussian:
        m_kernel = {
            1.0f / 16, 2.0f / 16, 1.0f / 16,
            2.0f / 16, 4.0f / 16, 2.0f / 16,
            1.0f / 16, 2.0f / 16, 1.0f / 16
        };
        break;

    case Type::Laplacian:
        m_kernel = {
            1, 1, 1,
            1, -8, 1,
            1, 1, 1
        };
        break;

    default:
        m_kernel = {
            0, 0, 0,
            0, 1, 0,
            0, 0, 0
        };
        break;
    }
}

const float* PostProcessKernel::Data() const { 
    return m_kernel.data(); 
}

int PostProcessKernel::Size() const { 
    return static_cast<int>(m_kernel.size()); 
}

std::string PostProcessKernel::Name() const {
    switch (m_type) {
    case Type::Gaussian: return "Gaussian";
    case Type::Laplacian: return "Laplacian";
    default: return "Unknown";
    }
}