#pragma once

#include <osgViewer/Viewer>

class osgEventHandler: public osgGA::GUIEventHandler
{
public:
	osgEventHandler() {};
	~osgEventHandler() {};

protected:
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void pick(osg::ref_ptr<osgViewer::Viewer> viewer, const osgGA::GUIEventAdapter & ea);

private:
	bool pick_flag = true;

};

