#ifndef BACKDROP_H
#define BACKDROP_H

#include "view.h"

//backdrop widget
class Backdrop : public View
{
public:
	Backdrop();
	Backdrop *draw(const Ctx &ctx) override;
};

#endif
