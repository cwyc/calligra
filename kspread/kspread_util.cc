/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kspread_util.h"
#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_doc.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <math.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>

QString util_fractionFormat( double value , KSpreadCell::formatNumber _tmpFormat)
{
    double result = value-floor(value);
    int index = 0;
    QString tmp;
    if(result == 0 )
    {
        tmp = tmp.setNum( value );
    }
    else
    {
        switch( _tmpFormat )
        {
            case KSpreadCell::fraction_half:
                index=2;
                break;
            case KSpreadCell::fraction_quarter:
                index=4;
                break;
            case KSpreadCell::fraction_eighth:
                index=8;
                break;
            case KSpreadCell::fraction_sixteenth:
                index=16;
                break;
            case KSpreadCell::fraction_tenth:
                index=10;
                break;
            case KSpreadCell::fraction_hundredth:
                index=100;
                break;
            case KSpreadCell::fraction_one_digit:
                index=3;
                break;
            case KSpreadCell::fraction_two_digits:
                index=4;
                break;
            case KSpreadCell::fraction_three_digits:
                index=5;
                break;
            default:
                kdDebug(36001)<<"Error in Fraction format\n";
                break;
        }

        if( _tmpFormat !=KSpreadCell::fraction_three_digits
            && _tmpFormat !=KSpreadCell::fraction_two_digits
            && _tmpFormat !=KSpreadCell::fraction_one_digit)
        {
            double calc = 0;
            int index1 = 1;
            double diff = result;
            for(int i=1;i<index;i++)
            {
                calc = i*1.0 / index;
                if( fabs( result - calc ) < diff )
                {
                    index1=i;
                    diff = fabs(result-calc);
                }
            }
            tmp = tmp.setNum( floor(value) ) + " " + tmp.setNum( index1 ) + "/" + tmp.setNum( index );
        }
        else
        {
            int limit=0;

            double precision=0;

            if(_tmpFormat ==KSpreadCell::fraction_three_digits)
                limit=999;
            else if(_tmpFormat ==KSpreadCell::fraction_two_digits)
                limit=99;
            else if(_tmpFormat ==KSpreadCell::fraction_one_digit)
                limit=9;
            double denominator=0;
            double numerator=0;
            do
            {
                double val1=result;
                double inter2=1;
                double inter4=0;
                double p=0;
                double q=0;
                double val2=rint(result);

                precision=pow((float)10,(-1*index));
                numerator=rint(result);
                denominator=1;
                while(fabs(numerator/denominator-result)>precision)
                {
                    val1=(1/(val1-val2));
                    val2=rint(val1);
                    p= val2*numerator + inter2;
                    q= val2*denominator + inter4;
                    inter2=numerator;
                    inter4=denominator;
                    numerator=p;
                    denominator=q;
                }
                index--;
            }
            while (fabs(denominator)>limit) ;
            if(fabs(denominator)==fabs(numerator))
                tmp = tmp.setNum( floor(value+1) )+" ";
            else
                tmp = tmp.setNum( floor(value) ) + " " + tmp.setNum( fabs(numerator) ) + "/" + tmp.setNum( fabs(denominator) );
        }

    }

    return tmp;
}

QString util_timeFormat( KLocale* locale, QTime m_Time, KSpreadCell::formatNumber tmpFormat)
{
    QString tmp;
    if( tmpFormat == KSpreadCell::Time )
        tmp = locale->formatTime(m_Time,false);
    else if(tmpFormat == KSpreadCell::SecondeTime )
        tmp = locale->formatTime(m_Time,true);
    else if(tmpFormat == KSpreadCell::Time_format1)
        {// 9 : 01 AM
            QString tmpTime;
            if( m_Time.hour()>12)
                tmp=QString().sprintf("%02d", m_Time.hour()-12)+":"+QString().sprintf("%02d",m_Time.minute()) +" "+ i18n("PM");
            else
                tmp=QString().sprintf("%02d", m_Time.hour())+":"+QString().sprintf("%02d",m_Time.minute()) +" "+ i18n("AM");
        }
    else if(tmpFormat == KSpreadCell::Time_format2)
        { //9:01:05 AM
            QString tmpTime;
            if( m_Time.hour()>12)
                tmp=QString().sprintf("%02d", m_Time.hour()-12)+":"+QString().sprintf("%02d",m_Time.minute()) +":"+QString().sprintf("%02d",m_Time.second())+" "+ i18n("PM");
            else
                tmp=QString().sprintf("%02d", m_Time.hour())+":"+QString().sprintf("%02d",m_Time.minute()) +":"+QString().sprintf("%02d",m_Time.second())+" "+ i18n("AM");
        }
    else if(tmpFormat == KSpreadCell::Time_format3)
        { // 9 h 01 min 28 s
            QString tmpTime;
            tmp=QString().sprintf("%02d",m_Time.hour())+" "+i18n("h")+" "+QString().sprintf("%02d",m_Time.minute())+" "+i18n("min")+" "+QString().sprintf("%02d",m_Time.second())  +" "+ i18n("s");
        }
    else
        tmp = locale->formatTime(m_Time,false);
    return tmp;
}

