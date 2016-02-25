//===========================================
//  PC-BSD source code
//  Copyright (c) 2016, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "page_taskmanager.h"
#include "ui_page_taskmanager.h" //auto-generated from the .ui file

taskmanager_page::taskmanager_page(QWidget *parent, sysadm_client *core) : PageWidget(parent, core), ui(new Ui::taskmanager_ui){
  ui->setupUi(this);	
}

taskmanager_page::~taskmanager_page(){
  
}

//Initialize the CORE <-->Page connections
void taskmanager_page::setupCore(){
  connect(CORE, SIGNAL(newReply(QString, QString, QString, QJsonValue)), this, SLOT(ParseReply(QString, QString, QString, QJsonValue)) );
}

//Page embedded, go ahead and startup any core requests
void taskmanager_page::startPage(){
  //Let the main window know the name of this page
  emit ChangePageTitle( tr("Task Manager") );

  //Now run any CORE communications
  QJsonObject jsobj;
  jsobj.insert("action", "procinfo");
  CORE->communicate("taskquery", "sysadm", "systemmanager", jsobj);
  
  // Get PID info every 3 seconds
  proctimer = new QTimer(this);
  connect(proctimer, SIGNAL(timeout()), this, SLOT(slotRequestProcInfo()));
  proctimer->start(3000);

  qDebug() << "Start page!";
}


// === PRIVATE SLOTS ===
void taskmanager_page::ParseReply(QString id, QString namesp, QString name, QJsonValue args){

 // qDebug() << "reply" << id;

  // Read in the PID list
  if ( id == "taskquery")
  {
    if ( ! args.isObject() )
      return;
    parsePIDS(args.toObject());
  }
	
}

void taskmanager_page::parsePIDS(QJsonObject jsobj)
{
  //ui->taskWidget->clear();

  //qDebug() << "KEYS" << jsobj.keys();
  QStringList keys = jsobj.keys();
  if (keys.contains("message") ) {
    qDebug() << "MESSAGE" << jsobj.value("message").toString();
    return;
  }

  // Look for procinfo
  QJsonObject procobj = jsobj.value("procinfo").toObject();
  QStringList pids = procobj.keys();
  for ( int i=0; i < pids.size(); i++ )
  {
    QString PID = pids.at(i);
    QJsonObject pidinfo = procobj.value(PID).toObject();

    // Check if we have this PID already
    QList<QTreeWidgetItem *> foundItems = ui->taskWidget->findItems(PID, Qt::MatchExactly, 0);
    if ( ! foundItems.isEmpty() )
    {
      foundItems.at(0)->setText(1, pidinfo.value("username").toString());
      foundItems.at(0)->setText(2, pidinfo.value("thr").toString());
      foundItems.at(0)->setText(3, pidinfo.value("pri").toString());
      foundItems.at(0)->setText(4, pidinfo.value("nice").toString());
      foundItems.at(0)->setText(5, pidinfo.value("size").toString());
      foundItems.at(0)->setText(6, pidinfo.value("res").toString());
      foundItems.at(0)->setText(7, pidinfo.value("state").toString());
      foundItems.at(0)->setText(8, pidinfo.value("cpu").toString());
      foundItems.at(0)->setText(9, pidinfo.value("time").toString());
      foundItems.at(0)->setText(10, pidinfo.value("wcpu").toString());
      foundItems.at(0)->setText(11, pidinfo.value("command").toString());
    } else {
      // Create the new taskWidget item
      new QTreeWidgetItem(ui->taskWidget, QStringList() << PID
          << pidinfo.value("username").toString()
          << pidinfo.value("thr").toString()
          << pidinfo.value("pri").toString()
          << pidinfo.value("nice").toString()
          << pidinfo.value("size").toString()
          << pidinfo.value("res").toString()
          << pidinfo.value("state").toString()
          << pidinfo.value("cpu").toString()
          << pidinfo.value("time").toString()
          << pidinfo.value("wcpu").toString()
          << pidinfo.value("command").toString()
          );
    }
  }

  // Now loop through the UI and look for pids to remove
  for ( int i=0; i<ui->taskWidget->topLevelItemCount(); i++ ) {
    // Check if this item exists in the PID list
    if ( ! pids.contains(ui->taskWidget->topLevelItem(i)->text(0)) ) {
      // No? Remove it
      ui->taskWidget->takeTopLevelItem(i);
    }
  }

  // Resize the widget to the new contents
  ui->taskWidget->header()->resizeSections(QHeaderView::ResizeToContents);
  ui->taskWidget->sortItems(10, Qt::DescendingOrder);
}

void taskmanager_page::slotRequestProcInfo()
{
  QJsonObject jsobj;
  jsobj.insert("action", "procinfo");
  CORE->communicate("taskquery", "sysadm", "systemmanager", jsobj);
}
