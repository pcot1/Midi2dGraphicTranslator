#define DEBUG
#include "graphicDisplayer.h"

GraphicDisplayer *grDispl;

// *** Constructor
GraphicDisplayer::GraphicDisplayer() : QWidget()
{
                                                                    // Scene
    scene = new QGraphicsScene;                                         // create Qt scene object
    sceneRectangle = QRectF(worldUpLeftX,worldUpLeftY,worldWidth,worldHeight);  // initialize the part of the world to render
    //sceneRectangle = QRectF(-4,3,8,6);                                // initialize the part of the world to render
    scene->setBackgroundBrush(Qt::white);                               // background is white
                                                                    // View
    view = new QGraphicsView(scene);                                    // create Qt view object
    view->resize(initialViewportWidth,initialViewportHeight);           // set initial window size
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);           // remove vertical scrollbar
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);         // remove horizontal scrollbar
    //view->setSceneRect(-initialViewportWidth/2,initialViewportHeight/2,initialViewportWidth,initialViewportHeight); // set view coordinates
    //view->setSceneRect(initialSceneRectangle);
    view->setSceneRect(sceneRectangle);
                                                                    // scene2ViewMatrix
    scene2ViewMatrix.setMatrix(1,0,0,0,1,0,0,0,1);                      // initialize a default identity matrix
    //scene2ViewMatrix.setMatrix(100,0,0,0,-100,0,+400,+300,1);                      // initialize a default identity matrix
    updateScene2ViewMatrix();                                           // set the real matrix for default view window
    view->setTransform(scene2ViewMatrix);
    scene->addText(QString("Initialization ..."));
    view->show();                                                   // make the view window appears on the screen
    //PRINTF(("view position = %d, %d\n",view->x(),view->y()));
    //view->move(view->x()+200,view->y());
    //PRINTF(("view position = %d, %d\n",view->x(),view->y()));
    printObject();
    //drawSomething();
    //printObject();
    grDispl = this;
}

// *** Destructor
GraphicDisplayer::~GraphicDisplayer()
{
}

void GraphicDisplayer::printSceneDescription(void) const
{
    PRINTF((">   Scene Rectangle = (%f, %f, %f, %f)\n",sceneRectangle.x(),sceneRectangle.y(),sceneRectangle.width(),sceneRectangle.height()));
    const std::type_info &typGroupItem = typeid(QGraphicsItemGroup);
    QGraphicsItem *item = 0;
    QGraphicsItem *parentItem = 0;
    QList <QGraphicsItem *> itemList = scene->items();
    int nbItems = itemList.size();
    int nbTopLevelItems = 0;
    for (int i = 0; i < nbItems; i++) {
        item = itemList.at(i);
        parentItem = item->parentItem();
        if (parentItem == 0) {
            QList <QGraphicsItem *> subItemList = item->childItems();
            const std::type_info &t = typeid(*item);
            if (t == typGroupItem)
                PRINTF(("    scene \"group\" item %02d (0x%p) contains %02d items\n",nbTopLevelItems,item,subItemList.size()));
            else
                PRINTF(("    scene \"other\" item %02d (0x%p) contains %02d items\n",nbTopLevelItems,item,subItemList.size()));
            ++nbTopLevelItems;
        }
    }
    return;
}

void GraphicDisplayer::printObject(void) const
{
    PRINTF((">>> GraphicDisplayer:\n"));
    printSceneDescription();
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n",scene2ViewMatrix.m11(),scene2ViewMatrix.m12(),0.0));
    PRINTF((">   desiredMatrix = |%+4.1f, %+4.1f, %+4.1f|\n",scene2ViewMatrix.m21(),scene2ViewMatrix.m22(),0.0));
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n\n",scene2ViewMatrix.dx(),scene2ViewMatrix.dy(),1.0));
    QTransform gotMatrix = view->transform();
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n",gotMatrix.m11(),gotMatrix.m12(),gotMatrix.m13()));
    PRINTF((">   realMatrix =    |%+4.1f, %+4.1f, %+4.1f|\n",gotMatrix.m21(),gotMatrix.m22(),gotMatrix.m23()));
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n\n",gotMatrix.m31(),gotMatrix.m32(),gotMatrix.m33()));
    gotMatrix = view->viewportTransform();
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n",gotMatrix.m11(),gotMatrix.m12(),gotMatrix.m13()));
    PRINTF((">   viewportMatrix =|%+4.1f, %+4.1f, %+4.1f|\n",gotMatrix.m21(),gotMatrix.m22(),gotMatrix.m23()));
    PRINTF(("                    |%+4.1f, %+4.1f, %+4.1f|\n",gotMatrix.m31(),gotMatrix.m32(),gotMatrix.m33()));

}

