/*****************************************************************************
 * customwidgets.h: Custom widgets
 ****************************************************************************
 * Copyright (C) 2006 the VideoLAN team
 * Copyright (C) 2004 Daniel Molkentin <molkentin@kde.org>
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 * The "ClickLineEdit" control is based on code by  Daniel Molkentin
 * <molkentin@kde.org> for libkdepim
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef _SEARCHLINEEDIT_H_
#define _SEARCHLINEEDIT_H_

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QSpinBox>

class QVLCFramelessButton;
/**
  This class provides a QLineEdit which contains a greyed-out hinting
  text as long as the user didn't enter any text

  @short LineEdit with customizable "Click here" text
  @author Daniel Molkentin
*/
class ClickLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY( QString clickMessage READ clickMessage WRITE setClickMessage )
public:
    ClickLineEdit( const QString &msg, QWidget *parent );
    void setClickMessage( const QString &msg );
    const QString& clickMessage() const { return mClickMessage; }
    virtual void setText( const QString& txt );
protected:
    virtual void paintEvent( QPaintEvent *e );
    virtual void dropEvent( QDropEvent *ev );
    virtual void focusInEvent( QFocusEvent *ev );
    virtual void focusOutEvent( QFocusEvent *ev );
private:
    QString mClickMessage;
    bool mDrawClickMsg;
};

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit( QWidget *parent = NULL );

private:
    void resizeEvent ( QResizeEvent * event );
    void focusInEvent( QFocusEvent *event );
    void focusOutEvent( QFocusEvent *event );
    void paintEvent( QPaintEvent *event );
    void setMessageVisible( bool on );
    QVLCFramelessButton   *clearButton;
    bool message;

public slots:
    void clear();

private slots:
    void updateText( const QString& );
    void searchEditingFinished();

signals:
    void searchDelayedChanged( const QString& );
};

#endif
