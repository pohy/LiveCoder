#pragma once
#include <string>

namespace pohy {
	namespace shaderconverter {
		string convertShaderToy(string shaderSource) {
			auto mainFnIndex = shaderSource.find("main(");
			if (mainFnIndex != string::npos) {
				return shaderSource;
			}
			auto mainImageFnIndex = shaderSource.find("mainImage(");
			if (mainImageFnIndex == string::npos) {
				return shaderSource;
			}
			auto mainImageFnLineStartIndex = shaderSource.rfind("\n", mainImageFnIndex);
			if (mainImageFnLineStartIndex == string::npos) {
				return shaderSource;
			}
			auto sourceModified = shaderSource;
			if (sourceModified.find("out vec4 outputColor") == string::npos) {
				// Insert the outputColor declaration before the mainImage function
				sourceModified.insert(mainImageFnLineStartIndex, "\nout vec4 outputColor;\n");
			}
			sourceModified += "void main() { mainImage( outputColor, vec2(gl_FragCoord.x, iResolution.y - gl_FragCoord.y) ); }";
			return sourceModified;
		}

		string addUniforms(string source) {
			auto sourceModified = source;
			std::map<string, string> defaultUniforms = { {"iTime", "float"}, {"iResolution", "vec2"}, {"iMouse", "vec4"} };
			for (size_t i = 0; i < 4; i++) {
				stringstream channelName;
				channelName << "iChannel" << i;
				// TODO: Sometimes, we get "unknown uniform type sampler2D"
				//		 `sampler2DRect` does not work with `texelFetch`
				defaultUniforms.insert(std::make_pair(channelName.str(), "sampler2D"));
			}
			auto lastPreprocessorIndex = sourceModified.rfind("#define");
			if (lastPreprocessorIndex == string::npos) {
				lastPreprocessorIndex = 0;
			}
			auto lastPreprocessorNewLineIndex = lastPreprocessorIndex == 0 ? 0 : sourceModified.find("\n", lastPreprocessorIndex);
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
				uniformDeclaration = "\n" + uniformDeclaration + ";\n";
				sourceModified.insert(lastPreprocessorNewLineIndex, uniformDeclaration);
			}
			// TODO: We are skipping the ofxShader initialization and passing the modified source directly to ofShader
			sourceModified.insert(lastPreprocessorIndex, "\nprecision mediump float;\n");
			//sourceModified = "#version 330\n" + sourceModified;
			return sourceModified;
		}
	}
}
