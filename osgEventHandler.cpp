#include "osgEventHandler.h"

#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgUtil/RayIntersector>
#include <osg/ShapeDrawable>

#include <sstream>
#include <QDebug>
#include <QString>


bool osgEventHandler::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::PUSH):
		if (ea.getButton()== osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && pick_flag)
		{
			osg::ref_ptr<osgViewer::Viewer> viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
			//osg::ref_ptr<osg::Group> g = viewer->getSceneData()->asGroup();
			//qDebug()<< QString(g->getChild(0)->getName().c_str());
			//if (!view) return false;
			pick(viewer, ea);
		}

	}
	return false;

}

void osgEventHandler::pick(osg::ref_ptr<osgViewer::Viewer> viewer, const osgGA::GUIEventAdapter & ea)
{
	qDebug() << "pick";
	
	//osgUtil::LineSegmentIntersector::Intersections intersections;

	////osgUtil::PolytopeIntersector::Intersections i;
	//
	//std::string gdlist = "";

	//if (viewer->computeIntersections(ea, intersections))
	//{
	//	qDebug() << "in";
	//	for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
	//		hitr != intersections.end();
	//		++hitr)
	//	{
	//		std::ostringstream os;
	//		if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
	//		{
	//			// the geodes are identified by name.
	//			os << "Object \"" << hitr->nodePath.back()->getName() << "\"" << std::endl;
	//		}
	//		else if (hitr->drawable.valid())
	//		{
	//			os << "Object \"" << hitr->drawable->className() << "\"" << std::endl;
	//		}

	//		qDebug() << hitr->getLocalIntersectPoint().x() << " " << hitr->getLocalIntersectPoint().y() << " " << hitr->getLocalIntersectPoint().z();
	//		qDebug() << hitr->getWorldIntersectPoint().x() << " " << hitr->getWorldIntersectPoint().y() << " " << hitr->getWorldIntersectPoint().z();
	//		os << "        local coords vertex(" << hitr->getLocalIntersectPoint().x() <<" "<< hitr->getLocalIntersectPoint().y() << " " << hitr->getLocalIntersectPoint().z() << ")" << "  normal(" /*<< hitr->getLocalIntersectNormal()*/ << ")" << std::endl;
	//		os << "        world coords vertex(" << hitr->getWorldIntersectPoint().x() <<" "<< hitr->getWorldIntersectPoint().y() << " " << hitr->getWorldIntersectPoint().z() << ")" << "  normal(" /*<< hitr->getWorldIntersectNormal()*/ << ")" << std::endl;
	//		const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr->indexList;
	//		for (unsigned int i = 0; i < vil.size(); ++i)
	//		{
	//			os << "        vertex indices [" << i << "] = " << vil[i] << std::endl;
	//		}

	//		gdlist += os.str();
	//	}
	//}

	osg::ref_ptr<osgUtil::PolytopeIntersector> picker = new osgUtil::PolytopeIntersector(osgUtil::Intersector::WINDOW, ea.getX() - 1.0, ea.getY() - 1.0, ea.getX() + 1.0, ea.getY() + 1.0);
	osgUtil::IntersectionVisitor iv(picker.get());
	//osgViewer::View* pView = dynamic_cast<osgViewer::View*>(&us);
	viewer->getCamera()->accept(iv);
	if (picker->containsIntersections())
	{
		osg::Vec3f ptWorldIntersectPointFirst = picker->getFirstIntersection().intersectionPoints[0];
		osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(ptWorldIntersectPointFirst, 0.007);
		osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(sphere);
		sd->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
		viewer->getSceneData()->asGroup()->addChild(sd.release());
	}

	//osg::Vec3f vec3;
	//osg::ref_ptr<osg::Camera> camera = viewer->getCamera();
	//osg::Vec3 vScreen(ea.getX(), ea.getY(), 0);
	//osg::Matrix mVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	//osg::Matrix invertVPW;
	//invertVPW.invert(mVPW);
	//vec3 = vScreen * invertVPW;
	//osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(vec3, 0.007);
	//osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(sphere);
	//sd->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
	//viewer->getSceneData()->asGroup()->addChild(sd.release());
}
