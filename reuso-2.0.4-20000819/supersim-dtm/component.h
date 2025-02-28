
/*
 * component.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/21/2000
 */

#ifndef __COMPONENT_H
#define __COMPONENT_H 500

class CComponent {
 private:

  int ClassID;

 public:

  CComponent(int _classid);

  int GetClassID();

};

#endif
