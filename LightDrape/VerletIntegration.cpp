#include "VerletIntegration.h"
#include "ComponentForce.h"
VerletIntegration::VerletIntegration()
{
	mHasLasPositionPointCount = 0;
}

VerletIntegration::VerletIntegration( size_t pointSize )
{
	mHasLasPositionPointCount = 0;
	setPointSize(pointSize);
}

VerletIntegration::~VerletIntegration(void)
{
}

std::pair<Vec3d, Vec3d> VerletIntegration::integrate2( const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt )
{
	std::pair<Vec3d, Vec3d> ret;

	return ret;
}

// std::pair<Vec3d, Vec3d> VerletIntegration::integrate( size_t i, const std::vector<Vec3d>& curPositions, 
// 													 const std::vector<Vec3d>& curVelocities, 
// 													 const std::vector<double>& pointMass,
// 													 const std::vector<ComponentForce_> forces, double dt )
// {	
// 	Vec3d f(0, 0, 0);	
// 	for(auto it = forces.begin(); it != forces.end(); it++){
// 		f += (*it)->compute(i, curPositions, curVelocities, pointMass);			
// 	}
// 	std::pair<Vec3d, Vec3d> ret;
// 	size_t pointCount = curPositions.size();
// 	const Vec3d& v_t = curVelocities[i];
// 	const Vec3d& x_t = curPositions[i];
// 	double mass = pointMass[i];
// 	Vec3d a = f / mass;
// 	if(mHasLasPositionPointCount < pointCount){		
// 		ret.first = x_t + v_t * dt + 0.5 * a * dt * dt;
// 		//ret.second = v_t + a * dt;			
// 		ret.second = (ret.first - x_t) / dt;
// 	}
// 	else{
// 		ret.first = x_t * 2 - mLastPostion[i] + a * dt * dt;
// 		ret.second = (ret.first - x_t) / dt;
// 	}
// 	mLastPostion[i] = x_t;
// 	mHasLasPositionPointCount += 1;
// 	return ret;
// }

void VerletIntegration::setPointSize(size_t pointSize)
{
	mLastPostion.resize(pointSize);
}
