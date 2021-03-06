/***************************************************************************
                               matchdialog.cpp
                              -----------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <QtGui>
#include "matchdialog.h"
#include "main.h"
#include "qucs.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include "element.h"
#include "../components/capacitor.h"
#include "../components/inductor.h"
#include "../components/ground.h"

#include <qlabel.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <q3hgroupbox.h>
#include <q3vgroupbox.h>


MatchDialog::MatchDialog(QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  setCaption(tr("Create Matching Circuit"));
  DoubleVal = new QDoubleValidator(this);

  all = new Q3VBoxLayout(this, 3,3);

  TwoCheck = new QCheckBox(tr("calculate two-port matching"), this);
  all->addWidget(TwoCheck);
  TwoCheck->setChecked(true);
  connect(TwoCheck, SIGNAL(toggled(bool)), SLOT(slotSetTwoPort(bool)));

  // ...........................................................
  Q3HGroupBox *ImpBox = new Q3HGroupBox(tr("Reference Impedance"), this);
  all->addWidget(ImpBox);
  Port1Label = new QLabel(tr("Port 1"), ImpBox);
  Ref1Edit = new QLineEdit("50", ImpBox);
  Ref1Edit->setValidator(DoubleVal);
  Ohm1Label = new QLabel(tr("ohms"), ImpBox);
  connect(Ref1Edit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  ImpBox->addSpace(50);   // placeholder
  Port2Label = new QLabel(tr("Port 2"), ImpBox);
  Ref2Edit = new QLineEdit("50", ImpBox);
  Ref2Edit->setValidator(DoubleVal);
  Ohm2Label = new QLabel(tr("ohms"), ImpBox);


  // ...........................................................
  Q3VGroupBox *SParBox = new Q3VGroupBox(tr("S Parameter"), this);
  all->addWidget(SParBox);

  Q3HBox *h1 = new Q3HBox(SParBox);
  h1->setSpacing(3);
  FormatLabel = new QLabel(tr("Input format"), h1);
  FormatCombo = new QComboBox(h1);
  FormatCombo->insertItem(tr("real/imag"));
  FormatCombo->insertItem(tr("mag/deg"));
  connect(FormatCombo, SIGNAL(activated(int)), SLOT(slotChangeMode(int)));
  QWidget *place1 = new QWidget(h1); // stretchable placeholder
  h1->setStretchFactor(place1, 5);

  Q3HBox *h3 = new Q3HBox(SParBox);
  h3->setSpacing(3);
  Q3VBox *VBox1 = new Q3VBox(h3);
    S11Label = new QLabel(tr("S11"), VBox1);
    S21Label = new QLabel(tr("S21"), VBox1);
  Q3VBox *VBox2 = new Q3VBox(h3);
    S11magEdit = new QLineEdit("0.5", VBox2);
    S11magEdit->setValidator(DoubleVal);
    S21magEdit = new QLineEdit("0.5", VBox2);
    S21magEdit->setValidator(DoubleVal);
  Q3VBox *VBox3 = new Q3VBox(h3);
    S11sLabel = new QLabel("+j", VBox3);
    S21sLabel = new QLabel("+j", VBox3);
  Q3VBox *VBox4 = new Q3VBox(h3);
    S11degEdit = new QLineEdit("0", VBox4);
    S11degEdit->setValidator(DoubleVal);
    S21degEdit = new QLineEdit("0", VBox4);
    S21degEdit->setValidator(DoubleVal);
  Q3VBox *VBox5 = new Q3VBox(h3);
    S11uLabel = new QLabel(" ", VBox5);
    S21uLabel = new QLabel(" ", VBox5);
  QWidget *place4 = new QWidget(h3); // stretchable placeholder
  h3->setStretchFactor(place4, 5);
  Q3VBox *VBox6 = new Q3VBox(h3);
    S12Label = new QLabel(tr("S12"), VBox6);
    S22Label = new QLabel(tr("S22"), VBox6);
  Q3VBox *VBox7 = new Q3VBox(h3);
    S12magEdit = new QLineEdit("0", VBox7);
    S12magEdit->setValidator(DoubleVal);
    S22magEdit = new QLineEdit("0.5", VBox7);
    S22magEdit->setValidator(DoubleVal);
  Q3VBox *VBox8 = new Q3VBox(h3);
    S12sLabel = new QLabel("+j", VBox8);
    S22sLabel = new QLabel("+j", VBox8);
  Q3VBox *VBox9 = new Q3VBox(h3);
    S12degEdit = new QLineEdit("0", VBox9);
    S12degEdit->setValidator(DoubleVal);
    S22degEdit = new QLineEdit("0", VBox9);
    S22degEdit->setValidator(DoubleVal);
  Q3VBox *VBox0 = new Q3VBox(h3);
    S12uLabel = new QLabel(" ", VBox0);
    S22uLabel = new QLabel(" ", VBox0);

  connect(S21magEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  connect(S21degEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotImpedanceChanged(const QString&)));
  connect(S11magEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotReflexionChanged(const QString&)));
  connect(S11degEdit, SIGNAL(textChanged(const QString&)),
	  SLOT(slotReflexionChanged(const QString&)));


  Q3HBox *h2 = new Q3HBox(SParBox);
  h2->setSpacing(3);
  FrequencyLabel = new QLabel(tr("Frequency:"), h2);
  FrequencyEdit = new QLineEdit(h2);
  FrequencyEdit->setValidator(DoubleVal);
  UnitCombo = new QComboBox(h2);
  UnitCombo->insertItem("Hz");
  UnitCombo->insertItem("kHz");
  UnitCombo->insertItem("MHz");
  UnitCombo->insertItem("GHz");
  QWidget *place2 = new QWidget(h2); // stretchable placeholder
  h2->setStretchFactor(place2, 5);

  // ...........................................................
  Q3HBox *h0 = new Q3HBox(this);
  h0->setSpacing(5);
  all->addWidget(h0);
  QWidget *place3 = new QWidget(h0); // stretchable placeholder
  h0->setStretchFactor(place3, 5);
  connect(new QPushButton(tr("Create"),h0), SIGNAL(clicked()),
	  SLOT(slotButtCreate()));
  connect(new QPushButton(tr("Cancel"),h0), SIGNAL(clicked()),
	  SLOT(reject()));


//  slotReflexionChanged("");  // calculate impedance
  setFrequency(1e9);  // set 1GHz
  resize(520, 100);
}

MatchDialog::~MatchDialog()
{
  delete all;
  delete DoubleVal;
}

// -----------------------------------------------------------------------
void MatchDialog::setFrequency(double Freq_)
{
  int Expo = int(log10(Freq_) / 3.0);
  if(Expo < 0) Expo = 0;
  else if(Expo > 3) Expo = 3;
  UnitCombo->setCurrentItem(Expo);
  Freq_ /= pow(10.0, double(3*Expo));
  FrequencyEdit->setText(QString::number(Freq_));
}

// -----------------------------------------------------------------------
// Is called when the checkbox for two-port matching changes.
void MatchDialog::slotSetTwoPort(bool on)
{
  if(on) { // two-port matching ?
    S11Label->setText(tr("S11"));
    S21Label->setText(tr("S21"));
    S12magEdit->setEnabled(true);
    S22magEdit->setEnabled(true);
    S12degEdit->setEnabled(true);
    S22degEdit->setEnabled(true);
    S12Label->setEnabled(true);
    S22Label->setEnabled(true);
    S12sLabel->setEnabled(true);
    S22sLabel->setEnabled(true);
    S12degEdit->setEnabled(true);
    S22degEdit->setEnabled(true);
    S12uLabel->setEnabled(true);
    S22uLabel->setEnabled(true);
    Port2Label->setEnabled(true);
    Ref2Edit->setEnabled(true);
    Ohm2Label->setEnabled(true);
  }
  else {
    S11Label->setText(tr("Reflexion Coefficient"));
    S21Label->setText(tr("Impedance (ohms)"));
    S12magEdit->setEnabled(false);
    S22magEdit->setEnabled(false);
    S12degEdit->setEnabled(false);
    S22degEdit->setEnabled(false);
    S12Label->setEnabled(false);
    S22Label->setEnabled(false);
    S12sLabel->setEnabled(false);
    S22sLabel->setEnabled(false);
    S12degEdit->setEnabled(false);
    S22degEdit->setEnabled(false);
    S12uLabel->setEnabled(false);
    S22uLabel->setEnabled(false);
    Port2Label->setEnabled(false);
    Ref2Edit->setEnabled(false);
    Ohm2Label->setEnabled(false);
  }
}

// -----------------------------------------------------------------------
// Is called when the combobox changes between mag/deg and real/imag.
void MatchDialog::slotChangeMode(int Index)
{
  if(Index) { // polar ?
    S11sLabel->setText("/");
    S12sLabel->setText("/");
    S21sLabel->setText("/");
    S22sLabel->setText("/");
    S11uLabel->setText("�");
    S12uLabel->setText("�");
    S21uLabel->setText("�");
    S22uLabel->setText("�");

    double Real = S11magEdit->text().toDouble();
    double Imag = S11degEdit->text().toDouble();
    c2p(Real, Imag);
    S11magEdit->setText(QString::number(Real));
    S11degEdit->setText(QString::number(Imag));

    Real = S12magEdit->text().toDouble();
    Imag = S12degEdit->text().toDouble();
    c2p(Real, Imag);
    S12magEdit->setText(QString::number(Real));
    S12degEdit->setText(QString::number(Imag));

    Real = S21magEdit->text().toDouble();
    Imag = S21degEdit->text().toDouble();
    c2p(Real, Imag);
    S21magEdit->setText(QString::number(Real));
    S21degEdit->setText(QString::number(Imag));

    Real = S22magEdit->text().toDouble();
    Imag = S22degEdit->text().toDouble();
    c2p(Real, Imag);
    S22magEdit->setText(QString::number(Real));
    S22degEdit->setText(QString::number(Imag));
  }
  else {  // cartesian
    S11sLabel->setText("+j");
    S12sLabel->setText("+j");
    S21sLabel->setText("+j");
    S22sLabel->setText("+j");
    S11uLabel->setText(" ");
    S12uLabel->setText(" ");
    S21uLabel->setText(" ");
    S22uLabel->setText(" ");

    double Mag   = S11magEdit->text().toDouble();
    double Phase = S11degEdit->text().toDouble();
    p2c(Mag, Phase);
    S11magEdit->setText(QString::number(Mag));
    S11degEdit->setText(QString::number(Phase));

    Mag   = S12magEdit->text().toDouble();
    Phase = S12degEdit->text().toDouble();
    p2c(Mag, Phase);
    S12magEdit->setText(QString::number(Mag));
    S12degEdit->setText(QString::number(Phase));

    Mag   = S21magEdit->text().toDouble();
    Phase = S21degEdit->text().toDouble();
    p2c(Mag, Phase);
    S21magEdit->setText(QString::number(Mag));
    S21degEdit->setText(QString::number(Phase));

    Mag   = S22magEdit->text().toDouble();
    Phase = S22degEdit->text().toDouble();
    p2c(Mag, Phase);
    S22magEdit->setText(QString::number(Mag));
    S22degEdit->setText(QString::number(Phase));
  }
}

// -----------------------------------------------------------------------
// Is called if the user changed the impedance. -> The reflexion
// coefficient is calculated.
void MatchDialog::slotImpedanceChanged(const QString&)
{
  if(TwoCheck->isChecked())  return;

  double Z0   = Ref1Edit->text().toDouble();
  double Real = S21magEdit->text().toDouble();
  double Imag = S21degEdit->text().toDouble();
  z2r(Real, Imag, Z0);
  S11magEdit->blockSignals(true); // do not call "changed-slot"
  S11magEdit->setText(QString::number(Real));
  S11magEdit->blockSignals(false);
  S11degEdit->blockSignals(true); // do not call "changed-slot"
  S11degEdit->setText(QString::number(Imag));
  S11degEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the user changed the reflexion coefficient. -> The impedance
// is calculated.
void MatchDialog::slotReflexionChanged(const QString&)
{
  if(TwoCheck->isChecked())  return;

  double Z0   = Ref1Edit->text().toDouble();
  double Real = S11magEdit->text().toDouble();
  double Imag = S11degEdit->text().toDouble();
  r2z(Real, Imag, Z0);
  S21magEdit->blockSignals(true); // do not call "changed-slot"
  S21magEdit->setText(QString::number(Real));
  S21magEdit->blockSignals(false);
  S21degEdit->blockSignals(true); // do not call "changed-slot"
  S21degEdit->setText(QString::number(Imag));
  S21degEdit->blockSignals(false);
}

// -----------------------------------------------------------------------
// Is called if the "Create"-button is pressed.
void MatchDialog::slotButtCreate()
{
  double Z1   = Ref1Edit->text().toDouble();
  double Z2   = Ref2Edit->text().toDouble();
  double Freq = FrequencyEdit->text().toDouble() *
                pow(10.0, 3.0*double(UnitCombo->currentItem()));

  double S11real = S11magEdit->text().toDouble();
  double S11imag = S11degEdit->text().toDouble();
  double S12real = S12magEdit->text().toDouble();
  double S12imag = S12degEdit->text().toDouble();
  double S21real = S21magEdit->text().toDouble();
  double S21imag = S21degEdit->text().toDouble();
  double S22real = S22magEdit->text().toDouble();
  double S22imag = S22degEdit->text().toDouble();
  if(FormatCombo->currentItem()) {  // are they polar ?
    p2c(S11real, S11imag);
    p2c(S12real, S12imag);
    p2c(S21real, S21imag);
    p2c(S22real, S22imag);
  }

  if(TwoCheck->isChecked()) {  // two-port matching ?
    // determinante of S-parameter matrix
    double DetReal = S11real*S22real - S11imag*S22imag
                   - S12real*S21real + S12imag*S21imag;
    double DetImag = S11real*S22imag + S11imag*S22real
                   - S12real*S21imag - S12imag*S21real;

    if(!MatchDialog::calc2PortMatch(S11real, S11imag, S22real, S22imag,
                                    DetReal, DetImag, Z1, Z2, Freq))
      return;
  }
  else
    if(!calcMatchingCircuit(S11real, S11imag, Z1, Freq))
      return;

  QucsMain->slotEditPaste(true);
  accept();
}


// -----------------------------------------------------------------------
// transform real/imag into mag/deg (cartesian to polar)
void MatchDialog::c2p(double& Real, double& Imag)
{
  double Real_ = Real;
  Real = sqrt(Real*Real + Imag*Imag);     // magnitude
  Imag = 180.0/M_PI * atan2(Imag, Real_); // phase in degree
}

// -----------------------------------------------------------------------
// transform mag/deg into real/imag (polar to cartesian)
void MatchDialog::p2c(double& Real, double& Imag)
{
  double Real_ = Real;
  Real = Real_ * cos(Imag * M_PI/180.0);  // real part
  Imag = Real_ * sin(Imag * M_PI/180.0);  // imaginary part
}

// -----------------------------------------------------------------------
// transform reflexion coefficient into impedance
void MatchDialog::r2z(double& Real, double& Imag, double Z0)
{
  double tmp  = Z0 / ((1.0-Real)*(1.0-Real) + Imag*Imag);
  Real  = (1.0 - Real*Real - Imag*Imag) * tmp;
  Imag *= 2.0 * tmp;
}

// -----------------------------------------------------------------------
// transform impedance into reflexion coefficient
void MatchDialog::z2r(double& Real, double& Imag, double Z0)
{
  double tmp  = (Real+Z0)*(Real+Z0) + Imag*Imag;
  Real  = (Real*Real + Imag*Imag - Z0*Z0) / tmp;
  Imag *= 2.0 * Z0 / tmp;
}

// -----------------------------------------------------------------------
// Calculate matching circuit. Returns string like "sp:1nH:5pF"
QString MatchDialog::calcMatching(double r_real, double r_imag,
				double Z0, double Freq)
{
  double Zreal = r_real, Zimag = r_imag;
  r2z(Zreal, Zimag, Z0);

  if (Zreal < 0.0) {
    if (Zreal < -1e-13) {
      QMessageBox::critical(0, tr("Error"),
	tr("Real part of impedance must be greater zero,\nbut is %1 !").arg(Zreal));
      return QString("");  // matching not possible
    }

    // In high-Q circuits, Zreal often becomes somewhat about -1e-16
    // because of numerical inaccuracy.
    Zreal = 0.0;
  }


  double X1, X2, Omega = 2.0 * M_PI * Freq;
  QString Str;
  if(r_real < 0) {
    // ...................................................
    // first serial than parallel component (possible if Zreal <= Z0)
    Str = "sp";
    X1 = sqrt(Zreal * (Z0 - Zreal));
    if (Zimag < 0.0)  X1 *= -1.0;   // always use shortest matching path
    X1 -= Zimag;

    // parallel component
    X2 = (Zimag + X1) / (Zreal*Zreal + (Zimag + X1)*(Zimag + X1));
  }
  else {

    Str = "ps";
    X1 = Zreal + Zimag*Zimag / Zreal - Z0;
    // ...................................................
    // first parallel than serial component (possible if X >= 0.0)
    X1 = sqrt(Z0 * X1);
    if (Zimag > 0.0)  X1 *= -1.0;   // always use shortest matching path

    // parallel component
    X2 = Zimag / (Zreal*Zreal + Zimag*Zimag) + X1 / (Z0*Z0 + X1*X1);
  }


  // serial component
  if (X1 < 0.0)   // capacitance ?
    Str += ':' + num2str(-1.0 / Omega / X1) + 'F';
  else   // inductance
    Str += ':' + num2str(X1 / Omega) + 'H';

  // parallel component
  if (X2 < 0.0)   // inductance ?
    Str += ':' + num2str(-1.0 / Omega / X2) + 'H';
  else   // capacitance
    Str += ':' + num2str(X2 / Omega) + 'F';

  return Str;
}

// -----------------------------------------------------------------------
bool MatchDialog::calcMatchingCircuit(double r_real, double r_imag,
					double Z0, double Freq)
{
  QString Schematic =
      "<Qucs Schematic " PACKAGE_VERSION ">\n"
      "<Components>\n";


  QString Str = calcMatching(r_real, r_imag, Z0, Freq);
  if(Str.isEmpty())  return false;

  if(Str.section(':', 0,0) == "sp") {
    // ...................................................
    // first serial than parallel component

    if(Str.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 100 10 -26 10 0 0 \"" + Str.section(':', 1,1) + "\" 1>\n";

    if(Str.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 30 80 17 -26 0 1 \"" + Str.section(':', 2,2) + "\" 1>\n";

    Schematic +=
       "<GND * 1 30 110 0 0 0 0>\n"
       "</Components>\n"
       "<Wires>\n"
       "<10 10 30 10 \"\" 0 0 0 \"\">\n"
       "<30 10 70 10 \"\" 0 0 0 \"\">\n"
       "<30 10 30 50 \"\" 0 0 0 \"\">\n";
  }
  else {
    // ...................................................
    // first parallel than serial component

    if(Str.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 50 10 -26 10 0 0 \"" + Str.section(':', 1,1) + "\" 1>\n";

    if(Str.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 130 70 17 -26 0 1 \"" + Str.section(':', 2,2) + "\" 1>\n";

    Schematic +=
       "<GND * 1 130 100 0 0 0 0>\n"
       "</Components>\n"
       "<Wires>\n"
       "<130 10 130 40 \"\" 0 0 0 \"\">\n"
       "<130 10 150 10 \"\" 0 0 0 \"\">\n"
       "<80 10 130 10 \"\" 0 0 0 \"\">\n";
  }

  Schematic += QString(
     "</Wires>\n"
     "<Diagrams>\n"
     "</Diagrams>\n"
     "<Paintings>\n"
     "  <Text -20 -10 12 #000000 0 \"%1 Ohm\">\n"
     "  <Text 120 -10 12 #000000 0 \"device\">\n"
     "</Paintings>\n").arg(Z0);

  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
  return true;
}

// -----------------------------------------------------------------------
// Fundamental calculations for concurrent 2-port matching.
QString MatchDialog::calcBiMatch(double S11real, double S11imag,
            double S22real, double S22imag, double DetReal, double DetImag,
            double Z0, double Freq)
{
  double B = 1.0 + S11real*S11real + S11imag*S11imag
                 - S22real*S22real - S22imag*S22imag
                 - DetReal*DetReal - DetImag*DetImag;
  double Creal = S11real - S22real*DetReal - S22imag*DetImag;
  double Cimag = S22real*DetImag - S11imag - S22imag*DetReal;
  double Cmag  = 2.0 * (Creal*Creal + Cimag*Cimag);
  Creal /= Cmag;
  Cimag /= Cmag;

  double Rreal = B*B - 2.0*Cmag;
  double Rimag;
  if(Rreal < 0.0) {
    Rimag = Cimag * B - Creal * sqrt(-Rreal);
    Rreal = Creal * B + Cimag * sqrt(-Rreal);
  }
  else {
    Rreal  = B - sqrt(Rreal);
    Rimag  = Cimag * Rreal;
    Rreal *= Creal;
  }

  return calcMatching(Rreal, -Rimag, Z0, Freq);
}

// -----------------------------------------------------------------------
bool MatchDialog::calc2PortMatch(double S11real, double S11imag,
            double S22real, double S22imag, double DetReal, double DetImag,
            double Z1, double Z2, double Freq)
{
  QString Input = calcBiMatch(S11real, S11imag, S22real, S22imag,
                              DetReal, DetImag, Z1, Freq);
  if(Input.isEmpty()) return false;

  QString Output = calcBiMatch(S22real, S22imag, S11real, S11imag,
                              DetReal, DetImag, Z2, Freq);
  if(Output.isEmpty()) return false;

  QString Schematic =
      "<Qucs Schematic " PACKAGE_VERSION ">\n"
      "<Components>\n";


  // ...................................................
  // create input circuit
  if(Input.section(':', 0,0) == "sp") {
    // first serial than parallel component

    if(Input.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 -50 10 -26 10 0 0 \"" + Input.section(':', 1,1) + "\" 1>\n";

    if(Input.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 -120 80 17 -26 0 1 \"" + Input.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 -120 110 0 0 0 0>\n";
  }
  else {
    // first parallel than serial component

    if(Input.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 -140 10 -26 10 0 0 \"" + Input.section(':', 1,1) + "\" 1>\n";

    if(Input.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 -60 70 17 -26 0 1 \"" + Input.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 -60 100 0 0 0 0>\n";
  }


  // ...................................................
  // create output circuit
  if(Output.section(':', 0,0) == "sp") {
    // first serial than parallel component

    if(Output.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 50 10 -26 10 0 0 \"" + Output.section(':', 1,1) + "\" 1>\n";

    if(Output.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 120 80 17 -26 0 1 \"" + Output.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 120 110 0 0 0 0>\n";
  }
  else {
    // ...................................................
    // first parallel than serial component

    if(Output.section(':', 1,1).right(1) == "F")
      Schematic += "<C C1";
    else
      Schematic += "<L L1";
    Schematic += " 1 140 10 -26 10 0 0 \"" + Output.section(':', 1,1) + "\" 1>\n";

    if(Output.section(':', 2,2).right(1) == "F")
      Schematic += "<C C2";
    else
      Schematic += "<L L2";
    Schematic += " 1 60 70 17 -26 0 1 \"" + Output.section(':', 2,2) + "\" 1>\n";

    Schematic += "<GND * 1 60 100 0 0 0 0>\n";
  }
  Schematic += "</Components>\n"
               "<Wires>\n";


  // ...................................................
  // create wires for input circuit
  if(Input.section(':', 0,0) == "sp") // first serial than parallel component
    Schematic +=
       "<-140 10 -120 10 \"\" 0 0 0 \"\">\n"
       "<-120 10 -80 10 \"\" 0 0 0 \"\">\n"
       "<-120 10 -120 50 \"\" 0 0 0 \"\">\n";
  else    // first parallel than serial component
    Schematic +=
       "<-60 10 -60 40 \"\" 0 0 0 \"\">\n"
       "<-60 10 -40 10 \"\" 0 0 0 \"\">\n"
       "<-110 10 -60 10 \"\" 0 0 0 \"\">\n";


  // ...................................................
  // create wires for output circuit
  if(Output.section(':', 0,0) == "sp") // first serial than parallel component
    Schematic +=
       "<140 10 120 10 \"\" 0 0 0 \"\">\n"
       "<120 10 80 10 \"\" 0 0 0 \"\">\n"
       "<120 10 120 50 \"\" 0 0 0 \"\">\n";
  else    // first parallel than serial component
    Schematic +=
       "<60 10 60 40 \"\" 0 0 0 \"\">\n"
       "<60 10 40 10 \"\" 0 0 0 \"\">\n"
       "<110 10 60 10 \"\" 0 0 0 \"\">\n";

  Schematic +=
     "</Wires>\n"
     "<Diagrams>\n"
     "</Diagrams>\n"
     "<Paintings>\n"
     "  <Text -200 -10 12 #000000 0 \"Port 1\">\n"
     "  <Text -20 -10 12 #000000 0 \"device\">\n"
     "  <Text 160 -10 12 #000000 0 \"Port 2\">\n"
     "</Paintings>\n";

  QApplication::clipboard()->setText(Schematic, QClipboard::Clipboard);
  return true;
}
