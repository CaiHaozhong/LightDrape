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
	/* 回调 */
	class MeshLoaderListener{
	public:
		/* 开始加载所有模型 */
		virtual void onBeginLoad(){};

		/* 开始加载人体模型 */
		virtual void onBeginLoadHuman(){};

		/* 开始加载第i个衣服模型 */
		virtual void onBeginLoadGarment(int i){};

		/* 人体模型加载结束 */
		virtual void onEndLoadHuman(Human_ human){};

		/* 第i个衣服模型加载结束 */
		virtual void onEndLoadGarment(int i, Garment_ gar){};

		/* 所有模型加载完毕 */
		virtual void onEndLoad(){};
	};
	S_PTR(MeshLoaderListener);
private:
	/* 消息 */
	struct Message{
		int what;		
		Human_ human;
		int arg1;
		Garment_ garment;
		Message(int _msg);
	};
	S_PTR(Message);


	/* 消息队列 */
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

	/* 异步加载所有模型 */
	void asynload();

	/* 添加加载监听器 */
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

