#version 420 core

layout (location = 0) out float FragmentDepth;

void main()
{
    FragmentDepth = gl_FragCoord.z;
}
