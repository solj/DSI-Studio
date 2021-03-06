#ifndef REGIONTABLEWIDGET_H
#define REGIONTABLEWIDGET_H
#include <QTableWidget>
#include <QItemDelegate>
#include <QComboBox>
#include <vector>
#include "Regions.h"
#include "tipl/tipl.hpp"
struct ThreadData;
class tracking_window;


class ImageDelegate : public QItemDelegate
 {
     Q_OBJECT

 public:
    ImageDelegate(QObject *parent)
         : QItemDelegate(parent)
     {
     }

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const;
          void setEditorData(QWidget *editor, const QModelIndex &index) const;
          void setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const;
private slots:
    void emitCommitData();
 };

class RegionTableWidget : public QTableWidget
{
    Q_OBJECT
protected:
    void contextMenuEvent ( QContextMenuEvent * event );
private:
    tracking_window& cur_tracking_window;
    void do_action(QString action);
    void whole_brain_points(std::vector<tipl::vector<3,short> >& points);
    bool load_multiple_roi_nii(QString file_name);
private:
    template<typename func>
    void for_each_checked_region(func fun)
    {
        for (unsigned int roi_index = 0;roi_index < regions.size();++roi_index)
        {
            if (item(roi_index,0)->checkState() != Qt::Checked)
                continue;
            fun(regions[roi_index]);
        }
    }
    std::vector<std::shared_ptr<ROIRegion> > get_checked_regions(void)
    {
        std::vector<std::shared_ptr<ROIRegion> > checked_regions;
        for_each_checked_region([&](std::shared_ptr<ROIRegion> region)
        {
            checked_regions.push_back(region);
        });
        return checked_regions;
    }

signals:
    void need_update(void);
public:
    std::vector<std::shared_ptr<ROIRegion> > regions;
    int color_gen = 10;
public:
    explicit RegionTableWidget(tracking_window& cur_tracking_window,QWidget *parent = 0);
    ~RegionTableWidget();

    QColor currentRowColor(void);
    void add_region_from_atlas(std::shared_ptr<atlas> at,unsigned int roi_is);
    void add_region(QString name,unsigned char type,int color = 0x00FFFFFF);
    void begin_update(void);
    void end_update(void);
    void setROIs(ThreadData* data);
    QString getROIname(void);
    template<typename type>
    void add_points(std::vector<tipl::vector<3,type> >& points,bool erase,bool all,float resolution = 1.0)
    {
        if (currentRow() < 0 || currentRow() >= regions.size())
            return;
        if(all)
        {
            auto regions = get_checked_regions();
            tipl::par_for(regions.size(),[&](int i)
            {
                std::vector<tipl::vector<3,type> > p = points;
                regions[i]->add_points(p,erase,resolution);
            });
        }
        else
            regions[currentRow()]->add_points(points,erase,resolution);
    }

    QString output_format(void);
public slots:
    void draw_region(tipl::color_image& I);
    void draw_edge(QImage& image,QImage& scaledimage);
    void draw_mosaic_region(QImage& image,unsigned int mosaic_size,unsigned int skip);
    void updateRegions(QTableWidgetItem* item);
    void new_region(void);
    void new_high_resolution_region(void);
    void copy_region(void);
    void save_region(void);
    void save_all_regions(void);
    void save_all_regions_to_dir(void);
    void save_region_info(void);
    void load_region(void);
    void load_mni_region(void);
    void delete_region(void);
    void delete_all_region(void);

    void check_check_status(int,int);
    void whole_brain(void);
    void show_statistics(void);
    void merge_all(void);
    void check_all(void);
    void uncheck_all(void);
    void move_up(void);
    void move_down(void);
    void undo(void);
    void redo(void);
    // actions
    void action_smoothing(void){do_action("smoothing");}
    void action_erosion(void){do_action("erosion");}
    void action_dilation(void){do_action("dilation");}
    void action_defragment(void){do_action("defragment");}
    void action_negate(void){do_action("negate");}
    void action_flipx(void){do_action("flipx");}
    void action_flipy(void){do_action("flipy");}
    void action_flipz(void){do_action("flipz");}
    void action_shiftx(void){do_action("shiftx");}
    void action_shiftnx(void){do_action("shiftnx");}
    void action_shifty(void){do_action("shifty");}
    void action_shiftny(void){do_action("shiftny");}
    void action_shiftz(void){do_action("shiftz");}
    void action_shiftnz(void){do_action("shiftnz");}
    void action_threshold(void){new_region();do_action("threshold");}
    void action_separate(void){do_action("separate");}
    void action_A_B(void){do_action("A-B");}
    void action_B_A(void){do_action("B-A");}
    void action_AB(void){do_action("A*B");}
    void action_set_opa(void){do_action("set_opacity");}
};

#endif // REGIONTABLEWIDGET_H
