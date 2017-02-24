#include "RungeKuttaIntegration.h"


RungeKuttaIntegration::RungeKuttaIntegration(void)
{
}


RungeKuttaIntegration::~RungeKuttaIntegration(void)
{
}

std::pair<Vec3d, Vec3d> RungeKuttaIntegration::integrate2( const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt )
{
	std::pair<Vec3d, Vec3d> ret;
	Vec3d v_half_dt = v_t + f_t / mass * dt / 2;
	return ret;
}

std::pair<Vec3d, Vec3d> RungeKuttaIntegration::integrate( size_t i, const std::vector<Vec3d>& curPositions, 
														 const std::vector<Vec3d>& curVelocities, const std::vector<double>& pointMass,
														 const std::vector<ComponentForce_> forces, double dt )
{
	std::pair<Vec3d, Vec3d> ret;

	return ret;
}
