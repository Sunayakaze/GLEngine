#version 450
#pragma optimize(off)
#pragma debug(on)

//	precision highp float;
//	precision highp int;
//	precision highp sampler2D;
//	precision highp sampler3D;

layout(location = 01) uniform vec2      iResolution;                  // viewport resolution (in pixels)
layout(location = 02) uniform float     iGlobalTime;                  // shader playback time (in seconds)
layout(location = 03) uniform vec4      iMouse;                       // mouse pixel coords
layout(location = 04) uniform vec4      iDate;                        // (year, month, day, time in seconds)
layout(location = 05) uniform float     iSampleRate;                  // sound sample rate (i.e., 44100)
layout(location = 06) uniform vec3      iChannelResolution[4];        // channel resolution (in pixels)
layout(location = 10) uniform float     iChannelTime[4];              // channel playback time (in sec)
layout(location = 14) uniform vec2      ifFragCoordOffsetUniform;     // used for tiled based hq rendering
layout(location = 15) uniform float     iTimeDelta;                   // render time (in seconds)
layout(location = 16) uniform int       iFrame;                       // shader playback frame

layout(location = 20) uniform sampler2D	iChannel0;
layout(location = 21) uniform sampler2D	iChannel1;
layout(location = 22) uniform sampler2D	iChannel2;
layout(location = 23) uniform sampler2D	iChannel3;

//	Output
out vec4 								Color0;

//- SHADER PROGRAM ------------------------------------------------------------
	
void main()
{	
	vec2 st = gl_FragCoord.xy / iResolution.xy;
/*	
	if((st.x < 0.5) && (st.y > 0.5)) {
		Color0 = texture2D(iChannel0, st * 2.0 - vec2(0.0, 1.0));
	}
	
	if((st.x > 0.5) && (st.y > 0.5)) {
		Color0 = texture2D(iChannel1, st * 2.0 - vec2(1.0, 1.0));
	}
	
	if((st.x < 0.5) && (st.y < 0.5)) {
		Color0 = texture2D(iChannel2, st * 2.0 - vec2(0.0, 0.0));
	}
	
	if((st.x > 0.5) && (st.y < 0.5)) {
		Color0 = texture2D(iChannel3, st * 2.0 - vec2(1.0, 0.0));
	}
*/
	Color0 = texture2D(iChannel0, st);
}
	
//- SHADER END ----------------------------------------------------------------
