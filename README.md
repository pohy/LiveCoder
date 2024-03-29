# LiveCoder
## Build
- CMD/PowerShell:  
  `msbuild -m -p:"Configuration=Debug;Platform=x64" .\LiveCoder.vcxproj`
- git-bash:  
  `cd bin`  
  `MSBuild.exe -m MSBuild.exe -m -p:"Configuration=Debug;Platform=x64" ../LiveCoder.vcxproj && ./LiveCoder_debug.exe`

## TODO
- ShaderToy import GUI
- Display shader errors on screen
- Document configuration
  - JSON Schema?
- Display successful reload toast/notice
- Display compiling toast/notice
- Option to disable NDI
- Quit confirm dialog
  - Only when NDI is enabled and listeners are connected?
  - Configurable
- Shader directory reload/watch
- Config class
  - So that we don't have to use string for access :)
  - Could it be generated from JSON Schema?
- UI Scaling
  - Fork ofxDatGui and modify scale multiplication
  - Refer `ofxDatGuiTheme::init` method
  - Fix button scaling
    - Aka. `ofxDatGuiComponent::drawLabel`
- ?Drop MIDI support
- ✔ Pause time
  - Using space
- ✔ ShaderToy format detection not working for custom shaders
  - Should be based on detecting `mainImage`?
- ✔ Option to toggle vertical sync
  => Removed vertical sync
- ✔ FPS runs slow for some reason
  - Even when disabling both the frame rate cap and vertical sync and NDI
  => Was just a GUI issue
- ✔ Disable quit using escape key
- ✔ Uniforms GUI not working
  => Disabled for now
- ✔ Shader list closes after activation
- ✔ Remember and restore last opened shader
  - Has to have a fallback when the shader doesn't exist

## Features
- Live reloading (won't stop when the shader has errors)
- Use your own editor
- Texture loading (GUI/Config)
- NDI stream
- MIDI input
- JSON config
- ShaderToy import
## Doc
- Shaders are stored inside `data` directory
  - They have to have `.frag` extension
- Use left/right arrows to navigate between shaders
### In progress
- Texture UI

### Implemented
- Load custom textures
- Configurable shader directory
- Loading ShaderToy from URL
- Import Shadertoy shaders
  - [ ] String matching trickery, we should investigate using ASTs
### Planned
- Load last used shader on startup
- Explore `ofParamter` for binding dynamic values (texture/config/uniforms)
- Detect undefined uniforms (iMidi0, etc.)
- Shadertoy scraping, when API is not enabled for the post
- Fullscreen view in a second window
- Audio input sampling -> FFT texture
- Support vertex and geometry shaders
- Generate headers from config schema
- Bind MIDI/OSC
	- [x] Messages are normalized into the <0, 1> range
	- [x] To uniforms
	- [x] Automatically parse uniforms
	- Custom uniform <-> MIDI/OSC mapping
	- To shader position -> MIDI sequencing
- Save configuration into JSON
  - Auto reloading
  - Current shader
  - Loaded textures for the current shader
  - Uniform <-> MIDI mapping
- Multiple render passes

## Used libraries
- https://github.com/patriciogonzalezvivo/ofxShader
- https://github.com/leadedge/ofxNDI
- https://github.com/braitsch/ofxDatGui
- https://github.com/danomatika/ofxMidi
- https://github.com/nlohmann/json
