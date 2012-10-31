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

#ifndef FRAMEDBHELPERS_H__
#define FRAMEDBHELPERS_H__

#include "FrameDB.h"

int getId( FrameDB::RowType p ) {
    return p.second.id;
}

// Originals
Frame getOriginal( FrameDB::RowType p ) {
    return p.second.original;
}

void setOriginal( FrameDB::RowType &p, const Frame &v ) {
    p.second.original = v;
}

// Grays
Frame getGray( FrameDB::RowType p ) {
    return p.second.gray;
}

void setGray( FrameDB::RowType &p, const Frame &v ) {
    p.second.gray = v;
}

// Skins
Frame getSkin( FrameDB::RowType p ) {
    return p.second.skinMask;
}

void setSkin( FrameDB::RowType &p, const Frame &v ) {
    p.second.skinMask = v;
}

// SDs
Frame getSD( FrameDB::RowType p ) {
    return p.second.SD;
}

void setSD( FrameDB::RowType &p, const Frame &v ) {
    p.second.SD = v;
}

// Boundaries
Frame getBoundary( FrameDB::RowType p ) {
    return p.second.boundary;
}

void setBoundary( FrameDB::RowType &p, const Frame &v ) {
    p.second.boundary = v;
}

#endif
