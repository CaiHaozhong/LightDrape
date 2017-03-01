#pragma once
class GLWidgetCallBack
{
public:
	GLWidgetCallBack(void);
	~GLWidgetCallBack(void);
	virtual void onBeginInitializeGL() = 0;
	virtual void onEndInitializeGL() = 0;
	virtual void onBeginReshapeWindow() = 0;
	virtual void onEndReshapeWindow() = 0;
	virtual void draw() = 0;
};

