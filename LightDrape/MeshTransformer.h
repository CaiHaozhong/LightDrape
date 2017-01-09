#pragma once
#include "Common.h"
#include "Mesh.h"
class MeshTransformer
{
protected:
	Mesh_ mMesh;
public:
	MeshTransformer(void);
	~MeshTransformer(void);
	MeshTransformer(Mesh_ mesh){
		mMesh = mesh;
	}

	virtual void transform();
};
S_PTR(MeshTransformer);
class BackTransformer : public MeshTransformer{
public:
	BackTransformer();
	~BackTransformer();
	BackTransformer(Mesh_ mesh) : MeshTransformer(mesh){}
	void transform();
};
S_PTR(BackTransformer);
class LeftTransformer : public MeshTransformer{
public:
	LeftTransformer();
	~LeftTransformer();
	LeftTransformer(Mesh_ mesh) : MeshTransformer(mesh){}
	void transform();
};
S_PTR(LeftTransformer);
class RightTransformer : public MeshTransformer{
public:
	RightTransformer();
	~RightTransformer();
	RightTransformer(Mesh_ mesh) : MeshTransformer(mesh){}
	void transform();
};
S_PTR(RightTransformer);
class RightFrontTransformer : public MeshTransformer{
public:
	RightFrontTransformer();
	~RightFrontTransformer();
	RightFrontTransformer(Mesh_ mesh) : MeshTransformer(mesh){}
	void transform();
};
S_PTR(RightFrontTransformer);
class LeftFrontTransformer : public MeshTransformer{
public:
	LeftFrontTransformer();
	~LeftFrontTransformer();
	LeftFrontTransformer(Mesh_ mesh) : MeshTransformer(mesh){}
	void transform();
};
S_PTR(LeftFrontTransformer);