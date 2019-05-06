/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef KEYBOARD_DIALOG_H
#define KEYBOARD_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include "keyboard_common.h"

class KeyboardDialog : public QDialog
{
    Q_OBJECT
public:
   explicit KeyboardDialog(QWidget *parent = nullptr);
private:
    QComboBox *layoutBox;
    QComboBox *variantBox;
    QComboBox *modelBox;
private slots:
    void populateBox(QComboBox *box, xkbType type);
    void populateBoxes();
    void handleLayoutChanged(int index);
    void handleVariantChanged(int index);
    void handleModelChanged(int index);
    void setCurrentIndex(QComboBox *box, xkbType type);
};

#endif // KEYBOARD_DIALOG_H
