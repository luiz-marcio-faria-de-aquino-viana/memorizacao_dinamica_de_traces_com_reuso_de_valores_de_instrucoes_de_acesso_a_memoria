
/*
 * component.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/21/2000
 */

#include"component.h"

CComponent::CComponent(int _classid)
{
  ClassID = _classid;
}

int CComponent::GetClassID()
{
  return ClassID;
}
