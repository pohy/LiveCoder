# LiveCoder
## Build
`msbuild -m -p:"Configuration=Release;Platform=x64" .\LiveCoder.vcxproj`

## Features
- Live reloading (won't stop when the shader has errors)
- Use your own editor
- Texture loading
- NDI stream
- MIDI input
- JSON config
- ShaderToy import
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
