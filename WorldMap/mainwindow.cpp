#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QPainter>


namespace
{
   QRectF const mercator_plane(QPointF(-20037508.3428, -20000000),
                               QPointF( 20037508.3428,  20000000));

   class MapView : public QGraphicsView
   {
   public:
      MapView( QWidget * parent = nullptr )
         : QGraphicsView(parent)
      {
         auto scene = new QGraphicsScene(this);
         setScene(scene);
         setTransform(QTransform().scale(1, -1));
         setCacheMode(QGraphicsView::CacheNone);

         timer_.setSingleShot(true);
         timer_.setInterval(500);
         QObject::connect(&timer_, &QTimer::timeout, this, [this] { on_resize(false); });

         desired_rect_ = mercator_plane;
         on_resize(true);
      }

   private:
      void resizeEvent(QResizeEvent *) override
      {
         auto sz = size().scaled(desired_rect_.size().toSize(), Qt::KeepAspectRatioByExpanding);

         QRectF rect(QPointF(), sz);
         rect.moveCenter(desired_rect_.center());
         setSceneRect(rect);
         fitInView(rect);

         on_resize(true);
      }

      void drawBackground(QPainter *painter, const QRectF & rect) override
      {
         painter->fillRect(rect, Qt::darkGray);
         if (!pixmap_.isNull())
            painter->drawPixmap(desired_rect_, pixmap_, pixmap_.rect());
      }

   private:
      QNetworkReply * get_map_request( QRectF const & bbox, QSize const & size )
      {
         QString const str_bbox = QStringLiteral("%1,%2,%3,%4")
              .arg(bbox.left()  , 0, 'f')
              .arg(bbox.top()   , 0, 'f')
              .arg(bbox.right() , 0, 'f')
              .arg(bbox.bottom(), 0, 'f');

         QUrl url("http://ows.mundialis.de/services/service");
         QUrlQuery query;
         query.setQueryItems({
            {"version", "1.1.1"},
            {"request", "GetMap"},
            {"layers" , "TOPO-OSM-WMS"},
            {"styles" , ""},
            {"srs"    , "EPSG:3857"},
            {"bbox"   , str_bbox },
            {"width"  , QString::number(size.width()) },
            {"height" , QString::number(size.height()) },
            {"format" , "image/png"}
         });
         url.setQuery(query);

         return manager_.get(QNetworkRequest(url));
      }

      void on_resize(bool delayed)
      {
         if (delayed)
            return timer_.start();

         if (reply_)
         {
            reply_->abort();
            reply_->deleteLater();
         }

         reply_ = get_map_request(desired_rect_, size());
         connect(reply_, &QNetworkReply::finished, this, [this, rect = desired_rect_]
         {
            if (reply_->error())
            {
               // TODO
            }
            else
            {
               auto data = reply_->readAll();
               auto img = QImage::fromData(data);
               if (!img.isNull())
               {
                  pixmap_ = QPixmap::fromImage(img.mirrored());
                  pixmap_rect_ = rect;
                  viewport()->update();
               }
               else
                  qCritical() << QString(data);
            }
            reply_->deleteLater();
            reply_ = nullptr;
         });
      }

   private:
      QNetworkAccessManager manager_;
      QTimer  timer_;
      QRectF  desired_rect_;
      QPixmap pixmap_;
      QRectF  pixmap_rect_;

      QNetworkReply * reply_ = nullptr;
   };
}

struct MainWindow::impl_t
{
   impl_t(MainWindow * self)
      : self_(self)
   {
      ui_.setupUi(self);
      ui_.centralLayout->addWidget(new MapView());
   }

private:
   MainWindow * self_;
   Ui::MainWindow ui_;
};

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
   , pimpl_(new impl_t(this))
{
}

MainWindow::~MainWindow()
{
}
