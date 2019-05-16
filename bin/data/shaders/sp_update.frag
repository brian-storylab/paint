#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;
uniform sampler2DRect particles2;

uniform vec2 lHands[4];
uniform vec2 rHands[4];
uniform vec2 resolution;
uniform float radiusSquared;
uniform float elapsed;
uniform float sense;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;
layout(location = 2) out vec4 colOut;

void main()
{
    vec3 pos = texture(particles0, texCoordVarying.st).xyz;
    vec3 vel = texture(particles1, texCoordVarying.st).xyz;
    vec3 col = texture(particles2, texCoordVarying.st).xyz;

    //  mass
    float mass =  12.0 * col.z;
	mass = clamp(mass, 3.0, 12.0) + pos.z;

    // attraction
    for(int i = 0; i < 4; ++i)
    {
        if(lHands[i].x == -1.0) continue;

        vec2 direction = lHands[i] - pos.xy;
        float distSquared = dot(direction, direction);
        float magnitude = 2000.0 * (1.0 - distSquared / radiusSquared) / mass;
        vec2 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
        vel.xy += sense * elapsed * force;
    }

    for(int i = 0; i < 4; ++i)
    {
        if(rHands[i].x == -1.0) continue;

        vec2 direction = rHands[i] - pos.xy;
        float distSquared = dot(direction, direction);
        float magnitude = 2000.0 * (1.0 - distSquared / radiusSquared) / mass;
        vec2 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
        vel.xy += sense * elapsed * force;
    }
    
    // bounce off the sides
    if(pos.x > resolution.x || pos.x < 0) vel.x *= -1.0;
    if(pos.y > resolution.y || pos.y < 0) vel.y *= -1.0;
    
    // damping
    vel *= 0.995;
    vel.x = sign(vel.x) * clamp(abs(vel.x), 0.0, 125.0);
    vel.y = sign(vel.y) * clamp(abs(vel.y), 0.0, 125.0);

    // move
    pos += elapsed * vel;

    //  alpha
    float alpha = 0.5 * 0.01 * length(vel);

    posOut = vec4(pos, 1.0);
    velOut = vec4(vel, 0.0);
    colOut = vec4(col, alpha);
}