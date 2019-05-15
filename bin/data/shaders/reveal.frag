#version 330

uniform float radius;
uniform sampler2DRect image;
uniform vec2 lHands[4];
uniform vec2 rHands[4];
uniform vec2 offset;
uniform vec2 lOffset;
uniform vec2 rOffset;
uniform vec2 res;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{
    //  Screen coordinates
    vec2 uv = gl_FragCoord.xy / res.xy;
    uv.x *= res.x / res.y;

    //  Alpha
    float alpha = 1.0;
    float maxAlpha = 0.0;

    for(int i = 0; i < 4; ++i)
    {
        if(lHands[i].x == -1.0) continue;

        vec2 lPos = lHands[i] / res.xy;
        lPos.x *= res.x / res.y;
        maxAlpha = max(maxAlpha, 1.0 - smoothstep(0.0, radius, length(lPos - uv)));
    }

    for(int i = 0; i < 4; ++i)
    {
        if(rHands[i].x == -1.0) continue;

        vec2 rPos = rHands[i] / res.xy;
        rPos.x *= res.x / res.y;
        maxAlpha = max(maxAlpha, 1.0 - smoothstep(0.0, radius, length(rPos - uv)));
    }

    alpha = maxAlpha;

    //  Color sampling
    vec3 imageColor;
    vec3 imageColorL = texture(image, gl_FragCoord.xy - lOffset).rgb;
    vec3 imageColorR = texture(image, gl_FragCoord.xy - rOffset).rgb;
    imageColor = mix(imageColorL, imageColorR, 0.5);

    fragColor = vec4(imageColor, alpha);
}