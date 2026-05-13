#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    Vector3f N = hit.getNormal().normalized();
    Vector3f L = dirToLight.normalized();
    Vector3f E = (-ray.getDirection()).normalized();

    Vector3f result(0, 0, 0);

    // Only compute shading when light is on the front side (L·N > 0)
    float NdotL = Vector3f::dot(N, L);
    if (NdotL > 0) {
        // Diffuse: I_diffuse = clamp(L·N) * lightIntensity * k_diffuse
        result += NdotL * (lightIntensity * _diffuseColor);
        /*
        Vector3f R = (2.0f * NdotL * N - L).normalized();
        Vector3f E = (-ray.getDirection()).normalized();
        float RdotE = Vector3f::dot(R, E);
        */
        // Specular: R = 2(E·N)N - E, I_specular = clamp(R·L)^s * lightIntensity * k_specular
        float EdotN = Vector3f::dot(E, N);
        Vector3f R = (2.0f * EdotN * N - E).normalized();
        float RdotL = Vector3f::dot(R, L);
        if (RdotL > 0) {
            result += pow(RdotL, _shininess) * (lightIntensity * _specularColor);
        }
    }

    return result;
}
