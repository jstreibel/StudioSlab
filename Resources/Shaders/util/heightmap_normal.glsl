

vec3 computeNormal(sampler2D tex2d, vec2 texCoord, vec2 texelSize, float scale)
{
    vec2 dx=vec2(texelSize.x, 0), dy=vec2(0, texelSize.y);
    float fN = texture2D(tex2d, texCoord+dy).r;
    float fS = texture2D(tex2d, texCoord-dy).r;
    float fE = texture2D(tex2d, texCoord+dx).r;
    float fW = texture2D(tex2d, texCoord-dx).r;

    float slopeX = (fE - fW) * scale;
    float slopeY = (fN - fS) * scale;

    return normalize(vec3(-slopeX, -slopeY, 1.0));
}
