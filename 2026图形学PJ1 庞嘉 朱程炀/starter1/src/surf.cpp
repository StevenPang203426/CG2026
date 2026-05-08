#include "surf.h"
#include "vertexrecorder.h"
#include <cmath>
using namespace std;

static const float s_pi = 3.14159265358979323846f;

namespace
{
    // We're only implementing swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;

        return true;
    }

    // Build triangle faces connecting two successive "rings" of vertices.
    // ring0 starts at index base0, ring1 starts at base1.
    // n_profile = number of profile points per ring.
    // Triangles follow the slide convention: ACB and BCD
    // A=(i,j), B=(i+1,j), C=(i,j+1), D=(i+1,j+1)
    // face order: (A,C,B) then (B,C,D) for CCW outward normals.
    void addFaces(Surface& surface, int base0, int base1, int n_profile)
    {
        for (int i = 0; i < n_profile - 1; i++) {
            unsigned A = base0 + i;
            unsigned B = base0 + i + 1;
            unsigned C = base1 + i;
            unsigned D = base1 + i + 1;
            surface.VF.push_back(Tup3u(A, C, B));
            surface.VF.push_back(Tup3u(B, C, D));
        }
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    int n = (int)profile.size();

    // Generate vertices and normals for each rotation angle j (0..steps)
    // and profile point i (0..n-1).
    // Rotation around y-axis by theta = 2*pi*j/steps.
    // R_y(theta) * (x, y, 0) = (x*cos, y, -x*sin)
    // Normal: negate the rotated profile normal (so it faces outward).
    for (unsigned j = 0; j <= steps; j++) {
        float theta = 2.0f * s_pi * float(j) / float(steps);
        float cosT = cosf(theta);
        float sinT = sinf(theta);

        for (int i = 0; i < n; i++) {
            float px = profile[i].V[0];
            float py = profile[i].V[1];
            float nx = profile[i].N[0];
            float ny = profile[i].N[1];

            Vector3f wV(px * cosT, py, -px * sinT);
            // Negate to get outward-facing surface normal
            Vector3f wN(-nx * cosT, -ny, nx * sinT);

            surface.VV.push_back(wV);
            surface.VN.push_back(wN.normalized());
        }
    }

    // Generate triangle faces connecting successive rotation rings
    for (unsigned j = 0; j < steps; j++) {
        int base0 = j * n;
        int base1 = (j + 1) * n;
        addFaces(surface, base0, base1, n);
    }

    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    int n_profile = (int)profile.size();
    int n_sweep   = (int)sweep.size();

    // For each sweep point j, transform each profile point i by the local frame.
    // The frame at sweep[j]: N_j, B_j, T_j (columns of rotation), V_j (translation).
    // world_V = px * N_j + py * B_j + V_j
    // world_N = -(nx * N_j + ny * B_j)   (negate for outward normal)
    for (int j = 0; j < n_sweep; j++) {
        const Vector3f& Nj = sweep[j].N;
        const Vector3f& Bj = sweep[j].B;
        const Vector3f& Vj = sweep[j].V;

        for (int i = 0; i < n_profile; i++) {
            float px = profile[i].V[0];
            float py = profile[i].V[1];
            float nx = profile[i].N[0];
            float ny = profile[i].N[1];

            Vector3f wV = px * Nj + py * Bj + Vj;
            Vector3f wN = -(nx * Nj + ny * Bj);

            surface.VV.push_back(wV);
            surface.VN.push_back(wN.normalized());
        }
    }

    // Generate triangle faces connecting successive sweep rings
    for (int j = 0; j < n_sweep - 1; j++) {
        int base0 = j * n_profile;
        int base1 = (j + 1) * n_profile;
        addFaces(surface, base0, base1, n_profile);
    }

    return surface;
}

void recordSurface(const Surface &surface, VertexRecorder* recorder) {
	const Vector3f WIRECOLOR(0.4f, 0.4f, 0.4f);
    for (int i=0; i<(int)surface.VF.size(); i++)
    {
		recorder->record(surface.VV[surface.VF[i][0]], surface.VN[surface.VF[i][0]], WIRECOLOR);
		recorder->record(surface.VV[surface.VF[i][1]], surface.VN[surface.VF[i][1]], WIRECOLOR);
		recorder->record(surface.VV[surface.VF[i][2]], surface.VN[surface.VF[i][2]], WIRECOLOR);
    }
}

void recordNormals(const Surface &surface, VertexRecorder* recorder, float len)
{
	const Vector3f NORMALCOLOR(0, 1, 1);
    for (int i=0; i<(int)surface.VV.size(); i++)
    {
		recorder->record_poscolor(surface.VV[i], NORMALCOLOR);
		recorder->record_poscolor(surface.VV[i] + surface.VN[i] * len, NORMALCOLOR);
    }
}

void outputObjFile(ostream &out, const Surface &surface)
{

    for (int i=0; i<(int)surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (int i=0; i<(int)surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (int i=0; i<(int)surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
