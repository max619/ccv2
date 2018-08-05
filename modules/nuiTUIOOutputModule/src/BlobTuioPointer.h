#pragma once
#include <TuioPointer.h>
#include <nuiTrackerStructs.h>


class BlobTuioPointer : public TUIO2::TuioPointer
{
public:
	/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided 
		 * Session ID, Pointer ID, X and Y coordinate to the newly created TuioPointer.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
         * @param	ti	the Type ID  to assign
         * @param	ui	the User ID  to assign
		 * @param	pi	the Pointer ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
         * @param	a	the angle to assign
         * @param	sa	the shear angle to assign
         * @param	r	the radius to assign
         * @param	p	the pressure to assign
		 */
	BlobTuioPointer(Blob b, TUIO2::TuioTime ttime, TUIO2::TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int pi, float xp, float yp, float a, float sa, float r, float p);

	/**
	* This constructor takes the provided Session ID, Pointer ID, X and Y coordinate
	* and assigs these values to the newly created TuioPointer.
	*
	* @param	tobj	the TuioObject to assign
	* @param	ti	the Type ID  to assign
	* @param	ui	the User ID  to assign
	* @param	pi	the Pointer ID  to assign
	* @param	xp	the X coordinate to assign
	* @param	yp	the Y coordinate to assign
	* @param	a	the angle to assign
	* @param	sa	the shear angle to assign
	* @param	r	the radius to assign
	* @param	p	the pressure to assign
	*/
	BlobTuioPointer(Blob b, TUIO2::TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int pi, float xp, float yp, float a, float sa, float r, float p);


	/**
	* This constructor takes the provided Session ID, Pointer ID, X and Y coordinate
	* and assigs these values to the newly created TuioPointer.
	*
	* @param	tobj	the TuioObject to assign
	* @param	pi	the Pointer ID  to assign
	* @param	xp	the X coordinate to assign
	* @param	yp	the Y coordinate to assign
	* @param	a	the angle to assign
	* @param	sa	the shear angle to assign
	* @param	r	the radius to assign
	* @param	p	the pressure to assign
	*/
	BlobTuioPointer(Blob b, TUIO2::TuioObject *tobj, unsigned int pi, float xp, float yp, float a, float sa, float r, float p);

	BlobTuioPointer(Blob b, TUIO2::TuioObject *tobj);

	/**
	* This constructor takes the atttibutes of the provided TuioPointer
	* and assigs these values to the newly created TuioPointer.
	*
	* @param	tptr	the TuioPointer to assign
	*/
	~BlobTuioPointer();

	Blob blob;
};

