#pragma once

#include "gl/gl_common.hpp"

enum GpuTimeStamp
{
    GTS_Objects,
    GTS_Terrain,
    GTS_Sky,
    GTS_Shadows,
    GTS_SSAO,
    GTS_EnvMap,
    GTS_Refraction,
    GTS_Reflection,
    GTS_Max
};

class GpuProfiler
{
public:
    GpuProfiler ();
    ~GpuProfiler ();

    void Begin (GpuTimeStamp gts);
    void End (GpuTimeStamp gts);

    void EndFrame ();

    // Wait on GPU for last frame's data (not this frame's) to be available
    void WaitForDataAndUpdate ();

    float Dt (GpuTimeStamp gts)
	{ return m_adT[gts]; }
    float DtAvg (GpuTimeStamp gts)
	{ return m_adTAvg[gts]; }

protected:
    int m_iFrameQuery;
    int m_iFrameCollect;						// Which of the two did we last collect?


    GLuint m_apQueryTs[GTS_Max][2];		// Individual timestamp queries for each relevant point in the frame

    float m_adT[GTS_Max];						// Last frame's timings (each relative to previous GTS)
    float m_adTAvg[GTS_Max];					// Timings averaged over 0.5 second

    float m_adTTotalAvg[GTS_Max];				// Total timings thus far within this averaging period
    int m_frameCountAvg;						// Frames rendered in current averaging period
    float m_tBeginAvg;							// Time at which current averaging period started
};
