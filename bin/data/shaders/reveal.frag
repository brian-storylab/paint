#version 330

uniform float radius;
uniform sampler2DRect image;
uniform vec2 lHands[4];
uniform vec2 rHands[4];
uniform vec2 lOffset;
uniform vec2 rOffset;
uniform vec2 res;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{
    //  Screen coordinates
    vec2 uv = gl_FragCoord.xy / res.xy;
    uv.y = uv.y;

    //  Alpha
    float alpha = 1.0;

    vec2 lPos = lHands[0] / res.xy;
    float lAlpha = alpha * (1.0 - smoothstep(radius * 0.9, radius, length(lPos - uv)));

    vec2 rPos = rHands[0] / res.xy;
    float rAlpha = alpha * (1.0 - smoothstep(radius * 0.9, radius, length(rPos - uv)));

    alpha = max(lAlpha, rAlpha);

    //  Color sampling
    vec3 imageColor;
    vec3 imageColorL = texture(image, gl_FragCoord.xy - lOffset).rgb;
    vec3 imageColorR = texture(image, gl_FragCoord.xy - rOffset).rgb;
    if(lAlpha > rAlpha)
        imageColor = imageColorL;
    else if (lAlpha < rAlpha)
        imageColor = imageColorR;
    else
        imageColor = mix(imageColorL, imageColorR, 0.5);

    fragColor = vec4(imageColor, alpha);
}