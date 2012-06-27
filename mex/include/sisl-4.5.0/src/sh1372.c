//===========================================================================
// SISL - SINTEF Spline Library, version 4.5.0.
// Definition and interrogation of NURBS curves and surfaces. 
//
// Copyright (C) 2000-2005, 2010 SINTEF ICT, Applied Mathematics, Norway.
//
// This program is free software; you can redistribute it and/or          
// modify it under the terms of the GNU General Public License            
// as published by the Free Software Foundation version 2 of the License. 
//
// This program is distributed in the hope that it will be useful,        
// but WITHOUT ANY WARRANTY; without even the implied warranty of         
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
// GNU General Public License for more details.                           
//
// You should have received a copy of the GNU General Public License      
// along with this program; if not, write to the Free Software            
// Foundation, Inc.,                                                      
// 59 Temple Place - Suite 330,                                           
// Boston, MA  02111-1307, USA.                                           
//
// Contact information: E-mail: tor.dokken@sintef.no                      
// SINTEF ICT, Department of Applied Mathematics,                         
// P.O. Box 124 Blindern,                                                 
// 0314 Oslo, Norway.                                                     
//
// Other licenses are also available for this software, notably licenses
// for:
// - Building commercial software.                                        
// - Building software whose source code you wish to keep private.        
//===========================================================================

#include "sisl-copyright.h"

/*
 *
 * $Id: sh1372.c,v 1.4 2005-02-28 09:04:50 afr Exp $
 *
 */


#define SH1372

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void
sh1372(SISLCurve *pc1,double epoint[],double edirec[],double aradiu,int idim,
	   double aepsco,double aepsge,
	int trackflag,int *jtrack,SISLTrack ***wtrack,
	int *jpt,double **gpar,int **pretop,int *jcrv,SISLIntcurve ***wcurve,int *jstat)
#else
void sh1372(pc1,epoint,edirec,aradiu,idim,aepsco,aepsge,
	trackflag,jtrack,wtrack,jpt,gpar,pretop,jcrv,wcurve,jstat)
     SISLCurve    *pc1;
     double   epoint[];
     double   edirec[];
     double   aradiu;
     int      idim;
     double   aepsco;
     double   aepsge;
     int       trackflag;
     int       *jtrack;
     SISLTrack ***wtrack;
     int      *jpt;
     double   **gpar;
     int      **pretop;
     int      *jcrv;
     SISLIntcurve ***wcurve;
     int      *jstat;
