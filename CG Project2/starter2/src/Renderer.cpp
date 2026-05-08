#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>
#include <cstdlib>
#include <cmath>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
    int w = _args.width;
    int h = _args.height;
    Camera* cam = _scene.getCamera();

    // Determine render resolution (upscale for filter)
    int k = _args.filter ? 3 : 1;
    int rw = w * k;
    int rh = h * k;

    Image renderImage(rw, rh);
    Image nimage(w, h);
    Image dimage(w, h);

    int numSamples = _args.jitter ? 16 : 1;
    float pixelW = 2.0f / (rw - 1.0f);
    float pixelH = 2.0f / (rh - 1.0f);

    for (int y = 0; y < rh; ++y) {
        float ndcy = 2.0f * (y / (rh - 1.0f)) - 1.0f;
        for (int x = 0; x < rw; ++x) {
            float ndcx = 2.0f * (x / (rw - 1.0f)) - 1.0f;

            Vector3f color(0, 0, 0);
            Vector3f normalAccum(0, 0, 0);
            float depthAccum = 0;

            for (int s = 0; s < numSamples; ++s) {
                float jx = ndcx;
                float jy = ndcy;
                if (_args.jitter) {
                    // Random offset within [-0.5, 0.5] pixel width
                    jx += ((float)rand() / RAND_MAX - 0.5f) * pixelW;
                    jy += ((float)rand() / RAND_MAX - 0.5f) * pixelH;
                }
                Ray r = cam->generateRay(Vector2f(jx, jy));
                Hit hit;
                color += traceRay(r, cam->getTMin(), _args.bounces, hit);
                normalAccum += hit.getNormal();
                depthAccum += hit.getT();
            }

            color = color / (float)numSamples;
            renderImage.setPixel(x, y, color);

            // Normal and depth only at output resolution (non-upscaled coords)
            if (k == 1) {
                nimage.setPixel(x, y, (normalAccum / (float)numSamples + 1.0f) / 2.0f);
                float range = (_args.depth_max - _args.depth_min);
                if (range) {
                    dimage.setPixel(x, y, Vector3f((depthAccum / (float)numSamples - _args.depth_min) / range));
                }
            }
        }
    }

    // Downsample with Gaussian filter if enabled
    Image image(w, h);
    if (_args.filter) {
        // 3x3 Gaussian kernel (sigma=1), weights: 1 2 1 / 2 4 2 / 1 2 1, sum=16
        float kernel[3][3] = {
            {1.0f/16, 2.0f/16, 1.0f/16},
            {2.0f/16, 4.0f/16, 2.0f/16},
            {1.0f/16, 2.0f/16, 1.0f/16}
        };
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Vector3f sum(0, 0, 0);
                // Center of the k x k block
                int cx = x * k + k / 2;
                int cy = y * k + k / 2;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int sx = std::min(std::max(cx + dx, 0), rw - 1);
                        int sy = std::min(std::max(cy + dy, 0), rh - 1);
                        sum += kernel[dy + 1][dx + 1] * renderImage.getPixel(sx, sy);
                    }
                }
                image.setPixel(x, y, sum);
            }
        }
    } else {
        image = renderImage;
    }

    // save the files
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}



Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    if (_scene.getGroup()->intersect(r, tmin, h)) {
        Material *material = h.getMaterial();
        Vector3f hitPoint = r.pointAtParameter(h.getT());

        // Ambient light
        Vector3f color = _scene.getAmbientLight() * material->getDiffuseColor();

        // Iterate over all lights
        for (int i = 0; i < _scene.lights.size(); ++i) {
            Vector3f toLight, intensity;
            float distToLight;
            _scene.lights[i]->getIllumination(hitPoint, toLight, intensity, distToLight);

            // Shadow ray
            if (_args.shadows) {
                Ray shadowRay(hitPoint + 0.001f * toLight, toLight);
                Hit shadowHit;
                if (_scene.getGroup()->intersect(shadowRay, 0.0f, shadowHit)) {
                    if (shadowHit.getT() < distToLight) {
                        continue; // in shadow, skip this light
                    }
                }
            }

            color += material->shade(r, h, toLight, intensity);
        }

        // Recursive reflection
        if (bounces > 0) {
            Vector3f N = h.getNormal().normalized();
            Vector3f d = r.getDirection().normalized();
            Vector3f R = (d - 2.0f * Vector3f::dot(d, N) * N).normalized();
            Ray reflectRay(hitPoint + 0.001f * R, R);
            Hit reflectHit;
            Vector3f reflectColor = traceRay(reflectRay, 0.0f, bounces - 1, reflectHit);
            color += material->getSpecularColor() * reflectColor;
        }

        return color;
    } else {
        return _scene.getBackgroundColor(r.getDirection());
    }
}

