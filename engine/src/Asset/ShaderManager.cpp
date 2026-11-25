#include "pch.h"

#include "Rendering/GraphicsDevice.h"
#include "ShaderManager.h"

#include <iostream>
#include <regex>
#include <string>

namespace NXTN {
	namespace {
		const std::regex headerPattern(
			R"(^[ \t]*(INPUT|UNIFORM|OUTPUT|CODE)\s*\{)",
			std::regex_constants::ECMAScript | std::regex_constants::multiline
		);
		const std::regex variablePattern(
			R"([ \t]*(\w+)[ \t]+([A-Za-z_]\w*)[ \t]*;[ \t\r]*)"
		);
	}

	HandleMap<Shader> ShaderManager::s_Map;

	std::expected<Handle<Shader>, std::string> ShaderManager::Load(
		std::string name,
		const std::vector<ShaderStageDescriptor>& stages
	)
	{
		GraphicsDevice* device = GraphicsDevice::Get();
		if (!device)
		{
			return std::unexpected("Graphics device not initialized");
		}

		if (stages.empty())
		{
			return std::unexpected("Cannot create shader program without shader stages (" + name + ")");
		}

		std::vector<ShaderStageIntermediateDescriptor> stageIntermediates;
		for (const ShaderStageDescriptor& stage : stages) {
			std::ifstream fileStream(stage.filepath, std::ios::binary);
			if (!fileStream)
				return std::unexpected("Cannot open shader at " + stage.filepath);

			std::string sourceText(
				std::istreambuf_iterator<char>{fileStream},
				std::istreambuf_iterator<char>{}
			);

			if (sourceText.empty())
				return std::unexpected("Empty shader at " + stage.filepath);

			// Replace comments
			size_t pos = 0;
			size_t line = 1;
			while (pos < sourceText.size()) {
				const unsigned char c = static_cast<unsigned char>(sourceText[pos]);
				if (c == '\n') {
					pos++;
					line++;
				}
				else if (c == '/') {
					if (pos + 1 < sourceText.size()) {
						const unsigned char next = static_cast<unsigned char>(sourceText[pos + 1]);
						if (next == '/') {
							// Line comment
							size_t endPos = sourceText.find('\n', pos + 2);
							if (endPos == std::string::npos) {
								sourceText.erase(pos);
								break;
							}
							else {
								sourceText.erase(pos, endPos - pos);
							}
						}
						else if (next == '*') {
							// Block comment
							size_t endPos = sourceText.find("*/", pos + 2);
							if (endPos == std::string::npos) {
								return std::unexpected(
									"Unterminated block comment at line " +
									std::to_string(line) +
									" in shader at " +
									stage.filepath
								);
							}
							else {
								sourceText.replace(pos, endPos - pos + 2, 1, ' ');
							}
						}
						else {
							pos++;
						}
					}
					else {
						pos++;
					}
				}
				else {
					pos++;
				}
			}

			// Parse sections / generate intermediates
			stageIntermediates.push_back(ShaderStageIntermediateDescriptor());
			ShaderStageIntermediateDescriptor& intermediate = stageIntermediates.back();
			intermediate.stage = stage.stage;

			std::sregex_iterator end{};
			for (std::sregex_iterator section_itr(
				sourceText.begin(),
				sourceText.end(),
				headerPattern
			); section_itr != end; section_itr++)
			{
				const std::smatch& sectionMatch = *section_itr;

				std::string sectionType = sectionMatch[1].str();

				size_t contentStart = sectionMatch.position() + sectionMatch[0].length();

				const std::sregex_iterator& next = std::next(section_itr);

				size_t contentEnd =
					(next == end)
					? sourceText.size()
					: next->position();

				std::string sectionContent = sourceText.substr(
					contentStart,
					contentEnd - contentStart
				);
				const std::size_t sectionEnd = sectionContent.find_last_not_of(" \t\r\n");
				if (sectionEnd == std::string_view::npos || sectionContent[sectionEnd] != '}') {
					return std::unexpected(
						"Unterminated section (" + sectionType + ") in shader at " + stage.filepath
					);
				}
				sectionContent = sectionContent.substr(0, sectionEnd);
				if (sectionType == "CODE") {
					if (sectionContent.empty()) {
						return std::unexpected(
							"Empty section (" + sectionType + ") in shader at " + stage.filepath
						);
					}
					intermediate.code = sectionContent;
				}
				else {
					std::vector<ShaderVariableDescriptor>* variables = nullptr;
					if (sectionType == "INPUT") {
						variables = &intermediate.inputs;
					}
					else if (sectionType == "UNIFORM") {
						variables = &intermediate.uniforms;
					}
					else if (sectionType == "OUTPUT") {
						variables = &intermediate.outputs;
					}
					else {
						return std::unexpected(
							"I don't even know how this is possible..."
						);
					}
					// Parse variables
					for (std::sregex_iterator var_itr(
						sectionContent.begin(),
						sectionContent.end(),
						variablePattern
					); var_itr != end; var_itr++)
					{
						const std::smatch& variableMatch = *var_itr;
						// Type
						if (variableMatch[1] == "int") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Int);
						}
						else if (variableMatch[1] == "uint") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::UInt);
						}
						else if (variableMatch[1] == "float") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Float);
						}
						else if (variableMatch[1] == "float2") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Float2);
						}
						else if (variableMatch[1] == "float3") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Float3);
						}
						else if (variableMatch[1] == "float4" || variableMatch[1] == "Color") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Float4);
						}
						else if (variableMatch[1] == "mat4") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Mat4);
						}
						else if (variableMatch[1] == "bool") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Bool);
						}
						else if (variableMatch[1] == "Tex2D") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::Tex2D);
						}
						else if (variableMatch[1] == "TexCube") {
							variables->emplace_back(variableMatch[2], ShaderVariableType::TexCube);
						}
						else {
							return std::unexpected(
								"Unknown variable type (" +
								variableMatch[1].str() +
								") in shader at " +
								stage.filepath
							);
						}
					}
				}
			}
			if (intermediate.code.empty()) {
				return std::unexpected(
					"Missing CODE section in shader at " + stage.filepath
				);
			}
		}
		std::expected<Shader*, std::string> result = device->CreateShader(stageIntermediates);

		if (result) {
			return s_Map.Add(result.value());
		}
		return std::unexpected(result.error() + " (" + name + ")");
	}

	Shader* ShaderManager::Get(Handle<Shader> sh) {
		return s_Map.Get(sh);
	}

	bool ShaderManager::Validate(Handle<Shader> sh) {
		return s_Map.Validate(sh);
	}

	void ShaderManager::Shutdown()
	{
		s_Map.Clear();
	}
}
