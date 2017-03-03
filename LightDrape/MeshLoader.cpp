#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshLoader.h"
#include <thread>
#include "Config.h"
#include "Human.h"
#include "Garment.h"
#include "HumanFeature.h"
#include "MeshMaterial.h"


MeshLoader::MeshLoader(void)
{
	mMsgQueue = smartNew(MessageQueue);
}


MeshLoader::~MeshLoader(void)
{
}

void MeshLoader::load()
{
	mMsgQueue->push(std::make_shared<Message>(BEGIN_LOAD));

	loadHuman();

	loadGarments();

	mMsgQueue->push(std::make_shared<Message>(END_LOAD));
}

void MeshLoader::loadHuman()
{
	mMsgQueue->push(std::make_shared<Message>(BEGIN_LOAD_HUMAN));

	Config_ conf = Config::getInstance();
	Mesh_ raw = loadMesh(conf->humanInPath, conf->humanInFileName);
	Human_ human = std::make_shared<Human>(raw);

	HumanFeature_ feature = smartNew(HumanFeature);
	feature->fromMakeHumanMeasureFile(conf->humanInPath + human->getName() + ".par");
	human->setFeature(feature);

	Message_ msg = std::make_shared<Message>(END_LOAD_HUMAN);
	msg->human = human;
	mMsgQueue->push(msg);
}

void MeshLoader::asynload()
{
	mTimerID = startTimer(10);
	std::thread t(&MeshLoader::load, this);
	t.detach();
}

Mesh_ MeshLoader::loadMesh(std::string path, std::string name)
{
	Mesh_ ret = smartNew(Mesh);
	ret->request_vertex_normals();
	OpenMesh::IO::Options opt;
	opt += OpenMesh::IO::Options::VertexNormal;
	bool readSuc = OpenMesh::IO::read_mesh(*ret, path+name, opt);
	if(readSuc){
		if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
		{
			// we need face normals to update the vertex normals
			ret->request_face_normals();
			// let the mesh update the normals
			ret->update_normals();
			// dispose the face normals, as we don't need them anymore
			ret->release_face_normals();
		}
		ret->requestAABB();
		ret->setName(name.substr(0, name.size()-4));
		MeshMaterial_ mtl = MeshMaterial::fromMakeHumanMeasureFile(path + ret->getName()+".mtl");
		if(mtl != nullptr){
			ret->setMeshMaterial(mtl);
		}
	}	
	else{
		std::string err = "fail to read ";
		PRINTLN(err+path+name);
	}
	return ret;
}

void MeshLoader::addMeshLoaderListener( MeshLoaderListener_ listener )
{
	mListeners.push_back(listener);
}

void MeshLoader::loadGarments()
{
	Config_ conf = Config::getInstance();
	size_t count = conf->clothInFileNames.size();
	for(size_t g = 0; g < count; g++){

		Message_ msg = std::make_shared<Message>(BEGIN_LOAD_GARMENT);
		msg->arg1 = g;
		mMsgQueue->push(msg);

		std::string garFile = conf->clothInFileNames[g];
		Mesh_ raw = loadMesh(conf->clothInPath, garFile);
		Garment_ garment = std::make_shared<Garment>(raw);

		msg = std::make_shared<Message>(END_LOAD_GARMENT);
		msg->arg1 = g;
		msg->garment = garment;
		mMsgQueue->push(msg);
	}
}

void MeshLoader::timerEvent( QTimerEvent *event )
{
	while(mMsgQueue->isEmpty() == false){
		Message_ msg = mMsgQueue->pop();
		switch (msg->what)
		{
		case BEGIN_LOAD:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onBeginLoad();
			}
			break;
		case END_LOAD:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onEndLoad();
			}
			this->killTimer(mTimerID); //½áÊø»Øµ÷
			mTimerID = 0;
			break;
		case BEGIN_LOAD_HUMAN:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onBeginLoadHuman();
			}
			break;
		case END_LOAD_HUMAN:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onEndLoadHuman(msg->human);
			}
			break;
		case BEGIN_LOAD_GARMENT:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onBeginLoadGarment(msg->arg1);
			}
			break;
		case END_LOAD_GARMENT:
			for(size_t i = 0; i < mListeners.size(); i++){
				mListeners[i]->onEndLoadGarment(msg->arg1, msg->garment);
			}
			break;
		default:
			break;
		}
	}
}

MeshLoader::Message_ MeshLoader::MessageQueue::pop()
{
	Message_ ret = nullptr;
	m.lock();
	if(mMessages.size() > 0){
		ret = mMessages.front();
		mMessages.pop();
	}
	m.unlock();
	return ret;
}

void MeshLoader::MessageQueue::push( Message_ msg )
{
	m.lock();
	mMessages.push(msg);
	m.unlock();
}

bool MeshLoader::MessageQueue::isEmpty()
{
	bool ret = false;
	m.lock();
	ret = mMessages.empty();
	m.unlock();
	return ret;
}

MeshLoader::Message::Message( int _what ) : what(_what)
{
	human = nullptr;
	garment = nullptr;
	arg1 = -1;
}
