#ifndef __settingsdlg_h__
#define __settingsdlg_h__

#include <qframe.h>
#include "kpropdlg.h"

class SettingsDlg : public KPropDlg
{
  Q_OBJECT
public:
  SettingsDlg();
  ~SettingsDlg();
    
protected:
  void setupMiscTab();
  void setupFontsTab();
  void setupColorsTab();

private:
  QFrame *miscFrame;
  QFrame *fontsFrame;
  QFrame *colorsFrame;
};

#endif
