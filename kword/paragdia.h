/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Paragraph Dialog (header)                              */
/******************************************************************/

#ifndef paragdia_h
#define paragdia_h

#include <qtabdialog.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcolor.h>

#include <kapp.h>
#include <krestrictedline.h>

#include <stdlib.h>

/******************************************************************/
/* class KWPagePreview                                            */
/******************************************************************/

class KWPagePreview : public QGroupBox
{
  Q_OBJECT

public:
  KWPagePreview(QWidget*,const char*);
  ~KWPagePreview() {}
  
  void setLeft(float _left)
    { left = _left; repaint(false); }
  void setRight(float _right)
    { right = _right; repaint(false); }
  void setFirst(float _first)
    { first = _first; repaint(false); }

protected:
  void drawContents(QPainter*);

  float left,right,first,spacing;

};

/******************************************************************/
/* Class: KWParagDia                                              */
/******************************************************************/

class KWParagDia : public QTabDialog
{
  Q_OBJECT

public:
  KWParagDia(QWidget*,const char*);
  ~KWParagDia();              

protected:
  void setupTab1();
  void setupTab2();

  QWidget *tab1,*tab2;
  QGridLayout *grid1,*indentGrid,*spacingGrid;
  KRestrictedLine *eLeft,*eRight,*eFirstLine,*eSpacing;
  QLabel *lLeft,*lRight,*lFirstLine;
  QGroupBox *indentFrame,*spacingFrame;
  QComboBox *cSpacing;
  KWPagePreview *prev1;

protected slots:
  void leftChanged(const char*);
  void rightChanged(const char*);
  void firstChanged(const char*);
  void spacingActivated(int);

};

#endif


