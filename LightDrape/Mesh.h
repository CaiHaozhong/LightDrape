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
	
	/* ����һ�����õ�����λ�ã���Ҫʱ�����ڴ� */
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

	/* �ͷ��±�Ϊi���ڴ棬����i�´ο���ͨ��requestVecIndex���� */
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

/* ͨ��ΪMesh�Ķ����������� */
class Registerable{
protected:
	size_t mInternalIndex;
	PropertyManager_ mPropertyManager;

public:
	/* ��Ҫ�ֶ������������ */
	void registerSelf(PropertyManager_ pm){
		mPropertyManager = pm;
		requestInternalIndex();
	}
	virtual void requestInternalIndex() = 0;
	virtual ~Registerable(){}
};
S_PTR(Registerable);

/* ��ά���������� */
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

/* ������������ */
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

/* ���������� */
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
 * ���а�Χ�е���������
 * ÿ��������Զ�̬��������
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

	/** ��ģ�ͱ���֮����Ҫ���¼����Χ�У��������� **/
	void requestRecomputeAABB();

	/** �����Χ�� **/
	void requestAABB();	

	Vec3d getMin();

	Vec3d getMax();

	/** ��ȡ��Χ�жԽ��ߵĳ��� **/
	float getDigonalLen();

	/* x �� */
	float getWidth();

	/* y �� */
	float getHeight();

	/* z �� */
	float getLength();
	
	/* ��Mesh������һ�����ԣ�Ϊ�����Է���ռ䣬�ҽ��±�ָ����ȷ��λ�� */
	void registerProperty(Registerable_ vp){
		vp->registerSelf(mPropertyManager);
	}

	/* ��ȡ��mesh֮���ٵ�������������ö�������ʼ��PropertyManager */
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