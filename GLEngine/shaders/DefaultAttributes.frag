#version 450
#pragma optimize(off)
//	#pragma debug(on)

//	Output
layout(location = 0) out vec4 	Color0;

in vec4 color;

void main()
{
	Color0 = color;
}
