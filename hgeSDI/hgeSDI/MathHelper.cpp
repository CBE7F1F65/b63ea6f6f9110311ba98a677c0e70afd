#include "StdAfx.h"
#include "MathHelper.h"

#include "Const.h"

MathHelper::MathHelper(void)
{
}


MathHelper::~MathHelper(void)
{
}

int MathHelper::AngleRestrict( int angle )
{
	int anglebase_360 = ANGLEBASE_90*4;
	while (angle < 0)
	{
		angle += anglebase_360;
	}
	while (angle >= anglebase_360)
	{
		angle -= anglebase_360;
	}
	return angle;
}