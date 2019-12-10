#version 450
#pragma optimize(off)
#pragma debug(on)

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp sampler3D;

layout(location = 01) uniform vec2 iResolution;
layout(location = 02) uniform float iGlobalTime;
layout(location = 20) uniform sampler2D	iChannel0;
layout(location = 21) uniform sampler2D	iChannel1;
layout(location = 22) uniform sampler2D	iChannel2;
layout(location = 23) uniform sampler2D	iChannel3;
layout(location = 24) uniform sampler2D	iChannel4;
layout(location = 25) uniform sampler2D	iChannel5;
layout(location = 26) uniform sampler2D	iChannel6;
layout(location = 27) uniform sampler2D	iChannel7;
layout(location = 28) uniform sampler2D	iChannel8;
layout(location = 29) uniform sampler2D	iChannel9;

//	Output
out vec4 								Color0;

void main()
{
	vec2 st = gl_FragCoord.xy / iResolution.xy;
	
	if((st.x < 0.5) && (st.y > 0.5)) {
		Color0 = texture2D(iChannel0, st * 2.0);
	}
	
	if((st.x > 0.5) && (st.y > 0.5)) {
		Color0 = texture2D(iChannel1, st * 2.0);
	}
	
	if((st.x < 0.5) && (st.y < 0.5)) {
		Color0 = texture2D(iChannel2, st * 2.0);
	}
	
	if((st.x > 0.5) && (st.y < 0.5)) {
		Color0 = texture2D(iChannel3, st * 2.0);
	}
}