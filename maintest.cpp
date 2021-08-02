#include "sunrise.h"
#include <iostream>

int main(int argc, char **argv)
{
  QDate d;  
  if (argc != 2)
    d = QDate::currentDate();
  else
    d = QDate::fromString(argv[1], Qt::ISODate);
    
  using namespace std;
  if (!d.isValid()) {
      cerr << "Invalid date" << endl;
      return 1;
  }
  
  cout << d.toString().toStdString() << endl;
  // Sunrise sunrise(16.141, -22.904); // Boa Vista, Cabo Verde


/*
 * Hannover
   Position: 52° 23' 12" N,  9° 41' 52" E
 */
  Sunrise sunrise(52.386667, 9.697778);


  cout << "sunrise: " << sunrise.sunrise(d).toString().toStdString() << endl;
  cout << "noon:    " << sunrise.noon(d).toString().toStdString() << endl;
  cout << "sunset:  " << sunrise.sunset(d).toString().toStdString() << endl;
  
  return 0;
}
