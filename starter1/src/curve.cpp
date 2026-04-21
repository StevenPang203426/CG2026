#include "curve.h"
#include "vertexrecorder.h"
using namespace std;

const float c_pi = 3.14159265358979323846f;

namespace
{
// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vector3f& lhs, const Vector3f& rhs)
{
	const float eps = 1e-8f;
	return (lhs - rhs).absSquared() < eps;
}

// Compute a single Bezier segment (p0,p1,p2,p3), appending to 'curve'.
// If 'firstSeg' is true, include t=0; otherwise start from t=1/steps.
// prevB is the binormal from the previous point (updated in-place).
// firstEver: whether this is the very first point of the whole curve.
void evalBezierSegment(
    const Vector3f& p0, const Vector3f& p1,
    const Vector3f& p2, const Vector3f& p3,
    unsigned steps, bool firstSeg,
    Vector3f& prevB, bool& firstEver,
    Curve& curve)
{
    unsigned start = firstSeg ? 0 : 1;
    for (unsigned i = start; i <= steps; i++) {
        float t = float(i) / float(steps);
        float mt = 1.0f - t;

        // Position via Bernstein basis
        Vector3f V = mt*mt*mt*p0
                   + 3.0f*mt*mt*t*p1
                   + 3.0f*mt*t*t*p2
                   + t*t*t*p3;

        // Tangent (derivative)
        Vector3f dV = 3.0f*(mt*mt*(p1-p0)
                           + 2.0f*mt*t*(p2-p1)
                           + t*t*(p3-p2));
        Vector3f T = dV.normalized();

        // Initialize prevB on the very first point
        if (firstEver) {
            Vector3f arb(0.0f, 0.0f, 1.0f);
            if (fabs(Vector3f::dot(arb, T)) > 0.99f)
                arb = Vector3f(0.0f, 1.0f, 0.0f);
            // Gram-Schmidt: project arb onto the plane perpendicular to T.
            // For a flat xy-plane curve (T.z=0), this gives prevB=(0,0,1),
            // matching evalCircle's convention where N is in-plane and B points along z.
            prevB = (arb - Vector3f::dot(arb, T) * T).normalized();
            firstEver = false;
        }

        Vector3f N = Vector3f::cross(prevB, T).normalized();
        Vector3f B = Vector3f::cross(T, N).normalized();
        prevB = B;

        CurvePoint cp;
        cp.V = V; cp.T = T; cp.N = N; cp.B = B;
        curve.push_back(cp);
    }
}

// Closed-curve frame correction.
//
// Problem: parallel transport along a closed curve can accumulate a net twist
// alpha (the angle between first.N and last.N around T). Without correction,
// GenCyl surfaces have a visible seam where the first and last rings meet.
//
// Solution (only applied when the curve is truly closed):
//   1. Detect closure: positions AND tangents must match at both ends.
//   2. Measure alpha = signed angle from first.N to last.N around T
//      (positive = CCW when viewed from +T).
//   3. Distribute a counter-rotation of -alpha linearly over all frames:
//        theta_i = -alpha * i / (n-1)
//      so point 0 gets 0 correction and point n-1 gets exactly -alpha,
//      making last.N_corrected == first.N.
//
// Rodrigues rotation of N and B around local T by angle theta
// (since N⊥T and B⊥T, the (k·v) term vanishes):
//   N_rot = cos(θ)·N + sin(θ)·(T×N) = cos(θ)·N + sin(θ)·B
//   B_rot = cos(θ)·B + sin(θ)·(T×B) = cos(θ)·B - sin(θ)·N
//
// Non-closed curves are unchanged (closure check returns early).
void fixClosedCurve(Curve& curve)
{
    if (curve.size() < 2) return;

    const CurvePoint& first = curve.front();
    const CurvePoint& last  = curve.back();

    // Closure test: both position and tangent must coincide at the endpoints.
    const float eps = 1e-4f;
    if ((first.V - last.V).absSquared() > eps) return;
    if (fabs(Vector3f::dot(first.T, last.T) - 1.0f) > eps) return;

    // Compute the signed twist angle alpha = angle from first.N to last.N
    // measured around first.T (right-hand rule).
    float cosA = Vector3f::dot(first.N, last.N);
    cosA = fmax(-1.0f, fmin(1.0f, cosA));  // clamp for numerical safety
    float sinA = Vector3f::dot(Vector3f::cross(first.N, last.N), first.T);
    float alpha = atan2(sinA, cosA);

    if (fabs(alpha) < 1e-6f) return;  // frames already aligned, nothing to do

    int total = (int)curve.size();
    for (int i = 0; i < total; i++) {
        // Counter-rotation: -alpha distributed linearly.
        // At i=0: theta=0 (unchanged). At i=total-1: theta=-alpha (cancels twist).
        float theta = -alpha * float(i) / float(total - 1);
        float cosT = cos(theta), sinT = sin(theta);

        Vector3f Ni = curve[i].N;
        Vector3f Bi = curve[i].B;

        // Rodrigues around T: N_rot = cos·N + sin·B,  B_rot = -sin·N + cos·B
        curve[i].N =  cosT * Ni + sinT * Bi;
        curve[i].B = -sinT * Ni + cosT * Bi;
    }
}

} // namespace


