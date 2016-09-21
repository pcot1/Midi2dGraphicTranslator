#ifndef GRAPHICDISPLAYER_H
#define GRAPHICDISPLAYER_H
#include <stdio.h>
#include "logging.h"
#include <typeinfo>
#include <Qt>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include "world.h"

const int viewZoomScene = 1.0;
const int initialViewportWidth  = (int)(viewZoomScene*worldWidth);
const int initialViewportHeight = (int)(viewZoomScene*worldHeight);

/*********************************************************************************************************************/
//
// GraphicDisplayer : 2D Graphic Rendering Machine = scene + window
//      each Graphic Translator has its own item that includes items it wants to display
//
/*********************************************************************************************************************/


class GraphicDisplayer : public QWidget
{
    Q_OBJECT

public:
    GraphicDisplayer();
    ~GraphicDisplayer();
    void cleanScene(void);                                  // empty the scene
    void removeItemFromScene(QGraphicsItem *item);          // remove an item from the scene
    void addItemToScene(QGraphicsItem *item);               // add an item to the scene (on top)
    void printObject(void) const;                           // debug
    void update(void);
    void drawSomething(void);                               // draw something on the screen for debug

protected:
    void resizeEvent(QResizeEvent *event);                  // process user action of resizing the rendering window
    void printSceneDescription(void) const;

private:
    QGraphicsScene *scene;                                  // the scene (abstract world containing graphic items)
    QRectF sceneRectangle;                                  // part of the world to render
    QGraphicsView *view;                                    // the view = projection of scene to window
    QTransform scene2ViewMatrix;                            // the matrix to project scene coordinates into view coordinates
    void updateSceneRectangle(void);                        // recompute all scene-view relationship (after a view window user resize)
    void updateScene2ViewMatrix(void);                      // recompute matrix (after a view window user resize)
};

#endif // GRAPHICDISPLAYER_H
