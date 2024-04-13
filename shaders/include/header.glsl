#define pi 3.14159265358979323846264338327950
#define PHI 1.618033988749894848204586834365638117720
#define PHI2 1.324717957244746
#define PHI3 1.22074408460575947536
#define PHI4 1.1673039782614187

#define sq(x) ((x)*(x))

#define nan (0.0/0.0)

uint hasi(uint x)
{ //https://nullprogram.com/blog/2018/07/31/
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

float hash(vec2 x)
{
    uvec2 i = uvec2(abs(x)+vec2(10000.0));
    return float(hasi(i.x+hasi(i.y)))/float(0xffffffffU);
}

vec2 hash2d(vec2 x)
{
    uvec2 i = uvec2(abs(x)+vec2(10000.0));
    return vec2(float(hasi(i.x+hasi(i.y)))/float(0xffffffffU),
                float(hasi(i.x+i.y<<16))/float(0xffffffffU));
}

float hash(uint i)
{
    return float(hasi(i))/float(0xffffffffU);
}

vec2 hash2d(uint i)
{
    uint h = hasi(i);
    return vec2(float(h)/float(0xffffffffU),
                float(hasi(h))/float(0xffffffffU));
}
