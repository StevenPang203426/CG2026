#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    Vector3f N = hit.getNormal().normalized();
    Vector3f L = dirToLight.normalized();
    float NdotL = Vector3f::dot(N, L);

    Vector3f result(0, 0, 0);

    if (NdotL > 0) {
        // Diffuse term
        result += NdotL * (_diffuseColor * lightIntensity);

        // Specular term
        Vector3f R = (2.0f * NdotL * N - L).normalized();
        Vector3f E = (-ray.getDirection()).normalized();
        float RdotE = Vector3f::dot(R, E);
        if (RdotE > 0) {
            result += pow(RdotE, _shininess) * (_specularColor * lightIntensity);
        }
    }

    return result;
}
