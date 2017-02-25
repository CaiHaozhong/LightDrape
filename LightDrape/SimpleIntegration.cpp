#include "SimpleIntegration.h"
#include "ComponentForce.h"

SimpleIntegration::SimpleIntegration(void)
{
}


SimpleIntegration::~SimpleIntegration(void)
{
}

std::pair<Vec3d, Vec3d> SimpleIntegration::integrate2(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt)
{
	std::pair<Vec3d, Vec3d> ret;
	ret.second = v_t + f_t / mass * dt;
	ret.first = x_t + ret.second * dt;
	return ret;
}

// std::pair<Vec3d, Vec3d> SimpleIntegration::integrate( size_t i, const std::vector<Vec3d>& curPositions, 
// 													 const std::vector<Vec3d>& curVelocities, const std::vector<double>& pointMass, 
// 													 const std::vector<ComponentForce_> forces, double dt )
// {
// 	Vec3d f(0, 0, 0);
// 	for(auto it = forces.begin(); it != forces.end(); it++){
// 		f += (*it)->compute(i, curPositions, curVelocities, pointMass);			
// 	}
// 	std::pair<Vec3d, Vec3d> ret;
// 	ret.second = curVelocities[i] + f / pointMass[i] * dt;
// 	ret.first = curPositions[i] + ret.second * dt;
// 	return ret;
// }

void SimpleIntegration::integrate(std::vector<Vec3d>& curPositions, 
								  std::vector<Vec3d>& curVelocities, 
								  const std::vector<double>& pointMass, 
								  const Mesh_ mesh, 
								  const std::vector<ComponentForce_>& forces, 
								  double dt )
{
	size_t pointCount = curPositions.size();
	std::vector<Vec3d> f(pointCount, Vec3d(0, 0, 0));
	for(auto it = forces.begin(); it != forces.end(); it++){
		(*it)->compute(curPositions, curVelocities, pointMass, f);			
	}
	for(size_t i = 0; i < pointCount; i++){
		curVelocities[i] = curVelocities[i] + f[i] / pointMass[i] * dt;
		curPositions[i] = curPositions[i] + curVelocities[i] * dt;
	}
}
