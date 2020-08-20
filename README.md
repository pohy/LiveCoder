# LiveCoder

## Features
### In progress
- Load custom textures

### Implemented
- Configurable shader directory
- Loading ShaderToy from URL
- Import Shadertoy shaders
  - String matching trickery, we should investigate using ASTs
### Planned
- Support vertex and geometry shaders
- Generate headers from config schema
- Detect undefined uniforms (iMidi0, etc.)
- Fullscreen view in a second window
- Audio input sampling -> FFT texture
- Live config reloading
- Bind MIDI/OSC
	- Messages are normalized into the <0, 1> range
	- To uniforms
		- Automatically parse uniforms and offer them for binding
	- To shader position -> MIDI sequencing
- Save configuration into JSON
  - Current shader
  - Loaded textures for the current shader
- Multiple render passes

## Used libraries
- https://github.com/patriciogonzalezvivo/ofxShader
- https://github.com/leadedge/ofxNDI
- https://github.com/braitsch/ofxDatGui
- https://github.com/danomatika/ofxMidi
- https://github.com/nlohmann/json
