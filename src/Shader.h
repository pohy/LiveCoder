#pragma once

#include "ofMain.h"
#include "ofxShader.h"

namespace pohy {
	typedef struct {
		int index;
		std::string name;
	} ShaderInfo;
	typedef int GLSLType;
	typedef std::pair<string, GLSLType> Uniform;

	class LiveShader {
	public:
		LiveShader();
		void load(string directory = ".");
		void draw(glm::ivec2 drawResolution = { ofGetWidth(), ofGetHeight() });
		void draw(unsigned int width, unsigned int height) {
			draw({ width, height });
		}
		void activate(string shaderName);
		void activate(size_t shaderIndex);
		void advance(int increment);
		ShaderInfo getCurrentShaderInfo();
		void addUniformFunction(string uniformName, UniformFunction uniformFunction) {
			shaderA.addUniformFunction(uniformName, uniformFunction);
			shaderB.addUniformFunction(uniformName, uniformFunction);
		};

		ofEvent<ShaderInfo> onChange;

		std::vector<string> getAvailableShaders() {
			return availableShaders;
		}

	private:
		ofxShader* pFrontShader;
		ofxShader* pBackShader;
		ofxShader shaderA;
		ofxShader shaderB;

		std::vector<string> availableShaders;
		int currentShaderIndex;
		std::vector<Uniform> uniforms;

		void loadShader(size_t index);
		void onShaderLoad(bool& e);
		void parseUniforms();
		void loadAvailableShaders();
	};
}
