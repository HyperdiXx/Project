#include <iostream>

#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>

#include <wrl.h>

struct InternalState_D3DCompiler
{
	using PFN_D3DCOMPILE = decltype(&D3DCompile);
	PFN_D3DCOMPILE D3DCompile = nullptr;

	InternalState_D3DCompiler()
	{
		if (D3DCompile != nullptr)
		{
			return; // already initialized
		}

		HMODULE d3dcompiler = LoadLibraryA("d3dcompiler_47.dll");
		if (d3dcompiler != nullptr)
		{
			D3DCompile = (PFN_D3DCOMPILE)GetProcAddress(d3dcompiler, "D3DCompile");
			if (D3DCompile != nullptr)
			{
				std::cout << "Loaded d3dcompiler.dll!\n";
			}
		}
	}
};

inline InternalState_D3DCompiler& d3d_compiler()
{
	static InternalState_D3DCompiler internal_state;
	return internal_state;
}

bool ReadFile(const std::string& fileName, std::vector<uint8_t>& data)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		size_t dataSize = (size_t)file.tellg();
		file.seekg(0, file.beg);
		data.resize(dataSize);
		file.read((char*)data.data(), dataSize);
		file.close();
		return true;
	}

	return false;
}

struct CompilerInput
{
	std::string shaderSource; 
	std::string shaderSourceFilename; 
	std::string entryPoint = "main";


};

struct CompilerOutput
{
	std::shared_ptr<void> internal_state;
	inline bool IsValid() const { return internal_state.get() != nullptr; }

	const uint8_t* shaderData = nullptr;
	size_t shaderSize = 0;
	std::vector<uint8_t> shaderHash;
	//std::vector<std::string> dependencies;
	std::string errorMessage = "";
};

void CompileShader();

int main(int argc, char** argv)
{
	using namespace Microsoft::WRL;

	auto& compiler = d3d_compiler();
	auto currentPath = std::filesystem::current_path();
	
	auto triangleShader = currentPath.parent_path() / + "Data" / "Shaders" / "triangle.hlsl";

	std::vector<uint8_t> shaderData = {};
	if (!ReadFile(triangleShader.string(), shaderData))
	{
		std::cout << "Failed to read shader file!\n";
		return -1;
	}

	D3D_SHADER_MACRO defines[] = 
	{
			"HLSL5", "1",
			"DISABLE_WAVE_INTRINSICS", "1",
			NULL, NULL
	};

	int flags = 0;

	CompilerInput input = {};
	CompilerOutput output = {};
	
	input.entryPoint = "vs_main";
	input.shaderSourceFilename = "triangle.hlsl";


	const char* targetVS = "vs_5_0";
	const char* targetPS = "ps_5_0";

	//switch (input.stage)
	//{
	//default:
	//case ShaderStage::MS:
	//case ShaderStage::AS:
	//case ShaderStage::LIB:
	//	// not applicable
	//	return;
	//case ShaderStage::VS:
	//	target = "vs_5_0";
	//	break;
	//case ShaderStage::HS:
	//	target = "hs_5_0";
	//	break;
	//case ShaderStage::DS:
	//	target = "ds_5_0";
	//	break;
	//case ShaderStage::GS:
	//	target = "gs_5_0";
	//	break;
	//case ShaderStage::PS:
	//	target = "ps_5_0";
	//	break;
	//case ShaderStage::CS:
	//	target = "cs_5_0";
	//	break;
	//}

	ComPtr<ID3DBlob> code = {};
	ComPtr<ID3DBlob> errors = {};

	HRESULT hr = d3d_compiler().D3DCompile(
		shaderData.data(),
		shaderData.size(),
		input.shaderSourceFilename.c_str(),
		defines,
		//&includehandler, //D3D_COMPILE_STANDARD_FILE_INCLUDE,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		input.entryPoint.c_str(),
		targetVS,
		flags,
		0,
		&code,
		&errors
	);

	if (errors)
	{
		output.errorMessage = (const char*)errors->GetBufferPointer();
	}

	if (SUCCEEDED(hr))
	{
		//output.dependencies.push_back(input.shaderSourceFilename);
		output.shaderData = (const uint8_t*)code->GetBufferPointer();
		output.shaderSize = code->GetBufferSize();

		// keep the blob alive == keep shader pointer valid!
		auto internal_state = std::make_shared<ComPtr<ID3D10Blob>>();
		*internal_state = code;
		output.internal_state = internal_state;
	}





    return 0;
}
