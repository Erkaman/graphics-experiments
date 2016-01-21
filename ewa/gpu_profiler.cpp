#include "gpu_profiler.hpp"

#include <chrono>
#include <ctime>
#include <string.h>

#include "log.hpp"

float Time() {
    std::clock_t startcputime = std::clock();
    //  LOG_I("startcputime: %ld", startcputime);
    float cpu_duration = (float)(startcputime) / (float)CLOCKS_PER_SEC;
    return cpu_duration;
}

GpuProfiler::GpuProfiler ()
    :	m_iFrameQuery(0),
	m_iFrameCollect(-1),
	m_frameCountAvg(0),
	m_tBeginAvg(0.0f)
{
    memset(m_apQueryTs, 0, sizeof(m_apQueryTs));
    memset(m_adT, 0, sizeof(m_adT));
    memset(m_adTAvg, 0, sizeof(m_adT));
    memset(m_adTTotalAvg, 0, sizeof(m_adT));

    // Create all the queries we'll need

    for (GpuTimeStamp gts = GTS_Objects; gts < GTS_Max; gts = GpuTimeStamp(gts + 1))
    {
	GL_C(glGenQueries(1,&m_apQueryTs[gts][0] ));
	GL_C(glGenQueries(1,&m_apQueryTs[gts][1] ));

    }
}


GpuProfiler::~GpuProfiler () {

    for (GpuTimeStamp gts = GTS_Objects; gts < GTS_Max; gts = GpuTimeStamp(gts + 1)) {
	GL_C(glDeleteQueries(1, &m_apQueryTs[gts][0]) );
	GL_C(glDeleteQueries(1, &m_apQueryTs[gts][1]) );

    }

}

void GpuProfiler::Begin (GpuTimeStamp gts) {
    GL_C(glBeginQuery(GL_TIME_ELAPSED,m_apQueryTs[gts][m_iFrameQuery]));
}


void GpuProfiler::End (GpuTimeStamp gts) {
    GL_C(glEndQuery(GL_TIME_ELAPSED));
}


void GpuProfiler::EndFrame ()
{
    ++m_iFrameQuery &= 1;
}

void GpuProfiler::WaitForDataAndUpdate ()
{
	if (m_iFrameCollect < 0)
	{
		// Haven't run enough frames yet to have data
		m_iFrameCollect = 0;
		return;
	}

	int iFrame = m_iFrameCollect;
	++m_iFrameCollect &= 1;

	for (GpuTimeStamp gts = GTS_Objects; gts < GTS_Max; gts = GpuTimeStamp(gts + 1))
	{

	    GLuint64 timer;

	    glGetQueryObjectui64v(m_apQueryTs[gts][iFrame],
                    GL_QUERY_RESULT, &timer);

		m_adT[gts] = float(timer) / float(1000.0f * 1000.0f );

		m_adTTotalAvg[gts] += m_adT[gts];
	}

	++m_frameCountAvg;
	if (Time() > m_tBeginAvg + 0.35f)
	{
	    //   LOG_I("avg");
		for (GpuTimeStamp gts = GTS_Objects; gts < GTS_Max; gts = GpuTimeStamp(gts + 1))
		{
			m_adTAvg[gts] = m_adTTotalAvg[gts] / m_frameCountAvg;
			m_adTTotalAvg[gts] = 0.0f;
		}

		m_frameCountAvg = 0;
		m_tBeginAvg = Time();
	}
}
