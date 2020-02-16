#include "QtOSGWidget.h"
#include "osgQtViewer.h"
#include <QDebug>
#include <QThread>

QtOSGWidget::QtOSGWidget(QWidget *parent)
	: QOpenGLWidget(parent)
	, m_graphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height()))
	, m_viewer(new osgViewer::Viewer)
	// take care of HDPI screen, e.g. Retina display on Mac
	, m_scale(QApplication::desktop()->devicePixelRatio())
{	
	//相机设置
	osg::Camera *camera = new osg::Camera;
	camera->setViewport(0, 0, this->width(), this->height());
	camera->setClearColor(osg::Vec4(0.2f, 0.2f, 0.29f, 1.0f));
	float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
	camera->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
	camera->setGraphicsContext(m_graphicsWindow);
	m_viewer->setCamera(camera);

	//addCow();

	osgGA::TrackballManipulator *manipulator = new osgGA::TrackballManipulator;
	manipulator->setAllowThrow(true);
	this->setMouseTracking(true);
	m_viewer->setCameraManipulator(manipulator);
	m_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	//m_viewer->addEventHandler(new EventHandler());
	m_viewer->addEventHandler(new osgEventHandler());
	m_viewer->realize();

	setFocusPolicy(Qt::ClickFocus);
}

//添加牛模型
void QtOSGWidget::addCow()
{
	//添加节点
	osg::Node *node = osgDB::readNodeFile("cow.osg");
	m_viewer->setSceneData(node);
}

//设置默认属性
void QtOSGWidget::setDefaultState()
{
	m_viewer->getLight()->setPosition(osg::Vec4(1.0f, 1.0f, 0.0f, 0.0f));
	// 环境光
	m_viewer->getLight()->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// 漫反射光
	//m_viewer->getLight()->setDiffuse(osg::Vec4(0.2f, 0.2f, 0.2f, 0.8f));
	// 设置常量衰减指数
	m_viewer->getLight()->setConstantAttenuation(1.0);
	// 设置线行衰减指数
	m_viewer->getLight()->setLinearAttenuation(0.0);
	// 设置二次衰减指数
	m_viewer->getLight()->setQuadraticAttenuation(0.0);

	//属性设置

	osg::StateSet *stateSet = m_viewer->getSceneData()->asGroup()->getOrCreateStateSet();
	osg::Material *material = new osg::Material;
	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

//添加geode节点
void QtOSGWidget::addNode(osg::ref_ptr<osg::Vec3Array> vertices, osg::ref_ptr<osg::Vec4Array> colors, QString file_name)
{
	bool first=false;
	//属性设置
	if (!m_viewer->getSceneData())
	{
		first = true;
	}
	qDebug() << "begin to add";
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geom->setVertexArray(vertices);
	geom->setColorArray(colors);

	qDebug() << vertices->size()<<"  "<<colors->size();

	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);//BIND_OVERALL//设置关联方式  
	geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices->size()));//几何组结点  
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	//osg::ref_ptr<osg::Group> node = new osg::Group;
	geode->addDrawable(geom.get());
	osg::StateSet* stateSet = geode->getOrCreateStateSet();
	osg::Point* point = new osg::Point;//设置一些样式，如线宽等
	point->setSize(4);//定义点大小
	stateSet->setAttribute(point);
	geode->getOrCreateStateSet()->setAttributeAndModes(point, osg::StateAttribute::ON);//设置透明效果
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	QByteArray cpath = file_name.toLocal8Bit();
	char *flag = cpath.data();
	geode->setName(flag);
	//osg::ref_ptr<osg::Node> node= dynamic_cast<osg::Node*>(geode.get());

	if (first)
	{
		m_viewer->setSceneData(geode);
		setDefaultState();
		qDebug() << "first added";
	}
	else
	{
		m_viewer->getSceneData()->asGroup()->addChild(geode);
		setDefaultState();
		qDebug() << "added";
	}
}

//添加node节点
void QtOSGWidget::addNode(osg::ref_ptr<osg::Node> node, QString file_name)
{
	//qDebug() << QString::number((unsigned int)QThread::currentThreadId());
	QByteArray cpath = file_name.toLocal8Bit();
	char *flag = cpath.data();

	if (m_viewer->getSceneData()) { //屏幕有数据
		setDefaultState();

		osg::ref_ptr<osg::Group> m_group = m_viewer->getSceneData()->asGroup();

		node->setName(flag);

		m_group->addChild(node);

		setDefaultState();
	}

	else
	{
		m_viewer->setSceneData(node);

		setDefaultState();
	}
}

