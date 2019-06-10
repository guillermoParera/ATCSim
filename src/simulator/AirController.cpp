/*
 * AirController.cpp
 *
 *  Created on: 21/07/2014
 *      Author: paco
 *
 *  Copyright 2014 Francisco Martín
 *
 *  This file is part of ATCSim.
 *
 *  ATCSim is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ATCSim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ATCSim.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AirController.h"
#include "Airport.h"
#include "Flight.h"
#include "Position.h"
#include <list>
#include <fstream>
#include <cmath>

namespace atcsim{

AirController::AirController() {
	// TODO Auto-generated constructor stub

}

AirController::~AirController() {
	// TODO Auto-generated destructor stub
}


float
AirController::distanceAv_Av(Flight *f1, Flight *f2)
{
	float x1 = f1->getPosition().get_x();
	float y1 = f1->getPosition().get_y();
	float z1 = f1->getPosition().get_z();
	float x2 = f2->getPosition().get_x();
	float y2 = f2->getPosition().get_y();
	float z2 = f2->getPosition().get_z();

	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
}

float
AirController::distanceFlightAirport(Flight *f)
{
	float x = f->getPosition().get_x();
	float y = f->getPosition().get_y();
	float z = f->getPosition().get_z();

	return sqrt( (x)*(x) + (y)*(y) + (z)*(z) );
}

void
AirController::anticollisionSystem(Flight *f)
{
  std::list<Flight*> flights = Airport::getInstance()->getFlights();
  std::list<Flight*>::iterator it;
  for(it = flights.begin(); it!=flights.end(); ++it){
    if(f != (*it) ){
      float dist_margen2 = 500;
      float dist_seguridad2 = 2000 + dist_margen2;
      float distance = distanceAv_Av(f, *it);
			float distance_to_airport1 = distanceFlightAirport(f);
			float distance_to_airport2 = distanceFlightAirport(*it);
      if (distance < dist_seguridad2){
        /*float h2 = sqrt(distance * distance + dist_seguridad2 * dist_seguridad2);
        float alpha2 = asin(dist_seguridad2/h2);
				*/
				float alpha2 = asin(5*M_PI/180);
				float cabeceo = asin(5*M_PI/180);

					if (distance_to_airport1 > distance_to_airport2){
						//f->setBearing(alpha2);
						f->setInclination(cabeceo); //sube
						(*it)->setInclination(-cabeceo); // baja
						if (f->getInInfinite() == false)
							f->setInInfinite(true);
					}else if (distance_to_airport1 < distance_to_airport2){
						//(*it)->setBearing(-alpha2);
						(*it)->setInclination( f->getInclination() + cabeceo ); //sube
						f->setInclination( f->getInclination() - cabeceo ); // baja
						if ((*it)->getInInfinite() == false)
							(*it)->setInInfinite(true);
					}

					if (f->getInInfinite()  && f->getPosition().get_x() < -5000 ){
						f->setBearing( f->getBearing() + alpha2);
						(*it)->setBearing( f->getBearing() - alpha2 );
					}
//					std::cerr << "ANTIIIIIIIII COLISIOOOOOOOOOOOOOOOOOOOOOON" << '\n';
    	}
    }
  }
}

void
AirController::finalAprox(Flight *f)
{
	;
}

void
AirController::landing(Flight *f)
{

  Position pos0(3500.0, 0.0, 100.0);
  Position pos1(1500.0, 0.0, 50.0);
  Position pos2(200.0, 0.0, 25.0);
  Position pos3(-750.0, 0.0, 25.0);

  Route r0, r1, r2, r3;

  r0.pos = pos0;
  r0.speed = 150.0;
  r1.pos = pos1;
  r1.speed = 100.0;
  r2.pos = pos2;
  r2.speed = 70.0;
  r3.pos = pos3;
  r3.speed = 10.0;


    if(f->getRoute()->empty())
    {
      f->getRoute()->push_back(r3);
      f->getRoute()->push_front(r2);
      f->getRoute()->push_front(r1);
      f->getRoute()->push_front(r0);
		}
}

void
AirController::blackHole(Flight *f)
{

	float closeAltitude;
	float nextAltitude;
	Route closePoint;

	std::list<Route>::iterator it;
	for( it = f->getRoute()->begin(); it != f->getRoute()->end(); ++it){
		closePoint = *it;
	}


	closeAltitude = closePoint.pos.get_z();
	nextAltitude = closeAltitude + 550;

	Position posA(-11000.0, 0.0);
	Position posB(-16000.0, -5000);
	Position posC(-21000.0, 0.0);
	Position posD(-16000.0, 5000.0);


	Route rA, rB, rC, rD;

	rA.pos = posA;
	rA.speed = 260;
	rB.pos = posB;
	rB.speed = 260;
	rC.pos = posC;
	rC.speed = 260;
	rD.pos = posD;
	rD.speed = 260;


	if(f->getNewInBlackHole()){
		std::cerr << "BLACK HOLEEEEEEEEEEEEEEEEEEEE" << '\n';
		f->getRoute()->push_back(rA);
		f->setNewInBlackHole(false);
	}

/*
	if( f->getRoute()->size() == 1 && f->getInBlackHole() ){
		std::cerr << "SUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU" << '\n';
		if( abs(closePoint.pos.get_x() - posA.get_x()) < 200){
			std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << '\n';
			rB.pos.set_z(nextAltitude);
			f->getRoute()->push_back(rB);
		}else if( abs(closePoint.pos.get_x() - posB.get_x()) < 200 && abs(closePoint.pos.get_y() - posB.get_y()) <200 ){
			rC.pos.set_z(nextAltitude);
			f->getRoute()->push_back(rC);
		}else if( abs(closePoint.pos.get_x() - posC.get_x()) < 200 ){
			rD.pos.set_z(nextAltitude);
			f->getRoute()->push_back(rD);
		}else{
			rA.pos.set_z(nextAltitude);
			f->getRoute()->push_back(rA);
		}
	}
	*/
	if( abs(closePoint.pos.get_x() - posA.get_x()) < 50 && f->getRoute()->size() <= 1){
		std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << '\n';
		rB.pos.set_z(nextAltitude);
		f->getRoute()->push_back(rB);
		rC.pos.set_z(nextAltitude+550);
		f->getRoute()->push_back(rC);
		rD.pos.set_z(nextAltitude+550);
		f->getRoute()->push_back(rD);
		rA.pos.set_z(nextAltitude+550);
		f->getRoute()->push_back(rA);
	}

}


void
AirController::goInfine(Flight *f)
{
	Position posFustrada(-10000.0, 0.0, 5000.0);

	Route r0;

	r0.pos = posFustrada;
	r0.speed = 260;

	if(f->getNewInIntinite())
		std::cerr << "INFINITOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" << '\n';

	if( !f->getRoute()->empty() && f->getNewInIntinite() ){
		f->getRoute()->clear();
		f->getRoute()->push_front(r0);
		f->setNewInIntinite(false);
		//blackHole(f);//---------------------
	}

	if( f->getPosition().get_x() <= posFustrada.get_x() ){
		f->setInBlackHole(true);
	}
}

void
AirController::doWork()
{
  std::list<Flight*> flights = Airport::getInstance()->getFlights();
  std::list<Flight*>::iterator it;

  for(it = flights.begin(); it!=flights.end(); ++it){
    anticollisionSystem(*it);
    if( !(*it)->getInInfinite() ){
			finalAprox(*it);
			landing(*it);
		}else if( (*it)->getInBlackHole() ){
			blackHole(*it);
		}else
			goInfine(*it);
  }
}

}  // namespace atcsim
