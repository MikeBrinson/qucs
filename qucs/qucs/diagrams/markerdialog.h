/***************************************************************************
                          markerdialog.h  -  description
                             -------------------
    begin                : Wed April 21 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MARKERDIALOG_H
#define MARKERDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "marker.h"


/**
  *@author Michael Margraf
  */

class MarkerDialog : public QDialog  {
Q_OBJECT
public:
  MarkerDialog(Marker *pm_, QWidget *parent=0, const char *name=0);
  ~MarkerDialog();

private slots:
  void slotAcceptValues();

public:
  Marker *pMarker;

  QComboBox  *NumberBox;
  QLineEdit  *Precision;
  QCheckBox  *TransBox;
};

#endif
