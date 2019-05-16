#version 330

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

int iterations = 8;
float formuparam = 0.89;
int volsteps = 7;
float stepsize = 0.1783;
float zoom = 2.08;
float tile = 0.77;
float brightness = 0.19;
float darkmatter = 0.37;
float distfading = 0.58;
float saturation = 0.85;
float transverseSpeed = 0.5;
float cloud = 0.10;
float FX1 = 0.0;

uniform float iTime;
uniform vec2 iMouse;
uniform vec2 iResolution;

out vec4 fragColor;

vec3 hash33(vec3 p) {
    float n = sin(dot(p, vec3(7, 157, 113)));
    return fract(vec3(2097152, 262144, 32768)*n)*2. - 1.; // return fract(vec3(64, 8, 1)*32768.0*n)*2.-1.;
    
}

float noise(in vec3 p)
{
    // Skewing the cubic grid, then determining the first vertice and fractional position.
    vec3 i = floor(p + dot(p, vec3(0.333333333333)) );  p -= i - dot(i, vec3(0.16666666666)) ;
    
    // Breaking the skewed cube into tetrahedra with partitioning planes, then determining which side of the
    // intersecting planes the skewed point is on. Ie: Determining which tetrahedron the point is in.
    vec3 i1 = step(p.yzx, p), i2 = max(i1, 1.0 - i1.zxy); i1 = min(i1, 1.0 - i1.zxy);
    
    // Using the above to calculate the other three vertices. Now we have all four tetrahedral vertices.
    // Technically, these are the vectors from "p" to the vertices, but you know what I mean. :)
    vec3 p1 = p - i1 + 0.16666666666, p2 = p - i2 + 0.333333333333, p3 = p - 0.5;
    
    
    // 3D simplex falloff - based on the squared distance from the fractional position "p" within the
    // tetrahedron to the four vertice points of the tetrahedron.
    vec4 v = max(0.5 - vec4(dot(p,p), dot(p1,p1), dot(p2,p2), dot(p3,p3)), 0.0);
    
    // Dotting the fractional position with a random vector generated for each corner -in order to determine
    // the weighted contribution distribution... Kind of. Just for the record, you can do a non-gradient, value
    // version that works almost as well.
    vec4 d = vec4(dot(p, hash33(i)), dot(p1, hash33(i + i1)), dot(p2, hash33(i + i2)), dot(p3, hash33(i + 1.0)));
    
    
    // Simplex noise... Not really, but close enough. :)
    return clamp(dot(d, v*v*v * 8.0) * 1.732 + 0.5, 0.0, 1.0); // Not sure if clamping is necessary. Might be overkill.
}

float field(vec3 p)
{
    float time = iTime;
    float strength = 12.0 - sin(time * 0.0005),
    power = 2.3 + sin(time * 0.003),
    accum = 0.0,
    prev = 0.0,
    tw = 0.0;
    p *= 0.5 + noise(p * zoom);
    
    for ( int i = 0; i < 8; i++ )
    {
        float mag = dot( p, p ),
		      w = exp( -float( i ) / 7.0 );
        
        p = abs( p ) / mag + vec3( -0.5, -0.4, -1.5 );
        accum += w * exp( -strength * pow( abs( mag - prev ), power ) );
        tw += w;
        prev = mag;
    }
    
    return max( 0.0, 5.0 * accum / tw - 0.7 );
}

