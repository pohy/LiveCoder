#include "Shader.h"
#include <regex>
#include <sstream>
#include "ShaderConverter.h"

namespace pohy {

	static GLenum stringToType(string type) {
		if (type == "float") {
			return GL_FLOAT;
		}
		else if (type == "vec2") {
			return GL_FLOAT_VEC2;
		}
		else if (type == "vec3") {
			return GL_FLOAT_VEC3;
		}
		else if (type == "vec4") {
			return GL_FLOAT_VEC4;
		}
		else if (type == "sampler2D") {
			return GL_SAMPLER_2D;
		}
		return NULL;
	}

	void LiveShader::load(std::vector<string> directories) {
		loadAvailableShaders(directories);
		//if (loadFirstShader && availableShaders.size() > 0) {
		//	loadShader(currentShaderIndex);
		//}
		pFrontShader = &shaderA;
		pBackShader = &shaderB;
		pFrontShader->disableWatchFiles();
		ofAddListener(pBackShader->onLoad, this, &LiveShader::onShaderLoad);
		ofRemoveListener(pFrontShader->onLoad, this, &LiveShader::onShaderLoad);

		auto shaderProcessor_ = [=](GLenum type, const string source) -> string {
			return shaderProcessor(type, source);
		};
		pFrontShader->setShaderProcessor(shaderProcessor_);
		pBackShader->setShaderProcessor(shaderProcessor_);
	}

	void LiveShader::draw(glm::ivec2 drawResolution) {
		pFrontShader->begin();
		updateUniforms(drawResolution);
		ofDrawRectangle(0, 0, drawResolution.x, drawResolution.y);
		pFrontShader->end();
	}

	void LiveShader::activate(size_t shaderIndex) {
		loadShader(shaderIndex);
	}
	void LiveShader::activate(string shaderName) {
		if (shaderName == "") {
			ofLogError("LiveCoder") << "activate(): Cannot load shader without a name";
			return;
		}
		for (size_t i = 0; i < availableShaders.size(); i++) {
			auto availableShader = availableShaders[i];
			auto shaderNameIndex = availableShader.rfind(shaderName);
			if (shaderNameIndex == string::npos) {
				continue;
			}
			return loadShader(i);
		}
		ofLogWarning("LiveCoder") << "activate(): Shader resembling name '" << shaderName << "' not found";
	}

	void LiveShader::advance(int increment) {
		currentShaderIndex += increment;
		int maxIndex = availableShaders.size() - 1;
		if (currentShaderIndex > maxIndex) {
			currentShaderIndex = 0;
		}
		else if (currentShaderIndex < 0) {
			currentShaderIndex = maxIndex;
		}
		loadShader(currentShaderIndex);
	}

	void LiveShader::loadShader(size_t index) {
		if (index < 0 || index > availableShaders.size() - 1) {
			ofLogError("LiveCoder") << "loadShader(): Shader index out of bounds: " << index;
			return;
		}
		currentShaderIndex = index;
		if (availableShaders.size() < 1) {
			ofLogWarning("LiveCoder") << "loadShader(): No shaders available";
			return;
		}
		shaderA.load(availableShaders[currentShaderIndex]);
		shaderB.load(availableShaders[currentShaderIndex]);

		auto shaderInfo = getCurrentShaderInfo();
		ofNotifyEvent(onChange, shaderInfo, this);
	}

	ShaderInfo LiveShader::getCurrentShaderInfo() {
		{
			auto name = availableShaders.size() > 0 ? availableShaders.at(currentShaderIndex) : "";
			return ShaderInfo({
				currentShaderIndex,
				name
				});
		}
	}

