#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
#include <vector>
#include "Vec3d.h"
#include "Common.h"

typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;

/*
 * 具有包围盒的三角网格
 * 每个顶点可以动态增添属性
 */

class PropertyManager{
	struct CustomProperties{
		std::vector<Vec3d> vecP;
		std::vector<bool> boolP;
		std::vector<double> doubleP;
	};
	std::vector<CustomProperties> mPropertyList;
	std::vector<size_t> mAvailableVecIndex;
	std::vector<size_t> mAvailableBoolIndex;
	std::vector<size_t> mAvailableDoubleIndex;
	size_t mVertexCount;
public:
	PropertyManager(){
		init(0);
	}
	PropertyManager(size_t vertexCount){
		init(vertexCount);
	}

	void init(size_t vertexCount){
		mPropertyList.resize(vertexCount);
		mVertexCount = vertexCount;
	}
	
	/* 请求一个可用的属性位置，必要时分配内存 */
	size_t requestVecIndex(){
		if(mVertexCount == 0){
			return -1;
		}
		if(mAvailableVecIndex.empty()){
			for(size_t i = 0; i < mVertexCount; i++){
				mPropertyList[i].vecP.push_back(Vec3d());
			}
			return mPropertyList[0].vecP.size()-1;
		}
		else{
			size_t ret = mAvailableVecIndex.back();
			mAvailableVecIndex.pop_back();
			return ret;
		}
	}

	/* 释放下标为i的内存，即让i下次可以通过requestVecIndex返回 */
	void releaseVecIndex(size_t i){
		mAvailableVecIndex.push_back(i);
	}

	size_t requestDoubleIndex(){
		if(mVertexCount == 0){
			return -1;
		}
		if(mAvailableDoubleIndex.empty()){
			for(size_t i = 0; i < mVertexCount; i++){
				mPropertyList[i].doubleP.push_back(double());
			}
			return mPropertyList[0].doubleP.size()-1;
		}
		else{
			size_t ret = mAvailableDoubleIndex.back();
			mAvailableDoubleIndex.pop_back();
			return ret;
		}
	}

	void releaseDoubleIndex(size_t i){
		mAvailableDoubleIndex.push_back(i);
	}

	size_t requestBoolIndex(){
		/* TO DO */
		return -1;
	}

	void releaseBoolIndex(size_t i) {
		mAvailableBoolIndex.push_back(i);
	}

	Vec3d getVec(size_t vertex, size_t internalIndex){
		return mPropertyList[vertex].vecP[internalIndex];
	}

	void setVec(size_t vertex, size_t internalIndex, const Vec3d& vec){
		mPropertyList[vertex].vecP[internalIndex] = vec;
	}
};
S_PTR(PropertyManager);
class Registerable{
protected:
	size_t mInternalIndex;
	PropertyManager_ mPropertyManager;

public:
	/* 不要手动调用这个函数 */
	void registerSelf(PropertyManager_ pm, size_t internalIndex){
		mPropertyManager = pm;
		mInternalIndex = internalIndex;
	}
	virtual ~Registerable(){
		mPropertyManager->releaseVecIndex(mInternalIndex);
	}
};
class Vec3dProperty : public Registerable{
public:
	Vec3d get(size_t index) const {
		return mPropertyManager->getVec(index, mInternalIndex);
	}

	void set(size_t index, const Vec3d& vec){
		mPropertyManager->setVec(index, mInternalIndex, vec);
	}
};
class Mesh : public _Mesh, public std::enable_shared_from_this<Mesh> {
public:
    S_PTR(Mesh);
private:

public:

public:
// 	/* Mesh Type */
// 	static const int TYPE_HUMAN = 0;
// 	static const int TYPE_CLOTH = 1;
// 	static const int TYPE_PANT = 2;
// 	static const int TYPE_SKIRT = 3;
// 	static const int TYPE_UNKNOWN = 4;

	Mesh();

	~Mesh();

	/** 当模型变形之后，需要重新计算包围盒，则调用这个 **/
	void requestRecomputeAABB();

	/** 计算包围盒 **/
	void requestAABB();	

	Vec3d getMin();

	Vec3d getMax();

	/** 获取包围盒对角线的长度 **/
	float getDigonalLen();

	/* x 轴 */
	float getWidth();

	/* y 轴 */
	float getHeight();

	/* z 轴 */
	float getLength();
	
	/* 新增一个属性 */
	void registerVec3dProperty(Vec3dProperty& vp){
		vp.registerSelf(mPropertyManager, mPropertyManager->requestVecIndex());
	}

private:
	bool mHasRequestAABB;

	Vec3d mMinPoint, mMaxPoint;

	PropertyManager_ mPropertyManager;

};
S_PTR(Mesh);