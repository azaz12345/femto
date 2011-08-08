/*-
 * Copyright (c) 2010
 *    Department of Communications Engineering, National Chung Cheng University.
 *    (COMM/CCU) All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the National Chung Cheng
 *      University, Chia-Yi, Taiwan, and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COMM/CCU AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COMM/CCU OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)class_oper.cpp	11/02/2010
 */
//
#include "stdhead.h"
#include "external_parameters.h"


//CART2POL Transform Cartesian to polar coordinates. Return value from -PI~PI
POLAXIS COORD::cart2pol(XYAXIS reference, XYAXIS target) //numerater,denomerater
{
    POLAXIS  pol;
    double   X=target.x-reference.x;
    double   Y=target.y-reference.y;
    pol.th = atan2(Y,X);
    pol.r  = sqrt(pow(X,2)+pow(Y,2));
    return   pol;
}

//POL2CART Transform polar to Cartesian coordinates.
XYAXIS COORD::pol2cart(POLAXIS pol)
{
    XYAXIS   cart;
    cart.x = pol.r*cos(pol.th);  //cos(x), x in radian
    cart.y = pol.r*sin(pol.th);
    return   cart;
}

//transfer the related axis from origin cell to destination cell
XYAXIS COORD::cart_change(XYAXIS origin , XYAXIS destination)
{
    XYAXIS   cart;
    cart.x = origin.x - destination.x;
    cart.y = origin.y - destination.y;
    return   cart;
}

//extern double Rand(long *seed);

XYAXIS COORD::init_position(long *seed_position_x,  long *seed_position_y, double R)
{
    cart.x = 2.0*R*Rand(seed_position_x)-1.0*R; //km
    cart.y = 1.732*R*Rand(seed_position_y)-0.866*R;
    return cart;
}

double COORD::distance(XYAXIS c1, XYAXIS c2)
{
    double dis;
    dis=sqrt(pow((c1.x-c2.x),2)+pow((c1.y-c2.y),2));
    return dis;
}

