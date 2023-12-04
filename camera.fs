#version 330 core
    out vec4 FragColor;
    in vec3 faceColor;
    in vec2 TexCoord;

    uniform sampler2D mytexture;

    void main()
    {
       //FragColor = vec4(faceColor,1.0);
       FragColor = texture(mytexture,TexCoord);
    }