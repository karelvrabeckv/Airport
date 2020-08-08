#pragma once

using namespace glm;

// ========================================

class Light
{
	private:

		vec3 pos, dir, amb, dif, spe;
		float cosCutOff /* cone size */, expo /* light power */;

	public:

		Light(vec3 pos, vec3 dir, vec3 amb, vec3 dif, vec3 spe, float cosCutOff, float expo)
		: pos(pos), dir(dir), amb(amb), dif(dif), spe(spe), cosCutOff(cosCutOff), expo(expo) {};

		// ****************************************
		
		vec3 getPos();
		void setPos(vec3 pos);
		
		vec3 getDir();
		void setDir(vec3 dir);
		
		vec3 getAmb();
		void setAmb(vec3 amb);
		
		vec3 getDif();
		void setDif(vec3 dif);
		
		vec3 getSpe();
		void setSpe(vec3 spe);
		
		float getCosCutOff();
		void setCosCutOff(float cosCutOff);
		
		float getExpo();
		void setExpo(float expo);
};