#endif
/*
*********************************************************************
*
*********************************************************************
*
* PURPOSE    : Find all intersections between a curve and a cylinder if
*              the dimension is equal to three (other dimensions are not
*              handled).
*
*
*
* INPUT      : pc1    - Pointer to the curve.
*              epoint - SISLPoint on cylinder axis
*              edirec - Direction of cylinder axis
*              aradiu - Radius of the circle or sphere
*              idim   - Dimension of the space in which the cyliner lies.
*                       idim should be equal to three.
*              aepsco - Computational resolution.
*              aepsge - Geometry resolution.
*              trackflag - If true, create tracks.
*
*
*
* OUTPUT     : jtrack - Number of tracks created
*              wtrack - Array of pointers to tracks
*              jpt    - Number of single intersection points.
*              gpar   - Array containing the parameter values of the
*                       single intersection points in the parameter
*                       interval of the curve. The points lie continuous.
*                       Intersection curves are stored in wcurve.
*              pretop - Topology info. for single intersection points.
*              *jcrv  - Number of intersection curves.
*              wcurve  - Array containing descriptions of the intersection
*                       curves. The curves are only described by points
*                       in the parameter interval. The curve-pointers points
*                       to nothing. (See description of Intcurve
*                       in intcurve.dcl).
*              jstat  - status messages
*                                         > 0      : warning
*                                         = 0      : ok
*                                         < 0      : error
*
*
* METHOD     : The vertices of the curve is put into the equation of the
*              cylinder achieving a curve in the one-dimentional space.
*              The zeroes of this curve is found.
*
*
* REFERENCES :
*
*-
* CALLS      : sh1761      - Find point/curve intersections.
*              hp_s1880      - Put intersections into output format.
*              s1322      - Make matrix description of cylinder
*              s1370      - Put curve into implicit surface/curve
*              make_cv_kreg - Ensure k-regularity of input curve.
*              newCurve   - Create new curve.
*              newPoint   - Create new point.
*              newObject  - Create new object.
*              freeObject - Free the space occupied by a given object.
*              freeIntdat - Free the space occupied by intersection result.
*
* WRITTEN BY : Tor Dokken, SI, 88-05.
* REWRITTEN BY : Bjoern Olav Hoset, SI, 89-06.
* Revised by : Paal Fugelli, SINTEF, Oslo, Norway, Nov. 1994.  Updated header
*              and comments to reflect the fact that only 3D input is handled.
*
*********************************************************************
*/
{
  double *nullp = SISL_NULL;
  int kstat = 0;          /* Local status variable.                       */
  int kpos = 0;           /* Position of error.                           */
  int kdim=1;             /* Dimension of curve in curve/point intersection.*/
  double sarray[16];      /* Array for represetnation of implicit surface */
  double spoint[1];       /* SISLPoint in curve/point intersection.           */
  double *spar = SISL_NULL;    /* Values of intersections in the parameter
			     area of the second object. Empty in this case. */
  SISLCurve *qc = SISL_NULL;       /* Pointer to curve in
			     curve/point intersection.  */
  SISLPoint *qp = SISL_NULL;       /* Pointer to point in
			     curve/point intersection.  */
  SISLObject *qo1 = SISL_NULL;     /* Pointer to object in
			     object/point intersection.*/
  SISLObject *qo2 = SISL_NULL;     /* Pointer to object in
			     object/point intersection.*/
  SISLIntdat *qintdat = SISL_NULL; /* Pointer to intersection data */
  int      ksurf=0;         /* Dummy number of Intsurfs. */
  SISLIntsurf **wsurf=SISL_NULL;    /* Dummy array of Intsurfs. */
  int      kdeg=2000;       /* input to int_join_per. */
  SISLObject *track_obj=SISL_NULL;
  SISLCurve *qkreg=SISL_NULL; /* Input surface ensured k-regularity. */

  /* -------------------------------------------------------- */

  if (pc1->cuopen == SISL_CRV_PERIODIC)
  {
     /* Cyclic curve. */

     make_cv_kreg(pc1,&qkreg,&kstat);
     if (kstat < 0) goto error;
   }
  else
    qkreg = pc1;

  /*
  * Create new object and connect curve to object.
  * ------------------------------------------------
  */

  if (!(track_obj = newObject (SISLCURVE)))
    goto err101;
  track_obj->c1 = pc1;

  /*
   * Check dimension.
   * ----------------
   */

  *jpt  = 0;
  *jcrv = 0;
  *jtrack = 0;

  if ( idim != 3) goto err104;
  if (qkreg -> idim != idim) goto err103;

  /*
   * Make a matrix of dimension (idim+1)(idim+1) to describe the cyliner
   * -------------------------------------------------------------------------
   */

  s1322(epoint,edirec,aradiu,idim,kdim,sarray,&kstat);
  if (kstat<0) goto error;

  /*
   * Put curve into curve into cylinder equation
   * -------------------------------------------
   */

  s1370(qkreg,sarray,idim,kdim,0,&qc,&kstat);
  if (kstat<0) goto error;

  /*
   * Create new object and connect curve to object.
   * ----------------------------------------------
   */

  if ((qo1 = newObject(SISLCURVE)) == SISL_NULL)
      goto err101;
  qo1 -> c1 = qc;
  qo1 -> o1 = qo1;

  /*
   * Create new object and connect point to object.
   * ----------------------------------------------
   */

  if(!(qo2 = newObject(SISLPOINT))) goto err101;
  spoint[0] = DZERO;
  if(!(qp = newPoint(spoint,kdim,1))) goto err101;
  qo2 -> p1 = qp;

  /*
   * Find intersections.
   * -------------------
   */

  sh1761(qo1,qo2,aepsge,&qintdat,&kstat);
  if (kstat < 0) goto error;

  /* Join periodic curves */
  int_join_per( &qintdat,track_obj,track_obj,nullp,kdeg,aepsge,&kstat);
  if (kstat < 0)
    goto error;

  /* Create tracks */
  if (trackflag && qintdat)
    {
      make_tracks (qo1, qo2, 0, nullp,
		   qintdat->ilist, qintdat->vlist, jtrack,
		   wtrack, aepsge, &kstat);
      if (kstat < 0)
	goto error;
    }

  /*
   * Express intersections on output format.
   * ---------------------------------------
   */

  if (qintdat)/* Only if there were intersections found */
    {
      hp_s1880(track_obj, track_obj, kdeg,
	       1,0,qintdat,jpt,gpar,&spar,pretop,jcrv,wcurve,&ksurf,&wsurf,&kstat);
      if (kstat < 0) goto error;
    }

  /*
   * Intersections found.
   * --------------------
   */

  *jstat = 0;
  goto out;

  /* Error in space allocation.  */

 err101: *jstat = -101;
        s6err("sh1372",*jstat,kpos);
        goto out;

  /* Dimensions conflicting.  */

 err103: *jstat = -103;
        s6err("sh1372",*jstat,kpos);
        goto out;

  /* Dimension not equal to two or three.  */

 err104: *jstat = -104;
        s6err("sh1372",*jstat,kpos);
        goto out;

  /* Error in lower level routine.  */

  error : *jstat = kstat;
        s6err("sh1372",*jstat,kpos);
        goto out;

 out:

  /* Free allocated space.  */

  if (spar)    freearray(spar);
  if (qo1)     freeObject(qo1);
  if (qo2)     freeObject(qo2);
  if (qintdat) freeIntdat(qintdat);
  if (track_obj)
    {
       track_obj->c1 = SISL_NULL;
       freeObject(track_obj);
    }
  /* Free local curve.  */

  if (qkreg != SISL_NULL && qkreg != pc1) freeCurve(qkreg);

return;
}