QString util_dateFormat( KLocale* locale, QDate m_Date, KSpreadCell::formatNumber tmpFormat)
{
    QString tmp,tmp2;
    if(tmpFormat==KSpreadCell::ShortDate)
        tmp = locale->formatDate(m_Date,true);
    else if(tmpFormat==KSpreadCell::TextDate)
        tmp=locale->formatDate(m_Date,false);
    else if(tmpFormat==KSpreadCell::date_format1)/*18-Feb-99*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==KSpreadCell::date_format2) /*18-Feb-1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==KSpreadCell::date_format3) /*18-Feb*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+locale->monthName(m_Date.month(), true);
    }
    else if(tmpFormat==KSpreadCell::date_format4) /*18-05*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"-"+QString().sprintf("%02d", m_Date.month());
    }
    else if(tmpFormat==KSpreadCell::date_format5) /*18/05/00*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+ QString().sprintf("%02d", m_Date.month())+"/";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==KSpreadCell::date_format6) /*18/05/1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+ QString().sprintf("%02d", m_Date.month())+"/";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==KSpreadCell::date_format7) /*Feb-99*/
    {
        tmp=locale->monthName(m_Date.month(), true)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==KSpreadCell::date_format8) /*February-99*/
    {
        tmp=locale->monthName(m_Date.month())+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==KSpreadCell::date_format9) /*February-1999*/
    {
        tmp=locale->monthName(m_Date.month())+"-";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==KSpreadCell::date_format10) /*F-99*/
    {
        tmp=locale->monthName(m_Date.month()).at(0)+"-";
        tmp=tmp+tmp2.setNum(m_Date.year()).right(2);
    }
    else if(tmpFormat==KSpreadCell::date_format11) /*18/Feb*/
    {
        tmp=QString().sprintf("%02d", m_Date.day())+"/";
        tmp+=locale->monthName(m_Date.month(),true);
    }
    else if(tmpFormat==KSpreadCell::date_format12) /*18/02*/
    {
        tmp=QString().sprintf("%02d", m_Date.day())+"/";
        tmp+=QString().sprintf("%02d", m_Date.month());
    }
    else if(tmpFormat==KSpreadCell::date_format13) /*18/Feb/1999*/
    {
        tmp=QString().sprintf("%02d", m_Date.day());
        tmp=tmp+"/"+locale->monthName(m_Date.month(),true)+"/";
        tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else if(tmpFormat==KSpreadCell::date_format14) /*2000/Feb/18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"/"+locale->monthName(m_Date.month(),true)+"/";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else if(tmpFormat==KSpreadCell::date_format15) /*2000-Feb-18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"-"+locale->monthName(m_Date.month(),true)+"-";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else if(tmpFormat==KSpreadCell::date_format16) /*2000-02-18*/
    {
        tmp=tmp2.setNum(m_Date.year());
        tmp=tmp+"-"+QString().sprintf("%02d", m_Date.month())+"-";
        tmp=tmp+QString().sprintf("%02d", m_Date.day());
    }
    else if(tmpFormat==KSpreadCell::date_format17) /*2 february 2000*/
    {
      tmp=QString().sprintf("%d", m_Date.day());
      tmp=tmp+" "+locale->monthName(m_Date.month())+" ";
      tmp=tmp+tmp2.setNum(m_Date.year());
    }
    else
        tmp = locale->formatDate(m_Date,true);
    return tmp;
}






QString util_columnLabel( int column )
{
  char buffer[ 100 ];

  if ( column <= 26 )
    sprintf( buffer, "%c", 'A' + column - 1 );
  else if ( column <= 26 * 26 )
    sprintf( buffer, "%c%c",'A'+((column-1)/26)-1,'A'+((column-1)%26));
  else
    sprintf( buffer, "%c%c",'Y','Z');
    //sprintf( buffer, "%c%c",'A'+((26-1)/26)-1,'A'+((26-1)%26));
    //limit is 26*26
    //strcpy( buffer,"@@@");

  return QString( buffer );
}

QString util_cellName( int _col, int _row )
{
  QString result( "%1%2" );
  result = result.arg( util_columnLabel( _col ) ).arg( _row );
  return result;
}

QString util_cellName( KSpreadTable* table, int _col, int _row )
{
  QString result( "%1!%2%3" );
  result = result.arg( table->tableName() ).arg( util_columnLabel( _col ) ).arg( _row );
  return result;
}

QString util_rangeName( QRect _area )
{
  QString result;
  result = util_cellName( _area.left(), _area.top() );
  result += ":";
  result += util_cellName( _area.right(), _area.bottom() );

  return result;
}

QString util_rangeName( KSpreadTable *_table, QRect _area )
{
  QString result( _table->tableName() );
  result += "!";
  result += util_rangeName( _area );

  return result;
}

KSpreadPoint::KSpreadPoint( const QString& _str )
{
  table = 0;
  init( _str );
}

void KSpreadPoint::init( const QString& _str )
{
  pos.setX( -1 );

  if ( _str.isEmpty() )
    return;

  uint p = 0;
  uint len = _str.length();

  // Fixed ?
  if ( _str[p] == '$' )
  {
    columnFixed = true;
    p++;
  } else
    columnFixed = false;

  // Malformed ?
  /*if ( p == len || _str[p] < 'A' || _str[p] > 'Z' )
    return;
  */
  if ( p == len || _str[p] < 'A' || _str[p] > 'Z' )
        {
        if(_str[p] < 'a' || _str[p] > 'z' )
                return;
        }
  int offset='a'-'A';
  int x=-1;
  if(_str[p] >= 'A' && _str[p] <= 'Z')
         x= _str[p++] - 'A' + 1;
  else if(_str[p] >= 'a' && _str[p] <= 'z')
        x= _str[p++]-offset - 'A' + 1;

  // Malformed ?
  if ( p == len  )
    return;

  while( p < len && ((_str[p] >= 'A' && _str[p] <= 'Z')||(_str[p] >= 'a' && _str[p] <= 'z' )))
    {
    if(_str[p] >= 'A' && _str[p] <= 'Z')
        x = x*26 + (char)_str[p++] - 'A' + 1;
    else if(_str[p] >= 'a' && _str[p] <= 'z')
        x = x*26 + (char)_str[p++] - 'A' + 1-offset;
    }

  //limit is 26*26
  if(x>(26*26))
    return;
  // Malformed ?
  if ( p == len  )
    return;

  if ( _str[p] == '$' )
  {
    rowFixed = true;
    p++;
    // Malformed ?
    if ( p == len  )
      return;
  }
  else
    rowFixed = false;

  uint p2 = p;
  while( p < len )
  {
    if ( !isdigit( _str[p++] ) )
      return;
  }

  int y = atoi( _str.latin1() + p2 );
  if(y>=0x7FFF)
      y=0x7FFF;
  if ( y <= 0 )
    return;

  pos = QPoint( x, y );
}

KSpreadPoint::KSpreadPoint( const QString& _str, KSpreadMap* _map, KSpreadTable* _table )
{
  uint p = 0;
  int p2 = _str.find( "!" );
  if ( p2 != -1 )
  {
    tableName = _str.left( p2++ );
    table = _map->findTable( tableName );
    p = p2;
  }
  else
    table = _table;

  init( _str.mid( p ) );
}

KSpreadCell* KSpreadPoint::cell()
{
  return table->cellAt( pos );
}

KSpreadRange::KSpreadRange( const QString& _str )
{
  range.setLeft( -1 );
  table = 0;

  int p = _str.find( ":" );
  if ( p == -1 )
    return;

  KSpreadPoint ul( _str.left( p ) );
  KSpreadPoint lr( _str.mid( p + 1 ) );
  range = QRect( ul.pos, lr.pos );

  leftFixed = ul.columnFixed;
  rightFixed = lr.columnFixed;
  topFixed = ul.rowFixed;
  bottomFixed = lr.rowFixed;
}

KSpreadRange::KSpreadRange( const QString& _str, KSpreadMap* _map, KSpreadTable* _table )
{
  range.setLeft( -1 );
  table = 0;
  //used area Name as range
  if(_str.at(0)=="'" && _str.at(_str.length()-1)=="'")
    {
      QString tmp=_str.right(_str.length()-1);
      tmp=tmp.left(tmp.length()-1);
      QValueList<Reference>::Iterator it;
      QValueList<Reference> area=_map->doc()->listArea();
      for ( it = area.begin(); it != area.end(); ++it )
    	{
	  if((*it).ref_name==tmp)
                {
		  range=(*it).rect;
		  table = _map->findTable( (*it).table_name );
		  break;
		}
    	}
      leftFixed = false;
      rightFixed = false;
      topFixed = false;
      bottomFixed = false;
      return;
    }
  range.setLeft( -1 );
  table = 0;

  int p = 0;
  int p2 = _str.find( "!" );
  if ( p2 != -1 )
  {
    tableName = _str.left( p2++ );
    table = _map->findTable( tableName );
    p = p2;
  }
  else
    table = _table;

  int p3 = _str.find( ":", p );
  if ( p3 == -1 )
    return;

  KSpreadPoint ul( _str.mid( p, p3 - p ) );
  KSpreadPoint lr( _str.mid( p3 + 1 ) );
  range = QRect( ul.pos, lr.pos );

  leftFixed = ul.columnFixed;
  rightFixed = lr.columnFixed;
  topFixed = ul.rowFixed;
  bottomFixed = lr.rowFixed;
}