	void LiveShader::loadAvailableShaders(std::vector<string> directories)
	{
		for (auto dir : directories) {
			auto files = ofDirectory(dir).getFiles();
			for (auto file : files) {
				auto fileName = file.getFileName();
				if (file.getExtension() != "frag") {
					continue;
				}
				auto shaderName = ofFilePath::join(file.getEnclosingDirectory(), file.getBaseName());//fileName.substr(0, fragIndex);
				availableShaders.push_back(shaderName);
			}
		}
		stringstream availableShadersText;
		for (auto name : availableShaders) {
			availableShadersText << name << ", ";
		}
		ofLogNotice("Shader") << "Available shaders: " << availableShadersText.str();
	}

	string LiveShader::shaderProcessor(GLenum type, const string source) {
		auto sourceModified = source;
		if (type == GL_FRAGMENT_SHADER) {
			sourceModified = pohy::shaderconverter::addUniforms(sourceModified);
			sourceModified = pohy::shaderconverter::convertShaderToy(sourceModified);
			return sourceModified;
		}
		return sourceModified;
	}

	void LiveShader::onShaderLoad(bool& e) {
		if (!e) {
			return;
		}
		ofLogNotice("LiveCoder") << "onShaderLoad(): Reloading shader: " << availableShaders[currentShaderIndex];

		//Swap front and back shaders
		auto pBackShaderOld = pBackShader;
		pBackShader = pFrontShader;
		pFrontShader = pBackShaderOld;
		pBackShader->enableWatchFiles();
		pFrontShader->disableWatchFiles();
		ofRemoveListener(pBackShaderOld->onLoad, this, &LiveShader::onShaderLoad);
		ofAddListener(pBackShader->onLoad, this, &LiveShader::onShaderLoad);

		parseUniforms();
		//pFolderUniforms->children.clear();
		//for (auto uniform : uniforms) {
		//	if (uniform.first == "iTime" || uniform.first == "iResolution") {
		//		continue;
		//	}
		//	switch (uniform.second)
		//	{
		//	case GL_FLOAT:
		//		pFolderUniforms->addSlider(uniform.first, 0, 1);
		//		break;
		//	case GL_FLOAT_VEC2:
		//		pFolderUniforms->addSlider(uniform.first + "_x", 0, 1);
		//		pFolderUniforms->addSlider(uniform.first + "_y", 0, 1);
		//		break;
		//	default:
		//		break;
		//	}
		//}
		//if (pFolderUniforms->getIsExpanded()) {
		//	pFolderUniforms->collapse();
		//	pFolderUniforms->expand();
		//}
	}

	void LiveShader::updateUniforms(glm::vec2 drawResolution) {
		float time = ofGetElapsedTimef();
		pFrontShader->setUniform1f("iTime", time);
		pFrontShader->setUniform2f("iResolution", drawResolution.x, drawResolution.y);
		pFrontShader->setUniform4f("iMouse", ofGetMouseX(), ofGetMouseY(), ofGetMousePressed(0), ofGetMousePressed(1));
		for (auto uniformTexture : uniformTextures) {
			int textureLocation = std::distance(uniformTextures.begin(), uniformTextures.find(uniformTexture.first));
			pFrontShader->setUniformTexture(uniformTexture.first, uniformTexture.second, textureLocation);
		}
	}

	void LiveShader::parseUniforms() {
		uniforms.clear();
		auto shaderSource = pFrontShader->getShaderSource(GL_FRAGMENT_SHADER);

		std::stringstream ss(shaderSource);
		string line;
		std::vector<string> lines;
		while (std::getline(ss, line, '\n')) {
			lines.push_back(line);
		}
		for (auto line : lines) {
			std::smatch match;
			std::regex regex("uniform (\\w+) (\\w+);");

			if (!std::regex_match(line, match, regex) || match.size() < 2) {
				continue;
			}
			auto type = match[1];
			GLenum uniformType = stringToType(match[1]);
			if (uniformType == NULL) {
				ofLogError("LiveCoder") << "Unknown uniform type: " << match[1];
				continue;
			}
			uniforms.push_back(std::make_pair(match[2], uniformType));
		}
	}

}