// *** update GraphicDisplayer after end user resize of rendering window
void GraphicDisplayer::resizeEvent(QResizeEvent *event)
{
    PRINTF(("GraphicDisplayer::resizeEvent of type %d\n", event->type()));
    updateSceneRectangle();                                             // recompute the scene rectangle
    updateScene2ViewMatrix();                                           // recompute the projection matrix
    printObject();
}

// *** update the scene reactangle (part of the world ro render) after end user resize of rendering window
void GraphicDisplayer::updateSceneRectangle(void)
{
    PRINTF(("GraphicDisplayer::updateSceneRectangle\n"));
    qreal initialRatio = (qreal)(worldWidth / worldHeight);
    qreal ratio = (qreal) (view->width()/view->height());
    qreal invRatio = (qreal) (view->height()/view->width());
    if (ratio >= initialRatio) {
        sceneRectangle.setX(worldUpLeftY*ratio);
        sceneRectangle.setY(worldUpLeftY);
        sceneRectangle.setWidth(2*worldHeight*ratio);
        sceneRectangle.setY(2*worldHeight);
    } else {
        sceneRectangle.setX(worldUpLeftX);
        sceneRectangle.setY(worldUpLeftX*invRatio);
        sceneRectangle.setWidth(2*worldWidth);
        sceneRectangle.setY(2*worldWidth*invRatio);
    }
    return;
}

// *** update the projection matrix after end user resize of rendering window
void GraphicDisplayer::updateScene2ViewMatrix(void)
{
    PRINTF(("GraphicDisplayer::updateScene2ViewMatrix\n"));
    qreal m11,m22,dx,dy;
    m11 = (qreal)(viewZoomScene);
    m22 = (qreal)(viewZoomScene);
    dx = -viewZoomScene * sceneRectangle.x();
    dy = -viewZoomScene * sceneRectangle.y();
    scene2ViewMatrix.setMatrix(m11,0.0,0.0,0.0,m22,0.0,dx,dy,1.0);
}

// *** remove all items from the scene
void GraphicDisplayer::cleanScene(void)
{
    QList <QGraphicsItem *> itemList = scene->items();
    PRINTF(("scene has %d items before cleaning\n",itemList.size()));
    for (int i = 0; i < itemList.size(); i++) {
        scene->removeItem(itemList.at(i));
    }
    itemList = scene->items();
    PRINTF(("scene has %d items after cleaning\n",itemList.size()));
}

// *** remove 1 item from the scene
void GraphicDisplayer::removeItemFromScene(QGraphicsItem *item)
{
    scene->removeItem(item);
}

// *** add 1 item to the scene (on top)
void GraphicDisplayer::addItemToScene(QGraphicsItem *item)
{
    scene->addItem(item);
}

void GraphicDisplayer::update(void)
{
    PRINTF(("GraphicDisplayer::update\n"));
    scene->update();
    printSceneDescription();
    view->update();
}

// *** draw something on the screen
void GraphicDisplayer::drawSomething(void)
{
    QPointF scenePoint;
    QPoint viewPoint;
    PRINTF(("draw a red circle\n"));
    //QGraphicsEllipseItem something(-1.0, 1.0, 2.0, 2.0);
    QGraphicsEllipseItem *something= new QGraphicsEllipseItem(-360, -270, 720, 540);
    something->resetTransform();
    something->setBrush(QBrush(QColor(255,0,0,255)));
    something->setPen(QPen(Qt::NoPen));
    scene->addItem(something);
    scenePoint.setX(-4); scenePoint.setY(-3); viewPoint = view->mapFromScene(scenePoint);
    PRINTF(("scene (%+3.1f,%+3.1f) -> view (%3d,%3d)\n", scenePoint.x(), scenePoint.y(), viewPoint.x(), viewPoint.y()));
    scenePoint.setX(-4); scenePoint.setY(+3); viewPoint = view->mapFromScene(scenePoint);
    PRINTF(("scene (%+3.1f,%+3.1f) -> view (%3d,%3d)\n", scenePoint.x(), scenePoint.y(), viewPoint.x(), viewPoint.y()));
    scenePoint.setX(4); scenePoint.setY(+3); viewPoint = view->mapFromScene(scenePoint);
    PRINTF(("scene (%+3.1f,%+3.1f) -> view (%3d,%3d)\n", scenePoint.x(), scenePoint.y(), viewPoint.x(), viewPoint.y()));
    scenePoint.setX(4); scenePoint.setY(-3); viewPoint = view->mapFromScene(scenePoint);
    PRINTF(("scene (%+3.1f,%+3.1f) -> view (%3d,%3d)\n", scenePoint.x(), scenePoint.y(), viewPoint.x(), viewPoint.y()));
    //scene->update();
    //view->update();
}