Curve evalBezier(const vector< Vector3f >& P, unsigned steps)
{
	// Check
	if (P.size() < 4 || P.size() % 3 != 1)
	{
		cerr << "evalBezier must be called with 3n+1 control points." << endl;
		exit(0);
	}

    int numSegments = (int)(P.size() - 1) / 3;
    Curve curve;
    curve.reserve(numSegments * steps + 1);

    Vector3f prevB;
    bool firstEver = true;

    for (int seg = 0; seg < numSegments; seg++) {
        evalBezierSegment(
            P[3*seg], P[3*seg+1], P[3*seg+2], P[3*seg+3],
            steps, (seg == 0),
            prevB, firstEver, curve);
    }

    fixClosedCurve(curve);
	return curve;
}

Curve evalBspline(const vector< Vector3f >& P, unsigned steps)
{
	// Check
	if (P.size() < 4)
	{
		cerr << "evalBspline must be called with 4 or more control points." << endl;
		exit(0);
	}

    // Convert each B-spline segment to Bezier control points, then call evalBezier.
    // For segment i with B-spline points b0=P[i], b1=P[i+1], b2=P[i+2], b3=P[i+3]:
    //   c0 = (b0 + 4*b1 + b2) / 6      (shared endpoint with previous segment's c3)
    //   c1 = (2*b1 + b2) / 3
    //   c2 = (b1 + 2*b2) / 3
    //   c3 = (b1 + 4*b2 + b3) / 6      (= next segment's c0)
    int n = (int)P.size() - 3; // number of B-spline segments
    vector<Vector3f> bezPts;
    bezPts.reserve(3*n + 1);

    for (int i = 0; i < n; i++) {
        const Vector3f& b0 = P[i];
        const Vector3f& b1 = P[i+1];
        const Vector3f& b2 = P[i+2];
        const Vector3f& b3 = P[i+3];

        Vector3f c0 = (b0 + 4.0f*b1 + b2) / 6.0f;
        Vector3f c1 = (2.0f*b1 + b2) / 3.0f;
        Vector3f c2 = (b1 + 2.0f*b2) / 3.0f;
        Vector3f c3 = (b1 + 4.0f*b2 + b3) / 6.0f;

        if (i == 0) bezPts.push_back(c0);
        bezPts.push_back(c1);
        bezPts.push_back(c2);
        bezPts.push_back(c3);
    }

	return evalBezier(bezPts, steps);
}

Curve evalCircle(float radius, unsigned steps)
{
	// This is a sample function on how to properly initialize a Curve
	// (which is a vector< CurvePoint >).

	// Preallocate a curve with steps+1 CurvePoints
	Curve R(steps + 1);

	// Fill it in counterclockwise
	for (unsigned i = 0; i <= steps; ++i)
	{
		// step from 0 to 2pi
		float t = 2.0f * c_pi * float(i) / steps;

		// Initialize position
		// We're pivoting counterclockwise around the y-axis
		R[i].V = radius * Vector3f(cos(t), sin(t), 0);

		// Tangent vector is first derivative
		R[i].T = Vector3f(-sin(t), cos(t), 0);

		// Normal vector is second derivative
		R[i].N = Vector3f(-cos(t), -sin(t), 0);

		// Finally, binormal is facing up.
		R[i].B = Vector3f(0, 0, 1);
	}

	return R;
}

void recordCurve(const Curve& curve, VertexRecorder* recorder)
{
	const Vector3f WHITE(1, 1, 1);
	for (int i = 0; i < (int)curve.size() - 1; ++i)
	{
		recorder->record_poscolor(curve[i].V, WHITE);
		recorder->record_poscolor(curve[i + 1].V, WHITE);
	}
}
void recordCurveFrames(const Curve& curve, VertexRecorder* recorder, float framesize)
{
	Matrix4f T;
	const Vector3f RED(1, 0, 0);
	const Vector3f GREEN(0, 1, 0);
	const Vector3f BLUE(0, 0, 1);

	const Vector4f ORGN(0, 0, 0, 1);
	const Vector4f AXISX(framesize, 0, 0, 1);
	const Vector4f AXISY(0, framesize, 0, 1);
	const Vector4f AXISZ(0, 0, framesize, 1);

	for (int i = 0; i < (int)curve.size(); ++i)
	{
		T.setCol(0, Vector4f(curve[i].N, 0));
		T.setCol(1, Vector4f(curve[i].B, 0));
		T.setCol(2, Vector4f(curve[i].T, 0));
		T.setCol(3, Vector4f(curve[i].V, 1));

		// Transform orthogonal frames into model space
		Vector4f MORGN  = T * ORGN;
		Vector4f MAXISX = T * AXISX;
		Vector4f MAXISY = T * AXISY;
		Vector4f MAXISZ = T * AXISZ;

		// Record in model space
		recorder->record_poscolor(MORGN.xyz(), RED);
		recorder->record_poscolor(MAXISX.xyz(), RED);

		recorder->record_poscolor(MORGN.xyz(), GREEN);
		recorder->record_poscolor(MAXISY.xyz(), GREEN);

		recorder->record_poscolor(MORGN.xyz(), BLUE);
		recorder->record_poscolor(MAXISZ.xyz(), BLUE);
	}
}