void main()
{
    //  Normalize screen coords from -1 to 1
    vec2 uv = 2.0 * gl_FragCoord.xy / max(iResolution.x, iResolution.y) - 1.0;
    vec2 centroidUV = 2.0 * iMouse / iResolution - 1.0;
    //  Calculate speed
    float speed = 0.005 * transverseSpeed * cos(iTime * 0.02 + PI * 0.25);
    //  Rotation
    float a_xz =  centroidUV.y;
    float a_yz =  centroidUV.x;
    float a_xy =  0.9 + iTime * 0.005;
    //  Rotation matrices
    mat2 rot_xz = mat2(cos(a_xz),sin(a_xz),-sin(a_xz),cos(a_xz));
    mat2 rot_yz = mat2(cos(a_yz),sin(a_yz),-sin(a_yz),cos(a_yz));
    mat2 rot_xy = mat2(cos(a_xy),sin(a_xy),-sin(a_xy),cos(a_xy));
    //  Direction (from origin pointing out of screen)
    float zoom_factor = 1.2 * sin(iTime * 0.1);
    zoom_factor = clamp(zoom_factor, -1.0, 1.0);
    vec3 dir = vec3(uv * (zoom + zoom_factor), 1.);
    //  Origin
    vec3 origin = vec3(0.0, 0.0, 0.0);
    origin.x -= 5.0 * sin(iTime * 0.001);
    origin.y -= 5.0 * cos(iTime * 0.001);
    //  Define forward direction
    vec3 forward = vec3(0.0, 0.0, 1.0);
    //  Rotate
    dir.xy*=rot_xy;
    forward.xy *= rot_xy;
    
    dir.xz*=rot_xz;
    forward.xz *= rot_xz;
    
    dir.yz*= rot_yz;
    forward.yz *= rot_yz;
    
    //  Zoom
    float zooom = -iTime * speed;
    origin += forward * zooom;
    float sampleShift = mod(zooom, stepsize);
    
    float zoffset = -sampleShift;
    sampleShift /= stepsize; // make from 0 to 1
    
    //volumetric rendering
    float s = 0.12;
    float s3 = s + stepsize * 0.5;
    vec3 v = vec3(0.0);
    float t3 = 0.0;
    
    //  Weird noise effects
    //dir *= 0.5 + noise(dir);
    float from_noise = noise(dir);
    float df = mix(0.5, from_noise, FX1);
    
    origin *= 0.5 + df;
    
    vec3 backCol2 = vec3(0.);
    for (int r=0; r<volsteps; r++) {
        vec3 p2=origin+(s+zoffset)*dir;
        vec3 p3=(origin+(s3+zoffset)*dir )* (1.9/zoom);
        
        p2 = abs(vec3(tile)-mod(p2,vec3(tile*2.))); // tiling fold
        p3 = abs(vec3(tile)-mod(p3,vec3(tile*2.))); // tiling fold
        t3 = field(p3);
        
        float pa,a=pa=0.;
        for (int i=0; i<iterations; i++) {
            //p2=abs(p2)/dot(p2,p2)-formuparam; // the magic formula
            p2=abs(p2) / max(dot(p2, p2), 0.005) - formuparam;
            //  Hubble effect
            //p2 *= 0.5 + 0.5 * noise(p2);
            float D = abs(length(p2) - pa);
            
            if (i > 2)
            {
                a += i > 7 ? min( 12.0, D) : D;
            }
            pa=length(p2);
        }
        
        
        float dm=max(0.,darkmatter-a*a*.001); //dark matter
        a*=a*a; // add contrast
        //if (r>3) fade*=1.-dm; // dark matter, don't render near
        // brightens stuff up a bit
        float s1 = s+zoffset;
        // need closed form expression for this, now that we shift samples
        float fade = pow(distfading,max(0.,float(r) - sampleShift));
        
        
        //t3 += fade;
        
        v+=fade;
        //backCol2 -= fade;
        
        // fade out samples as they approach the camera
        if( r == 0 )
            fade *= (1. - (sampleShift));
        // fade in samples as they approach from the distance
        if( r == volsteps-1 )
            fade *= sampleShift;
        v+=vec3(s1,s1*s1,s1*s1*s1*s1)*a*brightness*fade; // coloring based on distance
        
        backCol2 += mix(.4, 1., 1.0) * vec3(0.20 * t3 * t3 * t3, 0.1 * t3 * t3 * t3, t3 * 0.7) * fade;
        
        
        s+=stepsize;
        s3 += stepsize;
        
        
        
    }
    
    v=mix(vec3(length(v)),v,saturation);
    vec4 forCol2 = vec4(v*.01,1.);
    
    backCol2 *= cloud;
    
    fragColor = forCol2 + vec4(backCol2, 1.0);
}
