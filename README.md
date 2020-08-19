# LiveCoder

## Features
### In progress
- Loading ShaderToy from URL
- Load custom textures

### Implemented
- Import Shadertoy shaders
  - String matching trickery, we should investigate using ASTs
### Planned
- Detect undefined uniforms (iMidi0, etc.)
- Import Shadertoy shaders
- Fullscreen view in a second window
- Audio input sampling -> FFT texture
- Configurable shader directory
- Live config reloading
- Bind MIDI/OSC
	- Messages are normalized into the <0, 1> range
	- To uniforms
		- Automatically parse uniforms and offer them for binding
	- To shader position -> MIDI sequencing
- Save configuration into JSON

## Used libraries
- https://github.com/patriciogonzalezvivo/ofxShader
- https://github.com/leadedge/ofxNDI
- https://github.com/braitsch/ofxDatGui
- https://github.com/danomatika/ofxMidi
- https://github.com/nlohmann/json
