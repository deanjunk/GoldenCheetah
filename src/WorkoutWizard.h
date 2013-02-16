/*
 * Copyright (c) 2010 Greg Lonnon (greg.lonnon@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _GC_WORKOUTEDITOR_H
#define _GC_WORKOUTEDITOR_H
#include <QWizard>


#include <QFileDialog>
#include <QDebug>
#include "Settings.h"
#include <QTextStream>
#include <QRadioButton>
#include <QButtonGroup>
#include <QWidget>
#include <QGroupBox>
#include <QLCDNumber>
#include <QDebug>
#include <QSpinBox>
#include "RideFile.h"
#include "MainWindow.h"
#include "GoldenCheetah.h"
#include "Settings.h"
class QwtPlotCurve;
class RideItem;

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "RideItem.h"
#include "Units.h"
#include <QTableWidget>
#include "Zones.h"

#include "Settings.h"
class WorkoutPlot;

class WorkoutItem : public QTableWidgetItem
{
public:
    WorkoutItem(const QString &text) :QTableWidgetItem(QTableWidgetItem::UserType)
    {
        this->setText(text);
    }

    virtual QString validateData(const QString &text) = 0;

    void setData(int role, const QVariant &value)
    {
        if(role == Qt::EditRole)
        {
            QVariant v(validateData(value.toString()));
            QTableWidgetItem::setData(role,v);
        }
    }
};

class WorkoutItemDouble : public WorkoutItem
{
public:
    WorkoutItemDouble() : WorkoutItem("0") {}
    virtual QString validateData(const QString &text)
    {
        double d = text.toDouble();
        return QString::number(d);
    }
};

class WorkoutItemInt : public WorkoutItem
{
public:
    WorkoutItemInt() : WorkoutItem("0") {}
    QString validateData(const QString &text)
    {
        bool ok;
        double d = text.toDouble(&ok);
        return QString::number((int)d);
    }

};

class WorkoutItemLap : public QTableWidgetItem
{
public:
    WorkoutItemLap() : QTableWidgetItem(QTableWidgetItem::UserType) {
        setText("LAP");
        setFlags(flags() & (~Qt::ItemIsEditable));
        setBackgroundColor (Qt::gray);
    }
};

class WorkoutItemRO : public QTableWidgetItem
{
public:
    WorkoutItemRO() : QTableWidgetItem(QTableWidgetItem::UserType) {
        setFlags(flags() & (~Qt::ItemIsEditable));
        setBackgroundColor (Qt::gray);
    }
};

//class WorkoutItemMsg : public QTableWidgetItem
//{
//public:
//    WorkoutItemMsg() : QTableWidgetItem(QTableWidgetItem::UserType) {
//        setText("MSG");
//        setFlags(flags() & (~Qt::ItemIsEditable));
//    }
//};

class WorkoutEditorBase : public QFrame
{
    Q_OBJECT
protected:
    QTableWidget *table;

public slots:
    void delButtonClicked() { table->removeRow(table->currentRow()); }
    void lapButtonClicked()
    {        
        int row = table->currentRow();

        if (row < table->rowCount() && row > 0) {
            row++;
        } else if (table->rowCount() == 0) {
            row = 0;
        }

        table->insertRow(row);
        table->setItem(row,0,new WorkoutItemLap());

        if (table->columnCount() == 3) {
            table->setItem(row, 1, new WorkoutItemRO());
            table->setItem(row, 2, new QTableWidgetItem());
        } else if (table->columnCount() == 4) {
            table->setItem(row, 1, new WorkoutItemRO());
            table->setItem(row, 2, new WorkoutItemRO());
            table->setItem(row, 3, new QTableWidgetItem());
        }
    }
    
    void insertButtonClicked()
    {
        int row = table->currentRow();
        
        if (row < table->rowCount() && row > 0) {
            row++;
        } else if (table->rowCount() == 0) {
            row = 0;
        }

        insertDataRow(row);
        
    }

    //void msgButtonClicked()
    //{
    //    int row = table->currentRow();
    //
    //    if (row < table->rowCount() && row > 0) {
    //        row++;
    //    } else if (table->rowCount() == 0) {
    //        row = 0;
    //    }
    //
    //    table->insertRow(row);
    //    table->setItem(row,0,new WorkoutItemMsg());
    //    table->setItem(row,1,new QTableWidgetItem());
    //}
    
    void cellChanged(int, int) { dataChanged(); }

signals:
    void dataChanged();


public:
    WorkoutEditorBase(QStringList &colms, QWidget *parent=0);
    virtual void insertDataRow(int row) =0;
    virtual void rawData(QVector<QStringList> &rawData)
    {
        int maxRow = table->rowCount();
        for(int i = 0; i < maxRow; i++)
        {
            QStringList datarow;
            QTableWidgetItem *pos = table->item(i, 0);
            QTableWidgetItem *amt = table->item(i, 1);
            QTableWidgetItem *msg;
            if (table->columnCount() < 4) {
                msg = table->item(i, 2);
            } else {
                msg = table->item(i, 3);
            }
            
            if (pos) {
                datarow.append(QString(pos->text()));
            } else {
                datarow.append(QString("0"));
            }
            
            if (amt)
            {
                datarow.append(QString(amt->text()));
            } else {
                datarow.append(QString("0"));
            }
            
            if (msg) {
                datarow.append(QString(msg->text()));
            } else {
                datarow.append(QString(" "));
            }
            rawData.append(datarow);
        }
    }
};

class WorkoutEditorAbs : public WorkoutEditorBase
{
public:
    WorkoutEditorAbs(QStringList &colms, QWidget *parent = 0) : WorkoutEditorBase(colms,parent)
    {}
    void insertDataRow(int row);
};

class WorkoutEditorRel : public WorkoutEditorBase
{
    Q_OBJECT

    int ftp;
public slots:

    void updateWattage(int row, int colm)
    {
        if(colm == 1)
        {
            QTableWidgetItem *minItem = table->item(row,0);
            QTableWidgetItem *percentageItem = table->item(row,1);
            if(minItem->text() == "" || minItem->text() == "LAP" || percentageItem->text() == "") return;
            double percentage = percentageItem->text() .toDouble();
            WorkoutItemRO *item = new WorkoutItemRO();
            int wattage = (int) (percentage * ftp / 100);
            item->setData(Qt::EditRole,QVariant(QString::number(wattage)));
            table->setItem(row,2,item);

        }
    }

public:
    WorkoutEditorRel(QStringList &colms, int _ftp, QWidget *parent = 0) :  WorkoutEditorBase(colms,parent), ftp(_ftp)
    {
        connect(table,SIGNAL(cellChanged(int,int)),this,SLOT(updateWattage(int,int)));
    }
    void insertDataRow(int row);
};

class WorkoutEditorGradient :public WorkoutEditorBase
{
public:
    WorkoutEditorGradient(QStringList &colms, QWidget *parent = 0) : WorkoutEditorBase(colms,parent)
    {}
    void insertDataRow(int row);
};

class WorkoutMetricsSummary :public QGroupBox
{
    Q_OBJECT

    QGridLayout *layout;
    QMap<QString,QPair<QLabel*,QLabel*> > metricMap;
public:
    WorkoutMetricsSummary(QWidget *parent = 0) : QGroupBox(parent)
    {
        setTitle(tr("Workout Metrics"));
        layout = new QGridLayout();
        setLayout(layout);
    }
    void updateMetrics(QStringList &order, QHash<QString,RideMetricPtr>  &metrics);
    void updateMetrics(QMap<QString,QString> &map);

};

class WorkoutPage : public QWizardPage
{
public:
    WorkoutPage(QWidget *parent) : QWizardPage(parent) {}
    virtual void SaveWorkout() = 0;
    void SaveWorkoutHeader(QTextStream &stream, QString fileName, QString description, QString units)
    {
        stream << "[COURSE HEADER]" << endl;
        stream << "VERSION = 2" << endl;
        stream << "UNITS = METRIC" << endl;
        stream << "DESCRIPTION = " << description << endl;
        stream << "FILE NAME = " << fileName << endl;
        stream <<  units << endl;
        stream << "[END COURSE HEADER]" << endl;
    }

};

class WorkoutTypePage : public QWizardPage
{
    Q_OBJECT
    QButtonGroup *buttonGroupBox;
    QRadioButton *absWattageRadioButton, *relWattageRadioButton, *gradientRadioButton, *importRadioButton;
public:
    WorkoutTypePage(QWidget *parent=0);
    bool isComplete() const { return true; }
    void initializePage();
    int nextId() const;
};

class AbsWattagePage : public WorkoutPage
{
    Q_OBJECT
    WorkoutEditorAbs *we;
    WorkoutMetricsSummary *metricsSummary;
    WorkoutPlot *plot;
private slots:
    void updateMetrics();
public:
    AbsWattagePage(QWidget *parent=0);
    void initializePage();
    void SaveWorkout();
    bool isFinalPage() const { return true; }
    int nextId() const { return -1; }
};

class RelWattagePage : public WorkoutPage
{
    Q_OBJECT
    WorkoutEditorRel *we;
    WorkoutMetricsSummary *metricsSummary;
    WorkoutPlot *plot;
    int ftp;
private slots:
    void updateMetrics();
public:
    RelWattagePage(QWidget *parent=0);
    void initializePage();
    bool isFinalPage() const { return true; }
    int nextId()  const { return -1; }

    void SaveWorkout();
};

class GradientPage : public WorkoutPage
{
    Q_OBJECT

    WorkoutEditorGradient *we;
    WorkoutMetricsSummary *metricsSummary;
    MainWindow *mainWindow;
    int ftp;
    bool metricUnits;

private slots:

    void updateMetrics();
public:
    GradientPage(QWidget *parent=0);
    void initializePage();
    void SaveWorkout();
    bool isFinalPage() const { return true; }
    int nextId() const { return -1; }
};

class ImportPage : public WorkoutPage
{
    Q_OBJECT
    WorkoutPlot *plot;
    QVector<QPair<double,double> > rideData; // orignal distance/alt in metric
    bool metricUnits;
    QSpinBox *gradeBox;
    QSpinBox *segmentBox;
    WorkoutMetricsSummary *metricsSummary;
    QVector<QPair<double,double> >  rideProfile; // distance and slope
public slots:
     void updatePlot();
public:
    ImportPage(QWidget * parent=0);
     void initializePage();
    void SaveWorkout();
    bool isFinalPage() const { return true; }
};



class WorkoutWizard : public QWizard
{
    Q_OBJECT
public:
    enum { WW_WorkoutTypePage, WW_AbsWattagePage, WW_RelWattagePage, WW_GradientPage, WW_ImportPage };

    WorkoutWizard(QWidget *parent = NULL);

    // called at the end of the wizard...
    void accept();
};


#endif