//设置显示节点
void QtOSGWidget::setVisible(int index)
{
	qDebug() << "yes";
	osg::ref_ptr<osg::Group> m_group = m_viewer->getSceneData()->asGroup();
	qDebug() << m_group->getNumChildren();
	if (m_group == 0)
	{
		qDebug() << "return";
		return;
	}
	m_group->getChild(index)->setNodeMask(1);
	//osg::ref_ptr<osg::Switch> m_switch = m_group->getChild(index)->asSwitch();

	//if (m_switch)
	//{
		//qDebug() << "in";
		//m_switch->setValue(index, true);
	//}

}

//设置隐藏节点
void QtOSGWidget::setUnvisible(int index)
{
	qDebug() << "no";
	osg::ref_ptr<osg::Group> m_group = m_viewer->getSceneData()->asGroup();
	qDebug() << m_group->getNumChildren();
	if (m_group == 0)
	{
		qDebug() << "return";
		return;
	}
	m_group->getChild(index)->setNodeMask(0);
	//osg::ref_ptr<osg::Switch> m_switch = m_group->getChild(index)->asSwitch();

	//if (m_switch)
	//{
		//qDebug() << "in";
		//m_switch->setValue(index, false);
	//}

}

//删除节点
void QtOSGWidget::deleteNode(int index)
{
	if (m_viewer->getSceneData()) {
		osg::ref_ptr<osg::Group> t_group = m_viewer->getSceneData()->asGroup();
		t_group->removeChild(index);
	}
}

void QtOSGWidget::paintGL()
{
	m_viewer->frame();
}

void QtOSGWidget::resizeGL(int width, int height)
{
	qDebug() << "in";
	this->getEventQueue()->windowResize(this->x()*m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
	m_graphicsWindow->resized(this->x()*m_scale, this->y() * m_scale, width * m_scale, height * m_scale);
	osg::Camera *camera = m_viewer->getCamera();
	camera->setViewport(0, 0, this->width()*m_scale, this->height()* m_scale);
}

void QtOSGWidget::resizeGL(QResizeEvent * event)
{
	//qDebug()<<"in";
	if (true)
	{
		qDebug() << "do";
		osg::Camera *camera = m_viewer->getCamera();
		camera->setViewport(0, 0, this->width()*m_scale, this->height()* m_scale);
	}
}

void QtOSGWidget::mouseMoveEvent(QMouseEvent *event)
{
	this->getEventQueue()->mouseMotion(event->x()*m_scale, event->y()*m_scale);
}

void QtOSGWidget::mousePressEvent(QMouseEvent *event) {
	unsigned int button = 0;
	switch (event->button()) {
	case Qt::LeftButton:
		button = 1;
		break;
	case Qt::MiddleButton:
		button = 2;
		break;
	case Qt::RightButton:
		button = 3;
		break;
	default:
		break;
	}
	this->getEventQueue()->mouseButtonPress(event->x()*m_scale, event->y()*m_scale, button);
}

void QtOSGWidget::mouseReleaseEvent(QMouseEvent *event) {
	unsigned int button = 0;
	switch (event->button()) {
	case Qt::LeftButton:
		button = 1;
		break;
	case Qt::MiddleButton:
		button = 2;
		break;
	case Qt::RightButton:
		button = 3;
		break;
	default:
		break;
	}
	this->getEventQueue()->mouseButtonRelease(event->x()*m_scale, event->y()*m_scale, button);
}

void QtOSGWidget::keyPressEvent(QKeyEvent* event)
{
	if(event->key()==Qt::Key_Space) m_graphicsWindow->getEventQueue()->keyPress(32);
}

void QtOSGWidget::keyReleaseEvent(QKeyEvent* event)
{

	if (event->key() == Qt::Key_Space) m_graphicsWindow->getEventQueue()->keyRelease(32);

}

void QtOSGWidget::wheelEvent(QWheelEvent *event) {
	int delta = event->delta();
	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
		osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
	this->getEventQueue()->mouseScroll(motion);
}

bool QtOSGWidget::event(QEvent *event) {
	bool handled = QOpenGLWidget::event(event);
	this->update();
	return handled;
}

osgGA::EventQueue * QtOSGWidget::getEventQueue() const {
	osgGA::EventQueue *eventQueue = m_graphicsWindow->getEventQueue();
	return eventQueue;
}