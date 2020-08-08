#include "pgr.h"
#include "headers/light.h"

// ========================================

vec3  Light::getPos()                      {return this->pos;}
void  Light::setPos(vec3 pos)              {this->pos = pos;}

vec3  Light::getDir()                      {return this->dir;}
void  Light::setDir(vec3 dir)              {this->dir = dir;}

vec3  Light::getAmb()                      {return this->amb;}
void  Light::setAmb(vec3 amb)              {this->amb = amb;}

vec3  Light::getDif()                      {return this->dif;}
void  Light::setDif(vec3 dif)              {this->dif = dif;}

vec3  Light::getSpe()                      {return this->spe;}
void  Light::setSpe(vec3 spe)              {this->spe = spe;}

float Light::getCosCutOff()                {return this->cosCutOff;}
void  Light::setCosCutOff(float cosCutOff) {this->cosCutOff = cosCutOff;}

float Light::getExpo()                     {return this->expo;}
void  Light::setExpo(float expo)           {this->expo = expo;}
