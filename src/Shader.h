#pragma once

#include "ofMain.h"
#include "ofxShader.h"

namespace pohy {
	typedef struct {
		int index;
		std::string name;
	} ShaderInfo;
	typedef std::pair<string, GLenum> Uniform;

	class LiveShader {
	public:
		void load(std::vector<string> directories = { "." });
		void addTextureFromFile(string filePath, string uniformName);

		void update();
		void draw(glm::ivec2 drawResolution = { ofGetWidth(), ofGetHeight() });
		void draw(unsigned int width, unsigned int height) {
			draw({ width, height });
		}

		void activate(string shaderName);
		void activate(size_t shaderIndex);
		void advance(int increment);

		void addUniformFunction(string uniformName, UniformFunction uniformFunction) {
			shaderA.addUniformFunction(uniformName, uniformFunction);
			shaderB.addUniformFunction(uniformName, uniformFunction);
		};
		void addDefineKeyword(string define) {
			shaderA.addDefineKeyword(define);
			shaderB.addDefineKeyword(define);
		}

		ShaderInfo getCurrentShaderInfo();
		std::vector<string> getAvailableShaders() {
			return availableShaders;
		}
		void setUniformTexture(string uniformName, ofTexture texture) {
			uniformTextures[uniformName] = texture;
		}
		bool hasUniformTexture(string uniformName) {
			return uniformTextures.count(uniformName) > 0;
		}

		ofEvent<ShaderInfo> onChange;

	private:
	 	double elapsedTime = 0.0;
		ofxShader shaderA;
		ofxShader shaderB;
		ofxShader* pFrontShader{ &shaderA };
		ofxShader* pBackShader{ &shaderB };

		// TODO: Should be of type size_t
		int currentShaderIndex{ 0 };
		std::vector<string> availableShaders;
		std::vector<Uniform> uniforms;
		std::map<string, ofTexture> uniformTextures;

		void loadShader(size_t index);
		void onShaderLoad(bool& e);
		void updateUniforms(glm::vec2 drawResolution);
		void parseUniforms();
		void loadAvailableShaders(std::vector<string> directories = { "." });
		string shaderProcessor(GLenum type, const string source);
	};
}
