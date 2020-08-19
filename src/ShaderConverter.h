#pragma once
#include <string>

namespace pohy {
	namespace shaderconverter {
		string convertShaderToy(string shaderSource) {
			auto hasMainImageFn = shaderSource.find("mainImage(") != string::npos;
			if (!hasMainImageFn) {
				return shaderSource;
			}
			auto sourceModified = shaderSource;
			auto lastPreprocessorIndex = sourceModified.rfind("#define");
			auto lastPreprocessorNewLineIndex = sourceModified.find("\n", lastPreprocessorIndex);
			if (lastPreprocessorNewLineIndex == string::npos) {
				return sourceModified;
			}
			if (sourceModified.find("out vec4 outputColor") == string::npos) {
				sourceModified.insert(lastPreprocessorNewLineIndex, "\nout vec4 outputColor;\n");
			}
			sourceModified += "void main() { mainImage( outputColor, gl_FragCoord.xy ); }";

			return sourceModified;
		}

		string addUniforms(string source) {
			auto sourceModified = source;
			std::map<string, string> defaultUniforms = { {"iTime", "float"}, {"iResolution", "vec2"}, {"iMouse", "vec4"} };
			for (size_t i = 0; i < 4; i++) {
				stringstream channelName;
				channelName << "iChannel" << i;
				defaultUniforms.insert(std::make_pair(channelName.str(), "sampler2D"));
			}
			auto lastPreprocessorIndex = sourceModified.rfind("#define");
			auto lastPreprocessorNewLineIndex = sourceModified.find("\n", lastPreprocessorIndex);
			for (auto defaultUniform : defaultUniforms) {
				auto uniformPos = std::strstr(source.c_str(), defaultUniform.first.c_str());
				if (uniformPos == nullptr) {
					// Shader is not using the uniform
					continue;
				}
				// TODO: We should use AST, and not this string matching hackery
				auto uniformDeclaration = "uniform " + defaultUniform.second + " " + defaultUniform.first;
				auto hasUniformDeclaration = std::strstr(source.c_str(), uniformDeclaration.c_str());
				if (hasUniformDeclaration) {
					// Shader has uniform declaration
					continue;
				}
				// If the uniform is used in the source, add its declaration
				uniformDeclaration = "\n" + uniformDeclaration + ";";
				sourceModified.insert(lastPreprocessorNewLineIndex, uniformDeclaration);
			}
			// TODO: We are skipping the ofxShader initialization and passing the modified source directly to ofShader
			sourceModified.insert(lastPreprocessorIndex, "\nprecision mediump float;\n");
			sourceModified = "#version 330\n" + sourceModified;
			return sourceModified;
		}
	}
}
