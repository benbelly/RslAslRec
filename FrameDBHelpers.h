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

#endif
