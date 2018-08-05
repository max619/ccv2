#include "BlobTuioPointer.h"

BlobTuioPointer::BlobTuioPointer(Blob b, TUIO2::TuioTime ttime, TUIO2::TuioObject * tobj, unsigned short ti,
	unsigned short ui, unsigned int pi, float xp, float yp, float a, float sa, float r, float p) : TUIO2::TuioPointer(ttime, tobj, ti, ui, pi, xp, yp, a, sa,r,p)
{
	this->blob = b;
}

BlobTuioPointer::BlobTuioPointer(Blob b, TUIO2::TuioObject * tobj, unsigned short ti, unsigned short ui, unsigned int pi,
	float xp, float yp, float a, float sa, float r, float p) : TUIO2::TuioPointer(tobj, ti, ui, pi, xp, yp, a, sa, r, p)
{
	this->blob = b;
}

BlobTuioPointer::BlobTuioPointer(Blob b, TUIO2::TuioObject * tobj, unsigned int pi, float xp, float yp,
	float a, float sa, float r, float p) : TUIO2::TuioPointer(tobj, pi, xp, yp, a, sa, r, p)
{
	this->blob = b;
}

BlobTuioPointer::BlobTuioPointer(Blob b, TUIO2::TuioObject * tobj) : TUIO2::TuioPointer(tobj, b.id, b.keyPoint.pt.x, b.keyPoint.pt.y, 0, 0, 0, 255)
{
	this->blob = b;
}

BlobTuioPointer::~BlobTuioPointer()
{
	//TUIO2::TuioPointer::~TuioPointer();
}
