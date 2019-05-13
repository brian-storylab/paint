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
    uv.x *= res.x / res.y;

    //  Alpha
    float alpha = 1.0;

    vec2 lPos = lHands[0] / res.xy;
    lPos.x *= res.x / res.y;
    float lAlpha = alpha * (1.0 - smoothstep(radius * 0.9, radius, length(lPos - uv)));

    vec2 rPos = rHands[0] / res.xy;
    rPos.x *= res.x / res.y;
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

    //  Draw shapes
    float dist;
    if(lAlpha > rAlpha) 
        dist = sqrt(dot(uv - lPos - vec2(0.04, 0.04), uv - lPos - vec2(0.04, 0.04)));
    else
        dist = sqrt(dot(uv - rPos - vec2(0.04, 0.04), uv - rPos - vec2(0.04, 0.04)));

    float brightness = 1.0 + smoothstep(0.1, 0.1+0.05, dist) 
                - smoothstep(0.1-0.05, 0.1, dist);
    imageColor += brightness * vec3(0.898, 0.290, 0.196);

    fragColor = vec4(imageColor, alpha);
}