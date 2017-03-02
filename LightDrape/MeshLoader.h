#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <QObject>
#include <queue>
#include <mutex>
class Human;
S_PTR(Human);
class Garment;
S_PTR(Garment);
class Mesh;
S_PTR(Mesh);
class MeshLoader : public QObject
{
public:
	/* �ص� */
	class MeshLoaderListener{
	public:
		/* ��ʼ��������ģ�� */
		virtual void onBeginLoad(){};

		/* ��ʼ��������ģ�� */
		virtual void onBeginLoadHuman(){};

		/* ��ʼ���ص�i���·�ģ�� */
		virtual void onBeginLoadGarment(int i){};

		/* ����ģ�ͼ��ؽ��� */
		virtual void onEndLoadHuman(Human_ human){};

		/* ��i���·�ģ�ͼ��ؽ��� */
		virtual void onEndLoadGarment(int i, Garment_ gar){};

		/* ����ģ�ͼ������ */
		virtual void onEndLoad(){};
	};
	S_PTR(MeshLoaderListener);
private:
	/* ��Ϣ */
	struct Message{
		int what;		
		Human_ human;
		int arg1;
		Garment_ garment;
		Message(int _msg);
	};
	S_PTR(Message);


	/* ��Ϣ���� */
	class MessageQueue{
	private:
		std::queue<Message_> mMessages;
		std::mutex m;
	public:
		Message_ pop();

		void push(Message_ msg);

		bool isEmpty();
	};
	S_PTR(MessageQueue);


	std::vector<MeshLoaderListener_> mListeners;
	const static int BEGIN_LOAD = 1;
	const static int BEGIN_LOAD_HUMAN = 2;
	const static int BEGIN_LOAD_GARMENT = 3;
	const static int END_LOAD_HUMAN = 4;
	const static int END_LOAD_GARMENT = 5;
	const static int END_LOAD = 6;
	MessageQueue_ mMsgQueue;
	int mTimerID;
public:
	MeshLoader(void);
	~MeshLoader(void);

	/* �첽��������ģ�� */
	void asynload();

	/* ��Ӽ��ؼ����� */
	void addMeshLoaderListener(MeshLoaderListener_ listener);

protected:
	/* Override */
	void timerEvent( QTimerEvent *event );
private:

	void load();

	void loadHuman();

	void loadGarments();

	Mesh_ loadMesh(std::string file);
};
S_PTR(MeshLoader);

