#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
#include <vector>
#include "Vec3d.h"
#include "Common.h"
#include <OpenMesh/Core/Mesh/Traits.hh>
struct MyTrait : public OpenMesh::DefaultTraits{
  /// The default coordinate type is OpenMesh::Vec3f.
  typedef Vec3d  Point;

  /// The default normal type is OpenMesh::Vec3f.
  typedef Vec3d  Normal;

};
typedef OpenMesh::TriMesh_ArrayKernelT<MyTrait> _Mesh;


class PropertyManager{
	struct CustomProperties{
		std::vector<Vec3d> vecP;
		std::vector<bool> boolP;
		std::vector<double> doubleP;
		std::vector<int> intP; 
	};
	std::vector<CustomProperties> mPropertyList;
	std::vector<size_t> mAvailableVecIndex;
	std::vector<size_t> mAvailableBoolIndex;
	std::vector<size_t> mAvailableDoubleIndex;
	std::vector<size_t> mAvailableIntIndex;
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

	double getDouble(size_t vertex, size_t internalIndex){
		return mPropertyList[vertex].doubleP[internalIndex];
	}

	void setDouble(size_t vertex, size_t internalIndex, const double d){
		mPropertyList[vertex].doubleP[internalIndex] = d;
	}

	size_t requestIntIndex(){
		if(mVertexCount == 0){
			return -1;
		}
		if(mAvailableIntIndex.empty()){
			for(size_t i = 0; i < mVertexCount; i++){
				mPropertyList[i].intP.push_back(double());
			}
			return mPropertyList[0].intP.size()-1;
		}
		else{
			size_t ret = mAvailableIntIndex.back();
			mAvailableIntIndex.pop_back();
			return ret;
		}
	}

	void releaseIntIndex(size_t i){
		mAvailableIntIndex.push_back(i);
	}

	double getInt(size_t vertex, size_t internalIndex){
		return mPropertyList[vertex].intP[internalIndex];
	}

	void setInt(size_t vertex, size_t internalIndex, const double d){
		mPropertyList[vertex].intP[internalIndex] = d;
	}
};
S_PTR(PropertyManager);

/* 通过为Mesh的顶点新增属性 */
class Registerable{
protected:
	size_t mInternalIndex;
	PropertyManager_ mPropertyManager;

public:
	/* 不要手动调用这个函数 */
	void registerSelf(PropertyManager_ pm){
		mPropertyManager = pm;
		requestInternalIndex();
	}
	virtual void requestInternalIndex() = 0;
	virtual ~Registerable(){}
};
S_PTR(Registerable);

/* 三维向量的属性 */
class Vec3dProperty : public Registerable{
protected:
	void requestInternalIndex(){
		mInternalIndex = mPropertyManager->requestVecIndex();
	}
public:
	Vec3d get(size_t index) const {
		return mPropertyManager->getVec(index, mInternalIndex);
	}

	void set(size_t index, const Vec3d& vec){
		mPropertyManager->setVec(index, mInternalIndex, vec);
	}
	~Vec3dProperty(){
		mPropertyManager->releaseVecIndex(mInternalIndex);
	}
};
S_PTR(Vec3dProperty);

/* 浮点数的属性 */
class DoubleProperty : public Registerable{
protected:
	void requestInternalIndex(){
		mInternalIndex = mPropertyManager->requestDoubleIndex();
	}
public:
	double get(size_t index) const{
		return mPropertyManager->getDouble(index, mInternalIndex);
	}
	void set(size_t index, const double& d){
		mPropertyManager->setDouble(index, mInternalIndex, d);
	}
	~DoubleProperty(){
		mPropertyManager->releaseDoubleIndex(mInternalIndex);
	}
};
S_PTR(DoubleProperty);

/* 整数的属性 */
class IntProperty : public Registerable{
protected:
	void requestInternalIndex(){
		mInternalIndex = mPropertyManager->requestIntIndex();
	}
public:
	int get(size_t index) const{
		return mPropertyManager->getInt(index, mInternalIndex);
	}
	void set(size_t index, const int& i){
		mPropertyManager->setInt(index, mInternalIndex, i);
	}
	~IntProperty(){
		mPropertyManager->releaseIntIndex(mInternalIndex);
	}
};
S_PTR(IntProperty);
/*
 * 具有包围盒的三角网格
 * 每个顶点可以动态增添属性
 */
class Mesh : public _Mesh {
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
	
	/* 在Mesh中新增一个属性，为该属性分配空间，且将下标指向正确的位置 */
	void registerProperty(Registerable_ vp){
		vp->registerSelf(mPropertyManager);
	}

	/* 读取了mesh之后，再调用这个函数，用顶点数初始化PropertyManager */
	void initProperty(){
		mPropertyManager = smartNew(PropertyManager);
		mPropertyManager->init(n_vertices());
	}
private:
	bool mHasRequestAABB;

	Vec3d mMinPoint, mMaxPoint;

	PropertyManager_ mPropertyManager;

};
S_PTR(Mesh);