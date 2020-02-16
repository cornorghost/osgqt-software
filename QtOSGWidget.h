#pragma once

#include <QApplication>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDesktopWidget>
#include <QScreen>
#include <QtGlobal>
#include <QWindow>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/Point>

#include "osgEventHandler.h"

class QtOSGWidget :public QOpenGLWidget, public osgGA::GUIEventHandler
{
public:
	QtOSGWidget(QWidget *parent = 0);
	
	virtual ~QtOSGWidget() {}

	virtual void resizeGL(int width, int height);

	void addCow();

	void setDefaultState();

	void addNode(osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec4Array> colors, QString file_name);

	void addNode(osg::ref_ptr<osg::Node> node, QString file_name);

	void setVisible(int index);

	void setUnvisible(int index);

	void deleteNode(int index);
	
protected:
	virtual void paintGL();

	virtual void resizeGL(QResizeEvent *event);
	
	virtual void mouseMoveEvent(QMouseEvent *event);
	
	virtual void mousePressEvent(QMouseEvent *event);
	
	virtual void mouseReleaseEvent(QMouseEvent *event);

	virtual void keyPressEvent(QKeyEvent* event);

	virtual void keyReleaseEvent(QKeyEvent* event);
	
	virtual void wheelEvent(QWheelEvent *event);
	
	virtual bool event(QEvent *event);

protected:
	osgGA::EventQueue *getEventQueue() const ;
	
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_graphicsWindow;
	osg::ref_ptr<osgViewer::Viewer> m_viewer;
	qreal m_scale;

private slots:
	//void toPick();

	//void cancelPick();
};
