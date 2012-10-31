/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef CONSTS_H__
#define CONSTS_H__

const static unsigned int WIDTH = 640;
const static unsigned int HEIGHT = 480;

const static int MAX_SIGN_LEN = 100;

const static unsigned int T0 = 100;
const static unsigned int T0_SQR = T0 * T0;
const static unsigned int T2 = 1;
const static unsigned int T3 = 6000;

const static int SMALLCOMPONENT = 50;

extern double ALPHA_SCALE_FACTOR;

const static int maxComponents = 7;

const static double skinColorThresh = 0.4;

#endif
