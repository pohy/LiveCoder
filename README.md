# LiveCoder

## Current plan
- Configurable shader directory

## Features
- Load textures
- Live config reloading
- Bind MIDI/OSC
	- Messages are normalized into the <0, 1> range
	- To uniforms
		- Automatically parse uniforms and offer them for binding
	- To shader position -> MIDI sequencing
	- Save configuration into JSON
- Import Shadertoy shaders
- Audio input sampling -> FFT texture

## Used libraries
- https://github.com/patriciogonzalezvivo/ofxShader
- https://github.com/leadedge/ofxNDI
- https://github.com/braitsch/ofxDatGui
- https://github.com/danomatika/ofxMidi
- https://github.com/nlohmann/json
