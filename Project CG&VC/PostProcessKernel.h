#pragma once

#include <vector>
#include <string>

class PostProcessKernel {
public:
	enum class Type {
		Default,
		Gaussian,
		Laplacian
	};

	PostProcessKernel(Type type);

	const float* Data() const;
	int Size() const;

private:
	std::vector<float> m_kernel;
	Type m_type;

	std::string Name() const;
};